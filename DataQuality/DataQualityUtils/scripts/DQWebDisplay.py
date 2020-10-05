#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os

import logging
logging.basicConfig()

## Needed to correct ROOT behavior; see below
CWD = os.getcwd()
os.chdir(CWD)

## Get usable signal handlers
os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS'] = '1'

import sys
sys.argv.append( '-b-' )
import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem  # noqa: F401
os.chdir(CWD)

from DataQualityUtils.DQWebDisplayMod import DQWebDisplay

def importConfiguration(modname):
    from DataQualityConfigurations import getmodule
    print('getting configuration', modname)
    return getmodule(modname)

def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print("")
  print("Usage: ", cmd, "<data_file> <config> <processing_version> [run_accumulating [conditions_string]]")
  print("")
  print("This is a production utility; use TEST config for development and testing.")
  print("")
  print("Processing version is an integer, starting from 1 (not 0)")
  print("")

if __name__ == "__main__":
  print(len(sys.argv))
  if len(sys.argv) < 5 or len(sys.argv) > 7:
    usage()
    sys.exit(64)
  
  inputFile  = sys.argv[1] # data file
  runAccumulating = False
  if len(sys.argv) in (6,7):
    if sys.argv[4] == "True" or sys.argv[4] == "1":
      runAccumulating = True
  
  if len(sys.argv) == 7:
      print('Setting condition', sys.argv[5])
      ROOT.gSystem.Load('libDataQualityInterfaces')
      ROOT.dqi.ConditionsSingleton.getInstance().setCondition(sys.argv[5])

  configModule = ""
  
  if   sys.argv[2] == "TEST":
    configModule = "TestDisplay"
  elif sys.argv[2] == "RTT":
    configModule = "RTTDisplay"
  elif sys.argv[2] == "TCT":
    configModule = "TCTDisplay"
  elif sys.argv[2] == "FDR1":
    configModule = "fdr08_run1"
  elif sys.argv[2] == "FDR2" or sys.argv[2] == "FDR2a" or sys.argv[2] == "FDR2b" or sys.argv[2] == "FDR2c":
    configModule = "fdr08_run2"
  elif sys.argv[2] == "Cosmics08":
    configModule = "data08_cos"
  elif sys.argv[2] == "SingleBeam08":
    configModule = "data08_1beam"
  else:
    configModule = sys.argv[2]
  
  try:
    cmod = importConfiguration(configModule)
  except Exception as e:
    print("Could not import configuration module \'" + configModule + "\'")
    sys.exit(1)

  try:
    config = cmod.dqconfig
  except Exception as e:
    print("Configuration object 'dqconfig' not defined in module \'" + configModule + "\'")
    sys.exit(1)

  
  DQWebDisplay( inputFile, runAccumulating, config )
