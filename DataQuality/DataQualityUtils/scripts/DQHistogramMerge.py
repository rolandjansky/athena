#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import DataQualityUtils.DQHistogramMergeMod as mod
import sys, os

## Get usable signal handlers
os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS'] = '1'

def usage():
  cmd = sys.argv[0].split("/")[-1]
  print ("Usage: ", cmd, "<input_list_file_name> <merged_file_name> [run_post_processing] [is_incremental_merge] [output_file_compression_level] [debugLevel]")
  print ("  run_post_processing False/True/0/1 default=0")
  print ("  is_incremental_merge False/True/0/1 default=0")
  print ("  output_file_compression_level see ROOT TFile doc. default=1")
  print ("  debugLevel integer default=0")
  print ("  You need to pass all preceding parameters to set a parameter.")

########################################

if __name__ == "__main__":
  argc = len(sys.argv)

  if argc < 3 or argc > 7:
    usage()
    sys.exit(0)

  runPostProcessing = False
  if argc >= 4:
    runPostProcessing = sys.argv[3] in ["True", "1"]

  isIncremental = False
  if argc >= 5:
    isIncremental = sys.argv[4] in ["True", "1"]

  compressionLevel = 1
  if argc >= 6:
    compressionLevel = int(sys.argv[5])

  debugLevel = 0
  if argc >= 7:
    debugLevel = int(sys.argv[6])

  mod.DQHistogramMerge(sys.argv[1], sys.argv[2], runPostProcessing, isIncremental=isIncremental, compressionLevel=compressionLevel, debugLevel=debugLevel)
