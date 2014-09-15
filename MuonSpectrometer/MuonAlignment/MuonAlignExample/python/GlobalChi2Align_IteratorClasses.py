# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import commands
import shutil 
import time
import datetime # available in python 2.3
import re
import glob

from Jobs         import JobThread,LocalJobList,SgeJobList,LxbatchJobList,PathenaJobList,PbsJobList
from DefaultFiles import Files,DefaultFiles
from ScriptWriter import ScriptWriter

#------------------------------------------------------------------------
class GlobalChi2AlignIterator:
    """Class which runs the alignment iterations."""
    
    def __init__(self,logfile):
        self.nCPUs               = 1
        self.queue               = ""
        self.firstIteration      = 0
        self.iterations          = 1
        self.outputPath          = ""
        self.runMode             = ""    # pathena, lxbatch, local, sge, pbs
        self.runLocation         = "lxplus"
        self.doJobChecks         = True
        self.doAccumulate        = True  # if false, input expected from matrix/vector.bin
        self.solveLocally        = False 
        self.runargsJobOption    = "runargs.ESDtoESD.py"
        self.zipLogFiles         = False
        self.setupScript         = ""   
        self.alignmentLevel      = 'L3'
        self.rootName            = ''
        self.doEndcapOnly        = False
        self.doBarrelOnly        = False
        self.inputAsztFile       = ''
        self.inputL1Ascii        = ''
        self.inputL23Ascii       = ''
        self.files               = NotImplemented
        self.runOptions          = ''    # only used for global alignment flags
        self.errorScaleDb        = ''
        self.readEventList       = False 
        self.writeEventList      = True
        self.nFilesPerJob        = 0
        self.nEventsPerJob       = -1
        self.dataType            = 'MC'
        self.skipEvents          = 0
        self.resumePathenaJob    = False
        self.site                = ''
        self.doBinariesOnly      = True # only gets matrix.bin and vector.bin from pathena job output
        self.preIncludes         = []
        self.tag                 = ""
        self.libDS               = ""
        self.official            = False

    def configRunMode(self):
        
        #---------------------------------------
        # Auto config for batch running
        #---------------------------------------
        if self.queue == "" :
            if self.runMode == "lxbatch" :
                self.queue = "8nh"
            elif self.runMode == "sge" :
                self.queue = "all.q"
            elif self.runMode == "pbs" :
                self.queue = "batch"   

        #--------------------------------------
        # Auto config rootName 
        #--------------------------------------
        if self.rootName == "" :
            if self.alignmentLevel == 'L1' :
                self.rootName = "L1"
            if self.alignmentLevel == 'L2.3' :
                self.rootName = "L23"
            else :
                self.rootName = "L3"
                self.rootName+="_"+self.dataType
            #self.rootName+="_"+self.runMode
            self.rootName += "_";
            if self.doEndcapOnly:
                self.rootName += "EC";
            elif self.doBarrelOnly:
                self.rootName += "Barrel";
            else :
                self.rootName +="notshifted"

            print "\nUsing auto-configured rootName ",self.rootName

        #-------------------------------------
        # Auto config output path
        #-------------------------------------
        if self.outputPath =="" :
            if self.runLocation == "lxplus" :
                user = os.getenv("USER")
                self.outputPath = "/afs/cern.ch/user/"+user[0]+"/"+user+"/scratch0/"+self.rootName+"_results"
            else :
                self.outputPath = "/tmp/$USER/"+self.rootName+"_results"
            msg = "\nOutput path: "+self.outputPath
            print msg

        
        #------------------------------    
        # Auto config default files/datasets
        #-------------------------------
        filestype="files"
        if self.runMode == "pathena" or self.runMode == "prun":
            filestype="dataset"

        if self.files == NotImplemented :
            if self.dataType == "MC":
                self.files = DefaultFiles.default('zmumu_MC',filestype)
            elif self.dataType == "cosmics" :
                self.files = DefaultFiles.default('121080',filestype)
            elif self.dataType == "data" :
                self.files = DefaultFiles.default('periodD',filestype)        
            elif self.dataType == 'bfield_off_MC':
                if self.MCmomentum==4:
                    self.files = DefaultFiles.default('bfield_off_MC_4GeV',filestype)                    
                elif self.MCmomentum==6:
                    self.files = DefaultFiles.default('bfield_off_MC_6GeV',filestype)
                elif self.MCmomentum==8:
                    self.files = DefaultFiles.default('bfield_off_MC_8GeV',filestype)
                elif self.MCmomentum==12:
                    self.files = DefaultFiles.default('bfield_off_MC_12GeV',filestype)
                elif self.MCmomentum==20:
                    self.files = DefaultFiles.default('bfield_off_MC_20GeV',filestype)
                else:
                    print "bad momentum for bfield_off MC!"
                    sys.exit(32)
            elif self.dataType == 'bfield_off_data':
                self.files = DefaultFiles.default('bfield_off_data',filestype)
                self.preIncludes.append("bfield_off.py")
            elif self.dataType == 'L1_zmumu_MC':
                self.files = DefaultFiles.default('L1_zmumu_MC',filestype)
            else :
                msg = "ERROR: Unkown dataType! Known values are MC,cosmics,data. Given: "+repr(self.dataType)
                print msg
                sys.exit(3)
        else :
            print "Using user defined files in directory:"
            print self.files.inputFilePath

        self.inputDatasetDef=self.files.datasetdef
        print "\ninput files: \n",self.files.files
        
        #--------------------------------------------------
        # Auto config job parameters
        #--------------------------------------------------
        if self.runMode == "local" :
            if self.nCPUs == 0 :
                self.nCPUs = 1
            if self.nFilesPerJob == 0 :
                self.nFilesPerJob = 1
            if self.nEventsPerJob == 0:
                self.nEventsPerJob = 200
        else :
            if self.nFilesPerJob == 0 :
                self.nFilesPerJob = 1
            if self.nEventsPerJob == 0:
                self.nEventsPerJob = 500

        #-------------------------------------
        # Auto config output path
        #-------------------------------------
        
        #shutil.copy(self.alignJobOptions,self.outputPath)
        #shutil.copy(self.solveJobOptions,self.outputPath)
        #if self.errorScaleDb!='':
        #    shutil.copy(self.errorScaleDb,self.outputPath)

        return

    def run(self):
        
        # create output directory and copy files there
        self.setupOutputDir()
        
        print "----------------------------------------------"
        print "\t%s      " % datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        print "----------------------------------------------"
        print "output path:   ", self.outputPath
        print "athena release:", os.environ.get("AtlasVersion")
        print "run mode:      ", self.runMode
        
        #check the runmode
        self.checkRunMode()
        
        #start the total time counter  
        StartTime=time.time()

        #loop over iterations
        for iter in range(self.firstIteration, self.firstIteration+self.iterations):
            
            # print header
            print
            print "----------------------------------------------"
            print "--- Iteration "+repr(iter)
            print "----------------------------------------------"
            
            # get start time
            IterStartTime=time.time()

             #get the job handler for accumulate and solve jobs
            print "getting jobLists"
            accumJobList,solveJobList = self.getJobLists()

            #loop over Jobs/CPUs
            if self.doAccumulate:
                print "Processing..."
                self.processAccumulateJobs(iter,accumJobList)
                print "done processing"

                if self.doJobChecks:
                    print "Checking jobs..."
                    self.checkAccumulateJobs(iter,accumJobList)

            #AsztOutputDir = scriptWriter.IterPath
            #print "AsztOutputDir: ",AsztOutputDir

            # if pathena job, get the output files
            if self.runMode == 'pathena' or self.runMode == 'prun' :

                print "running post job"
                scriptWriter=ScriptWriter(self,iter)
                postjobscript = scriptWriter.pathenaPostJobScript()

                localJobList=LocalJobList(1)
                postLogFile = scriptWriter.LogDir + scriptWriter.JobName + \
                    '.postlog'
                localJobList.process(postjobscript,postLogFile)                
                print "done with post job"

            if self.doSolve:
                print "Solving Iteration ",repr(iter)," with solveJobList: ",solveJobList
                self.processSolveJob(iter,solveJobList)
            else:
                print "Skip Solving Iteration "+repr(iter)
                            
            print "----------------------------------------------" 
            print "  Iteration %d finished: %5.3f seconds" % (iter,(time.time()-IterStartTime))
            print "----------------------------------------------"

            ##end of for(iterations)
        print
        print "----------------------------------------------" 
        print "Processed %d Iterations in %5.3f seconds" % (self.iterations,time.time()-StartTime)
        print "----------------------------------------------" 
    
    
    def setupOutputDir(self):

        #create output directory
        self.outputPath = self.getPath(self.outputPath)
        if os.path.isdir(self.outputPath):
            print "\nOutputPath: ", self.outputPath, "already exists!"
            if self.doAccumulate and False:
                print "exiting..."
                sys.exit()
            else:
                print "continuing..."
        else:
            os.makedirs(self.outputPath)
            #os.makedirs(self.outputPath+"/MuonAlignExample")

        #copy authentication.xml since job cannot read it via afs
        if self.runMode=='sge' :            
            shutil.copy(os.environ['CORAL_AUTH_PATH']+'/authentication.xml',
                        self.outputPath)

        #copy PoolFileCatalog
        #shutil.copy("PoolFileCatalog.xml",self.outputPath)

        #copy joboptions
        cwd=os.getcwd()
        files=glob.glob(cwd+'/*py')
        print "\ncopying files to ",self.outputPath
        for file in files:
            print " ...",file
            #shutil.copy(file,self.outputPath+"/MuonAlignExample")
            shutil.copy(file,self.outputPath)
        print "\n"
        if self.inputAsztFile!="":
            shutil.copy(self.inputAsztFile,self.outputPath)
        
        #shutil.copy(self.alignJobOptions,self.outputPath)
        #shutil.copy(self.solveJobOptions,self.outputPath)
        if self.errorScaleDb!='':
            print "copying ",self.errorScaleDb," to ",self.outputPath
            shutil.copy(self.errorScaleDb,self.outputPath)
        return

    def getPath(self,path):
        if len(path)>0 and not path.endswith("/"):
            path += "/"
        elif len(path)==0:
            path = "./"
        return path
    
    def checkRunMode(self):
        print "in checkRunMode"
        if self.runMode == 'sge':
            print " -> Algorithm will run in SGE batch"
        elif self.runMode == 'pbs':
            print " -> Algorithm will run in Torque/PBS batch"
        elif self.runMode == 'lxbatch':
            print " -> Algorithm will run in lxbatch"
        elif self.runMode == 'local':
            print " -> Algorithm will run on local machine"
        elif self.runMode == 'pathena':
            print " -> Algorithm will run in pathena"
        elif self.runMode == 'prun':
            print " -> Algorithm will run with prun"
        else:
            print " -> runMode ",self.runMode," not implemented yet, exiting..."
            sys.exit()
            
        #if runMode=="local" check if number of jobs exceeds number of cpus available 
        #if self.runMode == 'local':
        #    nLocalCPUs = os.sysconf("SC_NPROCESSORS_ONLN")
        #    if isinstance(nLocalCPUs, int)\
        #           and nLocalCPUs>0 and self.nCPUs > nLocalCPUs: 
        #        print "More jobs assigned than CPUs available on local machine",\
        #              "(%d), exiting..." % nLocalCPUs
        #        sys.exit()

        #disable solving when running only a single job since no matrices have to be combined
        if self.nCPUs==1 and self.runMode!='pathena' and self.runMode!='prun' \
                and self.doSolve:
            self.doSolve=False
            print "Disabling solving. For a single job no matrices have to be combined."

        #zip logfiles when running on batch
        if self.runMode != 'local' and self.runMode != 'pathena' and self.runMode != 'prun':
            self.zipLogFiles = True
        return


    def getJobLists(self) :
        
        if self.runMode == 'local':
            jobList = LocalJobList(self.nCPUs) 
        elif self.runMode == 'sge':
            jobList = SgeJobList(self.queue)
        elif self.runMode == 'pbs':
            jobList = PbsJobList(self.queue) 
        elif self.runMode == 'lxbatch':
            jobList = LxbatchJobList(self.queue)
        elif self.runMode == 'pathena' or self.runMode == 'prun':
            doPrun = (self.runMode=='prun')
            jobList = PathenaJobList(doPrun)

        if self.runMode != 'local' and self.solveLocally:
            solveJobList = LocalJobList(self.nCPUs)
        else:
            solveJobList = jobList

        return jobList,solveJobList

    def getString(self,iteration,index) :
        retstring = ""
        if iteration>-1:
            retstring += "_iter"+str(iteration)
        #if self.runMode != 'pathena' :
        #    if str(index) == "*":
        #        retstring += "_index*"
        #    elif index>-1:
        #        retstring += "_index"+str(index)
        if str(index) == "*":
            retstring += "_index*"
        elif index>-1:
            retstring += "_index"+str(index)
        return retstring

    def getPath(self,path):
        if len(path)>0 and not path.endswith("/"):
            path += "/"
        elif len(path)==0:
            path = "./"
        return path


    def processAccumulateJobs(self,iter,accumJobList) :

        print "in processAccumulateJobs"

        scriptWriter=ScriptWriter(self,iter)

        njobs = self.nCPUs
        if self.runMode == 'pathena' or self.runMode == 'prun' :
            njobs = 1
        print "njobs: ",njobs,"\n"

        for j in range(0, njobs):
            print "  job: ",j,"\n"

            if njobs == 1:
                iCPU = -1
            else:
                iCPU = j

            #if not self.doAccumulate:
            #if j == 0:
            #    print "Skip Accumulation Iter"+repr(iter)
            #continue
        
            #write the joboption file which handles the inputfiles of the job
            if self.files==NotImplemented:
                print "files not set!"
                sys.exit(89)
            inputDataMap = self.files.writeFile(self,iCPU,scriptWriter.ScriptDir)
            if not inputDataMap:
                print "  No input files in list for this job, continue with next job."
                continue
            if inputDataMap.find("/") != -1:
                inputDataMap = inputDataMap.rsplit("/",1)[1]

            print "inputDataMap:", inputDataMap

            #-------------------------    
            #-- write run script
            #-------------------------   
            jobrunscript=scriptWriter.runScript(self,iter,j,iCPU,accumJobList,inputDataMap)

            #-------------------------    
            #-- process jobs
            #-------------------------
            #if self.doAccumulate:
            batchLogFile = scriptWriter.LogDir + scriptWriter.JobName + '.batchlog'
            if ((self.runMode=='pathena' or self.runMode=='prun') and self.resumePathenaJob):
                print "not running accumJob because resumePathenaJob==True"
            else:
                print "starting accumJob"
                accumJobList.process(jobrunscript,batchLogFile)

            #-------------------------    
            #-- wait for jobs to finish and solve
            #-------------------------    
        print "\nWaiting for jobs to finish"
        #if not self.doAccumulate:
        scriptWriter.setCPU(iter,-1)
        batchLogFile = scriptWriter.LogDir + scriptWriter.JobName + '.batchlog'
        accumJobList.wait(batchLogFile)
        print "jobs done"

        return

    def checkAccumulateJobs(self,iter,accumJobList) :
        #-------------------------    
        #-- check all jobs finished successfully             
        #-------------------------    
        
        scriptWriter=ScriptWriter(self,iter)
        
        #loop over Jobs/CPUs
        njobs = self.nCPUs
        reprocessing = True
        while reprocessing :
            reprocessing = False
            for j in range(0, njobs):
                if njobs == 1:
                    iCPU = -1
                    AsztOutputDir = scriptWriter.IterPath
                else:
                    iCPU = j
                    AsztOutputDir = scriptWriter.LogDir

                if not self.doAccumulate:
                  continue

                scriptWriter.setCPU(iter,iCPU)
                logFile=scriptWriter.LogDir + scriptWriter.JobName + '.log'
                if accumJobList.check(logFile):
                    continue
                        
                print "job ",iCPU," failed, resubmitting"
        
                #write the joboption file and run script
                inputDataMap = self.files.writeFile(self,iCPU,scriptWriter.ScriptDir)
                if not inputDataMap:
                    print "  No input files in list for this job, continue with next job."
                    continue
                if inputDataMap.find("/") != -1:
                    inputDataMap = inputDataMap.rsplit("/",1)[1]
                    jobrunscript=scriptWriter.runScript(self,iter,j,iCPU,accumJobList,inputDataMap)
                  
                #-------------------------    
                #-- reprocess jobs
                #-------------------------
                batchLogFile = scriptWriter.LogDir + scriptWriter.JobName + '.batchlog'
                if ((self.runMode=='pathena' or self.runMode=='prun') \
                        and self.resumePathenaJob):
                    print "not running accumJob because resumePathenaJob==True"
                else:
                    print "starting accumJob"
                    accumJobList.process(jobrunscript,batchLogFile)
                    reprocessing = True

            if reprocessing:
                print "Waiting for resubmitted jobs to finish"
                accumJobList.wait(batchLogFile)

        return


    def processSolveJob(self,iter,solveJobList) :

        scriptWriter=ScriptWriter(self,iter)

        scriptWriter.setSolve()

        # the input files aren't needed for solving, but it's easier to configure as for 
        # a normal job so the conditions database is configured properly
        inputDataMap = self.files.writeFile(self,-1,scriptWriter.ScriptDir)
        if not inputDataMap:
            if self.runMode=='local':
                print self.fileList.inputPaths
            print "  No input fileSs in list for this job!"
        if inputDataMap.find("/") != -1:
            inputDataMap = inputDataMap.rsplit("/",1)[1]

        jobrunscript=scriptWriter.solveScript(self,iter,solveJobList,inputDataMap)
        batchLogFile = scriptWriter.LogDir + scriptWriter.JobName + '.batchlog'

        print "starting solving job"
        solveJobList.process(jobrunscript,batchLogFile)

        print "Waiting for solving job to finish"
        solveJobList.wait(batchLogFile)       


class Flags():
    """Class to contain flags passed to athena job."""

    def __init__ (self):
        self.errorScaleDb  = ""
        self.alignmentJob  = ""
        self.doEndcapOnly  = False
        self.doBarrelOnly  = False
        self.readEventList = False
        self.nEventsPerJob = -1
        self.skipEvents    = 0
        self.iteration     = 0
        self.tag           = ""
