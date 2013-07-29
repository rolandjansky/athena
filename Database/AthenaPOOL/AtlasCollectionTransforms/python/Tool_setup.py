# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AtlasCollectionTransforms.ATLAS_setup import *

import sys, os

def parseConfig(util,release):

  # if not external, check the release argument
  config = release.split(",",2)
  source = ""
  rel = ""
  work = ""
  cmd = ""
  # are there enough args ... must be at least 1 /
  if len(config) < 2:
    print "Too little information in release = ",release
    print " ==> defaulting to takeFromEnv"
    source="AtlasPOOL"
  # if there are 2 arguments, then assume no workarea
  elif len(config) < 3:
    source=config[0]
    rel=config[1]
    if rel != "takeFromEnv":
      print "Currently only support takeFromEnv option. Setting rel to that from ",rel
    rel="takeFromEnv"
  elif len(config) < 4:
    source=config[0]
    rel=config[1]
    if rel != "takeFromEnv":
      print "Currently only support takeFromEnv option. Setting rel to that from ",rel
    rel="takeFromEnv"
    #rel=config[1]
    #work = config[2]

  # Now look at soure,rel,work values
  if source=="Atlas":
    util += ".exe"
    # check if setup was done externally
    if rel != "takeFromEnv": 
      cmd = atlas_setup_14(work,rel)
      print "INFO: Using Atlas release ",rel
      return ("OK",util,cmd)
    else: 
      print "Using Atlas release from run environment"
      return ("OK",util,cmd)
  elif source=="AtlasPOOL":
    # check if setup was done externally
    if rel != "takeFromEnv": 
      cmd = atlas_setup_14(work,rel)
      print "INFO: Using POOL from Atlas release ",rel
      return ("OK",util,cmd)
    else: 
      print "Using Atlas release from run environment"
      return ("OK",util,cmd)
  else:
    print "Error: Unrecognized source, ",source, " ---> ABORTING"
    return ("UNRECOGNIZED",util,cmd)


