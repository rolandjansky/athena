#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##################################################################
##  download.py
##----------------------------------------------------------------
##
##  Batch download of output datasets created by
##  jobs submitted by "submit.py".
##
##  Input datasets must be specified in the file "datasetsList.py"
##
##  Author: Daniel Scheirich <daniel.scheirich@cern.ch>
##
##  Part of B2JpsiV0 analysis algorithm
##
##################################################################

import string, os

##---------------------------------------------
##  Configuration
##---------------------------------------------

## Configure output dataset name
## Output dataset(s) are be named as follows:
##
##  userTag.tryNo.dataTag.runNo.runNo.alg
##
## runNo is automatically extracted from the name of the input datasets ( defined in "datasetsList.py" file)
## IMPORTANT! The settings must match those from "submit.py"

userTag           = "user10.DanielScheirich"      # put your user tag here
dataTag           = "data10_7TeV_MuonswBeam"      # string that describes the input data
alg               = "TriggerJpsiAnalysis"         # string that describes
tryNo             = "07"                          # must be unique for each execution of the job

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

  ## create DQ2 command

  command = "dq2-get " + userTag + "." + tryNo + "." + dataTag + "." +  runNo + "." + alg

  print command

  ## execute DQ2 command

  os.system( command )

## End of script
