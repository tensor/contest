#!/usr/bin/python
#coding:utf-8

import sys
from io import *

blend = True
folder = "../result"
source_file = "../result/50_suggestions_1_1_0.4.txt"
bar = 20
i = 1;
while i< len(sys.argv):
    if sys.argv[i] == '-b':	
	if sys.argv[i+1] == '0':
	    blend = False
    elif sys.argv[i] == '-bar':
	bar = int(sys.argv[i+1])
    elif sys.argv[i] == '-f':
	folder = sys.argv[i+1]
    elif sys.argv[i] == '-s':
	source_file = sys.argv[i+1]
    else:
	sys.stderr.write('blend.py -b 1 -f folder \n')
	break
    i+=2 

external = open(source_file).readlines()
children_50 = open("../result/50_children.txt").readlines()
popular = open("%s/popular_watched.txt"%(folder)).readlines()
pop_lang = open("%s/pop_lang.txt"%(folder)).readlines()
forked = open("%s/all_unwatched_sources.txt"%(folder)).readlines()

authors = read_suggests("%s/author.txt"%(folder))
users = readWatches("%s/user_watches.txt"%(folder))




pure_0 = readResult("../result/results.txt_pure_0")
pure_1 = readResult("../result/results.txt_pure_1")
pure_2 = readResult("../result/results.txt_pure_2")

fork_knn = readResult("../result/results.txt_fork")
no = readResult("../result/results.txt_no")

popular_values = []
for line in popular:
    repo,count = line.strip().split(':')
    popular_values.append(repo)


def blend_fork(value,external_values):
    forked_values = value.split(",")
    if len(forked_values) <=10:	
	tmp =filter(lambda x: x not in forked_values,external_values)
	if len(tmp) ==1 and len(tmp[0].strip())==0:
	    tmp = None
	if tmp != None:	    
	    external_values = (forked_values+tmp)[:10]
	else:
	    external_values = forked_values[:10]
    else:
	tmp1 = filter(lambda x: x in forked_values,external_values)
	tmp2 = filter(lambda x: x not in external_values,forked_values)
	if len(tmp1) ==1 and len(tmp1[0].strip())==0:
	    tmp1 = None
	if len(tmp2) ==1 and len(tmp2[0].strip())==0:
	    tmp2 = None
	    
	if tmp1!=None and tmp2!=None:	    
	    external_values = (tmp1+tmp2)[:10]
	elif tmp2!=None:
	    external_values = tmp2[:10]
	elif tmp1!=None:
	    external_values = tmp1[:10]
	else:
	    external_values= []
    return external_values

for index,line in enumerate(external):
    key,value = line.strip().split(":")
    
    if len(value.strip()) !=0:
	external_values = value.split(",")
    else:
	external_values = []

    if blend == False:
	print key+":"+','.join(external_values[:10])
	continue

 

    fkey,value = forked[index].strip().split(":")

    if int(key)!=int(fkey):
	print "Error, line not matched!"
	exit(1)

    if len(value.strip()) != 0:

	external_values = blend_fork(value,external_values)


    if len(external_values) == 0:
	records = authors[int(key)]
	if len(records) == 0:
	    tmp = []
	else:
	    tmp = [str(repo) for repo,value in records[:10]]


	# blend lang-seted popular item
	pops= pop_lang[index].strip().split(":")[1].split(',')
	for pop in pops:
	    if pop not in tmp:
		tmp.append(pop)
		
	print key+":"+','.join(tmp[:10])

    elif len(external_values)<10:
	tmp = []
	user,other = children_50[index].strip().split(":")
	if len(other.strip())!=0:
	    tmp = other.split(",")

	tmp =filter(lambda x: x not in external_values,tmp)
    	external_values = (external_values+tmp)[:10]	
    	print key+":"+','.join(external_values)

    else:
	if len(value.strip()) != 0:
	    print key+":"+fork_knn[int(key)]
	else:
	    records = authors[int(key)]
	    if len(records) == 0:
		tmp = []
	    else:
		tmp = [str(repo) for repo,value in records[:10]]
		
	    if len(tmp) < 10:
	    	for repo in external_values:
	    	    if repo not in tmp:
	    		tmp.append(repo)
		continue    
		print key+":"+','.join(tmp[:10])
	    else:	
		if int(key) in pure_0:		    
		    print key+":"+pure_0[int(key)]
		elif int(key) in pure_1:		    
		    print key+":"+pure_1[int(key)]
		else:
		    print key+":"+pure_2[int(key)]		    

    
