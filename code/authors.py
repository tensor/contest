#!/usr/bin/python
#coding:utf-8
import os,sys
from io import *

folder = sys.argv[1]

fileName = "../data/repos.txt"
testFile = folder+"/test.txt"
forks_of_r ={}
parent_of_r = {}
gparent_of_r = {}
r_info = {}
u_authoring = {}
r_name = {}

user_watches = {}
repo_watched = {}


def parse_name(name):
    names = []
    tmp =[]
    for char in name:
	if char == '_' or char == ':' or char == '-' or char == '.' or char == '':
	    if len(tmp)!=0:
		string = ''.join(tmp).lower()
		names.append(string)
	    tmp = []
	else:
	    tmp.append(char)

    if len(tmp)!=0:
	string = ''.join(tmp).lower()
	names.append(string)

    names.sort()
    return '_'.join(names)
    

def parse_repos():
    
    lines = file(fileName).read().split("\n")    
    pairs = [line.replace(":", ",").split(",") for line in lines if line]
    pairs = [tuple([int(pair[0]),
                        int(pair[3]) if pair[3:4] else 0,
                        pair[1],
                        pair[2]])
                 for pair in pairs]
 
    for repo, parent, name, creation in pairs:
	if parent > 0:
	    if forks_of_r.has_key(parent) is False:
		forks_of_r[parent]= []		
	    forks_of_r[parent].append(repo)
	    parent_of_r[repo] = parent

	author, name = name.split("/")
	# pdb.set_trace()
	name = parse_name(name)	
	r_info[repo] = (author, name, creation)
	if u_authoring.has_key(author) is False:
	    u_authoring[author] = []
	u_authoring[author].append(repo)

	if r_name.has_key(name) is False:
	    r_name[name] = []
	r_name[name].append(repo)
	
    for repos_gen1, repos_gen2 in parent_of_r.items():
	if repos_gen2 in parent_of_r:
	    repos_gen3 = parent_of_r[repos_gen2]
	    gparent_of_r[repos_gen1] = repos_gen3

    return r_info
		
def produce_name():
    w=open(folder+"/sameName.txt",'w')
    os.dup2(w.fileno(),1)
    w.close()

    lines = file(testFile).read().split("\n")        
    
    for i,line in enumerate(lines):
	try:
	    user = int(line)
	except:
	    sys.stderr.write(str(i))
	    break
	try:
	    repos = user_watches[user]
	except:
	    sys.stderr.write(str(user)+"\n")
	    print line+":"
	    continue

	author_suggested = {}
	authors = []
	for repo in repos:
	    author = r_info[repo][0]
	    authors.append(author)
	    
	for author in authors:
	    for repo in u_authoring[author]:
		if repo in repos:
		    continue
		if author_suggested.has_key(repo) is False:
		    author_suggested[repo] = 0
		author_suggested[repo] += 1
	# 	
	suggestion = {}
	names = []
	for repo in repos:
	    names.append(r_info[repo][1])
	for name in names:
	    for repo in r_name[name]:
		if repo in author_suggested:
		    continue
		if repo in repos:
		    continue
		if repo not in suggestion:
		    suggestion[repo]=0
		suggestion[repo]+=1

	for repo in suggestion.keys():
	    suggestion[repo]+=len(repo_watched[repo])/56554.0

	tmp = suggestion.items()
	tmp = sorted(tmp,reverse = True,key = lambda tmp:tmp[1])
	tmp = [str(repo)+","+str(value/tmp[0][1]) for repo,value in tmp[:1000]]	
	print line+":"+';'.join(tmp)

def produce_author():

    w = open(folder+"/author.txt",'w')
    print w.fileno()
    os.dup2(w.fileno(),1)
    w.close()

    lines = file(testFile).read().split("\n")        
    
    for i,line in enumerate(lines):
	try:
	    user = int(line)
	except:
	    sys.stderr.write(str(i))
	    break
	try:
	    repos = user_watches[user]
	except:
	    sys.stderr.write(str(user)+"\n")
	    print line+":"
	    continue
	suggested = {}
	authors = []
	for repo in repos:
	    author = r_info[repo][0]
	    authors.append(author)
	    # if authors.has_key(author) is False:
	    # 	authors[author] = 0
	    # authors[author] += 1
	    
	for author in authors:
	    for repo in u_authoring[author]:
		if repo in repos:
		    continue
		if suggested.has_key(repo) is False:
		    suggested[repo] = 0
		suggested[repo] += 1

	for repo in suggested.keys():
	    suggested[repo]+=len(repo_watched[repo])/56554.0

	tmp = suggested.items()
	tmp = sorted(tmp,reverse = True, key = lambda tmp:tmp[1])
	tmp = [str(repo)+","+str(value/tmp[0][1]) for repo,value in tmp]
	
	print line+":"+';'.join(tmp)

def produce_children():
    w = open(folder+"/children.txt",'w')
    os.dup2(w.fileno(),1)
    w.close()

    lines = file(testFile).read().split("\n")        
    
    for i,line in enumerate(lines):
	try:
	    user = int(line)
	except:
	    sys.stderr.write(str(i))
	    break
	try:
	    repos = user_watches[user]
	except:
	    sys.stderr.write(str(user)+"\n")
	    print line+":"
	    continue
	suggested = {}
	for repo in repos:
	    if repo not in forks_of_r:
		continue
	    children = forks_of_r[repo]
	    for child in children:
		if child in repos:
		    continue
		if suggested.has_key(child) is False:
		    suggested[child] = 0
		suggested[child] += 1
			    
	for repo in suggested.keys():
	    suggested[repo]+=len(repo_watched[repo])/56554.0

	tmp = suggested.items()
	tmp = sorted(tmp,reverse = True, key = lambda tmp:tmp[1])
	tmp = [str(repo)+","+str(value/tmp[0][1]) for repo,value in tmp]
	
	print line+":"+';'.join(tmp)

def produce_parent():
    w = open(folder+"/parent.txt",'w')
    os.dup2(w.fileno(),1)
    w.close()

    lines = file(testFile).read().split("\n")        
    
    for i,line in enumerate(lines):
	try:
	    user = int(line)
	except:
	    sys.stderr.write(str(i))
	    break
	try:
	    repos = user_watches[user]
	except:
	    sys.stderr.write(str(user)+"\n")
	    print line+":"
	    continue
	suggested = {}
	for repo in repos:
	    if repo not in parent_of_r:
		continue
	    parent = parent_of_r[repo]
	    if parent in repos:
		continue
	    if suggested.has_key(parent) is False:
		suggested[parent] = 0
	    suggested[parent] += 1
			    
	for repo in suggested.keys():
	    suggested[repo]+=len(repo_watched[repo])/56554.0

	tmp = suggested.items()
	tmp = sorted(tmp,reverse = True, key = lambda tmp:tmp[1])
	tmp = [str(repo)+","+str(value/tmp[0][1]) for repo,value in tmp]
	
	print line+":"+';'.join(tmp)


def produce_g_parent():
    w = open(folder+"/g_parent.txt",'w')
    os.dup2(w.fileno(),1)
    w.close()

    lines = file(testFile).read().split("\n")        
    
    for i,line in enumerate(lines):
	try:
	    user = int(line)
	except:
	    sys.stderr.write(str(i))
	    break
	try:
	    repos = user_watches[user]
	except:
	    sys.stderr.write(str(user)+"\n")
	    print line+":"
	    continue
	suggested = {}
	for repo in repos:
	    if repo not in gparent_of_r:
		continue
	    gparent = gparent_of_r[repo]
	    if gparent in repos:
		continue
	    if suggested.has_key(gparent) is False:
		suggested[gparent] = 0
	    suggested[gparent] += 1
			    
	for repo in suggested.keys():
	    suggested[repo]+=len(repo_watched[repo])/56554.0

	tmp = suggested.items()
	tmp = sorted(tmp,reverse = True, key = lambda tmp:tmp[1])
	tmp = [str(repo)+","+str(value/tmp[0][1]) for repo,value in tmp]
	
	print line+":"+';'.join(tmp)


if __name__ == '__main__':    
    user_watches = readWatches(folder+"/user_watches.txt")
    repo_watched = readWatches(folder+"/repo_watched.txt")
    parse_repos()
    produce_author()
    produce_children()
    produce_parent()
    produce_g_parent()
    produce_name()
