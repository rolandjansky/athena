#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import DataQualityUtils.DQHistogramMergeMod as mod
import sys, os

## Get usable signal handlers
os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS'] = '1'

def usage():
  cmd = sys.argv[0].split("/")[-1]
  print("Usage: ", cmd, "<input_list_file_name> <merged_file_name> [directory_regexp] [histogram_regexp] [run_post_processing [is_incremental_merge]]")
  print("If you don't give any regular expressions, this script will act like DQHistogramMerge.py <infilelist> <outfile> False")

########################################

if __name__ == "__main__":
  argc = len(sys.argv)

  if argc < 3 or argc > 7:
    usage()
    sys.exit(0)

  dirRegex = ".*"
  if argc >= 4:
    dirRegex = sys.argv[3]

  histRegex = ".*"
  if argc >= 5:
    histRegex = sys.argv[4]

  runPostProcessing = False
  if argc >= 6:
    runPostProcessing = sys.argv[5] in ["True", "1"]

  isIncremental = False
  if argc >= 7:
    isIncremental = sys.argv[6] in ["True", "1"]

  mod.DQHistogramMerge(sys.argv[1], sys.argv[2], runPostProcessing, dirRegex, histRegex, isIncremental=isIncremental)
