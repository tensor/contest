#!/usr/bin/python
#coding:utf-8
import sys,os,pdb
from io import *
from math import sqrt

folder = "../result"
    

users = {}
ulangs = {}
repo_langs = {}
repo_set = set()

def vec_sim(A,B):
    if len(A) == 0 or len(B) == 0:
	return 0;
    co = 0
    powA = 0
    powB = 0
    
    for key,value in A.items():
	powA += value*value
	if key in B:
	    co += value*B[key]
    
    for key,value in B.items():
	powB += value*value

    return co/sqrt(powA*powB)

def suggest(user):
    suggestion = []
    if user not in users or user not in ulangs:
	return suggestion

    for repo in repo_set:
	if repo in users[user]:
	    continue
	if repo not in repo_langs:
	    continue
	sim = vec_sim(ulangs[user],repo_langs[repo])
	if sim > 0:
	    suggestion.append((repo,sim))
    
    suggestion = sorted(suggestion,reverse = True,key = lambda suggestion:suggestion[1])
    return suggestion


def build():

    for user,repos in users.items():
	ulangs[user] = {}
	for repo in repos:
	    repo_set.add(repo)
	    if repo not in repo_langs:
		continue
	    for lang in repo_langs[repo].keys():
		if lang not in ulangs[user]:
		    ulangs[user][lang] = 0
		ulangs[user][lang] +=1

# normalize
    for user,langs in ulangs.items():
	total = 0.0
	for lang,count in langs.items():
	    total += count
	for lang,count in langs.items():
	    ulangs[user][lang] = count/total

    print ulangs[133]


def suggestAll():
    global repo_langs,users
    repo_langs = readLang("../data/lang.txt")
    users = readWatches("%s/user_watches.txt"%(folder))
    targetFile = open("%s/test.txt"%(folder))
    build()
    w = open("%s/language.txt"%(folder),'w')
    os.dup2(w.fileno(),1)
    w.close()

    for i,line in enumerate(targetFile.readlines()):
	sys.stderr.write(str(i)+"\n")
	user = int(line.strip())
	suggestion = suggest(user)
	results = [str(repo)+","+str(sim/suggestion[0][1]) for repo,sim in suggestion][:500]
	print str(user)+":"+";".join(results)



if __name__ == "__main__":

    try:
	folder = sys.argv[1]
    except:
	print './language.py folder'
	folder = "../result"
    
    suggestAll()
