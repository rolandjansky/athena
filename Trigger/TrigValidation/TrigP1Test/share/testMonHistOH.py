#!/usr/bin/env python

##
## Run athenaHLT, produce histograms in OH and check that entries match
##

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import os,sys
import glob

if len(sys.argv)!=2:
   print "Syntax: testMonHistOH.py DATAFILE"
   sys.exit(1)

data = sys.argv[1]
events = 100
# Possible histogram paths depending on oh_cp options used by athenaHLT
rootHist = ["run_0/lb_-1/HLT-Histogramming/SHIFT/MTTHist/h1ftest"]

ret = os.system("athenaHLT.py -M -f %s -n%d MTMonHist.py" % (data, events))

if ret!=0:
   print("testMonHistOH ERROR: athenaHLT failed.")
   sys.exit(ret)

from ROOT import *
rootFile = glob.glob("*part_athenaHLT_mon_HLT-Histogramming.root")[0]
f = TFile(rootFile)
if f==None:
   print("testMonHistOH ERROR: Cannot open histogram file %s" % rootFile)
   sys.exit(1)

for name in rootHist:
   h = f.Get(name)
   if h!=None: break
   
if h==None:
   print("testMonHistOH ERROR: Cannot find histogram %s" % rootHist)
   sys.exit(1)

if h.GetEntries()==events:
   print("testMonHistOH INFO: Histogram entries match")
   sys.exit(0)
else:
   print("testMonHistOH ERROR: Found %d histogram entries, expected %d." % (h.GetEntries(), events))
   sys.exit(1)

   
