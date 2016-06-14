#!/usr/bin/env python                                                                                                                                                                                      

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#by Ryan M. White modified by Patrick Czodrowski                                                                                                                                                           
#addiional input from dpiparo (Danilo Piparo): https://root.cern.ch/phpBB3/viewtopic.php?t=18684 

# usage: ./monHistoChecker.py  --file myNTUP_PHYSVAL.root --path /run_1/HLT/TauMon 
# we have (as of now)
# BjetMon 
# BphysMon 
# IDMon 
# JetMon 
# METMon 
# MinBiasMon 
# MuonMon 
# ResultMon 
# TauMon 


import os
if 'xterm' in os.environ['TERM']:
    os.environ['TERM'] = 'vt100'
#import glob
#import commands
#import subprocess
#import string
#import locale
import fnmatch
from sys import argv, exit
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--file', type=str,help='input file')
parser.add_argument('--path', type=str,help='path to desired TDirectory - example: ')
args = parser.parse_args()

myfile=""
for file in os.listdir('.'):
    if fnmatch.fnmatch(file, 'expert-monitoring.root'):
        f=open(file,"r")
        myfile=file

if not args.file:
    args.file=myfile

if not args.file:
    print "You must either have expert-monitoring.root in your working directory or set an input via --file -->> example usage: ./monHistoChecker.py --file myNTUP_PHYSVAL.root --path /run_1/HLT/TauMon or just up to HLT for full TrigMon dump"
    exit()

if not args.path:
    print "You must set --path -->> example usage: ./monHistoChecker.py --file myNTUP_PHYSVAL.root --path /run_1/HLT/TauMon or just up to HLT for full TrigMon dump"
    exit()

from ROOT import *


class looperClass :
  def __init__(self, filename, path) :
    self.init(filename, path)

  def init(self, filename, path) :
    f = TFile(filename)
    f.cd(path)
    keys = gDirectory.GetListOfKeys()
    for key in keys :
      if "metadata" in key.GetName():continue
      obj = key.ReadObj()
#      if "TTree" in obj.ClassName():print "TTree detected - skipping"
#      if "TTree" in obj.ClassName():continue
      if "TDirectoryFile" in obj.ClassName(): 
          l = looperClass(args.file,path+"/"+obj.GetName())
      else:
          hist = key.ReadObj()
#          print hist.ClassName()
#          print hist.Integral()
#          skip zero integral print outs
          if not hist.Integral() == 0.0:
              print path+"/"+obj.GetName()+"    "+str(hist.Integral())
#              for debugging only - print also the Class of the object in question - TTrees are not loved
#              print hist.ClassName()+" "+path+"/"+obj.GetName()+"    "+str(hist.Integral())

if __name__ == "__main__" :
    l = looperClass(args.file,args.path)
#  print "done"
