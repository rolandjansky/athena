#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

## *****************************************************************************
VERSION = '$Id: DQWebDisplay.py 690278 2015-08-19 22:18:53Z ponyisi $'
## *****************************************************************************

import os

## Needed to correct ROOT behavior; see below
CWD = os.getcwd()
os.chdir(CWD)

## Get usable signal handlers
os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS'] = '1'

import ROOT
import sys
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
from optparse import OptionParser
os.chdir(CWD)

parser = OptionParser()
parser.add_option("--htag", dest="htag",
                  help='Use configuration from a specified htag')

(options, args) = parser.parse_args()

from DataQualityUtils.DQWebDisplayMod import DQWebDisplay

def importConfiguration(modname, htag=None):
    from DataQualityConfigurations import getmodule
    print 'getting configuration', modname
    return getmodule(modname, options.htag)

def usage():
  #cmdi = args[0].rfind("/")
  #cmd = args[0][cmdi+1:]
  print ""
  print "Usage: DQWebDisplay.py <data_file> <config> <processing_version> [run_accumulating [conditions_string]] [--h htag]"
  print ""
  print "This is a production utility; use TEST config for development and testing."
  print ""
  print "Processing version is an integer, starting from 1 (not 0)"
  print ""

if __name__ == "__main__":

  print len(args)
  if len(args) < 5 or len(args) > 7:
    usage()
    sys.exit(64)
  
  inputFile  = args[0] # data file
  runAccumulating = False
  if len(args) in (6,7):
    if args[3] == "True" or args[3] == "1":
      runAccumulating = True
  
  if len(args) == 7:
      print 'Setting condition', args[4]
      ROOT.gSystem.Load('libDataQualityInterfaces')
      ROOT.dqi.ConditionsSingleton.getInstance().setCondition(args[4])

  parset = OptionParser

  configModule = ""
  
  if   args[1] == "TEST":
    configModule = "TestDisplay"
  elif args[1] == "RTT":
    configModule = "RTTDisplay"
  elif args[1] == "TCT":
    configModule = "TCTDisplay"
  elif args[1] == "FDR1":
    configModule = "fdr08_run1"
  elif args[1] == "FDR2" or args[2] == "FDR2a" or args[2] == "FDR2b" or args[2] == "FDR2c":
    configModule = "fdr08_run2"
  elif args[1] == "Cosmics08":
    configModule = "data08_cos"
  elif args[1] == "SingleBeam08":
    configModule = "data08_1beam"
  else:
    configModule = args[1]
  
  try:
    if options.htag:
      cmod = importConfiguration(configModule, options.htag)
    else:
      cmod = importConfiguration(configModule)  
  except Exception, e:
    print "Could not import configuration module \'" + configModule + "\'"
    sys.exit(1)

  try:
    if options.htag:
      config = cmod 
    else:
      config = cmod.dqconfig
  except Exception, e:
    print "Configuration object 'dqconfig' not defined in module \'" + configModule + "\'"
    sys.exit(1)


  DQWebDisplay( inputFile, runAccumulating, config )


