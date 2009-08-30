#!/usr/bin/python
#coding:utf-8

from io import *
from sim import *
from math import log
import sys
import time
import pdb

users = {}
repos = {}
related={}

def cmp_repos(r1,r2):
    return cmp(r1[1],r2[1])


def relatedRepos(repo,method):
    if related.has_key(repo):
	return related[repo]

    co_repos = set()    
    for co_user in repos[repo]:
	for co_repo in users[co_user]:
	    if co_repo == repo:
		continue
	    co_repos.add(co_repo)

    neighRepos={}
    for co_repo in co_repos:
	neighRepos[co_repo]=repo_sim(repos[repo],repo,repos[co_repo],co_repo,method)
    related[repo]=neighRepos
    return neighRepos
    
    
def topN(user,N,method):
    suggestRepos = {}
    for watchedRepo in users[user]:
	tmp = relatedRepos(watchedRepo,method)
	for repo,sim in tmp.items():
	    if repo in users[user]:
		continue
	    if suggestRepos.has_key(repo):
		suggestRepos[repo]+=sim
	    else:
		suggestRepos[repo]=sim

    tmp = suggestRepos.items()
    tmp.sort(reverse = True, cmp = cmp_repos)
    return [x[0] for x in tmp[:N]]

    
def knn(test,N,method,dest):
    test  = read_test(test)
    f = open(dest,'w')
    # pdb.set_trace()
    for i,user in enumerate(test):
	print i,user,len(users[user]),time.localtime()
	suggest = topN(user,N,method)
	f.write(str(user)+":"+','.join([str(x) for x in suggest])+"\n")
    f.close()


if __name__ == '__main__':
    print './knn.py test.txt 10 0 result.txt'
    testFile = sys.argv[1]
    N = int(sys.argv[2])
    method = int(sys.argv[3])
    dest = sys.argv[4]
    print time.localtime()
    loadCache(testFile)
    users = readWatches('../data/user_training.txt')
    repos = readWatches('../data/repo_training.txt')
    knn(testFile,N,method,dest)
    print time.localtime()
