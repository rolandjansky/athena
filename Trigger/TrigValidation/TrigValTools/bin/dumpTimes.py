#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file:    dumpTimes.py
# @purpose: Script to dump the timing histograms from expert-monitoring.root/TIMERS
# @author:  Stewart Martin-Haugh

import argparse
import ROOT
from optparse import OptionParser
import os
import re

def timing(hist):
  mean = hist.GetMean()
  error = hist.GetMeanError()
  if hist.GetEntries() == 0:
    return "0"
  mean_plus_err = str(round(mean,3)) + " +/- " + str(round(error,3))
  overflow =  str(hist.GetBinContent(hist.GetNbinsX()+1))
  if overflow != 0.0:
    return mean_plus_err
  else:
    return mean_plus_err + "\nOverflow: " + overflow


def match(pattern, name):
  regex = re.compile(pattern)
  return regex.match(name)

def get_matches(pattern, myFile, matches):
  for key in ROOT.gDirectory.GetListOfKeys():
    if match(pattern,key.GetName())!=None:
      hist = myFile.Get("TIMERS/" + key.GetName())
      print myFile.GetName() + " " + key.GetName() + " " + timing(hist)
      matches.append(key.GetName())

def main():
  parser = OptionParser()
  parser.add_option("-H", "--hist", dest="hist_name", type = "string", 
                                                                      help="Histogram to time")
  parser.add_option("-p", "--pattern", dest="pattern", type = "string", default = None,
                                                                      help="Pattern to match histogram to")
  (options, args) = parser.parse_args()        
  print options, args
  for arg in args:
    print ""
    myFile = ROOT.TFile(arg)
    if (options.hist_name!=None):
      hist = myFile.Get("TIMERS/" + options.hist_name)
      print myFile.GetName() + " " + options.hist_name + " " + timing(hist)
    elif (options.pattern!=None):
      ROOT.gDirectory.Cd("TIMERS")
      matches = []
      get_matches(options.pattern, myFile, matches)
      if (len(matches)==0):
        get_matches(".*" + options.pattern + ".*", myFile, matches)
    else:
      ROOT.gDirectory.Cd("TIMERS")
      for key in ROOT.gDirectory.GetListOfKeys():
          hist = myFile.Get("TIMERS/" + key.GetName())
          print myFile.GetName() + " " + key.GetName() + " " + timing(hist)
          
    
if __name__ == "__main__":
  main()
