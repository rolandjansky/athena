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

userTag           = "user.scheiric"                                             # put your user tag here
dataTag           = "data10_7TeV_Muons_DetStatus-v03-pass1-analysis-2010F"      # string that describes the input data
alg               = "TrigJpsiAnalysis"                                          # string that describes algorithm
tryNo             = "01"                                                        # must be unique for each execution of the job

## Further pathena configuration
##
##  some useful options:
##    --nFilesPerJob=2
##    --nEventsPerJob=100
##    --nFiles=1
##    --site=TRIUMF
##    --split=1
##
##  Just copy these strings into "config" variable

config = ''
config += ' --nFilesPerJob=4 '
config += '  --dbRelease=LATEST '
config += ' --excludedSite=ANALY_TECHNION-HEP,ANALY_AUSTRALIA,ANALY_TR-10-ULAKBIM,ANALY_VICTORIA-WG1,ANALY_IFAE,ANALY_HU_ATLAS_Tier2 '

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

  print "\n"
  print "------------------------------------------------------------"
  print " Working on ", dataset
  print "------------------------------------------------------------"

  ## get run number
  runNo = dataset.split(".",3)[1]

  ## create pathena command

  command  = "pathena " + jobO + " --noBuild " + " --supStream GLOBAL " + config + "  "
  command += "--inDS " + dataset + "  "
  command += "--outDS " + userTag + "." + tryNo + "." + dataTag + "." +  runNo + "." + alg + "  "
  command += "--excludeFile WorkArea/run,WorkArea/rootScripts  "
  command += "--extOutFile " + algOutputFile

  print command

  ## execute pathena command

  os.system( command )

## End of the script
