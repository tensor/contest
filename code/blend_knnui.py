#!/usr/bin/python
#coding:utf-8

from io import *
from filter import *
from random import random
import sys,pdb
user_max = 56554
removed_value = {}
users = {}
bar = 15

def filtering(results,filters,uses):
    filtered = {}
    for user,result in results.items():
	filtered[user]= result

    for i,f in enumerate(filters):
	if uses[i] is False:
	    continue
	for user,result in filtered.items():	  
	    if user in users and len(users[user]) >= bar:
		filtered[user] = f.filtering(user,result)
	    else:
		filtered[user] = result
    return filtered

def score(results):
    score = 0
    for user_id,repo_id in removed_value.items():
	if user_id not in results:
	    continue
	tmp = [repo for repo,sim in results[user_id][:10]]
	if repo_id[0] in tmp:
	    score +=1
	# else:
	#     print user_id,repo_id,rec[user_id]
    # print score,score	
    return score

def blend(suggestions,weights):
    results = {}
    for i,suggestion in enumerate(suggestions):	
	for uId,suggestList in suggestion.items():	    
	    if results.has_key(uId) is False:
		results[uId]={}
	    for repo,sim in suggestList:
		if results[uId].has_key(repo) is False:
		    results[uId][repo] = 0
		results[uId][repo] += weights[i]*sim

    for uId,result in results.items():
	tmp = result.items()
	tmp = sorted(tmp,reverse=True,key = lambda tmp:tmp[1])
	results[uId]=tmp
    return results

if __name__ == '__main__':
    try:
	N = int(sys.argv[1])
    except:
	sys.stderr.write("Set default N as 50")
	N = 50

    try:
	folder = sys.argv[2]
    except:
	sys.stderr.write("Set default N as ../data/train")
	folder = "../train"

    if folder == "../train":
	# requireFile = "../train/pure_knn.txt"
	# requireFile = "../train/test.txt"	
	requireFile = "../train/no_knns_fork.txt"
    else:
	# requireFile = "../result/pure_knn.txt"
	requireFile = "../result/test.txt"

    users = readWatches("%s/user_watches.txt"%(folder))
    targetList = read_target("%s/test.txt"%(folder))
    repoInfo = read_repo("../data/repos.txt","../data/lang.txt","%s/repo_watched.txt"%(folder))
    sys.stderr.write("Load repo info  compelete!\n")
    userInfo = buildInfo(repoInfo,users,targetList)
    sys.stderr.write("Load user info  compelete!\n")
	
    # iknn = read_suggests("%s/results-knni-iuf2.txt"%(folder),requireFile)
    # sys.stderr.write("Load item knn  compelete!\n")
    # uknn = read_suggests("%s/results-knnu-iif.txt"%(folder),requireFile)
    # sys.stderr.write("Load user knn  compelete!\n")
    # uiknn = read_suggests("%s/results-knnui.txt"%(folder),requireFile)
    # sys.stderr.write("Load user*item knn  compelete!\n")
    author = read_suggests("%s/author.txt"%(folder),requireFile)
    sys.stderr.write("Load author  compelete!\n")
    language = read_suggests("%s/language.txt"%(folder),requireFile)
    sys.stderr.write("Load language  compelete!\n")
    children = read_children("%s/fork_by.txt"%(folder),requireFile)
    sys.stderr.write("Load children compelete!\n")    

    suggestion=[]
    weights = []	
    # 2321
    # suggestion.append(iknn)
    # weights.append(1.0)
    # suggestion.append(uknn)
    # weights.append(1.0)	# 1.01
    # suggestion.append(uiknn)
    # weights.append(0.4)	# 1226 0.33

    # 2340
    # suggestion.append(iknn)
    # weights.append(1.288)
    # suggestion.append(uknn)
    # weights.append(1.098)    
    # suggestion.append(children)
    # weights.append(0.4)	
    # suggestion.append(uiknn)
    # weights.append(0.384)    
    # suggestion.append(author)
    # weights.append(0.594)	
    # suggestion.append(language)
    # weights.append(0.226)	


    # suggestion.append(iknn)
    # weights.append(1.288)
    # suggestion.append(uknn)
    # weights.append(1.098)    
    suggestion.append(children)
    weights.append(1)	
    # suggestion.append(uiknn)
    # weights.append(0.384)    
    suggestion.append(author)
    weights.append(1)	
    suggestion.append(language)
    weights.append(1)	




    sys.stderr.write("Load suggesstions compelete!\n")
    filters = []
    uses = []
    filters.append(PopularFilter(repoInfo,userInfo,"floor"))
    uses.append(False)
    filters.append(PopularFilter(repoInfo,userInfo,"ceil"))
    uses.append(False)
    filters.append(TimeFilter(repoInfo,userInfo,"floor"))
    uses.append(False)
    filters.append(TimeFilter(repoInfo,userInfo,"ceil"))
    uses.append(False)
    filters.append(CodeFilter(repoInfo,userInfo,"floor"))
    uses.append(False)
    filters.append(CodeFilter(repoInfo,userInfo,"ceil"))
    uses.append(True)
    filters.append(LangFilter(repoInfo,userInfo))
    uses.append(False)
    filters.append(ChildFilter(repoInfo,userInfo))
    uses.append(False)
    filters.append(FatherFilter(repoInfo,userInfo))
    uses.append(False)
    


    if folder == '../train':
	removed_value = readWatches("../train/removed_value.txt")
	sys.stderr.write("Load test text  compelete!\n")

	# TODO:对不同用户分类，然后分别混合
	j=0
	K = len(suggestion)
	for step in range(30):
	    i = j%K
	    j += 1 
	    origW = weights[i]
	    origR = score (blend(suggestion,weights))
	
	    weights[i] *= (1+0.5*(random()-0.5))
	    newR = score (blend(suggestion,weights))
	
	    if newR <= origR:
		weights[i]=origW
	    
	    print step,origR,newR,i,weights[i]
	for weight in weights:
	    print weight,
	print 

	
	results = blend(suggestion,weights)
	sys.stderr.write(str(score(results))+"\n")
	sys.stderr.write("blending over\n")
	    
	j=0
	K = len(uses)
	for step in range(pow(len(uses),2)):	
	    i = j%K
	    j += 1 
	    origW = uses[i]
	    origR = score (filtering(results,filters,uses))
	    uses[i] = not uses[i]
	    newR = score (filtering(results,filters,uses))
	    if newR <= origR:
		uses[i]=origW
	    
	    print step,origR,newR,i,uses[i]

	# results = blend(suggestion,weights)
	# for user in range(user_max+1):
	#     if results.has_key(user):
	# 	tmp = results[user]
	# 	tmp = [str(x) for x,y in tmp[:N]]
	    
	# 	print str(user)+":"+",".join(tmp)


    if folder == '../result':
	results = blend(suggestion,weights)
	# results = filtering(results,filters,uses)
	for user in range(user_max+1):
	    if results.has_key(user):
		tmp = results[user]
		tmp = [str(x) for x,y in tmp[:N]]	    
		print str(user)+":"+",".join(tmp)
