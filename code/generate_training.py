#!/usr/bin/python
#coding:utf-8
import random
from io import *

userWatches = readWatches("../data/user_watches.txt")
repoWatched = readWatches("../data/repo_watched.txt")

to_remove = 4788
removed = {}

# method 1, random pick from user list
while to_remove >0:
    randomPicked = random.sample(userWatches.keys(),to_remove)
    for picked_user in randomPicked:
	if len(userWatches[picked_user]) == 1:
	    continue
	if removed.has_key(picked_user):
	    continue

	picked_repo = random.sample(userWatches[picked_user],1)[0]
	if len(repoWatched[picked_repo]) == 1:
	    continue
	removed[picked_user]=[picked_repo]
	userWatches[picked_user].remove(picked_repo)
	repoWatched[picked_repo].remove(picked_user)
	to_remove -=1
    print to_remove,len(removed)

write_file("../data/user_training.txt",userWatches,"user")
write_file("../data/repo_training.txt",repoWatched,"repo")
write_test("../data/removed_value.txt","../data/removed_id.txt",removed)




