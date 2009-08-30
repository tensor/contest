#!/usr/bin/python
#coding:utf-8

from io import *
import sys,time
from math import log

cache = {}

def loadCache(testFile):
    f = open(testFile+"_cache")
    for line in f.readlines():
	parts = line.strip().split(':')

	watched = int(parts[0])
	co = int(parts[1])
	count = int(parts[2])

	if cache.has_key(watched) is False:
	    cache[watched]={}
	cache[watched][co]=count	    	
    f.close()

def ordered_list_intersec(l1,l2):
    i = 0
    j = 0
    intersec = 0
    while i < len(l1) and j < len(l2):
	if l1[i] == l2[j]:
	    intersec +=1
	    i+=1
	    j+=1
	elif l1[i] > l2[j]:
	    j += 1
	else:
	    i += 1
    return intersec


def co_occ(watched_repo_users,watched,co_repo_users,co):
    if watched == co:
	return len(watched_repo_users)
    if cache.has_key(watched):
	if cache[watched].has_key(co):
	    return cache[watched][co]

    result = ordered_list_intersec(watched_repo_users,co_repo_users)
    if cache.has_key(watched) is False:
	cache[watched] = {}
    cache[watched][co]=result
    return result

def repo_sim(watched_repo_users,watched,co_repo_users,co,method = 0):    
    count = co_occ(watched_repo_users,watched,co_repo_users,co)
    if method == 0:
	return count
    elif method == 1:
	return log(count+1)
    elif method == 2:
	return log(count)+1
    else:
	return 0.0



def testCoOcc(testFile,userTraining,repoTraining):
    users = readWatches(userTraining)
    repos = readWatches(repoTraining)

    test  = read_test(testFile)
    for i,user in enumerate(test):
	print i,user,len(users[user]),time.localtime()
	for repo in users[user]:
	    co_repos = set()    
	    for co_user in repos[repo]:
		for co_repo in users[co_user]:
		    if co_repo == repo:
			continue
		    co_repos.add(co_repo)
	    print repo,len(co_repos)
	    for co_repo in co_repos:
		co_occ(repos[repo],repo,repos[co_repo],co_repo)

    
def store(argv):
    testCoOcc(argv[0],argv[1],argv[2])    

if __name__ == '__main__':
    print 'testfile user_traing repo_traing'
    store(sys.argv[1:])    
    write_co_occ(sys.argv[1]+"_cache",cache,'repo')
    
