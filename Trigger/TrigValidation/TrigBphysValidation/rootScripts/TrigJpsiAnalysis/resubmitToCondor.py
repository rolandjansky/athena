#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string, os

##---------------------------------------------
##  Configuration
##---------------------------------------------

inFile  = "bphys.root"
jobName = "job08"
nJobs   = 123


##---------------------------------------------
##  Script itself
##---------------------------------------------

## loop over files

command = "hadd " + outFile

for i in range(0,nJobs) :
  file = jobName + "_" + `i` + "/" + inFile
  if os.access("./"+jobName + "_" + `i` + "/" + inFile , os.F_OK) :
    command += " " + file
  else:
    print "Resubmitting " + jobName + "_" + `i`

    os.system( "cd "+jobName + "_" + `i` + "; ~daits/bin/mybat " + jobName + "_" + `i` + ".sh" )


## End of script
