#!/usr/bin/python
#coding:utf-8
from authors import *
from io import *


def cut_for_blend(folder):
    userList = []
    f = open(folder+"/pure_knn.list")
    for line in f.readlines():
	userList.append(int(line.strip()))
    f.close()

    userWatches = readWatches(folder+"/user_watches.txt")
    repoInfo = parse_repos()

    w0 = open(folder+"/pure_knn.list_0",'w')
    w1 = open(folder+"/pure_knn.list_1",'w')
    w2 = open(folder+"/pure_knn.list_2",'w')

    for user in userList:
	watchesNum = len(userWatches[user])    
	names = set()
	for repo in userWatches[user]:
	    names.add(repoInfo[repo][0])
	
	sameAuthorCount = watchesNum - len(names)
	if sameAuthorCount == 0:
	    w0.write("%d\n"%(user))
	elif sameAuthorCount == 1:
	    w1.write("%d\n"%(user))
	else:
	    w2.write("%d\n"%(user))

    w0.close()
    w1.close()
    w2.close()

def clustersFormatter(fileName):
    f = open(fileName)
    w = open(fileName+"_f",'w')
    for i,line in enumerate(f.readlines()):
	if i == 0:
	    continue
	ID,cluster = line.strip().split(":")
	w.write(cluster+"\n")
    w.close()
    f.close()

def analyzer(userList,userWatches):
    repoInfo = parse_repos()
    totalWatchesNum = 0.0
    totalSameAuthorCount = 0.0
    sameLine = 0.0
    smallLine = 0.0
    for user in userList:
	watchesNum = len(userWatches[user])
	totalWatchesNum += watchesNum
	

	names = set()
	for repo in userWatches[user]:
	    names.add(repoInfo[repo][0])

	sameAuthorCount = watchesNum - len(names)
	totalSameAuthorCount += sameAuthorCount
	
	if sameAuthorCount > 1:
	    sameLine +=1
	
	if watchesNum <= 10:
	    smallLine += 1
	
	print user,watchesNum,sameAuthorCount

    print 'Summary:',totalWatchesNum/len(userList),totalSameAuthorCount/len(userList),sameLine/len(userList),smallLine/len(userList)

if __name__ == '__main__':
    clustersFormatter("../train/users/kmeans.txt")
