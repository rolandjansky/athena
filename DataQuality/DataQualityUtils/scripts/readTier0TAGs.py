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

import pathExtract         

from ROOT import *

#gROOT.Reset()
gStyle.SetPalette(1)
gStyle.SetOptStat("em")
  

# Main===========================================================================================================
parser = argparse.ArgumentParser()
parser.add_argument('-r','--run',type=int,dest='runNumber',default='267599',help="Run number",action='store')
parser.add_argument('-s','--stream',dest='stream',default='express',help="Stream without prefix: express, CosmicCalo, Egamma...",action='store')
parser.add_argument('-t','--tag',dest='tag',default='data15_13TeV',help="DAQ tag: data12_8TeV, data12_calocomm...",action='store')
parser.add_argument('-a','--amiTag',dest='amiTag',default='f',help="First letter of AMI tag: x->express / f->bulk",action='store')

parser.print_help()

args = parser.parse_args()

run = args.runNumber
stream = args.stream
tag = args.tag
amiTag = args.amiTag

listOfFiles = pathExtract.returnEosTagPath(run,stream,amiTag,tag)

tree = TChain("POOLCollectionTree")

file = {}
for fileNames in listOfFiles:
  print "Adding %s"%(fileNames)
  tree.AddFile("root://eosatlas.cern.ch/%s"%(fileNames))

entries = tree.GetEntries()
if entries != 0:
  print "The chained tree contains %d entries"%(entries)
else:
  print "Empty chain..."

