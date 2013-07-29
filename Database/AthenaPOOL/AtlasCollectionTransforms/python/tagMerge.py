#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## $Log: not supported by cvs2svn $
## Revision 1.4  2008/11/21 15:09:25  cranshaw
## Add POOLUtilityRun transform
##
## Revision 1.3  2008/11/20 21:01:45  cranshaw
## setup changes
##
## Revision 1.2  2008/11/13 14:26:04  cranshaw
## Merging branches for 14.4 and 14.5
##
## Revision 1.1.2.1  2008/11/11 15:53:11  cranshaw
## add flexibility
##
## Revision 1.1  2008/08/12 20:37:01  cranshaw
## Simplify transforms.
## - move meat of code into modules
## - rename transforms to *_trf
##
## Revision 1.1  2008/08/07 19:25:02  cranshaw
## Merged syntax for release for merge,upload,extract
## Atlas,AtlasPOOL,POOL are now arguments in scripts
##
## Revision 1.4  2008/08/05 16:12:55  cranshaw
## Fix tests
##
## Revision 1.3  2008/08/02 22:59:37  cranshaw
## Tests all exit with OK, now, in devval
##
## Revision 1.2  2008/08/02 00:27:53  cranshaw
## Fix problems for 64 bit builds
## Move setups to python directory
## Declare python scripts
## First workable atn tests
##
## Revision 1.1  2008/07/31 14:26:01  cranshaw
## Add merge script. Tests nearly ready.
##
## Revision 1.1  2008/07/30 15:23:08  cranshaw
## see ChangeLog
##
## Revision 1.1  2008/07/29 21:14:14  cranshaw
## Standardization, and first steps toward adding atn tests.
##
## Revision 1.3  2008/07/24 14:21:43  cranshaw
## Fixes for compilation
##
## Revision 1.3  2007/10/11 15:57:26  atlast0
## produce gpickle report
##
##
## @author $Author: cranshaw $
## @version $Revision: 1.5 $
##

#########################################################################
##
## Transformation for the Tier-0 merging of TAG files
##
##  - input: 
##    a. single TAG file  
##    b. TAG DB output destination (optional)
##       - default is project_physics if (a) follows nomenclature, otherwise take file name - extension  
##    c. Atlas release (optional), e.g. 14.2.10,
##       - "takeFromEnv" will cause it to assume setup was done outside script
##    d. TAG DB connect string (optional)
##       - default is oracle://int8r/atlas_tags_loading_test
##  - uploads TAGs to dedicated database
##  - no output data files
##  - creates gpickle report file
##
##  Example1: File would be loaded into table fdr08_run1_MinBias
##      % python ../tagMergeTest.py fdr08_run1.0003053.MinBias.merge.TAG.o1_r6_t2._0001.1 
##  Example2: File would be loaded into table t0tag
##      % python ../tagMergeTest.py fdr08_run1.0003053.MinBias.merge.TAG.o1_r6_t2._0001.1 t0tag 
## 
## (C) Jack Cranshaw, Luc Goossens, Armin Nairz 
##     (September 2006, October 2007, July 2008)
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time

from AtlasCollectionTransforms.Tool_setup import *
from AtlasCollectionTransforms.BuildCmd_Merge import *

def mergeTagDB(tagfile, outdest, release, query, atts) :
  #--------------------------------------------------------------------------
  # Job to upload TAG file for Tier0 testing
  # - uses rel. 12.0.6 + patches
  #--------------------------------------------------------------------------

  # run directory
  wcmd = "echo `pwd`"
  (s,o) = commands.getstatusoutput(wcmd)
  os.environ['BASEDIR'] = o

  # Get command
  xmloutname =""
  (executeLine,util) = BuildMergeCmd(tagfile, outdest, query, release, atts, xmloutname)
  print executeLine


  print " "
  print "##################################################################"
  print "## STEP 3: running the executable ..." 
  print "##################################################################"
  print " "
  print executeLine
  print " "


  cmd = "which " + util + " ; "
  cmd += executeLine + ' >& log'
  print cmd
  t1 = time.time()
  (retcode1, o) = commands.getstatusoutput(cmd)
  dttot = int(time.time() - t1)
  try : dtcmt = int(o[(o.find('CMTTIME = ') + 10):])
  except : dtcmt = 0
  dttagdb = dttot - dtcmt
  print
  print o
  txt1 = o
  if retcode1 != 0 : retcode1 = 62600

  print "##################################################################"
  print "## STEP 4: generating the return code ..." 
  print "##################################################################"
  print " "
  # check logfile for error messages
  (retcode2, o) = commands.getstatusoutput("grep -i 'error:' log")
  txt2 = o
  nevt = 0
  if retcode2 == 0 : 
    retcode2 = 62601  # "error" found
  else : 
    retcode2 = 0      # no "error" found 
    # determine number of processed (=uploaded) events
    cmd = "grep \"total events appended\" log"
    (s,o) = commands.getstatusoutput(cmd)
    if s == 0 : nevt = int(o.split()[1])

  print " "
  print "## ... job finished with (retcode1,retcode2) = (%s,%s)" % (retcode1, retcode2)
  print "## ... total elapsed time:        ", dttot, "sec"
  print "## ... CMT setup time:            ", dtcmt, "sec"
  print "## ... number of appended events: ", nevt
  print "## ... TAG uploading time:        ", dttagdb, "sec"

  # print logfile fragment
  print " "
  print "## ... logfile fragment"
  print "..."
  print "...skipping..."
  print "..."
  cmd = "tail -n 50 log"
  (s,o) = commands.getstatusoutput(cmd)
  print o

  retcode = 0
  acronym = 'OK'
  txt = 'trf finished OK'
  if retcode1 != 0 : 
    print "ERROR: commands.gso execution problem!"
    retcode = retcode1
    acronym = 'TRF_COMMANDSGSO_EXE'
    txt = 'commands.gso execution problem: ' + txt1

  elif retcode2 != 0 : 
    print "ERROR: Job execution problem!"
    retcode = retcode2
    acronym = 'TRF_EXEPROBLEM'
    txt = 'Job execution problem: ' + txt2

  print " "
  print "##"
  print "##################################################################"
  print "## End of job."
  print "##################################################################"
    
  return (retcode, acronym, txt, nevt, dttagdb) 

