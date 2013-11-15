#!/bin/env python

import os,sys

oldconsts=open(sys.argv[1]).readlines()

ps={}
t0s={}
for oldconst in oldconsts:
    if not oldconst.find('#')==0:
        if len(oldconst.split(":")[-1].split())>4: 
            #print oldconst.strip()
            keytokens=oldconst.strip().split(':')[0].strip().split()
            key="%s %s %s %s %s"%(keytokens[0],keytokens[1],keytokens[2],keytokens[3],keytokens[4])
            ps[key]=[]
            ps[key].append(float(oldconst.strip().split(':')[-1].split()[1].strip()))
            ps[key].append(float(oldconst.strip().split(':')[-1].split()[2].strip()))
            ps[key].append(float(oldconst.strip().split(':')[-1].split()[3].strip()))
            ps[key].append(float(oldconst.strip().split(':')[-1].split()[4].strip()))
        else:
            t0keytokens=oldconst.strip().split(':')[0].strip().split()
            t0key="%s %s %s %s %s"%(t0keytokens[0],t0keytokens[1],t0keytokens[2],t0keytokens[3],t0keytokens[4])
            t0s[t0key]=[]
            t0s[t0key].append(float(oldconst.strip().split(':')[-1].split()[0].strip()))
            t0s[t0key].append(float(oldconst.strip().split(':')[-1].split()[1].strip()))

psn={}
tshift={}
for part in ps:
    psn[part]=[]
    rdrift = 0.0
    ntries = 0
    precision = 0.0000000001
    maxtries = 500
    drdt = 0
    t0=20
#    print ps[part]

    driftradius = ps[part][0]+t0*(ps[part][1]+t0*(ps[part][2]));
#    print driftradius

    residual = abs(rdrift) - driftradius;
    while (abs(residual) > precision):  

        drdt = ps[part][1]+t0*(2*ps[part][2])
        t0 = t0 + residual/drdt
    
        driftradius = ps[part][0]+t0*(ps[part][1]+t0*(ps[part][2]));
        residual = rdrift - driftradius;
    
        ntries = ntries + 1

        if (ntries>maxtries):            
            break

#    print part,t0
    #tshift[part]=t0
    tshift[part]=ps[part][0]
    
    psn[part].append(0)
    psn[part].append(ps[part][1] + 2*t0*ps[part][2] + 3*t0*t0*ps[part][3])
    psn[part].append(ps[part][2] + 3*t0*ps[part][3])
    psn[part].append(ps[part][3])

newrts=[]
for part in psn:
    #newrts.append("%s : 0 %e %e %e %e"%(part,psn[part][0],psn[part][1],psn[part][2],psn[part][3]))
    newrts.append("%s : 0 %e %e %e %e"%(part,0,ps[part][1],ps[part][2],ps[part][3]))

newt0s=[]
for straw in t0s:
    key="%s %s -1 -1 -1"%(straw.split()[0],straw.split()[1])
    if tshift.has_key(key):
        #newt0s.append("%s : %f %f"%(straw,t0s[straw][0]+tshift[key],t0s[straw][1]))
        newt0s.append("%s : %f %f"%(straw,t0s[straw][0],t0s[straw][1]))

newrts.sort()
newt0s.sort()
print "# Fileformat=1"
print "# RtRelation"
for newrt in newrts:
    print newrt
print "# StrawT0"
for newt0 in newt0s:
    print newt0
print "#GLOBALOFFSET 0.0000"

oldconsts=open(sys.argv[1]).readlines()

