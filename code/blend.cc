// #include "github.h"
#include "tensor.h"

map<int,int> test_data;
vector<int> watches[UMAX];

void loadTestData(){
     ifstream in("../train/removed_value.txt");
     string line;
     while(getline(in,line)){
          line = replace(line, ':', ' ');
          istringstream iss(line);
          int uu,ii;
          iss >> uu >> ii;
	  if(watches[uu].size() > 10)
	    test_data[uu] = ii;
     }
     in.close();
}

void loadRequire(set<int> &require,const string &requireFile)
{
  ifstream in(requireFile.c_str());
  string line;
  while(getline(in,line)){
    istringstream iss(line);
    int uu;
    iss >> uu;
    require.insert(uu);
  }
  in.close();
}

int score(vector< map<int,float> > & reco, int N){
     int ret = 0;
     for(map<int,int>::iterator k = test_data.begin(); k != test_data.end(); ++k){
          vector< pair<int,float> > ru(reco[k->first].begin(), reco[k->first].end());
          sort(ru.begin(), ru.end(), GreaterSecond<int,float>);
          for(int i = 0; i < ru.size() && i < N; ++i){
               if(ru[i].first == k->second){
                    ++ret;
		    // cout<<k->first<<endl;
                    break;
               }
          }
     }
     return ret;
}

void loadResults(char* file, vector< map<int,float> > & test,set<int> &require){
     test = vector< map<int,float> >(UMAX);
     string line;
     ifstream in(file);
     while(getline(in, line)){
       line =replace(line,':' , ' ');       
       line =replace(line,';' , ' ');              
       line =replace(line,',' , ' ');              
       istringstream iss(line);
       int user;
       iss >> user;
       if (require.size()!=0 && require.find(user)==require.end())
       	 continue;
       int item;
       float p;
       while(iss >> item >> p){
	 test[user][item] = p;
       }
     }
     int s = score(test,10);
     cout << file << "\t" << s << endl;
}

void output(const vector<map<int,float> > &aa, int N)
{
  ofstream out("result-train.txt");
  for(int i = 0; i < aa.size(); ++i)
    {
      if(aa[i].empty()) continue;
      vector< pair<int,float> > reco(aa[i].begin(), aa[i].end());
      sort(reco.begin(), reco.end(), GreaterSecond<int,float>);
      out << i << ":" << reco[0].first;
      for(int k = 1; k < reco.size() && k < N; ++k)
	out << "," << reco[k].first;
      out << endl;
    }
  out.close();     
}


int maxBlend(vector< vector< map<int,float> > > & tests, vector<float> & weight)
{
     vector< map<int,float> > aa(UMAX);
     for(int k = 0; k < tests.size(); ++k){
          for(int u = 0; u < tests[k].size(); ++u){
               if(tests[k][u].empty()) continue;
	       vector< pair<int,float> > reco(tests[k][u].begin(), tests[k][u].end());
	       sort(reco.begin(), reco.end(), GreaterSecond<int,float>);

	       for(int j=0;j<reco.size() && j<10;j++)
		 {
                    int item = reco[j].first;
                    if(aa[u].find(item) == aa[u].end()) aa[u][item] = 0;
                    aa[u][item] += (float)(reco[j].second) * weight[k];
               }
          }
     }          
     int count = score(aa,tests.size()*10);
     aa.clear();
     return count; 
}

int bag(vector< vector< map<int,float> > > & tests, vector<float> & weight,bool needOut,int N){
     vector< map<int,float> > aa(UMAX);
     for(int k = 0; k < tests.size(); ++k){
          for(int u = 0; u < tests[k].size(); ++u){
               if(tests[k][u].empty()) continue;
               for(map<int,float>::iterator j = tests[k][u].begin(); j != tests[k][u].end(); ++j){
                    int item = j->first;
                    if(aa[u].find(item) == aa[u].end()) aa[u][item] = 0;
                    aa[u][item] += (float)(j->second) * weight[k];
               }
          }
     }     
     
     if(needOut)
       output(aa,N);
     int count = score(aa,10);
     aa.clear();
     return count;
}

int main(int argc, char ** argv)
{

  string folder = argv[1];
  char* tmp = new char[folder.size()+100];
  strcpy(tmp,folder.c_str());

  int STEP = atoi(argv[2]);
  string outFile = argv[3];

  set<int> require;
  string requireFile;
  readWatches("../train/user_watches.txt", watches);


  loadTestData();

  if(argc == 5)
    {
      requireFile = argv[4];
      loadRequire(require,requireFile);
    }
#define K 14
  vector< vector< map<int,float> > > tests(K);
  vector< float > weight(K);
  int k = 0;
  srand((unsigned)time(0));

  loadResults(strcat(tmp,"/kmeans_repo.txt"), tests[k],require); weight[k] = 0.08; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/kmeans_user.txt"), tests[k],require); weight[k] = 0.11; ++k;strcpy(tmp,folder.c_str());

  loadResults(strcat(tmp,"/svd.txt"), tests[k],require); weight[k] = 0.05; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/apcluster_repo.txt"), tests[k],require); weight[k] = 0.15; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/apcluster_user.txt"), tests[k],require); weight[k] = 0.05; ++k;strcpy(tmp,folder.c_str());

  loadResults(strcat(tmp,"/results-knni-iuf2.txt"), tests[k],require); weight[k] = 0.52; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/results-knnu-iif.txt"), tests[k],require); weight[k] = 0.84; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/results-knnui.txt"), tests[k],require); weight[k] = 0.41; ++k;strcpy(tmp,folder.c_str());

  loadResults(strcat(tmp,"/sameName.txt"), tests[k],require); weight[k] = 0.4; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/author.txt"), tests[k],require); weight[k] = 0.49; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/children.txt"), tests[k],require); weight[k] = 0.4; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/language.txt"), tests[k],require); weight[k] = 0.2; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/parent.txt"), tests[k],require); weight[k] = 7; ++k;strcpy(tmp,folder.c_str());
  loadResults(strcat(tmp,"/g_parent.txt"), tests[k],require); weight[k] = 0.2; ++k;strcpy(tmp,folder.c_str());
 

  cout<<"Loarding results and init weights over!"<<endl;
  if (!strcmp(folder.c_str(),"../train"))
    {
      cout<<maxBlend(tests,weight)<<endl;
      cout<<bag(tests, weight,true,500)<<endl;      
      ofstream out(outFile.c_str(),ios_base::app);
      int j = 0;
      int maxScore = 0;
      for(int step = 0; step < STEP; ++step)
  	{
  	  int i = j % K;
  	  ++j;
  	  float w0 = weight[i];
	  if (w0==0)
	    continue;
  	  int r0 = bag(tests, weight,false,10);
	  if( step == 0)
	    maxScore = r0;
  	  weight[i] *= (1 + 0.5 * (rand01() - 0.5));
  	  int r1 = bag(tests, weight,false,10);
  	  if(r1 <= r0)	    
  	      weight[i] = w0;	    
  	  else
  	    maxScore = r1;
  	  cout << step << ":" << r0 << "\t" << r1 <<"\t"<<weight[i]<< endl;
  	}
      out<<maxScore<<":";
      for(int i = 0; i < weight.size(); ++i) out << weight[i] << "\t";
      out<<endl;
      out.close();
    }
  else if(!strcmp(folder.c_str(),"../result"))
    {
      int r0 = bag(tests, weight,true,10);
      cout<<r0<<endl;      
    }
  else
    {
      cout<<"No such folder:"<<folder<<endl;
      exit(1);
    }
  delete[] tmp;
  cout<<"Blend over"<<endl;
}

