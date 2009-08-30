#ifndef _CLUSTER_H_
#define _CLUSTER_H_

#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "tensor.h"

using namespace std;

#define REPO 0
#define USER 1
#define UMAX 56555
#define RMAX 123345


class Item
{
 public:
 Item():ID(-1),cluster(-1)
    {
    }
 Item(int ID):ID(ID),cluster(-1)
    {
    }

  int ID;
  set<int> watches;
  int cluster;
  
  int size() const
  {
    return watches.size();
  }
};

typedef vector<Item> ItemList;
typedef vector< map<int,float> > SimList;

class Cluster
{
 public:
  map<int,float> centroid;
  vector<int> members;  
  void addMember(const Item &item);  
  float sim(const Item &item, int nu[]);
  void norm();
  void fill(float value);
};

class Data
{
 public:
  Data()
    {
      users.resize(UMAX);
      repos.resize(RMAX);
      userSim.resize(UMAX);
      repoSim.resize(RMAX);      
      nu = new int[UMAX];
      ni = new int[RMAX];
    }
  ~Data()
    {
      delete[] nu;
      delete[] ni;
    }
  ItemList users;
  ItemList repos;
  SimList userSim;
  SimList repoSim;  

  int* nu;
  int* ni;

  int existUser;
  int existRepo;

  void loadSim(char* fileName, int dType);
  void loadSim(char* fileName, SimList &sims);

  void loadWatches(char* fileName, int dType);
  void loadWatches(char* fileName, ItemList &items,int ns[]);

};

class ClusterSuggester
{
 private:
  int dType;

 public:
 ClusterSuggester(Data &data, int dType):data(data),dType(dType)
    {
      if(dType == REPO)
	in_cluster = new int[RMAX];
      else
	in_cluster = new int[UMAX];
    }
  ~ClusterSuggester()
    {
      delete[] in_cluster;
    }
  const Data &data;
  int* in_cluster;
  map<int,vector<int> > clusters;

  void loadCluster(char* fileName);
  void suggest(char* destFile,vector<int> &testList, int N=500);
};


void Cluster::addMember(const Item &item)
{
  set<int>::iterator it;
  for(it = item.watches.begin();it!=item.watches.end();++it)
    {
      int mId = *it;
      if(centroid.find(mId) == centroid.end())
	centroid[mId]=0;
      centroid[mId]+=1;
    }
  cout<<centroid.size()<<endl;
}

void Cluster::norm()
{
  map<int,float>::iterator mIt;
  int k = 1.0/members.size();

  for(mIt = centroid.begin();mIt != centroid.end();mIt++)
    mIt->second *= k;
}

float Cluster::sim(const Item &item,int nu[])
{
  map<int,float>::iterator mIt;
  set<int>::iterator sIt;
  float similarity = 0.0;
  cout<<centroid.size() <<"|"<<item.watches.size()<<endl;
  for(mIt = centroid.begin(),sIt = item.watches.begin();mIt!=centroid.end()&&sIt!=item.watches.end();)
    {     
      
      if(mIt->first == *sIt)
	{
	  similarity += mIt->second/log(3+nu[mIt->first]);
	  mIt++;
	  sIt++;
	}
      else if(mIt->first < *sIt)
	mIt++;
      else
	sIt++;
    }
  return similarity;
}
		  
void Cluster::fill(float value)
{
  map<int,float>::iterator it;
  for(it = centroid.begin();it!=centroid.end(); ++it)
      it->second = value;
}  
void Data::loadSim(char* fileName, int dType)
{
  if(dType == REPO)
    loadSim(fileName,repoSim);
  else
    loadSim(fileName,userSim);
}

void Data::loadSim(char* fileName, SimList &sims)
{
  ifstream in(fileName);
  int a, b;
  float s;
  while(in >> a >> b >> s)
    sims[a][b]=s;
  /* for(int i = 0; i < sims.size(); ++i){ */
  /*   sort(sims[i].begin(), sims[i].end(), GreaterSecond<int,float>); */
  /* } */
  cout << "load sim finished!" << endl;
}

void Data::loadWatches(char* fileName, int dType)
{
  if(dType == REPO)
    loadWatches(fileName,repos,ni);
  else
    loadWatches(fileName,users,nu);
}

void Data::loadWatches(char* fileName, ItemList &items, int ns[])
{

  ifstream in(fileName);
  string buffer,idList;
  int ID;
  while(in>>buffer)
    {
      vector<string> parts;
      split(buffer,':',parts);
      
      ID = atoi(parts[0].c_str());
      idList = parts[1];
      
      parts.clear();
      split(idList,',',parts);
      
      Item item(ID);

      for(int j=0 ; j<parts.size();j++)
	item.watches.insert(atoi(parts[j].c_str()));
      items[ID] = item;
      ns[ID] = item.watches.size();
    }
  in.close();	  
}


void ClusterSuggester::loadCluster(char* fileName)
{
  ifstream in(fileName);
  int cId;
  int line=0;
  while(in>>cId)
    {
      int ID = line+1;
      in_cluster[ID]=cId;
      clusters[cId].push_back(ID);
      line++;
    }
  in.close();
}

void ClusterSuggester::suggest(char* destFile,vector<int> &testList,int N)
{
  ofstream out(destFile);

  for(int i=0;i<testList.size();i++)
    {
      int user = testList[i];
      

      const set<int> &alreadyWatched = data.users[user].watches;
      map<int,float> cache;
      /* add by count only, not multipy sim */
      
      set<int>::iterator pos;
      
      if (dType == REPO)
	{
	  for(pos = alreadyWatched.begin();pos!=alreadyWatched.end();++pos)
	    {
	      int repo = *pos;
	      int cluster = in_cluster[repo];
	      for(int j=0;j<clusters[cluster].size();++j)
		{
		  int item = clusters[cluster][j];
		  if(alreadyWatched.find(item)!=alreadyWatched.end())
		    continue;
		  if(data.repos[item].size() == 0)
		    continue;	      
		  if(cache.find(item)==cache.end())
		    cache[item] = 0;
		  /* order by count */
		  /* cache[item] += 1; */
 		  /* order by popularity */
		  /* cache[item] += data.ni[item]; */
		  /* weight by sim */
		  const map<int,float> &sims =data.repoSim[repo];
		  map<int,float>::const_iterator mIt = sims.find(item);
		  if(mIt!= sims.end())
		    cache[item] += mIt->second;
		}
	    }
	}
      else
	{
	  int cluster = in_cluster[user];
	  for(int j=0;j<clusters[cluster].size();++j)
	    {
	      int item = clusters[cluster][j];
	      if(data.repos[item].size() == 0)
		continue;	      
	      if(item == user)
		continue;
	      if(cache.find(item)==cache.end())
		cache[item] = 0;
	      /* cache[item] += data.nu[item]; */
	      /* ordered by user sim */
	      const map<int,float> &sims =data.userSim[user];
	      map<int,float>::const_iterator mIt = sims.find(item);
	      if(mIt!= sims.end())
	      	cache[item] += mIt->second;
	      /* orderby by inverse user frequence */
	      /* cache[item] += 1/log(3+data.users[item].size()); */
	    }	  
	}
      
      vector<pair<int,float> > suggestions;
      
      if(dType == REPO)	
	suggestions.assign(cache.begin(),cache.end());       
      else
	{
	  map<int,float> tmp;
	  map<int,float>::iterator it = cache.begin();
	  for(;it!=cache.end();++it)
	    {
	      float weight = it->second;
	      const set<int> &watches = data.users[it->first].watches;
	      pos = watches.begin();
	      for(;pos!=watches.end();++pos)
		{
		  int repo = *pos;
		  if(alreadyWatched.find(repo)!=alreadyWatched.end())
		    continue;
		  if(data.repos[repo].size() == 0)
		    continue;
		  if(tmp.find(repo) == tmp.end())
		    tmp[repo] =0;
		  /* ordered by count */
		  tmp[repo] += weight;
		}
	    }
	  suggestions.assign(tmp.begin(),tmp.end());	  	  
	}
      
      sort(suggestions.begin(),suggestions.end(),GreaterSecond<int,float>);
      if (suggestions[0].second == 0)
	{
	  for(int i=0;i<suggestions.size();i++)	  
	    suggestions[i].second = data.ni[suggestions[i].first];
	  sort(suggestions.begin(),suggestions.end(),GreaterSecond<int,float>);	      
	}
      out << user << ":";
      for(int i=0;i<suggestions.size() && i<N;i++)
      	out<<suggestions[i].first<<','<<suggestions[i].second/suggestions[0].second<<';';
      out<<endl;
    }
  out.close();
}
#endif
