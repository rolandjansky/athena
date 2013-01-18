#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys

# --------- Function definition --------- #

def alleps2gif(setupok,keepeps=True,tareps=True,tardir="MuonRecRTT_epsimages"):

  if not setupok:
    print "Usage: " + sys.argv[0] + " [directory]"
    return

  old = ".eps"
  new = ".gif"

  nofiles = True

  for epsname in os.listdir("."):
    if epsname.count(old):
      nofiles = False
      gifname = epsname.replace(old,new)
      commands = [ "pstopnm -ppm -xborder 0 -yborder 0 -portrait -xsize=1000 " + epsname ,
                 "ppmtogif " + epsname + "001.ppm > " + gifname ,
                 "rm " + epsname + "001.ppm" ]
      if not keepeps:
        commands.append("rm " + epsname)
      for command in commands:
        print "Running: " + command
        os.system(command)

  if keepeps and tareps:
    commands = [ "mkdir " + tardir ,
                 "mv *" + old + " " + tardir ,
                 "tar -czf " + tardir + ".tar.gz " + tardir ,
                 "rm -rf " + tardir ]
    for command in commands:
      print "Running: " + command
      os.system(command)

  if nofiles:
    print "No " + old + " files in directory."

# -------- Start of running is here ------- #

setupok = True

if len(sys.argv) > 1:
  if len(sys.argv) > 2:
    setupok = False
  else:
    print "Changing to directory: " + sys.argv[1]
    try:
      os.chdir(sys.argv[1])
    except:
      print "Move to directory " + sys.argv[1] + " failed."
      setupok = False
else:
  print "Running in current directory."

alleps2gif(setupok)

#  sprintf(comm1,"%s %s","pstopnm -ppm -xborder 0 -yborder 0 -portrait",epsname);
#  sprintf(comm2,"%s %s%s %s","ppmtogif",epsname,"001.ppm >",gifname);
#  sprintf(comm3,"rm %s",epsname);
#  sprintf(comm4,"rm %s001.ppm",epsname);

