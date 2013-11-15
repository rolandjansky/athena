#!/bin/env python

import os,sys,numpy

def tshift_poly(dt,p0,p1,p2,p3):
    p0n = p0 + p1*dt + p2*dt*dt + p3*dt*dt*dt
    p1n = p1 + 2*p2*dt + 3*p3*dt*dt
    p2n = p2 + 3*p3*dt
    p3n = p3
    return p0n,p1n,p2n,p3n

keeprt=False
keept0=False
shiftrt=True
shiftt0=False
do_global=False

olddbs=open(sys.argv[1]).readlines()
newdbs=open(sys.argv[1]).readlines()
olddicts=open(sys.argv[1]).readlines()

print keeprt, keept0, shiftrt, shiftt0

oldrts=[]
oldt0s=[]
olderrors=[]
for olddb in olddbs:
    if not olddb.find('#')==0:
        if len(olddb.split(":")[-1].split())>10: 
            olderrors.append(olddb.strip())
        elif len(olddb.split(":")[-1].split())>4: 
            oldrts.append(olddb.strip())
        else:
            oldt0s.append(olddb.strip())

newrts		=[]
newt0s		=[]
newerrors	=[]
for newdb in newdbs:
    if not newdb.find('#')==0:
        if len(newdb.split(":")[-1].split())>10: 
            newerrors.append(newdb.strip())
        elif len(newdb.split(":")[-1].split())>4: 
            newrts.append(newdb.strip())
        else:
            newt0s.append(newdb.strip())

rts=newrts
t0s=newt0s

rtfix=[1,18]
rtconsts={}
rtshifts={}


## THis corrects the rts:

for rt in rts:
    #print rt.strip()
    rtkeytokens=rt.strip().split(':')[0].strip().split()
    rtdbkey="%s %s %s %s %s"%(rtkeytokens[0],rtkeytokens[1],rtkeytokens[2],rtkeytokens[3],rtkeytokens[4])
    rtdictkey="_%s_%s_%s_%s_%s"%(rtkeytokens[0],rtkeytokens[1],rtkeytokens[2],rtkeytokens[3],rtkeytokens[4])

    p0=float(rt.strip().split(':')[-1].split()[1].strip())
    p1=float(rt.strip().split(':')[-1].split()[2].strip())
    p2=float(rt.strip().split(':')[-1].split()[3].strip())
    p3=float(rt.strip().split(':')[-1].split()[4].strip())
    poly_orig = numpy.poly1d([p3,p2,p1,p0-rtfix[0]]) #create polynomial shifted to get the t where r(t)=1   
    #print poly_orig.r

    shiftval = poly_orig.r[2]-rtfix[1] #calculate first shift along t so that r(rtfix)=1 
    p0n,p1n,p2n,p3n=tshift_poly(shiftval,p0,p1,p2,p3)
    poly_new = numpy.poly1d([p3n,p2n,p1n,p0n])    

    shiftvaln = 0.0 #poly_new.r[2] #calculate second shift along t so that r(0)=0 
    p0nn,p1nn,p2nn,p3nn=tshift_poly(shiftvaln,p0n,p1n,p2n,p3n)
    poly_newnew = numpy.poly1d([p3nn,p2nn,p1nn,p0nn])

    rtconsts[rtdbkey]=[]
    if shiftrt:
        print '%16s ... original polynomial: r=%.1f mm @ %f ns, shifting %f ns in t, new polynomial: r(%.1f ns) = %f mm, t0 = %f'%(rtdbkey,rtfix[0],poly_orig.r[2],shiftval,rtfix[1],poly_new(rtfix[1]),shiftvaln)
        #print p0n,p1n,p2n,p3n
        #print p0nn,p1nn,p2nn,p3nn
        #rtconsts[rtdbkey].append(0.0)
        rtconsts[rtdbkey].append(p0nn)
        rtconsts[rtdbkey].append(p1nn)
        rtconsts[rtdbkey].append(p2nn)
        rtconsts[rtdbkey].append(p3nn)
        rtshifts[rtkeytokens[0]]=shiftvaln #only compensate t0s for the second shift
    else:
        rtconsts[rtdbkey].append(p0)
        rtconsts[rtdbkey].append(p1)
        rtconsts[rtdbkey].append(p2)
        rtconsts[rtdbkey].append(p3)
        rtshifts[rtkeytokens[0]]=p0
        
t0consts={}
for t0 in t0s:
    t0keytokens=t0.strip().split(':')[0].strip().split()
    t0dbkey="%s %s %s %s %s"%(t0keytokens[0],t0keytokens[1],t0keytokens[2],t0keytokens[3],t0keytokens[4])
    t0consts[t0dbkey]=[]
    t0consts[t0dbkey].append(float(t0.strip().split(':')[-1].split()[0].strip()))
    t0consts[t0dbkey].append(float(t0.strip().split(':')[-1].split()[1].strip()))




#shift rts
dbrtouts=[]
for part in rtconsts:
    dbrtouts.append("%s : 0 %e %e %e %e"%(part,rtconsts[part][0],rtconsts[part][1],rtconsts[part][2],rtconsts[part][3]))

#shift t0s
dbt0outs=[]
for straw in t0consts:
    dbt0outs.append("%s : %f %f"%(straw,t0consts[straw][0],t0consts[straw][1]))

dbrtouts.sort()
dbt0outs.sort()

dbfile=open("dbconst.txt","w")
# In case there are not errors:
if len (newerrors)==0:
	dbfile.write("# Fileformat=1\n")
	dbfile.write("# RtRelation\n")
	for dbrtout in dbrtouts:
	    dbfile.write(dbrtout + '\n')
	dbfile.write("# StrawT0\n")
	for dbt0out in dbt0outs:
	    dbfile.write(dbt0out + '\n')
	dbfile.write("#GLOBALOFFSET 0.0000\n")
elif len (newerrors)>1:
        dbfile.write("# Fileformat=2\n")
        dbfile.write("# RtRelation\n")
        for dbrtout in dbrtouts:
            dbfile.write(dbrtout + '\n')
        dbfile.write("# errors\n")
        for errorout in newerrors:
            dbfile.write(errorout + '\n')
        dbfile.write("# StrawT0\n")
        for dbt0out in dbt0outs:
            dbfile.write(dbt0out + '\n')
        dbfile.write("#GLOBALOFFSET 0.0000\n")


