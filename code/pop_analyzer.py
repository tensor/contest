#!/usr/bin/python
#coding:utf-8

from io import *
bars = [10,8,6,4,1]

def analyzer(userList,userWatches):
    pops ={}
    for bar in bars:
	pops[bar]= 0

    for user_id in userList:
	try:
	    length = len(userWatches[user_id])
	except:
	    length = 0
	    print user_id
	if length > bars[0]:
	    pops[bars[0]]+=1
	elif length > bars[1]:
	    pops[bars[1]]+=1
	elif length > bars[2]:
	    pops[bars[2]]+=1
	elif length > bars[3]:
	    pops[bars[3]]+=1
	else:
	    pops[bars[4]]+=1

    for i in range(len(bars)):
	print bars[i],float(pops[bars[i]])/len(userList)
 	    
def countZero(resultFile):
    f = open(resultFile)
    count =0
    for line in f.readlines():
	key,value = line.split(':')
	if len(value.strip()) != 0:
	    count+=1
    print count
    f.close()


# userWatches = readWatches("../data/user_watches.txt")
# userTrain = readWatches("../data/user_traing.txt")
# removed = readWatches("../data/removed_value.txt")
# test  = read_test()
# analyzer(userWatches.keys(),userWatches)
# analyzer(removed.keys(),userTrain)
# analyzer(test,userWatches)

# countZero("../data/all_unwatched_sources.txt")
import sys

countZero(sys.argv[1])
