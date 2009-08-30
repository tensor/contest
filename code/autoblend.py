#!/usr/bin/python
#coding:utf-8

import sys,os,time
from random import random
f = open("auto.log",'a')
for i in range(10000):
    print 'Iteration',i
    f.write("Iteration:%d \t "%(i)+str(time.localtime())+"\n")
    pid = os.fork()
    if pid==0:
	print 'child 1'
	os.execv("blend.o",['./blend.o','../train','200','../train/weights.txt.1'])	
    else:
	sPid = os.fork()
	if sPid == 0:
	    print 'child 2'	    
	    time.sleep(random()*10+1)
	    os.execv("blend.o",['./blend.o','../train','200','../train/weights.txt.2'])	    
	else:
	    print 'start waiting 2'	
	    os.wait()
	    print 'wait over 2'
	print 'start waiting 1'
	os.wait()
	print 'wait over 1'
f.close()
