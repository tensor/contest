#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

// struct of record. user is orgnized by a list of record
typedef struct Record
{
  int item;
  double rate;
};

typedef Record* RecordPtr;
typedef double* ListPtr;

#define NU 56555		// number of ordered users
#define NI 123345		// number of ordered items
#define LAMDA 0.005		// weight of q,p,y

// files for saving params
char *q_file = "q.txt";
char *p_file = "p.txt";
char *check_file = "check.txt";
char *recommendation_file = "../train/svd.txt";
char *train_file = "../train/data.txt";
// global values
vector<RecordPtr> *userRate;
// factors of users and items
double *p_u,*q_i;
// cache of predicted values
ListPtr *cache;
// number of items should be recommended
int N=10;
// torrence for every param
double TOR=0.0000000001;
// number of factors
int K=40;
// number of max iteration
int I = 40;
// learning rate, for gradient
double LRATE = 0.001;		


double cacheError = 0;

// free the global values
void free()
{
  for(int user=0;user<NU;user++)
    {
      for(int i=0;i<userRate[user].size();i++)
	delete(userRate[user][i]);
      userRate[user].clear();
      delete[] cache[user];
    }
  delete[] userRate;

  delete[] p_u;
  delete[] q_i;
  delete[] cache;  
}

string replace(const string & line, char ch1, char ch2){
     string ret;
     for(int i = 0; i < line.length(); ++i){
          if(line[i] == ch1) ret += ch2;
          else ret += line[i];
     }
     return ret;
}

// reading orgnized ratings from file and init the global values
void init(char* fileName)
{
  int user,item,rate;
  int count=0;
  double total =0;
  p_u = new double[NU*K];
  q_i = new double[NI*K];
  
  srand((unsigned)time(0));
  for(int i=0;i<NU;i++)
    {
      total = 0;
      for(int k=0;k<K;k++)
	{
	  p_u[i*K+k]=rand()%1000;
	  total+=p_u[i*K+k];
	}
      for(int k=0;k<K;k++)
	p_u[i*K+k]/=total;
    }

  for(int i=1;i<NI;i++)
    {
      total = 0;
      for(int k=0;k<K;k++)
	{
	  q_i[i*K+k]=rand()%1000;
	  total+=q_i[i*K+k];
	}
      for(int k=0;k<K;k++)
	q_i[i*K+k]/=total;
    }

  userRate = new vector<RecordPtr>[NU];
  cache = new ListPtr[NU];

  ifstream ifs(fileName);
  string line;
  while(getline(ifs,line))
    {
      string tmp = replace(line,':',' ');
      istringstream iss(tmp);
      iss>>user>>item;
      RecordPtr rp = new Record();
      rp->item = item;
      rp->rate = 1.0;
      userRate[user].push_back(rp);
    }
  ifs.close();

  // init cache      
  for(int user =0 ;user<NU;user++)
    {
      cache[user] = new double[userRate[user].size()];
      memset(cache[user],0,sizeof(double)*userRate[user].size());
    }
 }


// predict the vaule for every pair of user and item
double predict_SVD_Occ(int user,int item)
{
  double p_rate =0;
  // r = q_i*p_u
  for(int k=0;k<K;k++)
    p_rate+=p_u[user*K+k]*q_i[item*K+k];
  return p_rate;
}

double predict_SVD_Occ(int user,int i,int f)
{
  double p_rate;
  int item = userRate[user][i]->item;
  p_rate = cache[user][i];
  // r = q_i*p_u
  for(int k=f;k<K;k++)
    p_rate+=p_u[user*K+k]*q_i[item*K+k];
  return p_rate;
}

// cache contributes of fixed factors
void cache_contr(int user,int i,int f)
{
  int item = userRate[user][i]->item;
  cache[user][i]+=p_u[user*K+f]*q_i[item*K+f];
}

// train svd & occ model
void SVD_Occ()
{
  double torrence,totalError,p_rate,error,prevError;
  double *tP_u,*tQ_i;
  int user,item,k;
  double rate;
  int count;
  tP_u = new double[NU*K];
  tQ_i = new double[NI*K];

  for(int f=0;f<K;f++)
    {
      cout<<"\nFix factor "<<f+1<<"/"<<K<<endl;
      count=0;
      torrence = 1;
      while (torrence > TOR && count < I)
	{
	  memcpy(tP_u,p_u,sizeof(double)*NU*K);
	  memcpy(tQ_i,q_i,sizeof(double)*NI*K);
	  totalError = 0;	  
	  for(user = 0;user < NU;user++)
	    {
	      if (user%10000 == 0)
		cout<<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"	       
		    <<user<<"/"<<NU<<flush;
	      for(int i = 0;i < userRate[user].size();i++)
		{
		  double tmp=0;
		  item = userRate[user][i]->item;
		  rate = userRate[user][i]->rate;
		  
		  p_rate = predict_SVD_Occ(user,i,f);
		  error = rate - p_rate;
		  		  
		  double tmpQ = q_i[item*K+f];
		  // q_i = q_i + step_2*(e*p_u-lamda_7*q_i) 
		  q_i[item*K+f] += LRATE*(error*p_u[user*K+f]);
		  // q_i[item*K+f] += LRATE*(error*p_u[user*K+f]-LAMDA*q_i[item*K+f]);
		  // p_u = p_u + step_2*(e*q_i-lamda_7*p_u) 
		  p_u[user*K+f] += LRATE*(error*tmpQ);
		  // p_u[user*K+f] += LRATE*(error*tmpQ-LAMDA*p_u[user*K+f]);
		}
	    }
	  // }
	  // update total error
	  for(int user = 0;user<NU;user++)
	    for(int i=0;i<userRate[user].size();i++)
	      totalError+= abs(userRate[user][i]->rate-predict_SVD_Occ(user,i,f));
	  count++;
	  cout<<":"<<totalError<<"  ";
	  if(count == 1)
	    torrence=1;
	  else
	    torrence = abs(totalError-prevError)/prevError;
	  printf("\n                  Iter %d, Change: %f, Torrence: %f\n",count,torrence,TOR);	
	  // fflush(stdout);
	  prevError=totalError;
	}
      for(int user=0;user<NU;user++)
	for(int i=0;i<userRate[user].size();i++)
	  cache_contr(user,i,f);
    }
  cout<<endl;
}

// get the top N
void sortTopN(double *result,int *ids,int N,int start,int end)
{
  int sIdx,tmp;
  int i,j;
  double value;
  if(start>=end)
    return;
  if(N==0)
    return;
  srand((unsigned)time(0));
  sIdx=start+(int)((end-start)*(rand()/(float)RAND_MAX));
  value = result[ids[sIdx]];
  
  tmp = ids[end];
  ids[end] = ids[sIdx];
  ids[sIdx] = tmp;

  for(i=start,j=end-1;i<=j;)
    {
      if(result[ids[i]]<value)
	{
	  tmp=ids[i];
	  ids[i]=ids[j];
	  ids[j]=tmp;
	  j--;
	}
      else
	i++;
    }

  tmp=ids[j+1];
  ids[j+1]=ids[end];
  ids[end]=tmp;
  if(j-start+1>=N-1)
    sortTopN(result,ids,N,start,j);
  else
    {
      sortTopN(result,ids,j-start+1,start,j);      
      sortTopN(result,ids,N-(j-start+1)-1,j+2,end);
    }
}

void getTestSet(map<int,int> & us){
     ifstream in("../train/test.txt");
     int u;
     while(in >> u) us[u] = 0;
     in.close();
}


// recommending top N items for ever user
void recommend()
{
  double result[NI];
  int ids[NI];
  double defaultValue = 0.00001;
  time_t rawtime;
  struct tm* timeinfo;
  
  map<int,int> test;
  getTestSet(test);

  ofstream ofs(recommendation_file);
  for(int user=0;user<NU;user++)
    {
      if (test.find(user) == test.end())
	continue;

      time (&rawtime);
      timeinfo = localtime (&rawtime); 
      if(user%100 == 0)
	cout<<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
	    <<timeinfo->tm_hour<<":"<<timeinfo->tm_min<<":"<<timeinfo->tm_sec<<"  "
	    <<user<<"/"<<NU<<flush;
      memset(&result,0,sizeof(result));
      memset(&result,0,sizeof(ids));
      // predict value for user 
      for(int i = 0;i < userRate[user].size();i++)
	  result[userRate[user][i]->item] = defaultValue;

      result[0]=0;ids[0]=0;
      for(int item = 1;item < NI;item++)
	{
	  if (result[item]!=defaultValue)
	    result[item]=predict_SVD_Occ(user,item);
	  ids[item]=item;
	}
      // recommend top 10
      sortTopN(result,ids,N,0,NI-1);
      ofs<<user<<":";
      for(int i=0;i<N;i++)
      	if(i!=N-1)
      	  ofs<<ids[i]<<","<<result[ids[i]]/result[ids[0]]<<";";
      	else
      	  ofs<<ids[i]<<","<<result[ids[i]]/result[ids[0]];
      // for(int i=0;i<N;i++)
      // 	if(i!=N-1)
      // 	  ofs<<ids[i]<<",";
      // 	else
      // 	  ofs<<ids[i];
      ofs<<endl;      
    }
  ofs.close();
}


// check if the predict values fit the known values
void check()
{
  ofstream check(check_file);
  check<<"NU:"<<NU<<"  ";
  check<<"NI:"<<NI<<"  ";
  check<<"K:"<<K<<"  ";
  check<<"I:"<<I<<"  ";
  check<<"Lamda:"<<LAMDA<<"  ";
  check<<"Learning Rate:"<<LRATE<<"  ";
  check<<"torrence::"<<TOR<<endl;

  for(int user=0;user<NU;user++)
    for(int i=0;i<userRate[user].size();i++)
      {
	double rate = userRate[user][i]->rate;
	int item = userRate[user][i]->item;
	double p_rate = predict_SVD_Occ(user,item);
	check<<user<<"    "<<item<<"    "<<rate<<"    "<<p_rate
	     <<"    "<<abs(rate-p_rate)<<endl;
      }
  check.close();
}

void saveParams()
{
  // save params q
  ofstream q(q_file);
  q<<"Q_i,K="<<K<<endl;  
  for(int item =1;item < NI;item++)
    {
      q<<item<<":";
      for(int k=0;k<K;k++)
	q<<"    "<<q_i[item*K+k];
      q<<endl;
    }
  q.close();
  // save params p
  ofstream p(p_file);
  p<<"P_u,K="<<K<<endl;
  for(int user=0;user < NU;user++)
    {
      p<<user<<":";
      for(int k=0;k<K;k++)	
	p<<"    "<<p_u[user*K+k];
      p<<endl;
    }
  p.close();
}


void help()
{
  cout<<"NAME"<<endl;
  cout<<"    SVD_OCC -"<<"Recommending top N items for users based on SVD & occurence"<<endl;
  cout<<"SYNOPSIS"<<endl;
  cout<<"    SVD_OCC "<<"[-k factors] "<<"[-t torrence] "
      <<"[-n top N]"<<endl;
  cout<<"COMMANDS"<<endl;
  cout<<"    -k"<<endl;
  cout<<"        set the number of factors for user and item. "
      <<" Default value is 100"<<endl;
 cout<<"    -i"<<endl;
  cout<<"        set the max number of iteration "
      <<" Default value is 100"<<endl;
  cout<<"    -t"<<endl;
  cout<<"        set the torrence for param."
      <<" Default value is 0.0001"<<endl;
  cout<<"    -n"<<endl;
  cout<<"        set the number of items should be recommended."
      <<" Default value is 10"<<endl;
  cout<<"    -l"<<endl;
  cout<<"        set the learning rate of gradient."
      <<" Default value is 0.001"<<endl;

  exit(1);
}

void parse(int argc,char* argv[])
{
  if(argc == 1)
    return;
  if(argc%2 == 0)      
    help();
  for(int i=1;i<(argc-1);i++)
    {
      if(argv[i][0]!='-')
	help();
      switch(argv[i][1])
	{
	case 'k':
	  K = atoi(argv[i+1]);
	  i+=1;
	  break;
	case 'i':
	  I = atoi(argv[i+1]);
	  i+=1;
	  break;
	case 't':
	  TOR = atof(argv[i+1]);
	  i+=1;
	  break;
	case 'n':
	  N = atoi(argv[i+1]);
	  i+=1;
	  break;
	case 'l':
	  LRATE = atof(argv[i+1]);
	  i+=1;
	  break;
	default:
	  help();
	}
    }
}

int main(int argc, char* argv[])
{
  time_t rawtime,rawtime_2;
  struct tm* tinfo;

  // parse the command
  parse(argc,argv);

  // step 1 : train the model and save the params
  time (&rawtime);
  tinfo = localtime (&rawtime);   
  cout<<"Step 1:Train model! "<<tinfo->tm_hour<<":"<<tinfo->tm_min<<":"<<tinfo->tm_sec<<endl;
  init(train_file);
  SVD_Occ(); 
  // save the params
  saveParams();
  check();

  time(&rawtime_2);
  tinfo = localtime (&rawtime_2);   
  cout<<"Train Over! "<<tinfo->tm_hour<<":"<<tinfo->tm_min<<":"<<tinfo->tm_sec<<endl;
  cout<<"Train Duration: "<<rawtime_2-rawtime<<" seconds!"<<endl;

  // step 2: for every user, recommend top N 
  time (&rawtime);
  tinfo = localtime (&rawtime);   
  cout<<"Step 2: Recommending "<<tinfo->tm_hour<<":"<<tinfo->tm_min<<":"<<tinfo->tm_sec<<endl;

  recommend();

  time(&rawtime_2);
  tinfo = localtime (&rawtime_2);   
  cout<<"Recommending Over! "<<tinfo->tm_hour<<":"<<tinfo->tm_min<<":"<<tinfo->tm_sec<<endl;
  cout<<"Recommendating Duration: "<<rawtime_2-rawtime<<" seconds!"<<endl;

  // free the resource
  free();
  return 0;
}
