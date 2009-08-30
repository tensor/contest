#ifndef _TENSOR_H_
#define _TENSOE_H_

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <map>
#include <set>
#include <ext/hash_set>
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

#define UMAX 56555
#define RMAX 123345
#define TESTSIZE 4788


typedef vector< pair<int,int> > CacheList;


template < typename A, typename B >
bool GreaterSecond(const pair<A, B> & a, const pair<A,B> & b){
     return a.second > b.second;
}

string replace(const string & line, char ch1, char ch2){
     string ret;
     for(int i = 0; i < line.length(); ++i){
          if(line[i] == ch1) ret += ch2;
          else ret += line[i];
     }
     return ret;
}

void split(string src,char bar,vector<string>& parts)
{
  int start,end;
  start = 0;
  end = src.find(bar);
  while(end!=-1)
    {
      parts.push_back(src.substr(start,end-start));
      start = end+1;
      end = src.find(bar,start);
    }
  parts.push_back(src.substr(start,src.length()-start));  
}

bool readCountCache(char *testFile, char* neighbor, CacheList* countCache)
{
  string cacheName;
  cacheName.append(testFile).append(neighbor);
  ifstream in(cacheName.c_str());
  string buffer;
  int watchedRepo,coRepo,count;
  vector<string> parts;
  vector<string>::iterator it;
  char bar = ':';
  bool cacheExist = false;

  if(in == NULL)
    {
      cout<<neighbor<<"Count Cache file doesn't exist!"<<endl;
      cacheExist = false;
    }
  else
    cacheExist = true;

  while(in>>buffer)
    {
      split(buffer,bar,parts);
      watchedRepo=atoi(parts[0].c_str());
      coRepo = atoi(parts[1].c_str());
      count = atoi(parts[2].c_str());
      parts.clear();
      countCache[watchedRepo].push_back(make_pair<int,int>(coRepo,count));
    }
  in.close();

  return cacheExist;
}

void readWatches(char *fileName, vector<int> *watches)
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
      
      for(int i=0;i<parts.size();i++)
	watches[ID].push_back(atoi(parts[i].c_str()));
    }
  in.close();
}

void read_test(char* testName, vector<int> &testList)
{
  ifstream in(testName);
  int ID;
  while(in>>ID)
    testList.push_back(ID);
  in.close();
}

void store_cache(char* testFile, char* neighbor,bool cacheExist,CacheList* countCache)
{
  cout<<cacheExist<<endl;
  if(cacheExist)
    return;

  string cacheName;
  cacheName.append(testFile).append(neighbor);
  ofstream out(cacheName.c_str());
  for(int i=0;i<RMAX;i++)
    {
      if(countCache[i].size()==0)
	continue;
      for(int j=0;j<countCache[i].size();j++)
	out<<i<<":"<<countCache[i][j].first<<":"<<countCache[i][j].second<<endl;
    }
  
  out.close();      
}

int count_in_cache(int watched, int co,CacheList* countCache)
{
  for(int i=0;i<countCache[watched].size();i++)
    {
      if(countCache[watched][i].first == co)
	return countCache[watched][i].second;
    }
  return 0;
}

int ordered_list_intersec(vector<int>& l1,vector<int>& l2)
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


int co_occ(int watched,int co,vector<int>* users,vector<int>* repos,CacheList* countCache,char* neighbor)
{
  if (watched == co)
    {
      if(!strcmp(neighbor,"ITEM"))
	return repos[watched].size(); 
      else
	return users[watched].size();
    }

  int count = count_in_cache(watched,co,countCache);
  if (count >0)
    return count;
  

  if(!strcmp(neighbor,"ITEM"))
    count = ordered_list_intersec(repos[watched],repos[co]);
  else
    count = ordered_list_intersec(users[watched],users[co]);

  countCache[watched].push_back(make_pair<int,int>(co,count));
  return count;
}


float sim(int watched,int co,int method,vector<int>* users,vector<int>* repos,CacheList* countCache,char* neighbor)
{
  float count = (float)co_occ(watched,co,users,repos,countCache,neighbor);
  int watchedLength = 0;
  int coLength = 0;

  if(!strcmp(neighbor,"ITEM"))
    {
      watchedLength = repos[watched].size();
      coLength = repos[co].size();
    }
  else
    {
      watchedLength = users[watched].size();
      coLength = users[co].size();
    }

  if(method == 0)
    return count;
  else if( method == 1)
    return logf(count+1.0);
  else if(method == 2)
    return logf(count)+1;
  else if(method == 3)
    return count*(count/(watchedLength*coLength));
  else if(method == 4)
    return count/(watchedLength*coLength);
  else
    return 0.0;
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

double rand01(){
     return (double)(rand() % 1000000) / 1000000;
}
#endif




