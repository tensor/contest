#include "tensor.h"


vector <pair<int,int> > countCache[RMAXID+1];
vector <pair<int,float> > related[RMAXID+1];

vector<int> users[UMAXID+1];
vector<int> repos[RMAXID+1];

bool popFirst = false; 
int method = 0;
char neighbor[]= "USER";
int N = 500;


void clear()
{
  for(int i=0;i<=RMAXID;i++)    
    {
      countCache[i].clear();
      related[i].clear();
    }
}


void relatedUsers(int user,int method)
{
  if (user == 69)
    cout<<user<<endl;
  if (related[user].size()>0)
    return;
  
  hash_set<int> co_users;  
  int co_user,co_repo;

  for(int i=0;i<users[user].size();i++)
    {
      co_repo = users[user][i];
      for(int j=0;j<repos[co_repo].size();j++)
	{
	  co_user = repos[co_repo][j];
	  if (co_user == user)
	    continue;
	  co_users.insert(co_user);
	}
    }
  
  float maxSim = 0;
  hash_set<int>::iterator it;  
  for(it=co_users.begin();it!=co_users.end();it++)
    {

      float uSim = sim(user,*it,method,users,repos,countCache,neighbor);

      related[user].push_back(make_pair<int,float>(*it,uSim));
      maxSim = uSim > maxSim ? uSim:maxSim;
    }
  co_users.clear();
  // normalize, set the highest sim score as 1
  for(int i=0;i<related[user].size();i++)
    related[user][i].second /= maxSim;
}

void topN(int user,int N,int method,vector<pair<int,float> > &suggestRepos)
{
  hash_map<int,float> suggestions;
  relatedUsers(user,method);

  for(int i=0;i<related[user].size();i++)
    {
      int  co_user= related[user][i].first;      
      // reweight the weight of watchedRepo
      float sim = related[user][i].second;
      for(int j=0;j<users[co_user].size();j++)
	{
	  int repo = users[co_user][j];
	  if(list_find(users[user],repo)!=-1)
	    continue;			       
	  if (suggestions.find(repo) == suggestions.end())
	    suggestions[repo]=sim;
	  else
	    suggestions[repo] += sim;
	}
    }
  
  hash_map<int,float>::iterator it;
  for(it = suggestions.begin();it!=suggestions.end();it++)
    {
      suggestRepos.push_back(make_pair<int,float>(it->first,it->second));
    }
  suggestions.clear();
  sort(suggestRepos.begin(),suggestRepos.end(), GreaterSecond<int,float>);
}

void uknn(char *testName,int N,int method,char* dest)
{
  vector<int> testList;
  time_t rawtime;
  struct tm* timeinfo;
  vector<pair<int,float> > suggestions;

  read_test(testName,testList);
  
  ofstream out(dest);
  for(int i=0;i<testList.size();i++)
    {
      int user = testList[i];
      time (&rawtime);
      timeinfo = localtime (&rawtime); 
      printf("%d,%d,%d--%d,%d,%d\n",i,user,users[user].size()
	     ,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
      topN(user,N,method,suggestions);
      out<<user<<":";
      for(int j=0;j<suggestions.size() && j< N;j++)
	{
	  if(j==suggestions.size()-1 || j == N-1)	    
	    out<<suggestions[j].first<<","<<suggestions[j].second/suggestions[0].second;
	  else
	    out<<suggestions[j].first<<","<<suggestions[j].second/suggestions[0].second<<';';
	}
      suggestions.clear();
      out<<endl;
    }
  out.close();
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
	case 'n':
	  N = atoi(argv[i+1]);
	  i+=1;
	  break;
	default:
	  help();
	}
    }
}

int main(int argc, char* argv[])
{
  parse(argc,argv);

  char testFile[] = "../data/test.txt";
  char destFile[] = "../data/500_suggestions.user";
  char user_training[] = "../data/user_watches.txt";
  char repo_training[] = "../data/repo_watched.txt";
  
  // char testFile[] = "../data/removed_id.txt";
  // char destFile[] = "../data/500_suggestions.user";
  // char user_training[] = "../data/user_training.txt";
  // char repo_training[] = "../data/repo_training.txt";

  // popFirst = true;

  bool cacheExisted = readCountCache(testFile,neighbor,countCache);
  readWatches(user_training,users);
  readWatches(repo_training,repos);
  uknn(testFile,N,method,destFile);
  store_cache(testFile,neighbor,cacheExisted,countCache);
  clear();
}
