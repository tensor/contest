#!/usr/bin/python
#coding:utf-8
from io import *

users = {}
repos = {}

user_size = 0;
repo_size = 0;

user_max = 0;
repo_max = 0;

f=open("../data/data.txt")
for line in f.readlines():

    parts = line.strip().split(":")
    user_id = int(parts[0])
    repo_id = int(parts[1])

    if users.has_key(user_id) is False:
	users[user_id]=[]
	user_size +=1
	if (user_id > user_max):user_max = user_id
    users[user_id].append(repo_id)

    if repos.has_key(repo_id) is False:
	repos[repo_id]=[]
	repo_size +=1
	if (repo_id > repo_max):repo_max = repo_id
    repos[repo_id].append(user_id)

f.close()

print "users",user_max,user_size
print "repos",repo_max,repo_size

write_file('../data/user_watches.txt',users,'user')
write_file('../data/repo_watched.txt',repos,'repo')
