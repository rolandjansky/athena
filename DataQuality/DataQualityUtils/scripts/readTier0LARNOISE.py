#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Simple script to extract the path of the TAG outputs of Tier0 monitoring, 
# open them and chain them in a single TChain
# Uses the pathExtract library to extract the EOS path
# Options: 
#  -r RUNNUMBER, --run RUNNUMBER : Run number
#  -s STREAM, --stream STREAM    : Stream without prefix: express, CosmicCalo, Egamma...
#  -t TAG, --tag TAG             : DAQ tag: data12_8TeV, data12_calocomm...
#  -a AMITAG, --amiTag AMITAG    : First letter of AMI tag: x->express / f->bulk
# Author : Benjamin Trocme / Summer 2012

import os, sys  
import argparse

from DataQualityUtils import pathExtract         
import xmlrpclib

from ROOT import TFile,TBrowser,TChain
from ROOT import gStyle

gStyle.SetPalette(1)
gStyle.SetOptStat("em")
  

# Main===========================================================================================================
parser = argparse.ArgumentParser()
parser.add_argument('-r','--run',type=int,dest='runNumber',default='267599',help="Run number",action='store')
parser.add_argument('-s','--stream',dest='stream',default='express',help="Stream without prefix: express, CosmicCalo, Egamma...",action='store')
parser.add_argument('-t','--tag',dest='tag',default='',help="DAQ tag: data12_8TeV, data12_calocomm...By default retrieve it via atlasdqm",action='store')
parser.add_argument('-a','--amiTag',dest='amiTag',default='x',help="First letter of AMI tag: x->express / f->bulk",action='store')

parser.print_help()

args = parser.parse_args()

runNumber = args.runNumber
stream = args.stream
if args.tag != "":
  tag = args.tag
else: # Try to retrieve the data project tag via atlasdqm
  if (not os.path.isfile("atlasdqmpass.txt")):
    print "To retrieve the data project tag, you need to generate an atlasdqm key and store it in this directory as atlasdqmpass.txt (yourname:key)"
    print "To generate a kay, go here : https://atlasdqm.cern.ch/dqauth/"
    print "You can also define by hand the data project tag wit hthe option -t"
    sys.exit()
  passfile = open("atlasdqmpass.txt")
  passwd = passfile.read().strip(); passfile.close()
  passurl = 'https://%s@atlasdqm.cern.ch'%passwd
  s = xmlrpclib.ServerProxy(passurl)
  run_spec = {'stream': 'physics_CosmicCalo', 'proc_ver': 1,'source': 'tier0', 'low_run': runNumber, 'high_run':runNumber}
  run_info= s.get_run_information(run_spec)
  if '%d'%runNumber not in run_info.keys() or len(run_info['%d'%runNumber])<2:
    print "Unable to retrieve the data project tag via atlasdqm... Please double check your atlasdqmpass.txt or define it by hand with -t option"
    sys.exit()
  tag = run_info['%d'%runNumber][1]

amiTag = args.amiTag

listOfFiles = pathExtract.returnEosLArNoisePath(runNumber,stream,amiTag,tag)

tree = TChain("CollectionTree")

print listOfFiles
for fileNames in listOfFiles:
  print "Adding %s"%(fileNames)
  tree.AddFile("root://eosatlas/%s"%(fileNames))

entries = tree.GetEntries()
if entries != 0:
  print "The chained tree contains %d entries"%(entries)
else:
  print "Empty chain..."

