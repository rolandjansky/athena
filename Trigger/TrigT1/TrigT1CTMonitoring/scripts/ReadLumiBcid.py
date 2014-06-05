#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,getopt,os, math
from PyCool import cool,coral
from ROOT import TH1F, TH1D,TFile, TProfile, TObject, TGraph
import CoolHelper

def usage():
    print "%s" % sys.argv[0]
    print "  --help                         this printout"
    print "  --run <runNo>                  run number"

try:
    longopts=['run=','help']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

run=int(-1)
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--run':
        run = int(a)

if (run == -1) : 
    usage()
    sys.exit(2)

lbList = CoolHelper.readLumiBCID(run)
fileName = "BcidLumi_Run"
fileName += str(run)
fileName += ".root"
hAll = TH1F("TotalLumi","Total raw lumi;BCID;Lumi",3564,-0.5,3563.5)
histList = []
for lb in lbList.keys():
    hName = "Lumi_lb"
    hName += str(int(lb))
    hTitle = "Raw lumi LB "
    hTitle += str(int(lb))
    hTitle += ";BCID;Lumi"
    h = TH1F(hName,hTitle,3564,-0.5,3563.5)
    for bcid in lbList[lb].keys():
        hAll.Fill(bcid,lbList[lb][bcid])
        h.Fill(bcid,lbList[lb][bcid])
    histList.append(h)

tfile = TFile(fileName,"recreate")
hAll.Write()
for h in histList:
    h.Write()
tfile.Close()
