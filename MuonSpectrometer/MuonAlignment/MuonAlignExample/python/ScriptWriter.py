# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import commands

#from IOFiles import IOFiles
from Jobs import JobThread,LocalJobList,SgeJobList,LxbatchJobList,PathenaJobList,PbsJobList

#-----------------------------------------------------------------------
class ScriptWriter:
    """ Class used to write scripts for accumulation and solving jobs."""
    
    def __init__(self,iterator,iteration):
        self.tmpDir=""
        self.JobName=iterator.rootName + iterator.getString(iteration,-1)
        self.iterator=iterator
        self.iteration=iteration
        self.iCPU=0
        self.rootName = iterator.rootName
        self.IterPath  = iterator.outputPath
        self.LastIterPath = self.IterPath
        self.FirstIterPath = self.IterPath
        if iterator.nCPUs > 1:
            self.IterPath      += 'iter' + repr(iteration) + '/'
            self.LastIterPath  += 'iter' + repr(iteration-1)+'/'
            self.FirstIterPath += 'iter0/'

        self.MatrixDir = self.IterPath + "matrices/"
        self.ScriptDir = self.IterPath + "scripts/"        
        self.LogDir    = self.IterPath + "logs/"        
        self.extInputFiles = []

        #if iterator.doAccumulate:
        print "creating directories in ",self.IterPath
        if not os.path.isdir(self.IterPath):
            os.mkdir(self.IterPath)
        if not os.path.isdir(self.ScriptDir):
            os.mkdir(self.ScriptDir)
            os.mkdir(self.LogDir)
            os.mkdir(self.MatrixDir)
            #os.mkdir(self.IterPath + "MuonAlignExample/")
 
        self.AsztOutput = self.JobName + '.txt'
        self.RootFile   = ''
        self.execString = ''

    def setCPU(self,iteration,iCPU):
        self.iteration=iteration
        self.iCPU=iCPU
        self.JobName = self.iterator.rootName + \
            self.iterator.getString(iteration,iCPU)
        self.AsztOutput = self.JobName + '.txt'
        #if iteration>0:
        #    self.Level1InputDest = 'level1_' + self.iterator.rootName + self.iterator.getString(iteration-1,iCPU)+'.txt'
        #else:
        #    self.Level1InputDest = 'level1_' + self.iterator.rootName + '.txt'

    def setSolve(self):
        self.JobName = "%s_iter%d_Solve" % (self.iterator.rootName,self.iteration)
        self.outputName = "%s_iter%d" % (self.iterator.rootName,self.iteration)
        self.AsztOutput = self.outputName + '.txt'

    def solveScript(self,iterator,iteration,solveJobList,inputDataMap):
        
        self.setSolve()
        
        # create jobscript
        jobrun=self.initJobScript()
        
        # run setup script
        jobrun.write(self.doSetupStr())
        
        # create tmp directory for running job
        if isinstance(solveJobList,SgeJobList)\
               or isinstance(solveJobList,PbsJobList):
            tmpdir = "$TMPDIR"
        else:
            tmpdir = "\"/tmp/\"$USER"            
            jobrun.write(self.createTmpDirStr(tmpdir,self.JobName))
            
        # set CORAL environment (for SGE)
        if iterator.runMode=='sge':
            jobrun.write(self.setCoralEnvStr(iterator.outputPath))

        # input ASZT/multilevel alignment files
        jobrun.write(self.setInputAlignmentStr())

        # input jobOptions
        jobrun.write(self.setInputJobOptionsStr())

        # copy input files
        if inputDataMap and iterator.runMode!='prun':
            jobrun.write(self.copyInputFilelistStr(inputDataMap))

        # copy input matrix/vector files
        jobrun.write(self.copyInputMatrixFilesStr())

        doSolve=1
        asztOutputDir=self.IterPath
        
        # get athena command
        athCommandStr=self.athenaCommandStr(inputDataMap,doSolve)
        if iterator.runMode=='prun':
            jobrun.write(self.prunExecStr())
        jobrun.write(athCommandStr)

        # get output files
        jobrun.write(self.getOutputStr(iterator,doSolve))

        # cleanup and close file
        jobrun.write(self.cleanupStr())
        jobrun.close()
        os.chmod(jobrun.name,0744)

        return jobrun

    def runScript(self,iterator,iteration,job,iCPU,jobList,inputDataMap):
        
        self.extInputFiles=[]

        self.setCPU(iteration,iCPU)

        print "in ScriptWriter.runScript"

        # run setup script
        if not isinstance(jobList,PathenaJobList):
            jobstring=self.doSetupStr()
        else:
            jobstring=""

        # create tmp directory or use current directory for running job
        if isinstance(jobList,SgeJobList)\
               or isinstance(jobList,PbsJobList):
            tmpdir = "$TMPDIR"
        elif isinstance(jobList,PathenaJobList):
            ret, out=commands.getstatusoutput("echo $PWD")
            tmpdir = out
            print "tmpdir: ",tmpdir
        else:
            tmpdir = "\"/tmp/\"$USER"
        
        #if not isinstance(jobList,PathenaJobList):
        jobstring += self.createTmpDirStr(tmpdir,self.JobName)
        #else:
        #    jobstring += self.setCurrentDirStr()
        
        # set CORAL environment (for SGE)
        if iterator.runMode=='sge':
            jobstring += self.setCoralEnvStr(iterator.outputPath)

        # input ASZT/multilevel alignment files
        jobstring += self.setInputAlignmentStr()

        # input jobOptions
        #if iterator.runMode != 'pathena':
        jobstring += self.setInputJobOptionsStr()
        
        # copy input files
        print "inputDataMap: ",inputDataMap,", itertor.runMode: ",iterator.runMode

        if inputDataMap and iterator.runMode!='prun':
            print "copying inputFilelistStr"
            jobstring += self.copyInputFilelistStr(inputDataMap)
        
        # copy athena command
        doSolve=0
        athCommandStr=self.athenaCommandStr(inputDataMap,doSolve,job)
        if iterator.runMode=='prun':
            jobstring += self.prunExecStr()
        jobstring += athCommandStr

        # remove input files
        if inputDataMap:
            jobstring += self.rmInputFilelistStr(inputDataMap)

        # create jobscript
        jobrun = self.initJobScript()
        jobrun.write(jobstring)

        # get output files and clean up
        if iterator.runMode != 'pathena':
            jobrun.write(self.getOutputStr(iterator,doSolve))
            jobrun.write(self.cleanupStr())

        jobrun.close()
        os.chmod(jobrun.name,0744)

        print "returning from runScript"
        return jobrun

    def pathenaPostJobScript(self):
        
        print "in pathenaPostJobScript"

        self.setSolve()

        jobName = self.iterator.rootName + \
            self.iterator.getString(self.iteration,-1)

        # create tmp directory for running job
        jobstring = "files=$(ls %s*log.tgz 2> /dev/null | wc -l)\n" % (self.LogDir)
        jobstring += "if [ \"$files\" != \"0\" ]; then\n"
        jobstring += "  exit\n"
        jobstring += "fi\n"

        # create tmp directory for running solving job
        tmpdir="\"/tmp/\"$USER"
        jobstring += self.createTmpDirStr(tmpdir,jobName)

        # get output files
        jobstring += self.getPathenaOutputStr()

        # clean up
        jobstring += self.cleanupStr()

        # create job script
        jobrun=self.initJobScript("postjobscript")
        jobrun.write(jobstring)
        jobrun.close()
        os.chmod(jobrun.name,0744)

        return jobrun

#-------------------------------------------------------------------------
    def initJobScript(self,scriptname="runscript"):

        name=self.ScriptDir+self.JobName+'_'+scriptname+'.sh'

        jobrun=open(name,'w')
        jobrun.write("#!"+os.environ['SHELL']+"\n")
        jobrun.write("echo 'host='`hostname`\n")        
        return jobrun

#-------------------------------------------------------------------------
    def doSetupStr(self):

        retstr  = "cd "+os.environ.get('TestArea')+"\n"
        retstr += "asetup "+os.environ.get('AtlasVersion')+",here\n\n"

        return retstr
    
    def createTmpDirStr(self,tmpdir,jobName):
        retstring = "RUNDIR=%s\"/run_%s\"\n" % (tmpdir,jobName)
        retstring += "RUNDIR=${RUNDIR}\"_\"$(date +\%Y\%m\%d_\%H\%M\%S)\n"
        retstring += "mkdir -p $RUNDIR\n"
        retstring += "cd $RUNDIR\n"
        retstring += "echo 'RUNDIR='`pwd`\n\n"
        retstring += "mkdir MuonAlignExample\n\n"
        return retstring

    def setCurrentDirStr(self):
        ret, out=commands.getstatusoutput("echo $PWD")
        return "cd "+out+"\n\n"
    
    def setCoralEnvStr(self,outputPath):
        return "export CORAL_AUTH_PATH=%s\n\n" % outputPath        
    
    def setInputAlignmentStr(self):

        iterator=self.iterator
        iter    =self.iteration

        # input ASZT file
        retstr=""
        if (iter==0 or iterator.alignmentLevel!='L3') :
            inputAsztFileOrig=iterator.inputAsztFile
        else:
            inputAsztFileOrig=iterator.outputPath
            inputAsztFileOrig = self.LastIterPath + iterator.rootName + \
                iterator.getString(iter-1,-1)
            if iterator.doSolve:
                inputAsztFileOrig += '_Solve'
            inputAsztFileOrig += '.txt'
        if inputAsztFileOrig!='':
            inputAsztFileDest = iterator.rootName
            #if (iter>0 and iterator.doLevel1):
            #    inputAsztFileDest += iterator.getString(iter-1,iCPU)
            #inputAsztFileDest +=
            #retstr += "cp %s %s\n" % (inputAsztFileOrig,inputAsztFileDest)         
            retstr += "cp %s %s\n" % (inputAsztFileOrig,"asztInput.txt") 
 
        self.extInputFiles.append("asztInput.txt")

        if iterator.errorScaleDb!="":
            errScaleDbName=iterator.errorScaleDb
            errScaleDbNameSplit=errScaleDbName.rsplit("/",1)
            if len(errScaleDbNameSplit)>1:
                errScaleDbName=errScaleDbNameSplit[1]            
            retstr += "cp %s/%s %s\n" % (iterator.outputPath,errScaleDbName,errScaleDbName)
            self.extInputFiles.append(errScaleDbName)

        if iterator.alignmentLevel=='L3':
            return retstr

        # input L1 and L2.3 ascii file
        levelstr = ''
        if iterator.alignmentLevel == 'L1':
            levelstr = 'level1'
        if iterator.alignmentLevel == 'L3':
            levelstr = 'level3'
        elif iterator.alignmentLevel == 'L2.3':
            levelstr = 'level23'
        
        asciiInputOrig=""
        if iter>0:            
            asciiInputOrig = self.LastIterPath + levelstr+'_' + iterator.rootName + \
                                iterator.getString(iter-1,-1) + '.txt'
            #level1InputDest = 'level1_' + iterator.rootName + \
            #    iterator.getString(iter-1,iCPU) + '.txt'
        elif (iterator.alignmentLevel == 'L1' and iterator.inputL1Ascii != ''):
            asciiInputOrig = iterator.inputL1Ascii
        elif (iterator.alignmentLevel == 'L2.3' and iterator.inputL23Ascii != ''):
            asciiInputOrig = iterator.inputL23Ascii   
        if asciiInputOrig!="":
            retstr += "cp %s %s\n" % (asciiInputOrig,levelstr+"input.txt") 
        retstr += "\n"

        self.extInputFiles.append(levelstr+"input.txt")

        return retstr

    def setInputJobOptionsStr(self):
        
        retstr  = "cp -Lr "+self.iterator.outputPath+"*.py MuonAlignExample/.\n"
        #retstr += "cp -Lr "+self.iterator.outputPath+"PoolFileCatalog.xml .\n"
        return retstr

    def copyInputFilelistStr(self,inputDataMap):
        retstr  = "cp "+self.ScriptDir+inputDataMap +" .\n"
        if self.iterator.readEventList and self.iteration>0:
            eventListOrig="goodEvts_" + self.iterator.rootName + \
                self.iterator.getString(0,self.iCPU)  + ".txt"
            eventListDest="goodEvts" + self.iterator.getString(0,-1) + ".txt"
            #+ self.iterator.rootName + 
            
            retstr += "cp "+self.FirstIterPath+eventListOrig+' '+eventListDest+'\n\n'
            self.extInputFiles.append(eventListDest) 
        return retstr

    def rmInputFilelistStr(self,inputDataMap):
        retstr  = "rm -f "+inputDataMap +"\n"
        if self.iterator.readEventList:
            eventListOrig="goodEvts" + self.iterator.getString(0,self.iCPU) + ".txt"
            retstr += "rm -f " + eventListOrig + '\n\n'
        return retstr

    def copyInputMatrixFilesStr(self):

        #retstr  = "cp "+self.MatrixDir+"matrix_*.bin .\n"
        #retstr += "cp "+self.MatrixDir+"vector_*.bin .\n"
        #retstr += "cp "+self.MatrixDir+"hitmap_*.bin .\n"
        #retstr += "cp "+self.MatrixDir+"matrix_*.txt .\n"
        #retstr += "cp "+self.MatrixDir+"vector_*.txt .\n\n"
        #retstr += "cp "+self.MatrixDir+"hitmap_*.txt .\n\n"

        retstr  = "cp "+self.MatrixDir+"*matrix*.bin* .\n"
        retstr += "cp "+self.MatrixDir+"*vector*.bin* .\n"
        retstr += "cp "+self.MatrixDir+"*hitmap*.bin* .\n"
        if not self.iterator.doBinariesOnly:
            retstr += "cp "+self.MatrixDir+"*matrix*.txt* .\n"
            retstr += "cp "+self.MatrixDir+"*vector*.txt* .\n\n"
            retstr += "cp "+self.MatrixDir+"*hitmap*.txt* .\n\n"
        return retstr

    def athenaCommandStr(self,inputDataMap,doSolve,job=0):
        
        print "athenaCommandStr"

        iterator=self.iterator
        iteration=self.iteration


        
        execstring = "-c 'doSolve="+str(doSolve)
        if iterator.runOptions:
            execstring += ";"+iterator.runOptions
        if iterator.runMode!='pathena' and iterator.errorScaleDb!='':
            execstring += ";errorScaleDb=\\\""+iterator.errorScaleDb+"\\\""
        execstring += ";alignmentJob=\""+iterator.alignmentJob+"\""
        execstring += ";doEndcapOnly=" +str(iterator.doEndcapOnly)
        execstring += ";doBarrelOnly=" +str(iterator.doBarrelOnly)
        execstring += ";readEventList="+str(iterator.readEventList)
        execstring += ";maxEvents="    +str(iterator.nEventsPerJob)
        execstring += ";skipEvents="   +str(iterator.skipEvents)
        execstring += ";iteration="    +str(iteration)
        if iterator.tag!="":
            execstring += ";tag=\""+iterator.tag+"\""
        commissioning='True'
        data='True'
        if iterator.dataType=='MC':
            commissioning='False'
            data='False'
        execstring += ";commissioning="+commissioning+";data="+data
        if (doSolve):
            execstring += ";solveOnly=True"
        if iterator.preIncludes!=[]:
            execstring += ";preIncludes=["
            first=True
            for preInclude in iterator.preIncludes:
                if not first:
                    execstring += ","
                execstring += "\"MuonAlignExample/"+preInclude+"\""
                first=False
            execstring += "]"

        #if self.iCPU>-1 and not doSolve:
        #    execstring += ";jobIndex="+str(job)
        #elif doSolve:
        #    execstring += ";jobIndex=-1"

        jobstring = ""
        
        print "doSolve: ",doSolve
        if doSolve:
            print "adding matrixfiles"
            if inputDataMap:
                #jobstring += "\necho \"inputMatrixFiles="+self.inputFiles('matrix').replace('"','\"')+"\">"+inputDataMap
                f = open(self.ScriptDir+"/"+inputDataMap,"a")
                f.write("\ninputMatrixFiles="+self.inputFiles('matrix'))
                f.write("\ninputVectorFiles="+self.inputFiles('vector'))
                f.write("\ninputHitmapFiles="+self.inputFiles('hitmap'))
                f.close()
            else:
                execstring += ";inputMatrixFiles="+self.inputFiles('matrix')
                execstring += ";inputVectorFiles="+self.inputFiles('vector')
                execstring += ";inputHitmapFiles="+self.inputFiles('hitmap')
        execstring += "'"
        print "jobstring: ",jobstring

        if inputDataMap and iterator.runMode!='prun':
            execstring += " "+inputDataMap
        execstring += " MuonAlignExample/"+self.iterator.runargsJobOption

        print "execstring: ",execstring

        if not doSolve and iterator.files!=NotImplemented:
            jobstring += "\n"+iterator.files.scpString(job,iterator.nFilesPerJob)+"\n"
        if iterator.runMode!='prun' and iterator.runMode!='pathena' and not doSolve:
            #if not doSolve and iterator.files!=NotImplemented:
            #    jobstring += "\n"+iterator.files.scpString(job,iterator.nFilesPerJob)+"\n"
            jobstring += "\nathena.py "
        elif doSolve:
            jobstring += "\nathena.py "
        elif iterator.runMode=='pathena':
                jobstring += "\npathena -s "
        else:
                jobstring += "\nprun "

        if (iterator.runMode!='pathena' and iterator.runMode!='prun') or doSolve:
            jobstring += execstring

        elif (not doSolve or iterator.solveLocally==False):

            doESD=False
            if iterator.runargsJobOption.find('ESD')!=-1:
                doESD=True

            # input and output datasets
            if iterator.runMode=='pathena':
                jobstring += "--inDS="+iterator.inputDatasetDef
                jobstring += " --outDS="+iterator.outputDatasetDef
                if iterator.libDS!="":
                    jobstring += " --libDS="+iterator.libDS
            else:
                jobstring += "--inDS="+iterator.inputDatasetDef
                jobstring += " --outDS="+iterator.outputDatasetDef

                
            # prun options
            if iterator.runMode=='prun':
                jobstring += " --noBuild --useAthenaPackages"
                if doESD:
                    jobstring += " --match \'*ESD*pool*\'"

            # output files
            outputfiles = "asztOutput.txt,"
            outputfiles += "alignlogfile.txt,"
            outputfiles += "Align.root,"
            outputfiles += "matrix.bin,matrix.txt,vector.bin,vector.txt,"
            outputfiles += "hitmap.bin,hitmap.txt"
            if self.iteration==0:
                outputfiles += ",goodEvts.txt"

            # output files
            if iterator.runMode=='pathena':
                jobstring += " --extOutFile=" + outputfiles
            else:
                jobstring += " --outputs=" + outputfiles

            # nFilesPerJob, site
            if iterator.nFilesPerJob > 0:
                jobstring += " --nFilesPerJob="+str(iterator.nFilesPerJob)
            if iterator.site != "":
                jobstring += " --site="+iterator.site

            # dbRelease
            jobstring += " --dbRelease=LATEST"
            jobstring += " --tmpDir=/tmp"

             # for official jobs
            if iterator.official:
                jobstring += " --official"
                jobstring += " --voms=atlas:/atlas/det-muon/Role=production"
                
            # input files
            if len(self.extInputFiles)>0:
                jobstring += " --extFile="
                nfiles = 0
                for file in self.extInputFiles:
                    jobstring += file
                    nfiles += 1
                    if nfiles != len(self.extInputFiles):
                      jobstring += ","
                jobstring += " "

            if iterator.runMode=='prun':
                jobstring += ' --exec=\"runathena.sh %IN\"'
                self.execString = execstring
            else:
                jobstring += execstring
 
        print "nCpus: ",iterator.nCPUs
        if (iterator.runMode == 'local' and \
                (iterator.nCPUs == 1 or not iterator.doAccumulate)):
            jobstring += " 2>&1 |tee "
        else:
            jobstring += " 2>&1 > "
        jobstring += self.JobName+".log"
        jobstring += "\n\n"
        return jobstring
        
    def prunExecStr(self):
        print "in prunExecStr"
        if self.execString=='':
            print "no prun exec!"
            return
        retstr ="\nrm -f runathena.sh"
        retstr+="\necho \"athena.py -c \'inputFile=\\\"\'\$1\'\\\".split(\\\",\\\")\' "+self.execString+"\" > runathena.sh\n"
        print "retstr: ",retstr

        return retstr

    def getOutputStr(self,iterator,doSolve=False):
        retstr=""

        # L1, L2.3, L3 output
        if self.iterator.alignmentLevel == 'L1'\
                or self.iterator.alignmentLevel == 'L2.3'\
                or  self.iterator.alignmentLevel == 'L3'  :

            asciiOutput = ''
            if self.iterator.alignmentLevel == 'L1':
                asciiOutput = "level1_" + self.iterator.rootName + \
                               self.iterator.getString(self.iteration,-1) + '.txt'
                retstr += "echo \"/Muon/Align/L1\" > %s\n" % (asciiOutput)
            elif self.iterator.alignmentLevel == 'L2.3':
                asciiOutput = "level23_" + self.iterator.rootName + \
                               self.iterator.getString(self.iteration,-1) + '.txt'
                retstr += "echo \"/Muon/Align/L23_Bar\" > %s\n" % (asciiOutput)
            elif self.iterator.alignmentLevel == 'L3':
                asciiOutput = "level3_" + self.iterator.rootName + \
                               self.iterator.getString(self.iteration,-1) + '.txt'
                retstr += "echo \"/Muon/Align/L3\" > %s\n" % (asciiOutput)
               
            #retstr += "cat %s >> %s\n" % (self.AsztOutput,level1Output)
            retstr += "cat %s >> %s\n" % ("asztOutput.txt",asciiOutput)
            retstr += "mv %s %s.\n"    % (asciiOutput,self.IterPath)
        else:
            # L3 aszt output
            retstr += "mv %s %s/%s\n"  % ("asztOutput.txt",self.IterPath,self.JobName+".txt")

        # log files
        logFile = self.JobName + '.log'
        if self.iterator.zipLogFiles:
            retstr += "gzip %s\n" % logFile
            retstr += "mv %s.gz %s.\n" % (logFile,self.LogDir)
        else:
            retstr += "mv %s %s.\n"  % (logFile,self.LogDir)
        retstr += "mv %s %s%s\n" % ('alignlogfile.txt',self.LogDir,self.JobName+".alglog")

        # output root file
        rootFile = "MuonAlign_%s.root" % self.JobName
        if self.iterator.doSolve and not self.iterator.doAccumulate:
            retstr += "mv %s %s%s\n" % ("AlignSolve.root",self.IterPath,rootFile)
        else:
            retstr += "mv %s %s/%s\n" % ("Align.root",self.IterPath,rootFile)
                
        retstr += "mv %s %s\n"   % ("matrix.bin",self.MatrixDir+'matrix_'+self.JobName+'.bin')
        retstr += "mv %s %s\n"   % ("matrix.txt",self.MatrixDir+'matrix_'+self.JobName+'.txt')
        retstr += "mv %s %s\n"   % ("vector.bin",self.MatrixDir+'vector_'+self.JobName+'.bin')
        retstr += "mv %s %s\n"   % ("vector.txt",self.MatrixDir+'vector_'+self.JobName+'.txt')
        retstr += "mv %s %s\n"   % ("hitmap.bin",self.MatrixDir+'hitmap_'+self.JobName+'.bin')
        retstr += "mv %s %s\n"   % ("hitmap.txt",self.MatrixDir+'hitmap_'+self.JobName+'.txt')
        if self.iteration == 0:
            outputEventList = "goodEvts_" + self.iterator.rootName + \
                self.iterator.getString(self.iteration,-1) + '.txt'
            eventListOrig = "goodEvts_" + self.iterator.rootName + \
                self.iterator.getString(0,self.iCPU) + '.txt'
            retstr += "mv %s %s%s\n" % ("goodEvts.txt",self.IterPath,eventListOrig)
        retstr += '\n'

        return retstr

    def getPathenaOutputStr(self):

        iterator = self.iterator
        iter     = self.iteration

        dsdef = iterator.outputDatasetDef

        # dq2-get output files
        jobstr = "\n"
        if not iterator.doBinariesOnly:
            jobstr += "dq2-get "+dsdef+"/\n"
        else:
            jobstr += "dq2-get -f *matrix.bin* "+dsdef+"/\n"
            jobstr += "dq2-get -f *vector.bin* "+dsdef+"/\n"
            jobstr += "dq2-get -f *hitmap.bin* "+dsdef+"/\n"
        jobstr += "cd "+dsdef+"*\n\n"

        # rename to expected names and move to current directory        
        if not iterator.doBinariesOnly:
            jobstr += "mv %s*log.tgz %s.\n\n" % (dsdef,self.LogDir)
            
        rootFile = "MuonAlign_%s.root"  % self.JobName

        outputEventList = "goodEvts_" + iterator.rootName + \
            iterator.getString(iter,-1) + ".txt"
        
        jobstr += "mv *.EXT3.*.%s* %s/.\n\n" % \
            ("matrix.bin",self.MatrixDir) 

        jobstr += "mv *.EXT5.*.%s* %s/.\n\n" % \
            ("vector.bin",self.MatrixDir) 

        jobstr += "mv *.EXT7.*.%s* %s/.\n\n" % \
            ("hitmap.bin",self.MatrixDir) 
            
        if not iterator.doBinariesOnly:
            jobstr += "mv *.EXT0.*.%s* %s%s/.\n\n" % \
                ("asztOutput.txt",self.IterPath,self.AsztOutput) 
                
            jobstr += "mv *.EXT1.*.%s* %s/.\n\n" % \
                ('alignlogfile.txt',self.LogDir)
                
            jobstr += "mv *.EXT2.*.%s* %s/.\n\n" % \
                ('Align.root',self.IterPath )

            jobstr += "mv *.EXT4.*.%s* %s/.\n\n" % \
                ("matrix.txt",self.MatrixDir) 

            jobstr += "mv *.EXT6.*.%s* %s/.\n\n" % \
                ("vector.txt",self.MatrixDir) 

            jobstr += "mv *.EXT8.*.%s* %s/.\n\n" % \
                ("hitmap.txt",self.MatrixDir) 
                
            eventListOrig = "goodEvts_" + iterator.rootName + \
                iterator.getString(0,job) + '.txt'
                
            jobstr += "mv *.EXT9.*.%s* %s/.\n\n" % \
                ("goodEvts.txt",self.IterPath) 

        """
        for job in range(0,iterator.nCPUs):
            self.setCPU(iter,job)

            jobindex = "%05d" % (job+1)

            jobstr += "mv %s.EXT3._%s.%s %s%s%s%s\n\n" % \
                ("*",jobindex,"matrix.bin",self.MatrixDir,"matrix_",self.JobName,".bin") 

            jobstr += "mv %s.EXT5._%s.%s %s%s%s%s\n\n" % \
                ("*",jobindex,"vector.bin",self.MatrixDir,"vector_",self.JobName,".bin") 

            jobstr += "mv %s.EXT7._%s.%s %s%s%s%s\n\n" % \
                ("*",jobindex,"hitmap.bin",self.MatrixDir,"hitmap_",self.JobName,".txt") 
            
            if not iterator.doBinariesOnly:
                jobstr += "mv %s.EXT0._%s.%s %s%s\n\n" % \
                    ("*",jobindex,"asztOutput.txt",self.IterPath,self.AsztOutput) 
                
                jobstr += "mv %s.EXT1._%s.%s %s%s\n\n" % \
                    ("*",jobindex,'alignlogfile.txt',self.LogDir,self.JobName+".alglog")
                
                jobstr += "mv %s.EXT2._%s.%s %s%s\n\n" % \
                    ("*",jobindex,'Align.root',self.IterPath,rootFile) 

                jobstr += "mv %s.EXT4._%s.%s %s%s%s%s\n\n" % \
                    ("*",jobindex,"matrix.txt",self.MatrixDir,"matrix_",self.JobName,".txt") 

                jobstr += "mv %s.EXT6._%s.%s %s%s%s%s\n\n" % \
                    ("*",jobindex,"vector.txt",self.MatrixDir,"vector_",self.JobName,".txt") 

                jobstr += "mv %s.EXT8._%s.%s %s%s%s%s\n\n" % \
                    ("*",jobindex,"hitmap.txt",self.MatrixDir,"hitmap_",self.JobName,".bin") 
                
                eventListOrig = "goodEvts_" + iterator.rootName + \
                    iterator.getString(0,job) + '.txt'
                
                jobstr += "mv %s.EXT9._%s.%s %s%s\n\n" % \
                    ("*",jobindex,"goodEvts.txt",self.IterPath,eventListOrig) 
                """


        return jobstr
          
    def cleanupStr(self):
        retstr  = "cd ..\n"
        retstr += "rm -rf $RUNDIR\n"
        retstr += "echo '--finished--'\n"
        
        return retstr

    def inputFiles(self,filetype):
        """Method which writes the input vector, matrix, and hitmap files needed for solving jobs."""
        
        print "\n\n\ngetting inputFiles\n\n\n"

        if filetype!="matrix" and filetype!="vector" and filetype!="hitmap":
            print "Error: filetype should be matrix, vector, or hitmap, exiting..."
            sys.exit()

        jobName  = self.JobName            

        print "MatrixDir: ",self.MatrixDir
        import glob
        print "looking for: ",self.MatrixDir+"*"+filetype+"*bin*"

        filenames = glob.glob(self.MatrixDir+"*"+filetype+"*bin*")
        filenames.sort()
        print "filenames: ",filenames

        if self.MatrixDir.find("/") != -1:
            filenames = map(lambda filename: filename.rsplit("/",1)[1], filenames)
            filestring = str(filenames).replace(",",",\n")
            filestring = str(filenames).replace("'","\"")
            
        print "filestring: ",filestring
        return filestring
    
