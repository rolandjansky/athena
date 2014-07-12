#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import DataQualityUtils.historyDisplayMod as mod

import sys


def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print ""
  print "Usage: ", cmd, " ''<run list xml >'' <output html dir> <dbConnection> <detstatus> <config>"
  print ""
  print " This is a test"


########################################
## main()
########################################


if __name__ == "__main__":

  # Input
  

  if (len(sys.argv) ==7 ):
      xml=sys.argv[1] # xml list
      htmlDirectory = sys.argv[2] # output html directory
      dbConnectionHisto = sys.argv[3] # Cool DB connection string 
      dqmfOflHisto = sys.argv[4] # DB Histo folder
      dbTagName = sys.argv[5] # DB Folder tag name 
      configuration = sys.argv[6]

  elif (len(sys.argv) == 3):
      xml=sys.argv[1] # xml list
      htmlDirectory = sys.argv[2] # output html directory
#      dbConnectionHisto = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMFTutorial_2/14.0.0/WorkArea/MyCOOL_histo_FDR2b.db;dbname=OFLP200"
      dbConnectionHisto = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio//DQMF_08112008/14.2.10/WorkArea/python/MyCOOL_histo_FDR2c.db;dbname=OFLP200"
      dqmfOflHisto =  "/GLOBAL/DETSTATUS/DQMFOFLH"
      dbTagName = "DetStatusDQMFOFLH-FDR2-02"
      configuration = "TEST"
  else: 
      usage()
      sys.exit(0)

#      xml="/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/tier0/FDR2a/runlist_FDR2a.xml"
#      htmlDirectory = "./"
#      dbConnectionHisto = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMFTutorial/14.0.0/MyCOOL_histo.db;dbname=COMP200"
#      dqmfOflHisto =  "/GLOBAL/DETSTATUS/DQMFOFLH"
#      configuration = "TEST"

# check of CPU time (test-mode only)
  if  (configuration.find('TEST')>-1):
      start = cpu_time()
      mod.historyDisplay( xml, htmlDirectory, dbConnectionHisto, dqmfOflHisto, dbTagName, configuration )
      dt = cpu_time() - start
      print dt
  else:
      mod.historyDisplay( xml, htmlDirectory, dbConnectionHisto, dqmfOflHisto, dbTagName, configuration )    
