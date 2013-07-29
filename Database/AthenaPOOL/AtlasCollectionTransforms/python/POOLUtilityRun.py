#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Transformation for running one of the POOL utilities based 
##   on a configuration in an xml file.
##
##  - input: 
##    a. Name of utility, e.g. CollAppend 
##       (WARNING: DO NOT ADD EXTENSIONS EVEN IF YOU THINK YOU KNOW THEM)
##    b. XML file name, e.g. CollListFileGUID.exe.xml
##
## Uses Atlas (version = 14.4.0 + patches)
##
## (C) Jack Cranshaw (November 2008)
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time

from AtlasCollectionTransforms.Tool_setup import *

def POOLUtilityRun(util,xmlFile,release,xmlout="POOLUtilityRun-Args.xml",outdest="out.txt") :
  #--------------------------------------------------------------------------
  # Job to upload TAG file for Tier0 testing
  # - uses rel. 12.0.0 + patches
  #--------------------------------------------------------------------------
  print " "
  print " "
  print "###################   ATLAS Tier0 TagDB Loading   ################"
  print "##"
  print "##################################################################"
  print "## STEP 1: setting up the run environment ..." 
  print "##################################################################"
  print " "

  # build the setup cmd based on the release config
  (code,util,cmd) = parseConfig(util,release)

  # check that cmd,util are ready
  if code != "OK": return (1,code,0)

  executeLine = cmd

  print "##################################################################"
  print "## STEP 2: preparing the run ..." 
  print "##################################################################"
  print " "
  # assemble the run command
  executeLine += util + " -xmlInput " + xmlFile
  if xmlout != "":
    executeLine += " -xmlOut \"" + xmlout + "\""

  print executeLine

  print "##################################################################"
  print "## STEP 3: running the executable ..." 
  print "##################################################################"
  print " "

  cmd = executeLine + ' >& ' + outdest
  print cmd
  t1 = time.time()
  (xhk,o) = commands.getstatusoutput('which ' + util )
  print o
  (retcode1, o) = commands.getstatusoutput(cmd)
  dttot = int(time.time() - t1)
  try : dtcmt = int(o[(o.find('CMTTIME = ') + 10):])
  except : dtcmt = 0
  dttagdb = dttot - dtcmt
  print
  print o

  print "##################################################################"
  print "## STEP 4: generating the return code ..." 
  print "##################################################################"
  print " "
  # check logfile for error messages
  (retcode2, msg) = commands.getstatusoutput("grep -i 'error:' " + outdest)
         
  print " "
  print "## ... job finished with (retcode1,retcode2) = (%s,%s)" % (retcode1, retcode2)
  print "## ... total elapsed time: ", dttot, " sec"
  print "## ... CMT setup time:     ", dtcmt, " sec"
  print "## ... TAG uploading time: ", dttagdb, " sec"

  # print logfile fragment
  print " "
  print "## ... logfile fragment"
  print "..."
  print "...skipping..."
  print "..."
  cmd = "tail -n 100 " + outdest
  (s,o) = commands.getstatusoutput(cmd)
  print o
  print retcode1, retcode2

  retcode = 0
  acronym = 'OK'
  bad1 = (retcode1 != 0)
  bad2 = (retcode2 != 256 and retcode2 != 0)
  bad3 = msg != ''
  print bad1," ",bad2," ",bad3
  bad = bad1 or bad2 or bad3
  #if retcode1 != 0  or  retcode2 != 256 :
  #if retcode1 != 0  or  (retcode2 != 256 and retcode2 != 0) or msg != '' :
  if bad:
    # general problem or error message found in log file
    print "ERROR: Setup or job execution problem!"
    retcode = 62600
    acronym = 'EXEPROBLEM'

  print " "
  print "##"
  print "##################################################################"
  print "## End of job."
  print "##################################################################"
    
  return (retcode, acronym, dttagdb) 

