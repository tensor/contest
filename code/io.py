#!/usr/bin/python
#coding:utf-8
import sys,pdb
from datetime import date
from math import log
user_max = 56554
repo_max = 123344
barChar = ':'


def readResult(fileName):
    data = open(fileName).readlines()
    blended = {}
    for line in data:
	user,suggestion = line.strip().split(":")
	blended[int(user)]=suggestion
    return blended

def readWatches(fileName):
    watches= {}
    f=open(fileName)
    for line in f.readlines():
	parts = line.strip().split(barChar)
	ID = int(parts[0])
	tmp = parts[1].split(',')
	watches[ID] = []
	for i in range(len(tmp)):
	    try:
		watches[ID].append(int(tmp[i]))
	    except Exception,e:
		print line,e.message
    f.close()
    return watches


# write file as order
def write_file(name,dic,keyType):
    if(keyType == 'repo'):
	maxID = repo_max
    else:
	maxID = user_max

    f=open(name,'w')
    for ID in range(maxID+1):
	if dic.has_key(ID):
	    temp = dic[ID]
	    temp.sort()
	    temp = [str(x) for x in temp]
	    f.write(str(ID)+barChar+','.join(temp)+"\n")
    f.close()


    
def write_test(name,idName,dic):
    write_file(name,dic,"user")
    f=open(idName,'w')
    for ID in range(user_max+1):
	if dic.has_key(ID):
	    f.write(str(ID)+"\n")
    f.close()
    


def read_test(name = '../data/test.txt'):
    test_list= []
    f = open(name)
    for line in f.readlines():
	test_list.append(int(line.strip()))
    f.close()
    return test_list

def write_co_occ(name,cache,keyType):
    if(keyType == 'repo'):
	maxID = repo_max
    else:
	maxID = user_max
    
    f=open(name,'w')
    for ID in range(maxID+1):
	if cache.has_key(ID):
	    temp = cache[ID].items()
	    temp.sort(reverse= False,key =lambda temp:temp[0])
	    for repo,co_occ in temp:		
		f.write(str(ID)+barChar+str(repo)+barChar+str(co_occ)+"\n")
    f.close()
    
def read_suggests(name,requireFile=None):
    if requireFile is not None:
	targetList = read_test(requireFile)
    suggests = {}
    f = open(name)
    for line in f.readlines():
	idx = line.find(":")	
	user = int(line[:idx])
	if requireFile is not None and user not in targetList:
	    continue
	other = line.strip()[idx+1:]
	suggests[user] = []
	if len(other.strip())!=0:
	    tmp = []
	    repo = ''
	    value = ''
	    for char in other:
		if char == ",":
		    repo = ''.join(tmp)
		    tmp = []
		elif char == ';':
		    value = ''.join(tmp)
		    suggests[user].append((int(repo),float(value)))
		    tmp = []
		    repo = ''
		    value = ''
		else:
		    tmp.append(char)
	    if repo != '' and len(tmp) != 0:
		suggests[user].append((int(repo),float(''.join(tmp))))


    f.close()
    return suggests

def read_children(name,requireFile=None):
    if requireFile is not None:
	targetList = read_test(requireFile)
    suggests = {}
    f = open(name)
    for line in f.readlines():
	idx = line.find(":")	
	user = int(line[:idx])
	if requireFile is not None and user not in targetList:
	    continue
	other = line.strip()[idx+1:]
	suggests[user] = []
	if len(other.strip())!=0:
	    repos = other.split(",")
	    for repo in repos:
		suggests[user].append((int(repo),1.0))
    f.close()
    return suggests


def readLang(name):
    repo_langs = {}
    
    f = open(name)
    for line in f.readlines():
	repo,langs = line.strip().split(':')
	repo_langs[int(repo)]={}
	if len(langs.strip()) != 0:
	    parts = langs.split(",")
	    for part in parts:
		lang,codeLine = part.split(";")
		repo_langs[int(repo)][lang]=1.0/len(parts)
    f.close()
    return repo_langs


class Repo(object):
 
    def __init__(self, id):
        self.id = id 
        self.year = 0
        self.has_fork = False
        self.is_forked = False
	self.codeSize = 0;
        self.popularity = 0
        self.langs = set()

class User(object):
 
    def __init__(self, id):
        self.id = id 
        self.beginYear = 3000
	self.endYear = 0
	self.minSize = 10000000000000
	self.maxSize = 0
        self.minPop = 1000000000000
	self.maxPop = 0
        self.langs = set()
	self.is_forked = False
	self.has_fork = False

    def update(self,repo):
	if repo.year < self.beginYear:
	    self.beginYear = repo.year
	if repo.year > self.endYear:
	    self.endYear = repo.year
	if repo.codeSize != 0:
	    if repo.codeSize < self.minSize:
		self.minSize = repo.codeSize
	    if repo.codeSize > self.maxSize:
		self.maxSize = repo.codeSize
	if repo.popularity !=0:
	    if repo.popularity < self.minPop:
		self.minPop = repo.popularity
	    if repo.popularity > self.maxPop:
		self.maxPop = repo.popularity
	if repo.is_forked:
	    self.is_forked = True
	if repo.has_fork:
	    self.has_fork = True
	self.langs = self.langs.union(repo.langs)

def read_target(name):
    users = []
    f = open(name)
    for line in f.readlines():
	users.append(int(line.strip()))
    f.close()
    return users

def  buildInfo(repoInfo,users,targetList):
    userInfo = {}
    for uId in targetList:
	if uId not in users:
	    continue
	user = User(uId)
	for repo in users[uId]:
	    if repo in repoInfo:
		user.update(repoInfo[repo])
	userInfo[uId]=user
		
    return userInfo


def read_repo(repoDetail,repoLang,repoWatched):
    repoWatched = readWatches(repoWatched)
    repos = {}
    repo_txt = open(repoDetail, 'r')
    for line in repo_txt.readlines():
        id, other = line.strip().split(':')
        id = int(id)
	
	if id not in repos:
	    repos[id] =  Repo(id)
	repo = repos[id]

        parts = other.split(',')	
	timeParts= parts[1].split('-')
	repo.year = int(timeParts[0])
        if len(parts) > 2 and int(parts[2]) in repoWatched:
            repo.is_forked = True
	    if int(parts[2]) not in repos:
		repos[int(parts[2])] = Repo(int(parts[2]))
	    repos[int(parts[2])].has_fork = True
		

	repo.popularity = log(len(repoWatched[id])+1)
 
    repo_txt.close()

    lang_txt = open(repoLang,'r')
    for line in lang_txt.readlines():
	id,other = line.strip().split(":")
	if int(id) not in repos:
	    continue
	repo = repos[int(id)]	
	parts = other.split(',')
	for part in parts:
	    language,code = part.split(';')
	    repo.langs.add(language)
	    repo.codeSize += int(code)	
	
	repo.codeSize  = log(repo.codeSize+1)

    lang_txt.close()
    
    return repos 
 
     
# def read_userInfo(repoInfo,users):
#     userInfo = {}
#     for user,watchList in users.items():
# 	if user not in userInfo:
# 	    userInfo[user]=User(user)
# 	u = userInfo[user]
	
# 	for repo in watchList:

	    
# if __name__ == '__main__':
#     srcName = "../train/user_watches.txt"
#     destName = "../train/data.txt"
#     watches = readWatches(srcName)
#     w = open(destName,'w')
#     for ID in range(user_max+1):
# 	if ID not in watches:
# 	    continue
# 	for repo in watches[ID]:
# 	    w.write("%d:%d\n"%(ID,repo))
#     w.close()
