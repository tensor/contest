#!/usr/bin/python
#coding:utf-8
import sys
from io import *

if __name__ == '__main__':
    print "./score.py rec.txt"
    removed_value = readWatches("../data/removed_value.txt")
    rec = readWatches(sys.argv[1])
    
    score = 0.0
    for user_id,repo_id in removed_value.items():
	if repo_id[0] in rec[user_id][:10]:
	    score +=1
	else:
	    print user_id,repo_id,rec[user_id]
    print score,score/len(removed_value)

	

    
