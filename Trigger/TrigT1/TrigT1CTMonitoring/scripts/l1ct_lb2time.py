#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

import sys,getopt,os

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                   this printout"
    print " "
    print " From COOL database, read lumiblock times and write a root file"
    print " with a histogram with N bin per second, filled with interpolated"
    print " lumiblock number."
    print " Example, a value of 15.9 is at the end of lumiblock 15"
    print " the interpolation is linear in time within each lumi block"
    
    print ""
    print "  --run <runNo>              run number (single number), eg 134967"    
    print "  --dt seconds               time granularity of time->lb map "    
    print " "
    print "  --since_runstart           subtract start of first lb from all times"
    print " "
    print "  --outbase | -o <filename>  Output base name "
    print "       (default='CTPMON_', note: _<runNo>.<extensions> are added)"
    print " "
    print "  --list                     print the lb times and exit."
    print "  --summary                  Short summary and exit. "
try:
    longopts=['norunname','run=','dt=','counters=','bcid=','summary',
              'help','verbose','textbc','textsum','list',
              'outbase=','nodraw=','nofillHist','nodrawHist','nobatch']
    opts,args=getopt.getopt(sys.argv[1:],'h',longopts)
except getopt.GetoptError,e:
    print "Option parsing error:"
    usage()
    sys.exit(2)

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)

import l1ctpy
# defaults:
OPT={}
OPT["--run"]=int(-1)
OPT["--summary"]=False
OPT["--list"]=False
OPT["--verbose"]=False
OPT["--filename"]="LBINTERP_"
OPT['--norunname'] = False
OPT["--dt"]=1
OPT["--lb"]=l1ctpy.text_to_ranges('-')
OPT['--since_runstart']=0

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--run':
        OPT["--run"] = a
    if o == '--dt':
        OPT["--dt"] = float(a)
    if o == '--since_runstart':
        OPT['--since_runstart']=1
    if o == '--outbase':                 
        OPT["--filename"]=str(a)
    if o == '--summary':                 
        OPT["--summary"]=True
    if o == '--list':                 
        OPT["--list"]=True

try:
    OPT["--run"]=int(OPT["--run"])

    if OPT["--run"] < 1:
        print "No run specified. Finding last run in cool"
        OPT["--run"]=l1ctpy.get_current_run()
except BaseException,e:
    usage()
    print e
    raise ValueError("You must specify a single run number, not:",OPT["--run"])


if not OPT['--norunname'] :
    OPT["--filename"]+=str(OPT["--run"])
    

OPT['--RateTitle']='Run '+str(OPT["--run"])+ " : CTPMON Counter Rates, Hz"

if  OPT['--verbose'] :
    print "Options:",OPT
    
import l1ctpy

#+-------------------------------------------
# Open databases and get the required folders
#+-------------------------------------------
lbfolder=l1ctpy.getCoolFolder(l1ctpy.get_dbid_COMP(),'/TRIGGER/LUMI/LBLB')
LBs,TimeRanges=l1ctpy.getLbTimesMulti(lbfolder,OPT["--run"],OPT["--lb"])


monblockLBs=[]
monTimeRanges0=TimeRanges[0]
monTimeRangeslast=TimeRanges[-1][0]

if OPT['--list']:
    import time

    print "lumiblocks:"
    for l,t in zip(LBs,TimeRanges):
        print "%4d:  %20s -- %20s" % ( l , time.asctime(time.localtime(t[0]/1e9)),time.asctime(time.localtime(t[1]/1e9)))

if OPT['--list'] or  OPT['--summary']:
    ## NOTE! do not change the format returned by --summary, it is used
    ## by some bash scripts
    print len(LBs), int(TimeRanges[0][0]/1e9),int(TimeRanges[-1][-1]/1e9) 
    sys.exit()
    
monTimeRanges=[];
m=monTimeRanges0
#print m
duration=OPT["--dt"] # i second

m=(m[0],m[0]+duration*1e9) # 10 seconds
print "building time vector"
while m[0]<monTimeRangeslast+duration*1000*1e9:
    monTimeRanges.append(m);
    #print m[0]/1.e18
    m=(m[1],m[1]+duration*1e9) # 

print "matching lumi blocks for",len(monTimeRanges),"time entries"
for tt in monTimeRanges:
    monblockLBs.append(l1ctpy.time2lb(LBs,TimeRanges,tt))

times=[]
lbs=[]
for i,tt in enumerate(monTimeRanges):
    times.append(tt[0]*1e-9)
    lbs.append(monblockLBs[i][0])

for i in range(len(lbs)-1):
    if lbs[i+1]<=lbs[i]:
        lbs[i+1]=lbs[i]+1e-7;

#print LBs

print "writing root file"
import ROOT
import numpy

def rootarray(inp):
    return numpy.array(inp, dtype=numpy.double)

if OPT['--since_runstart']:
    firsttime=TimeRanges[0][0]
else:
    firsttime=0
TimeRanges=map(lambda x: [(x[0]-firsttime)*1e-9,(x[1]-firsttime)*1e-9], TimeRanges)

alltimes=[]
for i,t in enumerate(TimeRanges):
    alltimes.append(t[0])
alltimes.append(TimeRanges[-1][1])

starttimes=ROOT.TH1D("LB_start","LB_start",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)
stoptimes=ROOT.TH1D("LB_stop","LB_stop",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)
dttimes=ROOT.TH1D("LB_dt","LB_dt",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)
centertimes=ROOT.TH1D("LB_center","LB_center",len(LBs),LBs[0]-0.5,LBs[-1]+0.5)

times_lb=ROOT.TH1D("exact_lb_at_time","exact lb at time",len(LBs),rootarray(alltimes))

times_lb_elapsed=ROOT.TH1D("exact_lb_at_elapsed_time_sec","exact lb at elapsed time [s]",len(LBs),rootarray(alltimes))

for i,t in enumerate(TimeRanges):
    starttimes.Fill(LBs[i],TimeRanges[i][0])
    stoptimes.Fill(LBs[i],TimeRanges[i][1])
    dttimes.Fill(LBs[i],TimeRanges[i][1]-TimeRanges[i][0])
    centertimes.Fill(LBs[i],0.5*(TimeRanges[i][1]+TimeRanges[i][0]))
    times_lb.Fill(0.5*(TimeRanges[i][1]+TimeRanges[i][0]),LBs[i])
    times_lb_elapsed.Fill(0.5*(alltimes[i]+alltimes[i+1]),LBs[i])

# print monblockLBs
outfile=ROOT.TFile(OPT['--filename']+".root","recreate");
time2lbHist=ROOT.TH1D(str("time2lb"),str(""),len(lbs)-1,rootarray(lbs))

for i,tt in enumerate(times):
    time2lbHist.SetBinContent(i,tt)

time2lbHist.Write()
starttimes.Write()
stoptimes.Write()
dttimes.Write()
centertimes.Write()
times_lb.Write()
times_lb_elapsed.Write()

outfile.Close()
print "wrote root file. Done."



