# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from datetime import datetime

class CastorFileTool:
  def __init__(self, baseCastorDir="/castor/cern.ch/grid/atlas/DAQ/2008/91890", inStream=["IDCosmic"], tmpdir="${TMPDIR}/",
               nJobs=30, nFilesPerJob=7, nFilesNeeded=-1, linkdir="./", useBaseSubdirs=True,
               runSubdirToSearch = "", staticRuns = [], lumiBlock = None):

    # need this conversion map somewhere ...
    self.monthMap = {}
    self.monthMap["Jan"] = "01"
    self.monthMap["Feb"] = "02"
    self.monthMap["Mar"] = "03"
    self.monthMap["Apr"] = "04"
    self.monthMap["May"] = "05"
    self.monthMap["Jun"] = "06"
    self.monthMap["Jul"] = "07"
    self.monthMap["Aug"] = "08"
    self.monthMap["Sep"] = "09"
    self.monthMap["Oct"] = "10"
    self.monthMap["Nov"] = "11"
    self.monthMap["Dec"] = "12"

    self.twogblimit = 2000000000

    self.baseCastorDir = baseCastorDir
    self.inStream = inStream

    if ( tmpdir.rfind("/")!=(len(tmpdir)-1) ):
        tmpdir+="/"
    self.tmpdir = tmpdir

    self.nJobs = nJobs
    self.nFilesPerJob = nFilesPerJob

    self.nFilesNeeded = self.nJobs * self.nFilesPerJob
    if (nFilesNeeded>0): self.nFilesNeeded = nFilesNeeded

    self.fileIdx = 0
    self.fileIdxEndOfRun = 0
    self.fileList = []
    self.fileListEndOfRun = []

    if ( linkdir.rfind("/")!=(len(linkdir)-1) ):
        linkdir+="/"
    self.linkdir = linkdir
    self.useBaseSubdirs = useBaseSubdirs
    self.runSubdirToSearch = runSubdirToSearch
    self.staticRuns = staticRuns
    self.lumiBlock = lumiBlock

  def init(self):
    self.GetLatestRunFilesFromCastor()

  def ListFromLocal(self,localDir):
    timenow = datetime.today().strftime("%d%b%y.%Hh%M")
    os.system("ls -l "+localDir+" > filelist.temp")
    FileList=open("filelist.temp","r").readlines()
    FileList1=[localDir+"/"+file.split()[8]
               for file in FileList
               if file.split()[4]!="0"]
    os.system("rm -f filelist.temp")
    return FileList1

  def ListFromCastor(self,castorDir,stream="Express"):
    timenow = datetime.today().strftime("%d%b%y.%Hh%M")
    os.system("nsls -l "+castorDir+" | grep "+stream+" > filelist.temp")
    FileList=open("filelist.temp","r").readlines()
    FileList1=["rfio:"+castorDir+file.split()[8]
               for file in FileList
               if file.split()[0][0]!="d"]
    os.system("rm filelist.temp")
    return FileList1

  def GetTmpdir(self):
    # default
    defaultTmpdir = ""

    try:
        defaultTmpdir = os.environ['TMPDIR'] 
    except Exception,inst:
        pass

    # cern lxbatch machines
    hostname = ""

    try:
        hostname = os.environ['HOSTNAME']
    except Exception,inst:
        pass

    #hostname = os.environ['HOSTNAME']
    hostPieces = hostname.split(".")
    nhostPieces = len(hostPieces)
    if(nhostPieces>=3):
        if (hostPieces[0].find("lxb")!=-1) and (hostPieces[nhostPieces-2]=="cern"):
            pwddir = os.environ['PWD']
            if (pwddir.find("pool")!=-1): defaultTmpdir = pwddir

    # use given tmpdir
    if (len(self.tmpdir)>0): defaultTmpdir = self.tmpdir

    # fall back option
    if (len(defaultTmpdir)==0): defaultTmpdir="/tmp"

    return defaultTmpdir

  def StageCollections(self,inputFiles=[],stageFirstCollection=False):
    baseTmpdir = self.GetTmpdir()

    if ( baseTmpdir.rfind("/")!=(len(baseTmpdir)-1) ):
        baseTmpdir+="/"

    outputFiles = []

    for input in inputFiles:
        filePiece = input.split("/")        
        nPieces = len(filePiece)
        tmpFile = baseTmpdir + "tcf_" + filePiece[nPieces-1]
        outputFiles.append(tmpFile)

    # stage first file ...
    if(len(inputFiles)>=1 and stageFirstCollection):
        rfioPiece = inputFiles[0].split(":")
        nPieces = len(rfioPiece)
        inFile = rfioPiece[nPieces-1]
        outFile = outputFiles[0]
        print "StageCollections() : Waiting till <%s> is staged." % (inputFiles[0])
        os.system("rfcp "+inFile+" "+outFile)
        os.system("touch "+outFile+"_stage.out")
        os.system("touch "+outFile+"_stage.err")
        os.system("ls -l -h %s" % (baseTmpdir))

    return outputFiles

  def GetFileTimeInt(self, castorLine):
    # get current year
    #from datetime import datetime
    thisyearstr = datetime.today().strftime("%Y")

    linesplit = castorLine.split()

    # reconstruct directory creation time for sorting
    monthstr    = linesplit[5]
    daystr      = linesplit[6]
    hourminstr  = linesplit[7]

    yearstr  = ""
    if (hourminstr.find(":")!=-1):
        yearstr = thisyearstr
    else:
        yearstr = hourminstr
        hourminstr = "00:00"

    hourstr = hourminstr.split(":")[0]
    minstr  = hourminstr.split(":")[1]

    # sortable time integer
    timeint = int(yearstr+self.monthMap[monthstr]+daystr+hourstr+minstr)

    return timeint

  def GetRunNumber(self, fileName):
    nameSplit = fileName.split(".")

    # use first number in filename as runnumber
    runString = ""
    runNumber = -1
    idx = 0
    while ( idx<len(nameSplit) ):
        try:
          runNumber = int(nameSplit[idx]) # replace
        except Exception,inst:
          pass

        if( runNumber!=-1 ): break

        idx+=1

    return runNumber

  def LatestSubdirsFromCastor(self, castorDir, recursiveSearch=False):
    if ( castorDir.rfind("/")!=(len(castorDir)-1) ):
        castorDir+="/"
    
    # dirs in given castor directory
    timenow = datetime.today().strftime("%d%b%y.%Hh%M")
    dirlist = "dirlist.temp" + timenow
    os.system("eos ls -l " + castorDir + " > " + dirlist)
    FileList=open(dirlist,"r").readlines()

    # construct map with subdirectories
    dirList = {}
    for line in FileList:
        linesplit = line.split()

        # filesize == 0 for a directory
        # TC: not true with eos!!
        #if (linesplit[4]=="0"):
        if (linesplit[0][0]=="d"):

            # sortable time integer
            timeint = self.GetFileTimeInt(line)
    
            timeFound = False
            for key in dirList.keys():
                if (timeint==key): 
                    timeFound = True
                    break
            newdir=castorDir+linesplit[8]
            if newdir.find("merge")!=-1 or newdir.find("recon")!=-1:
                continue
            if (not timeFound): dirList[timeint] = []
    
            dirList[timeint].append(newdir)

    # Be sure to delete tmpfile before next iteration.
    os.system("rm -f "+dirlist)

    keys = dirList.keys()
    keys.sort()

    # default is given castordir
    if (len(keys)==0): 
        dirList[0] = [ castorDir ]
        print "Using basedir : "
        print castorDir
        recursiveSearch = False

    # return
    if (recursiveSearch): 
        return self.LatestSubdirsFromCastor(dirList[keys[len(keys)-1]][0],recursiveSearch)
    else:
        return dirList

  def RunFilesFromCastor(self, castorDir, streamStr=["Express"]):
    if ( castorDir.rfind("/")!=(len(castorDir)-1) ):
        castorDir+="/"
    if ( self.runSubdirToSearch != "" ): 
        if ( self.runSubdirToSearch.rfind("/")!=(len(self.runSubdirToSearch)-1) ):
            self.runSubdirToSearch += "/"
    #castorDir += self.runSubdirToSearch

    # dirs in given castor directory
    timenow = datetime.today().strftime("%d%b%y.%Hh%M")
    filelist = "filelist.temp" + timenow
    if self.lumiBlock != None and ',' in self.lumiBlock:
      FileList = []
      for iblock in range(int(self.lumiBlock.split(',')[0]),int(self.lumiBlock.split(',')[1])+1):
        cmd = "eos ls -l " + castorDir
        for grepStr in streamStr:
          cmd += " | grep " + grepStr
          pass
        cmd += " | grep _lb%0.4d" % iblock
        cmd += " > " + filelist
        print "RunFilesFromCastor : running command",cmd
        os.system(cmd)
        FileList += open(filelist,"r").readlines()
        pass
    else:
      #cmd = "nsls -l " + castorDir
      cmd = "eos ls -l " + castorDir
      #for grepStr in streamStr:
      #  cmd += " | grep " + grepStr
      if self.lumiBlock != None: cmd += " | grep _lb%0.4d" % int(self.lumiBlock)
      cmd += " > " + filelist
      print "RunFilesFromCastor : running command",cmd
      os.system(cmd)
      FileList=open(filelist,"r").readlines()
      pass

    # construct map with subdirectories
    runMap = {}
    for line in FileList:
        linesplit = line.split()

        # filesize == 0 for a directory
        # TC: not true with eos!!
        #print "test ",linesplit[0]," ",linesplit[0][0]," ",linesplit[4]
        #if (linesplit[4]!="0"):
        if (linesplit[0][0]!="d"):

            # run number
            runNumber = self.GetRunNumber(linesplit[8])

            runFound = False
            for key in runMap.keys():
                if (runNumber==key): 
                    runFound = True
                    break
            if (not runFound): runMap[runNumber] = []

            # two gb webserver limit
            if (self.nFilesNeeded==5 and self.nFilesPerJob==1 and int(linesplit[4])>=self.twogblimit): continue
	    #if (int(linesplit[4])<100000000): continue

            runMap[runNumber].append(castorDir+linesplit[8])

    # Be sure to delete tmpfile before next iteration.
    os.system("rm -f " + filelist)

    return runMap

  def GetLatestRunFilesFromCastor(self,nFilesNeeded=-1):

    if (nFilesNeeded>0): self.nFilesNeeded = nFilesNeeded

    print "CastorTool.GetLatestRunFilesFromCastor() : INFO"
    print ">> Now retrieving names of latest <", self.nFilesNeeded, "> runfiles on Castor. This may take some time." 
    print ">> Searching in latest subdirector(y/ies) of <", self.baseCastorDir, ">, matching <", self.inStream, ">, and runsubdir <", self.runSubdirToSearch, ">."

    fileList = []
    fileListEndOfRun = []
    dirMap = {}

    # retrieve castor directories, with timestamps
    if (self.useBaseSubdirs):
        dirMap = self.LatestSubdirsFromCastor( self.baseCastorDir+"/"+self.runSubdirToSearch, recursiveSearch=True )
    else: dirMap[len(self.staticRuns)] = [ self.baseCastorDir ]

    # sort directories in time
    timeints = dirMap.keys()
    timeints.sort()

    # add static runs to the top
    idx = 1
    for staticRunNumber in self.staticRuns:
      dirMap[timeints[len(timeints)-1]+idx] = [ self.baseCastorDir + "/" + str(staticRunNumber)]
      idx+=1

    # sort directories again in time
    timeints = dirMap.keys()
    timeints.sort()

    # loop over latest castor directories, till enough files found
    noFilesInRunMap = 0
    idx = len(timeints)-1
    while (idx>=0):
        timeint = timeints[idx]
        # collect runfiles from next-to-latest directory
        print "Now retrieving files from directory : " + dirMap[timeint][0],timeint
        runMapTmp = self.RunFilesFromCastor( dirMap[timeint][0], self.inStream )

        #print runMapTmp

        # sort runs
        runs = runMapTmp.keys()
        runs.sort()

        #if len(self.staticRuns)>0:
        #  runs = self.staticRuns + runs

        # add enough files from next-to-latest run
        jdx = len(runs)-1
        while (jdx>=0 and noFilesInRunMap<self.nFilesNeeded):
            run = runs[jdx]
            print ">> Now including run <", run, ">"

            noLoop = 0
            if (len(runMapTmp[run]) > self.nFilesNeeded-noFilesInRunMap): # add only number of files needed
               noLoop = self.nFilesNeeded-noFilesInRunMap
            else: noLoop = len(runMapTmp[run])  # add everything

            kdx = 0
            while (kdx<noLoop):
            #while (kdx<self.nFilesNeeded-noFilesInRunMap):
                fileList.append(runMapTmp[run][kdx])   # reverse map for later use
                fileListEndOfRun.append(runMapTmp[run][len(runMapTmp[run])-(kdx+1)])
                kdx+=1

            noFilesInRunMap += noLoop 

            jdx-=1

        # stop when enough files have been found
        if (noFilesInRunMap>=self.nFilesNeeded): break

        idx-=1

    self.fileList = fileList 
    self.fileListEndOfRun = fileListEndOfRun
    self.nJobs = int(len(fileList)/self.nFilesPerJob)

    print ">> Retrieved <", len(fileList), "> file names. Enough for <", self.nJobs, "> jobs with <", self.nFilesPerJob, "> files per job."

    return fileList,fileListEndOfRun

  def GetFilesForJob(self, nFiles=-1, fileList=[], fileListEndOfRun=[], addFilesEndOfRun=False, jobIdx=-1):

    if (fileList==[]): fileList = self.fileList
    if (fileListEndOfRun==[] and addFilesEndOfRun): 
      fileListEndOfRun = self.fileListEndOfRun
    if (nFiles<=0): nFiles = self.nFilesPerJob

    fileIdxTemp = self.fileIdx
    if (jobIdx>=0): self.fileIdx = jobIdx * nFiles

    if (self.fileIdx+nFiles>=len(fileList)): 
        print "CastorTool.GetFilesForJob() : WARNING"
        print "fileindex > number of files",self.fileIdx,len(fileList),nFiles
        self.fileIdx = (self.fileIdx+nFiles)%(len(fileList))
        print "new fileindex is",self.fileIdx
        pass

    jobFileList = []

    jdx = 0
    eoridx = 0
    if (addFilesEndOfRun): eoridx = 1

    firstrunnumber = -1
    runnumber = -1

    while (jdx<(nFiles-eoridx) and self.fileIdx<len(fileList)):
        if (firstrunnumber == -1): firstrunnumber = self.GetRunNumber(fileList[self.fileIdx])
        runnumber = self.GetRunNumber(fileList[self.fileIdx])
        if (runnumber == firstrunnumber): 
          jobFileList.append(fileList[self.fileIdx])
          self.fileIdx+=1
        jdx+=1

    if (addFilesEndOfRun): 
      jobFileList.append(fileListEndOfRun[self.fileIdxEndOfRun])
      self.fileIdxEndOfRun+=1

    if (jobIdx>=0): self.fileIdx = fileIdxTemp

    return jobFileList

  def GetFilesString(self, fileList, prefix = "datasetname#"):
    fileStr = "\""

    jdx=0
    while (jdx<len(fileList)):
        fileStr += prefix
        fileStr += fileList[jdx]
        if (jdx+1<len(fileList)): fileStr += " "
        jdx+=1

    fileStr += "\""

    return fileStr

  def TmpFilesCopyCommands(self, jobFileList,tmpFileList):
    cpcommand = "time xrdcp root://eosatlas.cern.ch/"
    #cpcommand = "time rfcp "
    inprefix = ""
    outprefix = ""

    tmpcommands = []
    baseTmpdir = self.GetTmpdir()

    if False:
      #tmpcommands.append('export STAGE_SVCCLASS=atlcal')
      tmpcommands.append('\n# *** To run this script from a shell, one needs to set RUNDIR and WORKDIR below. ***')
      tmpcommands.append('\n# Uncomment following line to specify where the transform should be run. (Otherwise this script will not work.)')
      tmpcommands.append('#export RUNDIR=afsRunDirectory    # Provide full afs path.')
    if True:
      tmpcommands.append('#export STAGE_SVCCLASS=default')
      tmpcommands.append('')
      tmpcommands.append('## Set WORKDIR to specify where to store temporary files.') 
      tmpcommands.append('## Note that WORKDIR is set dynamically when running in a batch queue.')
      tmpcommands.append('#export WORKDIR=afsDirectoryForTmpStorage # Provide full afs path.')
      tmpcommands.append('if [ "$WORKDIR" = "" ]; then')
      tmpcommands.append('  if [ "$TMPDIR" = "" ]; then')
      tmpcommands.append('    export WORKDIR=${PWD}')
      tmpcommands.append('  else')
      tmpcommands.append('    export WORKDIR=${TMPDIR}')
      tmpcommands.append('  fi')
      tmpcommands.append('fi\n')

    #idx=0
    #while (idx<len(tmpFileList)):
    #    tmpcommands.append("touch "+tmpFileList[idx])
    #    idx+=1

    idx=0
    while (idx<len(tmpFileList)):
        tmpcommands.append(cpcommand+inprefix+jobFileList[idx]+" "+outprefix+tmpFileList[idx])
        #tmpcommands.append(('ls -l -h %s' % baseTmpdir))
        idx+=1

    ## commands to compose filestager list
    if False:
      tmpcommands.append("\nrm -f $WORKDIR/castorsample.def")
      tmpcommands.append("cat > $WORKDIR/castorsample.def <<EOF")
      idx=0
      while (idx<len(tmpFileList)):
        tmpcommands.append("gridcopy://"+jobFileList[idx])
        idx+=1
      tmpcommands.append("EOF")

    return tmpcommands

  def GetLinkCommands(self, tmpFiles ):

    linkFiles = []
    pooldir = "" #"pooldir/"

    for input in tmpFiles:
        filePiece = input.split("/")
        nPieces = len(filePiece)
        tmpFile = pooldir + filePiece[nPieces-1]  #self.linkdir + pooldir + filePiece[nPieces-1]
        linkFiles.append(tmpFile)

    rmcommand = "rm -f"
    lncommand = "ln -s"

    lncommands = []
    #lncommands.append("\n# Copy over relevant data collections.")
    lncommands.append("mkdir -p "+self.linkdir)
    #if (len(pooldir)>1): 
    #    lncommands.append(rmcommand+" "+self.linkdir+pooldir)
    #    lncommands.append(lncommand+" "+"${WORKDIR}"+" "+self.linkdir+pooldir)
    #lncommands.append("ls -l")

    idx=0
    while (idx<len(tmpFiles)):
        #if (len(pooldir)==0):
            #lncommands.append(rmcommand+" "+linkFiles[idx])
            #lncommands.append(lncommand+" "+tmpFiles[idx]+" "+linkFiles[idx])
        #lncommands.append("gridcopy://"+tmpFiles[idx])
        idx+=1

    #lncommands.append("ls -l "+self.linkdir+pooldir)

    return lncommands, linkFiles


  def GetNextJob(self, nFiles=-1, fileList=[], fileListEndOfRun=[], addFilesEndOfRun=False, jobIdx=-1):
    jobFileList = self.GetFilesForJob(nFiles,fileList,fileListEndOfRun,addFilesEndOfRun,jobIdx)
    tmpFileList = self.StageCollections(jobFileList)
    #tmpFileStr  = self.GetFilesString(tmpFileList)
    cpCommands  = self.TmpFilesCopyCommands(jobFileList,tmpFileList)

    lnCommands, linkFiles = self.GetLinkCommands(tmpFileList)
    #cpCommands += lnCommands

    dict = {}
    dict["jobFileList"] = jobFileList
    dict["tmpFileList"] = tmpFileList
    dict["cpCommands"] = cpCommands
    dict["lnCommands"] = lnCommands
    dict["linkFiles"] = linkFiles

    return dict



##############################################################

