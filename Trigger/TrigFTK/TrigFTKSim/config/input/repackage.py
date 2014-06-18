#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Cat ALL of 350M single muons into this script
# It will use a supplied region lookup table to route events into per-region files

import sys,os

if len(sys.argv)!=5:
    print 'Need four arguments:'
    print sys.argv[0],'region_lookup_file','output_name_template','output dir','tmp dir'
    print sys.argv[0],'/share/ftkdata/antonk/bases/v14prod/lookup/region_V14PNFS.dat','single_muons_V14PNFS_reg%d_file%.3d.dat',
    print '/pnfs/output/path/','/tmp/antonk'
    sys.exit(0)
reglookupfilename=sys.argv[1]
outname=sys.argv[2]
outdir=sys.argv[3]
tmpdir=sys.argv[4]

print 'Region lookup:',reglookupfilename
print 'Output template:',outname

EVENTSPERFILE=500000
NREGIONS=8
AllowNoise=False

if not os.path.isdir(outdir):
    os.makedirs(outdir)
if not os.path.isdir(tmpdir):
    os.makedirs(tmpdir)

class RegLookup:
    "A class that encapsulates region lookup table"
    CACHESIZE=10000 
    def __init__(self,fname,region=-1):
        "opens a region lookup file for a given region (-1 means all)"
        self.fname=fname
        self.file=open(fname)
        self.region=region
        self.dic={}
        self.max=-1
    def Refill(self):
        "fills next bunch of events into dictionary"
        self.dic={}
        lnum=0
        val=0
        for l in self.file:
            z=l.split()
            if self.region==-1 or int(z[1])==self.region:
                val=int(z[0])
                self.dic[val]=int(z[1])
                lnum+=1
            if lnum==self.CACHESIZE:
                break
        self.max=val
    def Region(self,event):
        "warning: no checks"
        return self.dic[event]
    def In(self,event):
        if event>self.max:
            self.Refill()
        if event in self.dic:
            return True
        else:
            return False

lookup=RegLookup(reglookupfilename)

evtglb=1
evtsaved=[0,]*NREGIONS
outnum=[0,]*NREGIONS
curfile=[open(os.path.join(tmpdir,outname)%(i,outnum[i]),'w') for i in range(0,NREGIONS)]
region=-1
for line in sys.stdin:
    if lookup.In(evtglb):
        region=lookup.Region(evtglb)
        if not AllowNoise and line[0]=='S':
            tmpln = line.split()
            if int(tmpln[13]) != 10001:
                continue
        print >>curfile[region],line,
    if line[0]=='L':
        if lookup.In(evtglb):
            evtsaved[region]+=1
        evtglb+=1
        if evtglb%10000==0:
            print 'Starting event',evtglb
    if evtsaved[region]>=EVENTSPERFILE:
        evtsaved[region]=0
        curfile[region].close()
        cmd='LD_PRELOAD="" dccp'+' '+curfile[region].name+' '+outdir+'/'
        print '> ',cmd
        os.system(cmd)
        cmd='rm -f'+' '+curfile[region].name
        print '> ',cmd
        os.system(cmd)
        outnum[region]+=1
        curfile[region]=open(os.path.join(tmpdir,outname)%(region,outnum[region]),'w')
        print 'Starting new file:',curfile[region].name

for region in range(0,NREGIONS):
    if evtsaved[region]>0:
        evtsaved[region]=0
        curfile[region].close()
        cmd='LD_PRELOAD="" dccp'+' '+curfile[region].name+' '+outdir+'/'
        print '> ',cmd
        os.system(cmd)
        cmd='rm -f'+' '+curfile[region].name
        print '> ',cmd
        os.system(cmd)
        outnum[region]+=1

