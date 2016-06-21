#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Simple script to extract the path of the HIST output of Tier0 monitoring, 
# open it and open a TBrowser
# Uses the pathExtract library to extract the EOS path
# Options: 
#  -r RUNNUMBER, --run RUNNUMBER : Run number
#  -s STREAM, --stream STREAM    : Stream without prefix: express, CosmicCalo, Egamma...
#  -t TAG, --tag TAG             : DAQ tag: data12_8TeV, data12_calocomm...
#  -a AMITAG, --amiTag AMITAG    : First letter of AMI tag: x->express / f->bulk
#
# Author : Benjamin Trocme (LPSC Grenoble) / Summer 2012, updated in 2015



import os, sys  
import argparse

import pathExtract         

from ROOT import *

# Main ================================================================
gStyle.SetPalette(1)
gStyle.SetOptStat("emuo")

parser = argparse.ArgumentParser()
parser.add_argument('-r','--run',type=int,dest='runNumber',default='267599',help="Run number",action='store')
parser.add_argument('-s','--stream',dest='stream',default='express',help="Stream without prefix: express, CosmicCalo, Egamma...",action='store')
parser.add_argument('-t','--tag',dest='tag',default='data16_13TeV',help="DAQ tag: data12_8TeV, data12_calocomm...",action='store')
parser.add_argument('-a','--amiTag',dest='amiTag',default='x',help="First letter of AMI tag: x->express / f->bulk",action='store')
parser.add_argument('-l','--lumiblock',type=int,dest='lumiblock',default='0',help="if none empty, try to get unmerged HIST files for a specific LB",action='store')

parser.print_help()

args = parser.parse_args()

run = args.runNumber
LB = args.lumiblock
stream = args.stream
tag = args.tag
amiTag = args.amiTag


path = []

if args.lumiblock == 0:
   path.append("root://eosatlas.cern.ch/%s"%(pathExtract.returnEosHistPath(args.runNumber,args.stream,args.amiTag,args.tag)).rstrip())
else:
   allUnmerged = pathExtract.returnEosHistPathLB(run,LB,LB,stream,amiTag,tag)
   for iFile in allUnmerged:
      path.append("root://eosatlas.cern.ch/%s"%(iFile).rstrip())
#   path.add("root://eosatlas.cern.ch/%s"%(.rstrip())


file = []
for iPath in path:
   if ("NO FILE" not in iPath):
      print "I am opening %s"%(iPath)
      file.append( TFile.Open(iPath))

gStyle.SetPalette(1)
tb = TBrowser()
