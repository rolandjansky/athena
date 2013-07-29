#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Transformation for the Tier-0 TAG extraction form RDB to file
##
##  - input: 
##    a. Colon separated list of input collections
##    b. Output file name
##       - assumed to be PFN, so don't drop the .root
##    c. Query to be executed on all input collections
##    d. Lumi string to be added to output file metadata
##    e. Atlas release (optional), e.g. 14.2.10,
##       - "takeFromEnv" will cause it to assume setup was done outside script
##    f. TAG DB connect string (optional)
##       - default is oracle://int8r/atlas_tags_loading_test
##
##  Example1: File would include events from collections fdr08_run1_MinBias 
##      and fdr08_run1_Muon with run number > 0.
##      % ExtractAtlas.py fdr08_run1_MinBias:fdr08_run1_Muon mydir/myevents.root 'RunNumber>0'
## 
## Uses Atlas (version = 13.0.0 + patches)
##
## (C) Jack Cranshaw, Luc Goossens, Armin Nairz (September 2006)
## (U) Jack Cranshaw (February 2008)
## (U) Jack Cranshaw (July 2008)
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time, subprocess

from AtlasCollectionTransforms.Tool_setup import *
from AtlasCollectionTransforms.BuildCmd_Extract import *

def TAGExtract(tagname, outname, query, luminfo, release, srcconnectstr, atts) :

  # Get command
  xmloutname =""
  executeLine = BuildExtractCmd(tagname, outname, query, luminfo, release, srcconnectstr,atts,xmloutname)
  print executeLine

  print "##################################################################"
  print "## STEP 3: running the executable ..." 
  print "##################################################################"
  print " "

  logfile = outname + '.log'
  cmd = executeLine + ' >& ' + logfile
  print cmd
  t1 = time.time()
  #print o
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
  (retcode2, msg) = commands.getstatusoutput("grep -i 'error:' "+logfile)
         
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
  cmd = "tail -n 100 "+logfile
  (s,o) = commands.getstatusoutput(cmd)
  print o
  #print retcode1, retcode2

  retcode = 0
  nevt = -1
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
  else:
    cmd = "grep \"total events appended to each output\" "+logfile
    (s,o) = commands.getstatusoutput(cmd)
    if s == 0 : nevt = int(o.split()[1])

  print " "
  print "##"
  print "##################################################################"
  print "## End of job."
  print "##################################################################"
    
  return (retcode, acronym, nevt, dttagdb) 

def TAGExtractAsync(tagname, outname, query, luminfo, release,srcconnectstr,atts,announceUrl,xmloutname) :

  # Get command
  executeLine = BuildCmd(tagname, outname, query, luminfo, release, srcconnectstr,atts,xmloutname)
  print executeLine

  print "##################################################################"
  print "## running the executable ..." 
  print "##################################################################"
  print " "

  cmd  = '/bin/rm -f ' + outname + " " + outname + '.status'
  cmd += ';' + executeLine + ' >& ' + outname + '.log'
  cmd += '; echo "OK" > ' + outname + '.status'
  cmd += '; wget -q "' + announceUrl + '"'
  
  print cmd
  t1 = time.time()
  #print o
  (retcode1, o) = commands.getstatusoutput(cmd)
  subprocess.Popen(cmd, shell=True, stdout=open(outname + '.out', 'w'), stderr=open(outname + '.err', 'w'))


  print " "
  print "##"
  print "##################################################################"
  print "## End of job."
  print "##################################################################"
    
  return ("retcode", "acronym", "nevt", "dttagdb") 

