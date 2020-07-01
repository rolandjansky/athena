#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import DataQualityUtils.hancool_histo_mod as mod

import sys


##-------------------------------------------------------------

def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print("")
  print("Usage: ", cmd, "<runnumber> <path> <dbConnection> <detstatus> <dbtag>")
  print("or")
  print("Usage: ", cmd, "(picking up default settings)")
  print("")


########################################
## main()
########################################

if __name__ == "__main__":

  if (not ((len(sys.argv) == 1) or (len(sys.argv) == 6) or (len(sys.argv) == 3))):
    usage()
    sys.exit(0)
  
    inputFilePath = ""
    input_run = -1
    dbConnectionHisto  = ""
    dqmfOflHisto       = ""
    dbTagName = ""

  # Input

  if (len(sys.argv) == 6):
      inputFilePath=sys.argv[1] # path name 
      input_run = sys.argv[2] # run number 
      dbConnectionHisto  = sys.argv[3] # COOL db connection string
      dqmfOflHisto       = sys.argv[4] # dqmfOfl
      dbTagName          = sys.argv[5] # dbTagName
      
  elif (len(sys.argv) == 3):
      inputFilePath=sys.argv[1] # path name 
      input_run = sys.argv[2] # run number 
#      dbConnectionHisto  = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMFTutorial_2/14.0.0/WorkArea/MyCOOL_histo_FDR2b.db;dbname=OFLP200"
      dbConnectionHisto  = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMF_08112008/14.2.10/WorkArea/python/MyCOOL_histo_FDR2c.db;dbname=OFLP200"
      dqmfOflHisto       = "/GLOBAL/DETSTATUS/DQMFOFLH"
      dbTagName          = "DetStatusDQMFOFLH-FDR2-01"
      
  else: # set to defaults
      input_run    = 52280
      inputFilePath = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/han_results/tier0/FDR2b/1/physics_Express/"
#      dbConnectionHisto  = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMFTutorial_2/14.0.0/WorkArea/MyCOOL_histo_FDR2b.db;dbname=OFLP200"
      dbConnectionHisto  = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMF_08112008/14.2.10/WorkArea/python/MyCOOL_histo_FDR2c.db;dbname=OFLP200"
      dqmfOflHisto       = "/GLOBAL/DETSTATUS/DQMFOFLH"
      dbTagName          = "DetStatusDQMFOFLH-FDR2-02"
      
  mod.hancool_histo( inputFilePath, input_run, dbConnectionHisto,dqmfOflHisto, dbTagName)
