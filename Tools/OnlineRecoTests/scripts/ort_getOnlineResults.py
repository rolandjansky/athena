#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 
import commands, pickle, sys, string, os
from datetime import datetime
from OnlineRecoTests.ort_interpretTimestamp import *
import time


def waitForOnlineResults(webtimestampURL):
  datenow = int(datetime.today().strftime("%Y%m%d")) #("%Y%m%d.%H%M")

  fileList = []
  onlineTestsFinished = False

  ## wait till online tests finished
  while (not onlineTestsFinished):
    webtimestamp = getFile(webtimestampURL)
    if os.path.exists( webtimestamp ):
      if os.path.getsize( webtimestamp )>0:
        fileList = open(webtimestamp,"r").readlines()
        timestamp = int(fileList[0].strip())
        if (timestamp==datenow): ## ready 
          onlineTestsFinished=True
      else: os.remove( webtimestamp ) 
    if (not onlineTestsFinished):
      print "%s : Now sleeping one hour ..." % datetime.today().strftime("%Y%m%d.%H%M")
      time.sleep(3600)

  return onlineTestsFinished


def getWebfiles(weblistfile):

  weblistfile = getFile(weblistfile)
  fileList = open(weblistfile,"r").readlines()

  for webfile in fileList:
    webfile = webfile.strip()
    ## skip obvious lines
    if len(webfile)==0: continue
    if webfile.find("#")==0: continue
    if webfile.find("http")!=0: continue
    ## retrieve files
    print "Now retreiving : %s" % webfile
    cmd = "wget %s" % webfile
    o,s=commands.getstatusoutput(cmd)

  return 


def printLogfiles(weblistfile):

  weblistfile = getFile(weblistfile)
  fileList = open(weblistfile,"r").readlines()

  for webfile in fileList:
    webfile = webfile.strip()
    ## skip obvious lines
    if len(webfile)==0: continue
    if webfile.find("#")==0: continue
    if webfile.find("http")!=0: continue
    if not webfile.endswith(".log"): continue
    ## print logfiles
    logArr = webfile.split("/")
    logFile = logArr[len(logArr)-1]

    print "\n----------------------------------------------------------------------"
    print "> Now printing logfile : %s" % logFile
    print "----------------------------------------------------------------------\n"
    #cmd = 'cat %s' % logFile
    #os.system(cmd)
    logLines = open(logFile,"r").readlines()
    for line in logLines: print line.strip()
    print "\n----------------------------------------------------------------------"
    print "> Finished printing logfile : %s" % logFile
    print "----------------------------------------------------------------------\n"

  return


## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <localtimestampfile> <webtimestamp> <weblistfile>"
  if (len(sys.argv)<4):
    print usageline
    sys.exit(1)

  timestampfile = sys.argv[1]
  webtimestamp = sys.argv[2]
  weblistfile = sys.argv[3]

  newkitInstalled,pacmankey = interpretNewTimestamp(timestampfile)  

  if newkitInstalled:
    finished = waitForOnlineResults(webtimestamp)
    if finished: 
      getWebfiles(weblistfile)
      printLogfiles(weblistfile)

  print "\nErrorCode=0 (OK)"

