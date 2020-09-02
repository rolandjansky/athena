#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os,sys
from datetime import datetime

from future import standard_library
standard_library.install_aliases()
import subprocess


def getFile(timestampfile):
  ## weblocation?
  if timestampfile.find("http")==0:
    tsfArr = timestampfile.split("/")
    tmpstampfile = tsfArr[len(tsfArr)-1]
    cmd = "export http_proxy=\"http://atlasgw.cern.ch:3128/\"; wget -O %s %s" % (tmpstampfile,timestampfile)
    s,o = subprocess.getstatusoutput(cmd)
    timestampfile = tmpstampfile

  ## file exists?
  if not os.path.exists( timestampfile ):
    print("ERROR : File <%s> does not exist. Exit." % timestampfile)
    sys.exit(1)

  return timestampfile


def readTimestamp(timestampfile):
  fileLine=open(timestampfile,"r").readlines()

  if len(fileLine)<2: 
    print("ERROR : Cannot interpret timestampfile <%s>. Exit." % timestampfile)
    sys.exit(1)

  timestamp = int(fileLine[0].strip())
  pacmankey = fileLine[1].strip()

  return timestamp,pacmankey


def interpretNewTimestamp(newtimestampfile,prvtimestampfile=""):

  datenow = int(datetime.today().strftime("%Y%m%d")) #("%Y%m%d.%H%M")

  ## interpret timestamps
  dateprv = -1
  if len(prvtimestampfile)>0:
    dateprv,prvpacmankey = readTimestamp(getFile(prvtimestampfile))
  datenew,newpacmankey = readTimestamp(getFile(newtimestampfile))

  doInstall = False
  if (datenew>dateprv) and (datenew==datenow):
    doInstall = True

  return doInstall,newpacmankey


def updatePreviousTimestamp(newtimestampfile,prvtimestampfile):
  cpcmd = "cp -f %s %s" % (getFile(newtimestampfile),prvtimestampfile)
  s,o = subprocess.getstatusoutput(cpcmd)
  return s


## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <newtimestampfile> [<previoustimestampfile>] [<updatePreviousTimestamp>]"
  if (len(sys.argv)<2):
    print(usageline)
    sys.exit(1)

  ## getopts
  newtimestampfile = sys.argv[1]
  prvtimestampfile = ""
  if (len(sys.argv)>2):
    prvtimestampfile = sys.argv[2]
  updateTimestamp = 0
  if (len(sys.argv)>3):
    updateTimestamp = int(sys.argv[3])

  ## interpret timestamp
  doInstall,pacmankey = interpretNewTimestamp(newtimestampfile,
                                              prvtimestampfile)
  if doInstall:
    print("Install new nightly kit ? %s" % str(doInstall))
    print(pacmankey)
  else:
    print("No need to install new nightly kit.")

  if doInstall and updateTimestamp:
    if len(prvtimestampfile)==0:
      prvtimestampfile="previous_copied_release"
    updatePreviousTimestamp(newtimestampfile,prvtimestampfile)
    print("Timestamp has been updated, stored as <%s>." % prvtimestampfile)

  print("\nErrorCode=0 (OK)")
