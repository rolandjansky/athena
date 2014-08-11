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
  
def ls(fpath):
  errs=None
  fileList=os.listdir(fpath)
  return fileList,errs
  
def composeSctPath(rootDir,runStr):
  d="/"
  return rootDir+d+runStr+d+"calibration_SCTNoise"
  
def availableFiles():
  setenv()
  lastInList=-1
  rootDir="/afs/cern.ch/user/s/sctcalib/scratch0/test_results/data"
  listing,errs=ls(rootDir)
  runNames=listing
  try:
    #take the last run
#    runNumberStr=runNames[lastInList]
#    sctNoiseDir=composeSctPath(rootDir,runNumberStr)
    listing,errs=ls(rootDir)
    return rootDir, listing
  except IndexError:
    print "Files not found in %s"%rootDir
    print "listing gave %s"%listing
    return rootDir,""

def lastAvailableFile():
  lastInList=-1
  dirPath,fileList= availableFiles()
  try:    
    return dirPath+"/"+fileList[lastInList]
  except IndexError:
    print "No files in %s"%dirPath

def main():
  print lastAvailableFile()
  return 0
  
if __name__ == "__main__":
  sys.exit(main())
  
  
  
