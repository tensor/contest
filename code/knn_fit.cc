#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdio>
#include <ext/hash_set>
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

#define UMAXID 56554
#define RMAXID 123344
#define TESTSIZE 4788
#define MIN((a),(b)) (a)<(b)?(a):(b)

typedef struct
{
  int repo;
  int count;
}CountRecord;

typedef struct
{
  int repo;
  float sim;
}SuggestEntry;



typedef CountRecord* CountRecordPtr;
typedef SuggestEntry* SuggestEntryPtr;
typedef hash_map<int,float> RepoWeight;
typedef vector<SuggestEntryPtr> SuggestList;
typedef hash_map<int,float> HashSuggestList;
typedef hash_map<int,RepoWeight*> HashUserRepoWeight;

vector<CountRecordPtr> countCache[RMAXID+1];
SuggestList related[RMAXID+1];
vector<int> users[UMAXID+1];
vector<int> repos[RMAXID+1];
HashUserRepoWeight user_repo;


bool popFirst = false; 
bool cacheExist = true;
string NEIGHBOR = "ITEM";
int method = 0;
int neighborType = 0;

void split(string src,char bar,vector<string> *parts)
{
  int start,end;
  char* part;
  start = 0;
  end = src.find(bar);
  while(end!=-1)
    {
      parts->push_back(src.substr(start,end-start));
      start = end+1;
      end = src.find(bar,start);
    }
  parts->push_back(src.substr(start,src.length()-start));  
}

void readCountCache(char *testFile)
{
  string cacheName;
  cacheName.append(testFile).append(NEIGHBOR);
  ifstream in(cacheName.c_str());
  string buffer;
  int watchedRepo,coRepo,count;
  vector<string> parts;
  vector<string>::iterator it;
  char bar = ':';
  if(in == NULL)
    {
      cout<<NEIGHBOR<<"Count Cache file doesn't exist!"<<endl;
      cacheExist = false;
    }
  while(in>>buffer)
    {
      split(buffer,bar,&parts);
      watchedRepo=atoi(parts[0].c_str());
      coRepo = atoi(parts[1].c_str());
      count = atoi(parts[2].c_str());
      parts.clear();
      CountRecordPtr record = (CountRecordPtr)malloc(sizeof(CountRecord));
      record->count = count;
      record->repo = coRepo;
      countCache[watchedRepo].push_back(record);
    }
  in.close();
}

void readWatches(char *fileName, vector<int>* watches)
{
  ifstream in(fileName);
  string buffer,idList;
  int ID;
  while(in>>buffer)
    {
      vector<string> parts;
      split(buffer,':',&parts);
      
      ID = atoi(parts[0].c_str());
      idList = parts[1];
      
      parts.clear();
      split(idList,',',&parts);
      
      for(int i=0;i<parts.size();i++)
	watches[ID].push_back(atoi(parts[i].c_str()));
    }
  in.close();
}

void read_test(char* testName, vector<int> *testList)
{
  ifstream in(testName);
  int ID;
  while(in>>ID)
    testList->push_back(ID);
  in.close();
}

void store_cache(char* testFile)
{
  if(cacheExist)
    return;

  string cacheName;
  cacheName.append(testFile).append(NEIGHBOR);
  ofstream out(cacheName.c_str());
  for(int i=0;i<=RMAXID;i++)
    {
      if(countCache[i].size()==0)
	continue;
      for(int j=0;j<countCache[i].size();j++)
	out<<i<<":"<<countCache[i][j]->repo<<":"<<countCache[i][j]->count<<endl;
    }
  
  out.close();      
}

void clear()
{
  for(int i=0;i<=RMAXID;i++)    
    {
      for(int j=0;j<countCache[i].size();j++)
	free(countCache[i][j]);
      for(int j=0;j<related[i].size();j++)
	free(related[i][j]);
      countCache[i].clear();
      related[i].clear();
    } 
}


int count_in_cache(int watched, int co)
{
  for(int i=0;i<countCache[watched].size();i++)
    {
      if(countCache[watched][i]->repo == co)
	return countCache[watched][i]->count;
    }
  return 0;
}

int ordered_list_intersec(vector<int> l1,vector<int> l2)
{
  int i = 0;
  int j = 0;
  int intersec = 0;
  while (i < l1.size() && j < l2.size())
    {
      if (l1[i] == l2[j])
	{
	  intersec +=1;
	  i+=1;
	  j+=1;
	}
      else if(l1[i] > l2[j])	
	j += 1;
      else
	i += 1;
    }
  return intersec;
}

int list_find(vector<int> l, int ID)
{
  for(int i=0;i<l.size();i++)
    {
      if(l[i]==ID)
	return i;
    }
  return -1;
}

int co_occ(int watched,int co)
{
  if (watched == co)
    return users[watched].size(); 

  int count = count_in_cache(watched,co);
  if (count >0)
    return count;
  
  count = ordered_list_intersec(repos[watched],repos[co]);
  
  CountRecordPtr record = (CountRecordPtr)malloc(sizeof(record));
  record->repo = co;
  record->count = count;  
  countCache[watched].push_back(record);
  return count;
}

float repo_sim(int watched,int co,int method)
{
  float count = (float)co_occ(watched,co);
  if(method == 0)
    return count;
  else if( method == 1)
    return logf(count+1.0);
  else if(method == 2)
    return logf(count)+1;
  else if(method == 3)
    return count*(count/(repos[watched].size()*repos[co].size()));
  else if(method == 4)
    return count/(repos[watched].size()*repos[co].size());
  else
    return 0.0;
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

void relatedRepos(int repo,int method)
{
  if (related[repo].size()>0)
    return;
  
  hash_set<int> co_repos;  
  int co_user,co_repo;

  for(int i=0;i<repos[repo].size();i++)
    {
      co_user = repos[repo][i];
      for(int j=0;j<users[co_user].size();j++)
	{
	  co_repo = users[co_user][j];
	  if (co_repo == repo)
	    continue;
	  co_repos.insert(co_repo);
	}
    }
  
  float maxSim = 0;
  hash_set<int>::iterator it;  
  for(it=co_repos.begin();it!=co_repos.end();it++)
    {
      SuggestEntryPtr entry = (SuggestEntryPtr)malloc(sizeof(SuggestEntry));
      entry->repo = *it;
      entry->sim = repo_sim(repo,*it,method);
      related[repo].push_back(entry);
      maxSim = entry->sim > maxSim ? entry->sim:maxSim;
    }  
  co_repos.clear();
  // normalize, set the highest sim score as 1
  for(int i=0;i<related[repo].size();i++)
    related[repo][i]->sim /= maxSim;
}


void rank(double* result,int* ids,int* repoList,HashSuggestList suggestRepos,int N)
{
  HashSuggestList::iterator it;
  int idx = 0;
  for(it=suggestRepos.begin();it!=suggestRepos.end();it++)
    {
      ids[idx]=idx;
      repoList[idx]=it->first;
      // the order decided by popularity also
      double popularity = 0;
      if (popFirst)
		(repos[it->first].size())/double(UMAXID);
      // double popularity = 0;
      result[idx]=double(it->second)+popularity;
      idx++;
    }
  sortTopN(result,ids,N,0,suggestRepos.size()-1);
}


void update(HashSuggestList* suggestRepos,int targetRepo,float origin_weight,float new_weight);
{
  for(int j=0;j<related[targetRepo].size();j++)
    {
      int repo = related[targetRepo][j]->repo;
      float sim = (related[targetRepo][j]->sim)*(origin_weight-new_weight);
      suggestRepos[repo] -= sim;
    }             	    
}

// fit the weight of watched repo in suggest progress
void fitWeight(int user)
{
  // init , setall watching weight of user as 1.0
  RepoWeight* repo_weight = new RepoWeight();
  for(int i=0;i<users[user].size();i++)
    repo_weight[users[user][i]] = 1.0;
  user_repo[user]=repo_weight;

  // adjust the users who watches less than 4 only
  if (users[user].size()>4)
    return;


  HashSuggestList suggestRepos;
  HashSuggestList::iterator it;

  for(int i=0;i<users[user].size();i++)
    {
      int watchedRepo = users[user][i];
      for(int j=0;j<related[watchedRepo].size();j++)
	{
	  int repo = related[watchedRepo][j]->repo;
	  float sim = (related[watchedRepo][j]->sim)*repo_weight[watchedRepo];
	  it = suggestRepos.find(repo);
	  if (it == suggestRepos.end())
	    suggestRepos[repo]=sim;
	  else
	    suggestRepos[repo] += sim;
	}             
    }

  // the candidate is less than 10 already, no need to reorder
  if(suggestRepos.size() - users[user].size() < 10)
    return;

  float diff = 1000000000;
  float delta = 100000;
  float learning_rate = 0.1;
  while(delta >= 0)
    {
      float currentDiff = 0;
      for(int i=0;i<users[user].size();i++)
	{
	  // predict the order of watching repo, adjust the weight if not rand properly
	  // return the diff of order between 10th rank
	  int targetRepo = users[user][i];
	  // check if the target item is in the candidate set
	  it = suggestRepos.find(targetRepo);
	  if (it == suggestRepos.end())
	    continue;
	  // remove target item's related, assume it's unknown
	  for(int j=0;j<related[targetRepo].size();j++)
	    {
	      int repo = related[targetRepo][j]->repo;
	      float sim = (related[targetRepo][j]->sim)*repo_weight[targetRepo];
	      suggestRepos[repo] -=sim;
	    }             	  
	  // rank the 10 high,
	  double* result = new double[suggestRepos.size()];
	  int* ids = new int[suggestRepos.size()];
	  int* repoList = new int[suggestRepos.size()];
	  rank(result,ids,repoList,suggestRepos,10);
	  int 10th = MIN(suggestRepos.size(),10);
	  float 10Value = result[10th];	  
	  delete[] result;delete[] ids;delete[] repoList;	  	  

	  //  update the diff
	  if (suggestRepos[repo] < 10Value)
	    {
	      float tmp = 10Value-suggestRepos[repo];	      	    
	      currentDiff +=tmp;	      	      
	      for(int i=0;i<users[user].size();i++)
		{
		  int tmpRepo = users[user][i];
		  if(list_find(related[tmpRepo],repo)!=-1)
		    {
		      update(suggestRepos,tmpRepo,repo_weight[tmpRepo],repo_weight[tmpRepo]+learning_rate*tmp);
		      repo_weight[tmpRepo] += learning_rate*tmp;
		    }

		}

	    }
	  // restore back
	  for(int j=0;j<related[targetRepo].size();j++)
	    {
	      int repo = related[targetRepo][j]->repo;
	      float sim = (related[targetRepo][j]->sim)*repo_weight[targetRepo];
	      suggestRepos[repo] -= sim;
	    }             	  
	}
      // update diff and delta
      delta = diff - currentDiff;
      diff = currentDiff;
    }
  user_repo[user]=repo_weight;
}

void topN(int user,int N,int method,vector<int> *suggestions)
{
  HashSuggestList suggestRepos;
  HashSuggestList::iterator it;
  for(int i=0;i<users[user].size();i++)    
      relatedRepos(users[user][i],method);

  fitWeight(user);

  for(int i=0;i<users[user].size();i++)
    {
      int watchedRepo = users[user][i];
      for(int j=0;j<related[watchedRepo].size();j++)
	{
	  int repo = related[watchedRepo][j]->repo;
	  float sim = related[watchedRepo][j]->sim;
	  if(list_find(users[user],repo)!=-1)
	    continue;			       
	  it = suggestRepos.find(repo);
	  if (it == suggestRepos.end())
	    suggestRepos[repo]=sim;
	  else
	    suggestRepos[repo] += sim;
	}             
    }
  
  double* result = new double[suggestRepos.size()];
  int* ids = new int[suggestRepos.size()];
  int* repoList = new int[suggestRepos.size()];
  rank(result,ids,repoList,suggestRepos,N);
  for(int i=0;i<N&&i<suggestRepos.size();i++)
    suggestions->push_back(repoList[ids[i]]);

  delete[] result;delete[] ids;delete[] repoList;
}

void knn(char *testName,int N,int method,char* dest)
{
  vector<int> testList;
  time_t rawtime;
  struct tm* timeinfo;
  vector<int> suggestions;

  read_test(testName,&testList);
  
  ofstream out(dest);
  for(int i=0;i<testList.size();i++)
    {
      int user = testList[i];
      time (&rawtime);
      timeinfo = localtime (&rawtime); 
      printf("%d,%d,%d--%d,%d,%d\n",i,user,users[user].size()
	     ,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
      topN(user,N,method,&suggestions);
      out<<user<<":";
      for(int j=0;j<suggestions.size();j++)
	{
	  if(j==suggestions.size()-1)	    
	    out<<suggestions[j];
	  else
	    out<<suggestions[j]<<',';
	}
      suggestions.clear();
      out<<endl;
    }
  out.close();
}

void help()
{
  cout<<"NAME"<<endl;
  cout<<"    KNN -"<<"Recommending top N items for users based on KNN"<<endl;
  cout<<"SYNOPSIS"<<endl;
  cout<<"    KNN "<<"[-m method] "
      // <<"[-t torrence] "
      // <<"[-n top N]"
      <<endl;
  cout<<"COMMANDS"<<endl;
  cout<<"    -m"<<endl;
  cout<<"        method id "
      <<" Default value is 0"<<endl;
 // cout<<"    -i"<<endl;
 //  cout<<"        set the max number of iteration "
 //      <<" Default value is 100"<<endl;
 //  cout<<"    -t"<<endl;
 //  cout<<"        set the torrence for param."
 //      <<" Default value is 0.0001"<<endl;
 //  cout<<"    -n"<<endl;
 //  cout<<"        set the number of items should be recommended."
 //      <<" Default value is 10"<<endl;
 //  cout<<"    -l"<<endl;
 //  cout<<"        set the learning rate of gradient."
 //      <<" Default value is 0.001"<<endl;

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
	case 'm':
	  method = atoi(argv[i+1]);
	  i+=1;
	  break;
	// case 'i':
	//   I = atoi(argv[i+1]);
	//   i+=1;
	//   break;
	// case 't':
	//   TOR = atof(argv[i+1]);
	//   i+=1;
	//   break;
	// case 'n':
	//   N = atoi(argv[i+1]);
	//   i+=1;
	//   break;
	// case 'l':
	//   LRATE = atof(argv[i+1]);
	//   i+=1;
	//   break;
	default:
	  help();
	}
    }
}

int main(int argc, char* argv[])
{
  char testFile[] = "../data/removed_id.txt";
  char destFile[] = "../data/removed_test.txt";
  char user_training[] = "../data/user_training.txt";
  char repo_training[] = "../data/repo_training.txt";

  // popFirst = true;
  parse(argc,argv);

  readCountCache(testFile);
  readWatches(user_training,users);
  readWatches(repo_training,repos);
  knn(testFile,20,method,destFile);
  store_cache(testFile);
  clear();
}
