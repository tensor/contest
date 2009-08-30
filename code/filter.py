#!/usr/bin/python
#coding: utf-8

class Filter(object):
    def __init__(self):
	return

    def filtering(self,user,repo_sim):
	return

class PopularFilter(Filter):
    def __init__(self,repoInfo,userInfo,tType):
	self.userInfo = userInfo
	self.repoInfo = repoInfo
	self.tType = tType
	return

    def filtering(self,user,repo_sim):
	results = []
	for repo,sim in repo_sim:
	    repoPop = self.repoInfo[repo].popularity
	    minPop = self.userInfo[user].minPop
	    maxPop = self.userInfo[user].maxPop
	    if self.tType == 'floor':
		if  (repoPop >= minPop):
		    results.append((repo,sim))	    
	    else:
		if  (repoPop <= maxPop):
		    results.append((repo,sim))	    		
	return results


class TimeFilter(Filter):
    def __init__(self,repoInfo,userInfo,tType):
	self.userInfo = userInfo
	self.repoInfo = repoInfo
	self.tType = tType
	return

    def filtering(self,user,repo_sim):
	results = []
	for repo,sim in repo_sim:
	    repoYear = self.repoInfo[repo].year
	    beginYear = self.userInfo[user].beginYear
	    endYear = self.userInfo[user].endYear
	    if self.tType == 'floor':
		if  repoYear >= beginYear:
		    results.append((repo,sim))	    
	    else:
		if  repoYear <= endYear:
		    results.append((repo,sim))	    		
	return results

class LangFilter(Filter):
    def __init__(self,repoInfo,userInfo):
	self.userInfo = userInfo
	self.repoInfo = repoInfo
	return

    def filtering(self,user,repo_sim):
	results = []
	for repo,sim in repo_sim:	    
	    if  len(self.userInfo[user].langs.intersection(self.repoInfo[repo].langs)) >0:
		results.append((repo,sim))	    
	return results


class CodeFilter(Filter):
    def __init__(self,repoInfo,userInfo,tType):
	self.userInfo = userInfo
	self.repoInfo = repoInfo
	self.tType = tType
	return

    def filtering(self,user,repo_sim):
	results = []
	for repo,sim in repo_sim:	    
	    repoCode = self.repoInfo[repo].codeSize
	    minSize= self.userInfo[user].minSize
	    maxSize = self.userInfo[user].maxSize
	
	    if self.tType == 'floor':
		if  repoCode >= minSize:
		    results.append((repo,sim))	    
	    else:
		if  repoCode <= maxSize:
		    results.append((repo,sim))	    
		
	return results


class ChildFilter(Filter):
    def __init__(self,repoInfo,userInfo):
	self.userInfo = userInfo
	self.repoInfo = repoInfo

	return

    def filtering(self,user,repo_sim):
	results = []
	for repo,sim in repo_sim:	    	
	    if  self.userInfo[user].is_forked == self.repoInfo[repo].is_forked:
		results.append((repo,sim))	    
	return results

class FatherFilter(Filter):
    def __init__(self,repoInfo,userInfo):
	self.userInfo = userInfo
	self.repoInfo = repoInfo
	return

    def filtering(self,user,repo_sim):
	results = []
	for repo,sim in repo_sim:	    	
	    if  self.userInfo[user].has_fork == self.repoInfo[repo].has_fork:
		results.append((repo,sim))	    
	return results
    
