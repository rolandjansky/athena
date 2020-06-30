#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import DataQualityUtils.hancoolmod as mod

import sys


def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print("")
  print("Usage: ", cmd, "<runnumber> <path> <dbConnection> <detstatus> <dbtag>")
  print("or")
  print("Usage: ", cmd, "(picking up default settings)")
  print("")


if __name__ == "__main__":

  if (not ((len(sys.argv) == 1) or (len(sys.argv) == 5) or (len(sys.argv) == 6))):
    usage()
    sys.exit(0)

  runNumber     = -1
  filePath      = ""
  dbConnection  = ""
  dqmfOfl       = ""
  db_tag = ""
  
  if (len(sys.argv) == 6):
      runNumber     = int(sys.argv[1]) # run number
      filePath      = sys.argv[2] # histograms file path
      dbConnection  = sys.argv[3] # COOL db connection string
      dqmfOfl       = sys.argv[4] # dqmfOfl
      db_tag        = sys.argv[5] # COOL db folder tag 

  elif (len(sys.argv) == 5):
      runNumber     = int(sys.argv[1]) # run number
      filePath      = sys.argv[2] # histograms file path
      dbConnection  = sys.argv[3] # COOL db connection string
      dqmfOfl       = sys.argv[4] # dqmfOfl
      db_tag = "HEAD"
      
  else: # set to defaults
      runNumber     = 52280
      filePath      = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/tier0/FDR2a/1/physics_Express/"
      dbConnection  = "sqlite://;schema=MyCOOL_histo.db;dbname=CONDBR2"
      dqmfOfl       = "/GLOBAL/DETSTATUS/DQMFOFL"
      db_tag = "HEAD"

  mod.hancool(runNumber,filePath,dbConnection,dqmfOfl, db_tag)
