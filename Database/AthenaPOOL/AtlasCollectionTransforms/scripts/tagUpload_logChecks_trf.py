#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#########################################################################
##
## (U) Jack Cranshaw (Oct 2009)
##
#########################################################################

import sys

from AtlasCollectionTransforms.tagCopyTool import *

def logCheck(infile) :

  print " ... Processing ",infile
  #resultMsg = "First check if input/output are compatible \n"
  print ">>>"
  print ">>> BELOW IS FROM RUNNING THE CHECK"
  print ">>>"
  acrotxt = []
  dummymap = {}
  tool = tagCopyTool(dummymap)
  (level, acronym, retc, nevt) = tool.checkLog(infile)
  print ">>>"
  print ">>> BELOW THIS IS THE RETURNED ERROR CODE INFORMATION"
  print ">>>"
  if retc != 0:
    print "checkLog return values (level,retcode,acronym) = (%i,x%x,%s)\n" % (level, retc, acronym)
  else:
    print "No errors detected in log with current tagUpload.checkLog"
  print

logfile = ""
try: logfile = sys.argv[1]
except : 
  print "No input file name found"

if logfile != "":
  logCheck(logfile)   

