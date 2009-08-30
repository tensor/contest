#!/usr/bin/python
#coding:utf-8

from io import *

repos = readWatches("../data/repo_watched.txt")
N = 20


popular = {}

for repo,watches in repos.items():
    popular[repo] = len(watches)

tmp = popular.items()
result = sorted(tmp,reverse = True,key = lambda tmp:tmp[1])


for repo,count in result[:N]:
    print str(repo)+':'+str(count)
