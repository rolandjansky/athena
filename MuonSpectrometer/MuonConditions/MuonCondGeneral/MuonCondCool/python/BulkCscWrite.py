# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import subprocess
import sys
import shutil

def WriteBulk(
    inputFiles, 
    foldersToWrite, 
    tagsToWrite,  
    outputFileName = "bulk.db",
    jobOptions="cscCondCoolWrite.py",
    doMerge = True,
    cscMergeTag = "",
    numToRun = -1,
    useFileRunAsIOVEnd = True #If True, will treat last run number to be start of IOV
    ):
  """Go through several text calibration files, and produce a .db file for the cooldata base containing all the data.
  
  The IOV run numbers are taken frome the filenames, which are assumed to be of the form <runNumber>.cal

  It is assmumed that we want to start by merging with an existing tag in the database, which
  is done while running on the first file. After that, the local file is referenced. 


  Also, the input list should be in order of increasing IOV!
  """
    
  #Must be this to read from it
    
  readFileName = "mycool.db"

  if(outputFileName == readFileName):
    print "ERROR! you cannont use " + readFileName + " as the output!"

  if os.path.exists(outputFileName):
    print "Output file " + outputFileName + " already exists! Remove it to continue."
    return


  prevRunNumber = '0'

  #Check that input files follow correct syntax
  for file in inputFiles:
    if( file[-4:] != ".cal"):
      print file +  " does not follow allowed form <runNumber>.cal!"
      print "Quitting."
      return

  #loop over files, adding each one to coolout.db
  nFiles = len(inputFiles)
  for iFile in range(nFiles):
    if(iFile  == numToRun):
      break

    filePath = inputFiles[iFile]
    fileName = os.path.basename(filePath)
    runNumber = fileName[0:-4] #Assume file format is runNumber.cal

    #Get next run number
    if iFile != (nFiles -1):
      nextRunNumber = str(os.path.basename(inputFiles[iFile+1])[0:-4])
    else:
      nextRunNumber = '-1'

    #Remove leading zeros, otherwise will come out as hex number
    nextRunNumber = nextRunNumber.lstrip('0')
    runNumber = runNumber.lstrip('0')

    #Define athena options
    athOpt = ""
    if doMerge :
      athOpt += "doMerge = True; "
    else :
      athOpt += "doMerge = False; "
    athOpt += "input = '" + filePath + "'; "
    athOpt += "output = '" + outputFileName + "'; "
    athOpt += "readDb = 'COMP200'; "
    athOpt += "writeDb = 'COMP200'; " 
    athOpt += "CoolGlobalTag = 'COMCOND-ES1C-003-00'; " #Doesn't really matter
    athOpt += "overrideCscTag = '" + cscMergeTag + "'; "
    #Use last run number before this one as reference for merging
    forceRunNumber = str(int(runNumber) -1)
    athOpt += "forceRunNumber = " + forceRunNumber + "; "
    if( useFileRunAsIOVEnd == True):
      athOpt += "IOVRunStart = " + prevRunNumber + "; "
      #athOpt += "IOVRunEnd = " + runNumber + "; "
    else :
      athOpt += "IOVRunStart = " + runNumber + "; "
      athOpt += "IOVRunEnd = " + nextRunNumber + "; "
    athOpt += "FoldersToWrite = " + str(foldersToWrite) + "; "
    athOpt += "TagsToWrite = " + str(tagsToWrite) + "; "

    if iFile > 0 :
      #We want to read from entry we wrote out last cycle for merging.
      #Might not be needed, but we'll copy the database file to prevent any read/write conflicts
      shutil.copy(outputFileName,readFileName)
      athOpt += "MergeFromLocalFile = True; "



    logFileName = runNumber + ".log" 
    logFile = open(logFileName,"w")


    #run athena
    print "****************************************"
    print "Starting run on " + runNumber 
    cmdArgs = ["athena.py", "-c" , athOpt , jobOptions]
    print "args are " + str(cmdArgs)
    sys.stdout.flush()
    retcode = subprocess.Popen(cmdArgs,stdout = logFile, stderr = subprocess.STDOUT).wait()
    logFile.close()
    print "****************************************"


    prevRunNumber = runNumber


  print "All done!"

  
def rnum(fileName):
  """Gets run number from filename"""
  return fileName[0:-4]



