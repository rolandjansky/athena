# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from glob import glob
import subprocess
import os
import pickle
import sys

class BulkRun :
  """Run on multiple pedestal files

  Used to run on multiple pedestal bytestream files located somewhere on disk.
  Puts results into seperate directories by run-number. It extracts pertinent
  information from the file names, which are expected to be of the form:

  data10_calib.00157081.calibration_pedCSC.daq.RAW._lb0000._CSC-EB._0001.data

  run(numToRun) - process NumToRun (at most) runs out of those not yet run out of. This is the primary interface. 
  ReadProcessedFilesList() - find all files already run on from a local text file
  AddProcessedFiles() - save newly run on files to disk
  FindFiles() - Find pattern and run number of a set of files that have not yet been run on
  RunAthena() - run athena job for a set of runs

  """
  def __init__(
      self,
      inputPattern = "/raid02/schernau/ped/csc/csc/*.data" ,
      processedFilesList = "ProcessedFiles.list",
      outputDirBase = "/raid02/lampen/datasets/csc/PedProcessing2",
      debug = False,
      allowDirOverwrite = False,
      ):
    """initialize internal variables"""
    self.InputPattern = inputPattern
    self.ProcessedFilesList = processedFilesList
    self.OutputDirBase = outputDirBase
    self.debug = debug
    self.AllowDirOverwrite = allowDirOverwrite
    print self.InputPattern

  def run(self, numToRun = 10) :
    """
    Run over all run numbers, find files for each, and submit each set to
    CscCalcPedMon.py
    """

    print "Running on " + str(numToRun) + " runsSet."


    runNumbers = []
    for runCnt in range(numToRun) :
      print ">>>>Running on runSet " + str(runCnt+1) + " of " + str(numToRun)
      pattern,runNumber = self.FindFiles()
      if(pattern != ""):
        #have files to run on
        runNumbers += [runNumber]
        try:
          self.RunAthena(pattern,runNumber)
        except:
          "Failure during RunAthena!"
          raise
      else:
        print "No more unprocessed files. Congrats!"
        print "N runs done: " + str(runCnt +1)
        print runNumbers
        return
    print "finished all " + str(numToRun) 
    print "Run numbers include:"
    print runNumbers
    print
    print "All Processed files:" 
    print self.ReadProcessedFilesList()

  #Read list of previously processed files
  def ReadProcessedFilesList(self):

    ProcessedFiles = []

    try:
      #Get processed files
      f = open(self.ProcessedFilesList,"r") 
      ProcessedFiles = pickle.load(f)
      f.close()
    except:
      print "No processed file list yet..."
     
    #Remove newline character from each filename

    #for index in range(len(ProcessedFiles)) : 
    #  file = ProcessedFiles[index]
    #  ProcessedFiles[index] = file[0:len(file)-1] 

    print 'Processed String: '
    print ProcessedFiles

    return ProcessedFiles

  def AddProcessedFiles(self,newFiles):
    """Save new processed files to disk"""
    ProcessedFiles = self.ReadProcessedFilesList()
    ProcessedFiles += newFiles
    f = open(self.ProcessedFilesList,"w")
    pickle.dump(ProcessedFiles,f)
    f.close()

    return True


  #Read list of previously processed files
  def FindFiles(self):

    #Initial setup
    FoundUnprocessedFile = False

    #Get processed file list
    ProcessedFiles = self.ReadProcessedFilesList()

    #Get list of files in input dir
    print "Input pattern: " + self.InputPattern
    inputFiles = glob(self.InputPattern)

    if(self.debug):
      print
      print "Searching for file"
      print "InputPattern: " + self.InputPattern
      #print "inputFiles: "
      #print inputFiles

    pattern = ""
    runNumber = ""

    #Loop through list until find file that is 
    for file in inputFiles:
      if not ProcessedFiles.count(file):

        index = file.find("data10")
        if(index == -1):
          index = file.find("data11")
        if(index == -1):
          print "ERROR! Index of -1!"
          raise Exception("Index error") 
        FoundUnprocessedFile = True
        pattern = file[0:index + 22] + "*" #includes run number
        runNumber = file[index + 13: index + 21]

    if(not FoundUnprocessedFile):
      return "",""

    if(self.debug) : 
      print "Found unprocessed file with pattern: " + pattern
      print "This includes files:"
      print glob(pattern)

    return pattern, runNumber

  def RunAthena(self, pattern, runNumber):
    """Run athena on a particular set of files matching pattern"""
    outputDirPath = self.OutputDirBase + "/" + runNumber
      
    if(self.AllowDirOverwrite):
      try:
        subprocess.call("rm -rf " + outputDirPath)
      except:
        pass

    print "Making directory" + outputDirPath
    #Create output directory
    os.mkdir(outputDirPath,0755)

    #Change directory to output directory
    #os.chdir(outputDirPath)

    #Athena options
    athOpt = "outputPre='" + outputDirPath +"/" + runNumber 
    athOpt += "';inputPat='" + pattern 
    athOpt += "';reportPrefix='runNumber = " 
    athOpt += runNumber + "'"

    #athena arguments
    athArgs = ["athena.py", "-c", athOpt, "CscCalcPedMon.py"]

    #Output log file
    logFile = open(outputDirPath + "/run.log","w")

    try:
      print
      print"**************************************"
      print"Starting running on run " + str(runNumber)
      sys.stdout.flush()
      subprocess.Popen(athArgs,stdout=logFile,stderr=subprocess.STDOUT).wait()
      print"Finished run " + str(runNumber)
      print"**************************************"
      print
    except:
      print "Error while running athena!"
      raise

    logFile.close()


    #Add files we just ran on to file list
    newProcessedFiles = glob(pattern);
    self.AddProcessedFiles(newProcessedFiles)
