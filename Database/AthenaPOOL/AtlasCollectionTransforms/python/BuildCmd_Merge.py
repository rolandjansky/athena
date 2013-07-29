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

#import sys, string, os

from AtlasCollectionTransforms.Tool_setup import *

def BuildMergeCmd(tagfile, outdest, query, release, atts, xmloutname, metaopt='ALL') :

  print " "
  print " "
  print "###################   ATLAS Tier0 TagDB Loading   ################"
  print "##"
  print "##################################################################"
  print "## STEP 1: setting up the run environment ..." 
  print "##################################################################"
  print " "

  print 'Collection name = ', outdest
  
  # set base util name
  util="CollAppend"

  # decode release and modify util accordingly
  (code,util,cmd) = parseConfig(util,release)

  # check that cmd,util are ready
  if code != "OK": return (1,code,0)

  print "##################################################################"
  print "## STEP 2: preparing the run ..." 
  print "##################################################################"
  print " "
  # assemble the run command
  coll = tagfile
  executeLine = util + " -src"
  executeLine += " PFN:" + coll
  executeLine += " RootCollection "
  executeLine += " -merge "
  executeLine += " -metadata this that "
  executeLine += "-dst PFN:" + outdest + " RootCollection "
  executeLine += " -nevtcached 5000"
  executeLine += " -metaopt " + metaopt
  if query != "":
    executeLine += " -query '" + query +"'"
  if atts != "*":
    executeLine += " -queryopt \"" + atts + "\""
  if xmloutname != "":
    executeLine += " -xmlOut \"" + xmloutname + "\""

  return (executeLine,util)
