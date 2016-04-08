#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##################################################################
##  submit.py
##----------------------------------------------------------------
##
##  Batch submit of pathena jobs.
##
##  Input datasets must be specified in the file "datasetsList.py"
##
##  Author: Daniel Scheirich <daniel.scheirich@cern.ch>
##
##  Part of TrigBphysValidation package
##
##################################################################

import string, os

##---------------------------------------------
##  Configuration
##---------------------------------------------

jobO              = "BPhysAnalysisMasterAuto.py"  # master job option
algOutputFile     = "bphys.root"                  # name of the output file created by the algorithm

## Configure output dataset name
## Output dataset(s) will be named as follows:
##
##  userTag.tryNo.dataTag.runNo.runNo.alg
##
## runNo is automatically extracted from the name of the input datasets ( defined in "datasetsList.py" file)

userTag           = "user.scheiric"               # put your user tag here
dataTag           = "data10_7TeV_MuonswBeam"      # string that describes the input data
alg               = "TrigDiMuonCalib"             # string that describes algorithm
tryNo             = "14r"                          # must be unique for each execution of the job

## Further pathena configuration
##
##  some useful options:
##    --nFilesPerJob=2
##    --nEventsPerJob=100
##    --nFiles=1
##    --site=TRIUMF
##    --split=1
##    --excludedSite=ANALY_AGLT2
##
##  Just copy these strings into "config" variable

config            = " --dbRelease=LATEST --nFilesPerJob=2 --excludedSite=ANALY_SARA "

##---------------------------------------------
##  Import list of datasets
##---------------------------------------------
import datasetsList

datasets = datasetsList.datasets

##---------------------------------------------
##  Script itself
##---------------------------------------------

## loop over datasets

for dataset in datasets :

  print "Working on ", dataset

  ## get run number
  runNo = dataset.split(".",3)[1]

  ## create pathena command

  command  = "pathena " + jobO + " --noBuild " + " --supStream GLOBAL,AOD " + config + "  "
  command += "--inDS " + dataset + "  "
  command += "--outDS " + userTag + "." + tryNo + "." + dataTag + "." +  runNo + "." + alg + "  "
  command += "--extOutFile " + algOutputFile

  print command

  ## execute pathena command

  os.system( command )

## End of the script
