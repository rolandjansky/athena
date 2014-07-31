#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os, os.path

## main
if __name__ == "__main__":

  if len(sys.argv) < 2:
    print "usage: ", sys.argv[0], "<xmlfile>"
    sys.exit(1)

  xmlfile=sys.argv[1]
  if not os.path.exists( xmlfile ):
    print "Cannot find file <%s>. Exit." % xmlfile
    sys.exit(1)

  cmdArr = []
  cmdfound = False
  lineArr = open(xmlfile,"r").readlines()

  for line in lineArr: 
    ## skip empty lines
    line = line.strip()
    if len(line)==0: continue

    if line.find("/jobTransformCmd")>0: 
      cmdfound = False
      continue
    if cmdfound: 
      arr = line.split("<!--") 
      cmdArr.append(arr[0])
      cmdfound = False
      continue
    if line.find("jobTransformCmd")>0: 
      cmdfound = True
      continue

  print "## runcmds found in <%s> : \n" % xmlfile
  for cmd in cmdArr:
    print "##"
    print cmd
    print "" 

