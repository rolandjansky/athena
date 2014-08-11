#!/afs/cern.ch/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, re
import subprocess

'''set up the environment '''
def setenv():
  os.environ["STAGE_SVCCLASS"]="atlcal"
  
def execute(cmd,args=[]):
  argList=[cmd]
  argList.extend(args)
  myobj=None
  try:
    myobj=subprocess.Popen(argList,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    s,e=myobj.communicate()
    return s,e
  except:
    print e
    return s,e
      
def extractFilename(dirLine):
  #matchString="[drwx-]10\s+\d+\s+\d+\s+zp\s+\d+\s+[A-Za-z]3\s+\d+(.+)"
  items=dirLine.split(' ')
  return items[-1]
  
def rfdir(fpath):
  cmd="/usr/bin/rfdir"
  args=[fpath]
  listing, errs= execute(cmd,args)
  fileList=[extractFilename(line) for line in listing.split("\n") if line!=""]
  return fileList,errs
  
def composeSctPath(rootDir,runStr):
  d="/"
  return rootDir+d+runStr+d+"calibration_SCTNoise"
  
def availableFiles():
  setenv()
  lastInList=-1
  rootDir="/castor/cern.ch/grid/atlas/DAQ/2010"
  listing,errs=rfdir(rootDir)
  runNames=listing
  #take the last run
  runNumberStr=runNames[lastInList]
  sctNoiseDir=composeSctPath(rootDir,runNumberStr)
  listing,errs=rfdir(sctNoiseDir)
  return sctNoiseDir, [extractFilename(listingLine) for listingLine in listing]
  
def lastAvailableFile():
  lastInList=-1
  dirPath,fileList= availableFiles()
  return dirPath+"/"+fileList[lastInList]
  
def main():
  print lastAvailableFile()
  return 0
  
if __name__ == "__main__":
  sys.exit(main())
  
  
  
