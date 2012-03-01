# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys, string, commands
from datetime import datetime

## Needed to correct ROOT behavior; see below
CWD = os.getcwd()
import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)
## Import the ROOT library for reading han results
gSystem.Load('libFileStagerLib')
from ROOT import TStageManager

class FileStagerTool:
  def __init__(self, sampleFile="", baseDir=".", tmpDir="",
               CpCommand = "lcg-cp",
               CpArguments = [ "-v", "--vo", "atlas", "-t", "1200" ],
               InfilePrefix = "gridcopy://",
               OutfilePrefix = "file:",
               StageFirstFile = True,
               sampleList = [],
               checkGridProxy = True,
               tryGridRestart = True,
               gridCertificate = os.environ['HOME'] + "/.globus/gridproxy.cert",
               LogfileDir = ""
              ):

    self.sampleFile = sampleFile
    self.sampleList = sampleList
    self.tmpSampleList = []

    self.CpCommand      = CpCommand
    self.CpArguments    = CpArguments
    self.InfilePrefix   = InfilePrefix
    self.OutfilePrefix  = OutfilePrefix
    self.StageFirstFile = StageFirstFile

    self.checkGridProxy = checkGridProxy and (CpCommand == "lcg-cp")
    self.tryGridRestart = tryGridRestart
    self.gridCertificate = gridCertificate

    if ( len(LogfileDir)>0 ):
      if ( LogfileDir.rfind("/")!=(len(LogfileDir)-1) ):
        LogfileDir+="/"
    self.LogfileDir = LogfileDir

    if ( baseDir.rfind("/")!=(len(baseDir)-1) ):
      baseDir+="/"
    self.baseDir = baseDir

    if ( len(tmpDir)>0 ):
      if ( tmpDir.rfind("/")!=(len(tmpDir)-1) ):
        tmpDir+="/"
    self.tmpDir = tmpDir

    # process sample file
    self.ProcessSampleFile()

  def ProcessSampleFile(self):

    if (len(self.sampleFile)==0): return

    self.sampleList = []
    FileList=open(self.sampleFile).readlines()

    for file in FileList:
      file = file.strip()

      if (len(file)==0):
        continue
      elif (file.find("#")==0):
        continue
      elif (file.find("TITLE")==0):
        continue
      elif (file.find("FLAGS")==0):
        continue
      else:
        self.sampleList.append(file)

  def PrintSampleList(self,outputFile=""):
    printlist = "sampleList = %s" % self.sampleList
    if (len(outputFile)>0):
      outFile = open(outputFile,'w')
      outFile.write(printlist)
      outFile.close()
      print "Sample list has been written to <%s>. Goodbye.\n" % outputFile
    else: print printlist + "\n"

  def GetSampleList(self):
    return self.sampleList

  def ListFromLocal(self,localDir):
    os.system("ls -l "+localDir+" > /tmp/filelist.temp")
    FileList=open("/tmp/filelist.temp","r").readlines()
    FileList1=[localDir+"/"+file.split()[8]
               for file in FileList
               if file.split()[4]!="0"]
    os.system("rm -f /tmp/filelist.temp")
    return FileList1

  def ListFromCastor(self,castorDir):
    os.system("nsls -l "+castorDir+" > /tmp/filelist.temp")
    FileList=open("/tmp/filelist.temp","r").readlines()
    FileList1=["rfio:"+castorDir+file.split()[8]
               for file in FileList
               if file.split()[4]!="0"]
    os.system("rm /tmp/filelist.temp")
    return FileList1

  def GetTmpdir(self):
    # default
    defaultTmpdir = ""

    # last fallback
    try:
      defaultTmpdir = os.environ['TMPDIR'] 
    except Exception,inst:
      pass
    # cern lxbatch
    try:
      defaultTmpdir = os.environ['WORKDIR']
    except Exception,inst:
      pass
    # osg
    try:
      defaultTmpdir = os.environ['OSG_WN_TMP']
    except Exception,inst:
      pass
    # lcg
    try:
      defaultTmpdir = os.environ['EDG_WL_SCRATCH']
    except Exception,inst:
      pass

    # use given tmpdir
    if (len(self.tmpDir)>0): defaultTmpdir = self.tmpDir

    # fall back option
    if (len(defaultTmpdir)==0): defaultTmpdir="/tmp"

    return defaultTmpdir

  def GetStageCollections(self):
    baseTmpdir = self.GetTmpdir()

    if ( baseTmpdir.rfind("/")!=(len(baseTmpdir)-1) ):
        baseTmpdir+="/"

    self.tmpSampleList = []

    for input in self.sampleList:
        filePiece = input.split("/")        
        nPieces = len(filePiece)
        tmpFile = baseTmpdir + "tcf_" + filePiece[nPieces-1]
        self.tmpSampleList.append(tmpFile)

    if (self.StageFirstFile): self.StageFirstCollection()

    return self.tmpSampleList

  def StageFirstCollection(self):

    # stage first file ...
    if(len(self.sampleList)>=1):
      rfioPiece = self.sampleList[0].split(self.InfilePrefix)
      nPieces = len(rfioPiece)
      
      inFile  = rfioPiece[nPieces-1]
      outFile = self.tmpSampleList[0]
      stderr  = outFile+"_stage.err"
      stdout  = outFile+"_stage.out"

      if (len(self.LogfileDir)>0):
        baseTmpdir = self.GetTmpdir()
        if ( baseTmpdir.rfind("/")!=(len(baseTmpdir)-1) ): baseTmpdir+="/"
        stderr = stderr.replace(baseTmpdir,self.LogfileDir)
        stdout = stdout.replace(baseTmpdir,self.LogfileDir)
      
      #print "TStageManager::getFile()   : Waiting till <%s> is staged." % (self.sampleList[0])

      if (True):
        stageman = TStageManager.instance()
        stageman.setPidInLognames(False)
        stageman.setLogfileDir(self.LogfileDir)
        stageman.setBaseTmpdir(self.GetTmpdir())
        stageman.setInfilePrefix(self.InfilePrefix)
        stageman.setOutfilePrefix(self.OutfilePrefix)
        stageman.setCpCommand(self.CpCommand)
        for addArg in self.CpArguments:
          stageman.addCpArg(addArg)
        stageman.getFile(self.sampleList[0])
        stageman.setPidInLognames(True)
      #else:
      #  sysCommand = "time "+self.CpCommand
      #  for addArg in self.CpArguments:
      #    sysCommand += " " + addArg
      #  sysCommand += " "+inFile+" "+self.OutfilePrefix+outFile+" >"+stdout+" 2>"+stderr
      #  os.system(sysCommand)

  def DoStaging(self):
    doStaging = False
    if (len(self.sampleList)>0):
      if(self.sampleList[0].find(self.InfilePrefix)!=-1):
        doStaging = True

    if (doStaging and self.checkGridProxy):
      retcode = self.CheckGrid()
      if (retcode!=0):
        if (self.tryGridRestart):
          retcode2 = self.TryGridRestart()
          if (retcode2!=0): sys.exit(1)
        else: sys.exit(1)
    return doStaging

  def CheckGrid(self):
    (retcode,output) = commands.getstatusoutput("grid-proxy-info -exists")
    if (retcode!=0):
      print "\nFileStager.FileStagerTool() : ERROR : grid proxy certificate not found."
    return retcode

  def TryGridRestart(self):
    #os.environ['X509_USER_PROXY'] = self.gridCertificate
    (retcode,output) = commands.getstatusoutput("voms-proxy-init -voms atlas -noregen")
    if (retcode!=0):
      print "\nFileStager.FileStagerTool() : ERROR : grid proxy restart failed. Exiting."
    return retcode

