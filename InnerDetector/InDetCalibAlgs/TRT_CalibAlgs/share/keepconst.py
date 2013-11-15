#!/usr/bin/python

import os,sys

if len(sys.argv)<4:
    print "usage: keepconst rt calibout_old.txt  calibout_new.txt"
    sys.exit(0)
    
oldconsts=open(sys.argv[2]).readlines()
newconsts=open(sys.argv[3]).readlines()

oldrts=[]
oldt0s=[]
for oldconst in oldconsts:
    if not oldconst.find('#')==0:
        if len(oldconst.split(":")[-1].split())>4: 
            oldrts.append(oldconst.strip())
        else:
            oldt0s.append(oldconst.strip())

newrts=[]
newt0s=[]
for newconst in newconsts:
    if not newconst.find('#')==0:
        if len(newconst.split(":")[-1].split())>4: 
            newrts.append(newconst.strip())
        else:
            newt0s.append(newconst.strip())

if sys.argv[1]=="t0":
    print "# Fileformat=1"
    print "# RtRelation"
    for newrt in newrts:
        print newrt
    print "# StrawT0"
    for oldt0 in oldt0s:
        print oldt0
    print "#GLOBALOFFSET 0.0000"

if sys.argv[1]=="rt":
    print "# Fileformat=1"
    print "# RtRelation"
    for oldrt in oldrts:
        print oldrt
    print "# StrawT0"
    for newt0 in newt0s:
        print newt0
    print "#GLOBALOFFSET 0.0000"
