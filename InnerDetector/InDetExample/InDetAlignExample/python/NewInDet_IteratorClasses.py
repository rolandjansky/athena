# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# =====================================================================
#
#  Class for local processing
#
# =====================================================================

import os
import time

class getAthenaConfig:
    def __init__(self, options):
        self.atlasSet = "AtlasSetup"
        
        self.athenaRel = os.environ['AtlasVersion']
        self.athenaProj = os.environ['AtlasProject']
        self.athenaConf = os.environ['CMTCONFIG']
        if "x86_64" in self.athenaConf:
            self.athenaConf=self.athenaConf.replace("x86_64","64")
        if "devval" in os.environ['AtlasArea']:
            self.athenaConf += "-devval"
        
        try:
            self.athenaTestArea = os.environ['TestArea']
        except Exception:
            self.athenaTestArea = ""
        try:
            self.athenaTags = os.environ['AtlasProject'] + ',' + self.athenaConf.replace("-",",")
        except Exception:
            self.athenaTags = ""
        if self.atlasSet == "AtlasSetup":
            self.atlasSetupPath = os.environ['AtlasSetup']
        else:
            self.atlasSetupPath = ""
        # other options 
        self.options = options 

        print ("------------------------------------------")
        print ("Retrieved athena configuration:")
        print ("* Using " + self.atlasSet)
        print ("* Athena release: ", self.athenaRel)
        print ("* Athena tags: ", self.athenaTags)
        print ("* Athena TestArea: ", self.athenaTestArea)
        print ("------------------------------------------")
    def Release(self):
        return  self.athenaRel
    def Tags(self):
        return self.athenaTags
    def TestArea(self):
        return self.athenaTestArea
    def atlasSetup(self):
        return self.atlasSet
    def AtlasSetupPath(self):
        return self.atlasSetupPath
    def AtlasSetupOptions(self):
        return self.options




class manageJob:
    def __init__(self,
             OutputPath,
             dataName,
             iter,
             part,
             JOBNAME,
             preName,
             ATHENACFG,
             RecoOptions={},
             extraOptions={},
             AlignmentOptions={},
             RecoScript="InDetAlignExample/NewTopOptions.py",
             ConditionsScript="InDetAlignExample/jobOption_ConditionsOverrider.py",
             AlignmentScript="InDetAlignExample/NewInDetAlignAlgSetup.py",
             AlignmentLevels = "InDetAlignExample/NewInDetAlignLevels.py",
             #MonitoringScript = "InDetRecExample/InDetMonitoringAlignment.py", 
             QUEUE = "1nh",
             CMTDIR = "",
             inputPoolFiles = "",
             inputCoolFiles = "",
             folderSuffix=""):
        self.OutputPath = OutputPath
        self.preName = preName
        self.i=iter
        self.j=part
        self.dataName = dataName
        self.QUEUE=QUEUE
        self.CMTDIR=CMTDIR
        self.ATHENACFG=ATHENACFG
        self.inputPoolFiles = inputPoolFiles
        self.inputCoolFiles = inputCoolFiles
        self.JOBNAME=JOBNAME
        self.RecoOptions = RecoOptions
        self.ConditionsScript = ConditionsScript
        self.extraOptions = extraOptions
        self.RecoScript = RecoScript
        self.AlignmentOptions = AlignmentOptions
        self.AlignmentScript = AlignmentScript
        self.AlignmentLevels = AlignmentLevels
        self.folderSuffix = folderSuffix
        #self.MonitoringScript = MonitoringScript
        if self.j==-1:
            self.RunPath="%s/Iter%d%s/" % (self.OutputPath, self.i, self.folderSuffix)
            self.SCRIPTNAME = "%s_Iter%d_Solve.lsf" % (self.preName,self.i)
        else:
            self.RunPath="%s/Iter%d%s/%s/%02d/" % (self.OutputPath, self.i, self.folderSuffix, self.dataName, self.j)
            self.SCRIPTNAME = "%s_Iter%d%s_%s_Part%02d.lsf" % (self.preName,self.i,self.folderSuffix, self.dataName, self.j)

        
    def createDirectories(self):
        os.system('mkdir -p %s' % self.RunPath)
        #os.system('mkdir -p %s' % self.RunPath+"/InDetRecExample")
        #if self.MonitoringScript != "":
        #   os.system('ln -s %s %s' % (self.MonitoringScript,self.RunPath+"/InDetRecExample/InDetMonitoringAlignment.py") )

    def writeJO(self):

        job=open(self.RunPath + self.JOBNAME,'w')
        job.write('##-------- Alignment Configuration --------------------\n')

        job.write("outputPoolFile        = \"%s/Iter%d_AlignmentConstants.root\"\n" %(self.RunPath,self.i))

        for option in self.AlignmentOptions:
            if type(self.AlignmentOptions[option]) is str:
                customoption = option + "\t=\t\"" +  self.AlignmentOptions[option]+'\"\n'
                job.write(customoption)
            else:
                customoption = option + "\t=\t" +  str(self.AlignmentOptions[option])+'\n'
                job.write(customoption)
            
        job.write("\n")
        job.write('##-------- Loading the Alignment Levels --------------------\n')
        job.write('include("'+str(self.AlignmentLevels)+'") \n')
        job.write("\n")
        job.write('##-------- Reconstruction Configuration --------------------\n')
        
        for option in self.RecoOptions:
            if type(self.RecoOptions[option]) is str:
                customoption = option + "\t=\t\"" +  self.RecoOptions[option]+'\"\n'
                job.write(customoption)
            else:
                if option == "inputFiles":
                    customoption = option + "\t=\t" +  str(self.RecoOptions[option][:-1])+'\n'
                else:
                    customoption = option + "\t=\t" +  str(self.RecoOptions[option])+'\n'
                job.write(customoption)

        if len(self.extraOptions):
            job.write("\n")
            job.write('##-------- Extra Configuration --------------------\n')
        
        for option in self.extraOptions:
            if type(self.extraOptions[option]) is str:
                customoption = option + "\t=\t\"" +  self.extraOptions[option]+'\"\n'
                job.write(customoption)
            else:
                customoption = option + "\t=\t" +  str(self.extraOptions[option])+'\n'
                job.write(customoption)

        job.write("\n")
        job.write('##-------- End of custom options -------------\n')

        # Need to write the InDetRec Options Here:
        job.write("\n")
        job.write('##-------- Load Reconstruction --------------------\n')
        job.write('include("'+str(self.RecoScript)+'") \n')
        job.write("\n")
        job.write('##-------- Load Conditions Overrider --------------------\n')
        job.write('include("'+str(self.ConditionsScript)+'") \n')
        job.write("\n")
        job.write('##-------- Load Alignment --------------------\n')
        job.write('include("'+str(self.AlignmentScript)+'") \n')
        job.write("\n")
        job.close()

    def writeScript(self):
        self.SCRIPTNAME = self.RunPath + '/' + self.SCRIPTNAME
        script=open(self.SCRIPTNAME,'w')
        if self.j!=-1:      
            script.write("#BSUB -J %s_Iter%d%s_%s_Part%02d -C 1\n" % (self.preName,self.i,self.folderSuffix,self.dataName, self.j))
            script.write("#BSUB -o %s/Iter%d%s/logs/Iter%d_%s_Part%02d.log \n" % (self.OutputPath,self.i,self.folderSuffix,self.i,self.dataName,self.j))
            script.write("#BSUB -e %s/Iter%d%s/logs/Iter%d_%s_Part%02d.err \n" % (self.OutputPath,self.i,self.folderSuffix,self.i,self.dataName,self.j))
        else:
            script.write("#BSUB -J %s_Iter%d%sSolve -C 1\n" % (self.preName, self.i, self.folderSuffix))
            script.write("#BSUB -o %s/Iter%d%s/logs/Iter%d%sSolve.log \n" % (self.OutputPath,self.i,self.folderSuffix,self.i,self.folderSuffix))
            script.write("#BSUB -e %s/Iter%d%s/logs/Iter%d%sSolve.err \n" % (self.OutputPath,self.i,self.folderSuffix,self.i,self.folderSuffix))
            
        script.write("#BSUB -q %s \n" % self.QUEUE)
        script.write("\n")
        script.write("#   setup the environment \n")
        if self.ATHENACFG.atlasSetup() == "CMTHOME":
            script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))
        elif "single" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s --single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))

        elif "nightlies" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 20.1.X rel1 --testarea=%s --single \n" % (self.ATHENACFG.TestArea()))

        else:
            script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
        for file in self.inputPoolFiles:
            if "atlasdatadisk" in file:
                script.write("export STAGE_SVCCLASS=atldata\n")
                break
            if "DAQ" in file or "tzero" in file:
                script.write("export STAGE_SVCCLASS=atlcal\n") 
                break
            if "atlasgroupdisk" in file:
                script.write("export STAGE_SVCCLASS=atlasgroupdisk\n") 
                break
            if "atlasscratchdisk" in file:
                script.write("export STAGE_SVCCLASS=atlasscratchdisk\n")
                break
            if "atlasdatadisk" in file:
                script.write("export STAGE_SVCCLASS=atldata\n")
                break
            if "atlascerngroupdisk" in file:
                script.write("export STAGE_SVCCLASS=atlascerngroupdisk\n")
                script.write("export STAGE_HOST=castoratlast3\n")
                break
        
        script.write("cd %s \n" % self.RunPath)

        for file in self.inputPoolFiles:
            #if 'ESD' in file or 'AlignmentConstants' in file:
            if 'AlignmentConstants' in file or 'ESD' in file:
                script.write("pool_insertFileToCatalog "+ file + " \n")
        
        script.write("athena %s \n" % self.JOBNAME)
        #Cleaning the core in case of errors
        script.write("rm -rf core* \n")
        script.write("cd - \n")
        script.close()

    def send(self,runmode):
        os.system("chmod +x %s" % self.SCRIPTNAME)
        print ("----------------------------------------------")
        if runmode == "batch":
            os.system("bsub <%s" % self.SCRIPTNAME)
           
            if self.j==-1:
                print ("  Sending %s_Solve_Iter%d%s job " % (self.preName, self.i, self.folderSuffix))
            else:
                print ("  Sending %s_Iter%d%s_%s_Part%02d job to LxBatch" % (self.preName,self.i,self.folderSuffix,self.dataName, self.j))
                  
        elif runmode == "local":
            if self.j!=-1:
                print ("  Running %s_Iter%d%s_%s_Part%02d job" % (self.preName,self.i,self.folderSuffix,self.dataName,self.j))
                os.system("sh %s | tee %s/Iter%d%s/logs/Iter%d%s_%s_Part%02d.log \n" % (self.SCRIPTNAME, self.OutputPath,self.i,self.folderSuffix,self.i,self.folderSuffix,self.dataName,self.j))
            else:
                print ("  Running %s_Iter%dSolve job" % (self.preName,self.i))
                os.system("sh %s | tee %s/Iter%d%s/logs/Iter%d%sSolve.log \n" % (self.SCRIPTNAME, self.OutputPath,self.i,self.folderSuffix,self.i,self.folderSuffix))
            
        print ("----------------------------------------------")

    def wait(self):
        print ("Processing in lxbatch... look for jobs with ", self.preName, " and ", self.folderSuffix  )
        # Wait for signal
        #print (" wait for signal .salva. ")
        time.sleep(30)
        while (os.popen('bjobs -w').read().find(self.preName)!=-1 and os.popen('bjobs -w').read().find(self.folderSuffix)!=-1):
            time.sleep(30)
            
class SortCpus:
    def __init__(self, TOTALCPUS, LOCALDIR, FILELIST, OutputLevel,doDetailedSplitting = False, nEventsPerFile=-1):
            def sort_by_value(d):
                """ Returns the keys of dictionary d sorted by their values """
                items=d.items()
                backitems=[ [v[1],v[0]] for v in items]
                backitems.sort()
                backitems.reverse() 
                return [ backitems[i][1] for i in range(0,len(backitems))]      

            self.doDetailedSplitting = doDetailedSplitting
            self.OutputLevel = OutputLevel
            self.Files = FILELIST
            self.LocalDir = LOCALDIR
            self.nSubJobs = TOTALCPUS
            self.totalNumberOfEvents = 0
            self.totalNumberOfFiles = 0
            self.nEventsPerFile = nEventsPerFile
            inputFileList = []
            
            if not self.doDetailedSplitting:
                inputfiles = open(FILELIST, "r")
                filelist = inputfiles.read().split('\n')
                for i in range(0,len(filelist)):
                    filelist[i] = filelist[i].rstrip()
                inputfiles.close()
                if not LOCALDIR:

                    print ("Reading Custom File")
                    FinalListSorted = []
                    for line in filelist:
                        if line and line[0] != '#':
                            FinalListSorted.append(line)

                    #print (FinalListSorted)

                elif "castor" in LOCALDIR:
                    print ("Reading castor directory " + LOCALDIR + " Please wait...")
                    extendedFileList = os.popen("rfdir "+ LOCALDIR[7:]).read().splitlines()
                else:
                    print ("Reading directory " + LOCALDIR + " Please wait...")
                    extendedFileList = os.popen("ls -l "+ LOCALDIR).read().splitlines()

                if LOCALDIR:
                    i = 0
                    SizeList = {}
                    for line in extendedFileList:
                        curr = line.split()
                        if curr[0] != 'total':
                            #print (curr[8], " ", curr[4])
                            SizeList[i] = {}
                            SizeList[i][0] = curr[8].rstrip()
                            SizeList[i][1] = curr[4].rstrip()
                            i = i+1
                    FinalList = {}
                    
                    for i in range(0,len(SizeList)):
                        #print (SizeList[i][0])
                        if SizeList[i][0] in filelist:
                            #print ("Accepted")
                            #print (SizeList[i][0], " size:", SizeList[i][1])
                            FinalList[SizeList[i][0]] = int(SizeList[i][1])

                    #SizeListSorted = [ (k,SizeList[k]) for k in sorted(SizeList.values())]

                    FinalListSorted = sort_by_value(FinalList)
                    #print ("Sorted list" )
                    #for i in range(0,len(FinalListSorted)):
                    #   print (FinalListSorted[i], "\tsize:\t", FinalList[FinalListSorted[i]])
                currCPU = 0
                self.CPUsFiles = {}
                nFiles = len(FinalListSorted)
                nRemainder = nFiles % TOTALCPUS
                nFilesPerCpu = nFiles / TOTALCPUS
                nFilesForThisCpu = 1
                sumFileSize = 0
                if len(FinalListSorted[0].split())==2:
                    for i in range(0,nFiles):
                        sumFileSize += int(FinalListSorted[i].split()[1])
                print (sumFileSize)
                averageSizePerCpu =sumFileSize/TOTALCPUS*0.97
                print (averageSizePerCpu)
                sumSizeOnCpu = 0
                #print ("NFile, Remainder, NfilesperCpu  ",  nFiles, " ", nRemainder, " ", nFilesPerCpu)
        #If the file size is present then use it to split the files
                if sumFileSize != 0:
                    for i in range(0,nFiles):
                        if currCPU in self.CPUsFiles:
                            self.CPUsFiles[currCPU].append(LOCALDIR+FinalListSorted[i].split()[0])
                        else:
                            self.CPUsFiles[currCPU] = [LOCALDIR+FinalListSorted[i].split()[0]]
                            extraFiles = 0
                        sumSizeOnCpu += int(FinalListSorted[i].split()[1])                  
                        if(sumSizeOnCpu > averageSizePerCpu and i < nFiles-1 and ( sumSizeOnCpu + int(FinalListSorted[i+1].split()[1]) ) > averageSizePerCpu * 1.04 ):
                            print ("File size on CPU: " ,currCPU,'\t' ,  sumSizeOnCpu)
                            currCPU = currCPU + 1
                            if currCPU >= TOTALCPUS:
                                currCPU = TOTALCPUS-1
                            else:
                                sumSizeOnCpu = 0
                        elif(nFiles-i == TOTALCPUS - currCPU):
                            currCPU = currCPU + 1
                    print ("File size on CPU: " ,currCPU,'\t' ,  sumSizeOnCpu)
                else:
                    for i in range(0,nFiles):
                        #print (FinalListSorted[i], "CPU: ", currCPU, " FPCPU: " , nFilesForThisCpu)
                        if currCPU in self.CPUsFiles:
                            self.CPUsFiles[currCPU].append(LOCALDIR+FinalListSorted[i].split()[0])
                        else:
                            self.CPUsFiles[currCPU] = [LOCALDIR+FinalListSorted[i].split()[0]]
                        extraFiles = 0
                        if(currCPU<nRemainder):
                            extraFiles = 1
                        if(nFilesForThisCpu < nFilesPerCpu+extraFiles):
                            nFilesForThisCpu = nFilesForThisCpu + 1
                        else:
                            currCPU = currCPU + 1
                            nFilesForThisCpu = 1
                        

            # Doing the Detailed slitting
            else:
                import PyUtils.PoolFile as PF

                #Getting the number of events in each file.
                inputfiles = open(FILELIST, "r")
                numEventsPerFile = {}
                print ("==================================================")
                print ("The input file are: (May take some time..)")
                for line in inputfiles:
                    if line.rstrip().find(".root") > 0:

                        fullFileName = self.LocalDir + "/"+ line.rstrip()

                        inputFileList.append(fullFileName)

                        poolFile = PF.PoolFile(fullFileName)
                        thisNumEvents = int(poolFile.dataHeader.nEntries)
                        
                        self.totalNumberOfEvents += thisNumEvents

                        self.totalNumberOfFiles += 1

                        print (fullFileName," with ",thisNumEvents," events")

                        numEventsPerFile[fullFileName] = thisNumEvents
                print ("==================================================")

                #Getting the Number of events to process, to skip, and the inputFile

                # The relavent quantities for each subJob
                self.m_skipEvents = {}
                self.nEvents = 0
                self.m_inputFiles = {}

                # This means will do all the events
                if self.nEventsPerFile == -1:
                    self.nEvents = int(self.totalNumberOfEvents/self.nSubJobs)
                else:
                    self.nEvents = self.nEventsPerFile

                # local counters
                m_usedFiles = 0
                m_numberEventsUsed =0
                for subJob in range(self.nSubJobs):
                    self.m_inputFiles[subJob] = []
                    m_eventsNeeded = self.nEvents
                                        
                    while(m_eventsNeeded != 0 and m_usedFiles < self.totalNumberOfFiles):
                        # Two case the file indexed by m_usedFiles has enough event to complete
                        #  the events needed or it doesn't

                        # If it does
                        if m_eventsNeeded <= numEventsPerFile[inputFileList[m_usedFiles]]:
                            numEventsPerFile[inputFileList[m_usedFiles]] -= m_eventsNeeded

                            # Debugging
                            #print ("subJob",subJob)
                            #print ("m_eventsNeeded",m_eventsNeeded)
                            #print ("self.m_inputFiles",self.m_inputFiles)
                            #print ("m_usedFiles",m_usedFiles)
                            
                            self.m_inputFiles[subJob].append(inputFileList[m_usedFiles])
                            self.m_skipEvents[subJob] = m_numberEventsUsed
                            m_numberEventsUsed += m_eventsNeeded
                            m_eventsNeeded = 0
                            print ("self.m_skipEvents["+str(subJob)+"]",self.m_skipEvents[subJob])
                            print ("m_numberEventsUsed",m_numberEventsUsed)
                            
                        # If it doesn't
                        else:
                            m_eventsNeeded -= numEventsPerFile[inputFileList[m_usedFiles]]
                            self.m_skipEvents[subJob] = m_numberEventsUsed
                            self.m_inputFiles[subJob].append(inputFileList[m_usedFiles])
                            m_usedFiles += 1
                            m_numberEventsUsed = 0

                            # Debugging
                            #print ("self.m_skipEvents["+str(subJob)+"]",self.m_skipEvents[subJob])
                            #print ("m_numberEventsUsed",m_numberEventsUsed)
                            #print ("m_eventsNeeded",m_eventsNeeded)


                            
    def getCPU(self,CURRENTCPU):
        if self.OutputLevel=='DEBUG':
            print ("|",40*"-"," CPU #: ", CURRENTCPU, 40*"-", "|")
            for line in self.CPUsFiles[CURRENTCPU]:
                print ("|  - ",line)
            print ("|",93*"-","|")
        return self.CPUsFiles[CURRENTCPU]
           
    def getNumEvents(self,subJob):
        return self.nEvents

    def getSkipEvents(self,subJob):
        return self.m_skipEvents[subJob]

    def getInputFiles(self,subJob):
        return self.m_inputFiles[subJob]
        
def CreateFileList(RunList,recotag,Challenge,Stream,DataFile,Collision,BField=True):
    if os.path.isfile(DataFile):
        print ("The file ", DataFile, " exists, using the existing one.")
        return 0

    outputFile = open(DataFile, "w")
    if Collision:
        count = 0
        for run in RunList:
            run = int(run)
            if Stream == "MinBias" and Challenge.thisChallenge == "900GeV":
                path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/%07d/data09_900GeV.%08d.physics_MinBias.recon.ESD.%s/" % (run, run, recotag[count])
            elif Stream == "express" and Challenge.thisChallenge == "900GeV":
                path = "/castor/cern.ch/grid/atlas/DAQ/2009/%08d/express_express/" % run
            elif Stream == "IDTracks" and Challenge.thisChallenge == "900GeV":
                path = "/castor/cern.ch/grid/atlas/DAQ/2009/%08d/calibration_IDTracks/" % run
            elif Stream == "IDTracks" and Challenge.thisChallenge == "7TeV":
                path = "/castor/cern.ch/grid/atlas/DAQ/2010/%08d/calibration_IDTracks/" % run
            elif Stream == "MinBias" and Challenge.thisChallenge == "7TeV":
                path = "/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/%s/data10_7TeV.%08d.physics_MinBias.recon.ESD.%s/" % (recotag[count],run,recotag[count]) 
            
            else:
                print ("Combination Stream/challenge not coded for automatic run list")
                return 0
            print ("Reading castor directory: ", path)
            inputfiles = os.popen("rfdir "+ path).read().splitlines()
            for file in inputfiles:
                outputFile.write(path + file.split()[8] + '\t' + file.split()[4] + '\n')
              #outputFile.write(path + file.split()[8]  + '\n')
            count = count + 1
        print ("Created file " ,DataFile, " with the list of datafiles")
    
    if not Collision:
        if BField:
            count = 0
            for run in RunList:
                run = int(run)
                if Stream == "CosmicCalo" and Challenge.thisChallenge == "7TeV":
                    path = "/castor/cern.ch/grid/atlas/atlasdatadisk/data10_7TeV/ESD/%s/data10_7TeV.%08d.physics_CosmicCalo.recon.ESD.%s/" % (recotag[count],run,recotag[count])
                elif Stream == "IDCosmic" and Challenge.thisChallenge == "7TeV":
                    path = "/castor/cern.ch/grid/atlas/DAQ/merge/2010/%08d/physics_IDCosmic/" % (run)
                else:
                    print ("Combination Stream/challenge not coded for automatic run list")
                    return 0
                print ("Reading castor directory: ", path)
                inputfiles = os.popen("rfdir "+ path).read().splitlines()
                for file in inputfiles:
                    print (file)
                    outputFile.write(path + file.split()[8] + '\t' + file.split()[4] + '\n')
                count = count + 1   
            print ("Created file " ,DataFile, " with the list of datafiles")
            
        
        
        
            
def CreateCollisionFileList(RunList,recotag,Challenge,Stream):
    if Challenge.thisChallenge =='900GeV':
        outputFileName = "CustomRun_900GeV.txt"
    elif Challenge.thisChallenge == 'SingleBeam':   
        outputFileName = "CustomRun_SingleBeam.txt"
    elif Challenge.thisChallenge == '7TeV':
        outputFileName = "CustomRun_7TeV.txt"
    else:
        print ("Custom run list not coded for this challenge")
        return 0
    
    CreateFileList(RunList,recotag,Challenge,Stream,outputFileName,True)
    
def CreateCosmicNoBFFileList(RunList,recotag,Challenge,Stream):
    if Challenge.thisChallenge == '7TeV':
        outputFileName = "CustomRun_CosmicsNoBF_7TeV.txt"
    else:
        print ("Custom run list not coded for this challenge")
        return 0
    
    CreateFileList(RunList,recotag,Challenge,Stream,outputFileName,False,False)
    
def CreateCosmicBFFileList(RunList,recotag,Challenge,Stream):
    if Challenge.thisChallenge == '7TeV':
        outputFileName = "CustomRun_CosmicsBF_7TeV.txt"
    else:
        print ("Custom run list not coded for this challenge")
        return 0
    CreateFileList(RunList,recotag,Challenge,Stream,outputFileName,False,True)
    
    
def mergeMatrix(OutputPath, iter, DataToRun):

    print ("------------------------------------------")
    print ("  Setting Matrices list" )
    print ("------------------------------------------")

    matrixlist = []
    vectorlist = []
    hitmaplist = []
    for data in DataToRun:
        for j in range(0, data.getCPUs(iter)):
            TempPath = "%s/Iter%d/%s/%02d/" %  (OutputPath,iter,data.getName(),j)   
            if os.path.isfile( "%s/matrix.bin" % TempPath):
                matrixlist.append("%s/%02d/matrix.bin" % (data.getName(),j))
                vectorlist.append("%s/%02d/vector.bin" % (data.getName(),j))

            if os.path.isfile( "%s/hitmap.bin" % TempPath):
                hitmaplist.append("%s/%02d/hitmap.bin" % (data.getName(),j))

    return matrixlist,vectorlist,hitmaplist
    
def mergeTFiles(OutputPath, iter, folderSuffix, DataToRun):

    print ("------------------------------------------")
    print ("  Setting TFiles list" )
    print ("------------------------------------------")

    tfilelist = []
    for data in DataToRun:
        for j in range(0, data.getCPUs(iter)):
            TempPath = "%s/Iter%d%s/%s/%02d/" %  (OutputPath,iter, folderSuffix, data.getName(),j)  
            if os.path.isfile( "%s/AlignmentTFile.root" % TempPath) and int(os.stat("%s/AlignmentTFile.root" % TempPath).st_size) != 0:
                tfilelist.append("%s/%02d/AlignmentTFile.root" % (data.getName(),j))

    return tfilelist
        
             
# For the merging of the monitoring Files
class mergeScript:
    def __init__(self,
             OutputPath,
             preName,
             folderSuffix,
             iter,
             dataName,
             nCPUs,
             QUEUE,
             CMTDIR,
             ATHENACFG,
             SCRIPTNAME,
             JOBNAME
             ):
        self.OutputPath = OutputPath
        self.preName = preName
        self.folderSuffix=folderSuffix
        self.i=iter
        self.dataName = dataName
        self.nCPUs = nCPUs
        self.QUEUE=QUEUE
        self.CMTDIR=CMTDIR
        self.ATHENACFG=ATHENACFG
        self.SCRIPTNAME=SCRIPTNAME
        self.JOBNAME=JOBNAME

    def write(self):
        TempPath="%s/Iter%d%s" % (self.OutputPath, self.i, self.folderSuffix)

        self.SCRIPTNAME = TempPath + '/' + self.SCRIPTNAME
        
        # list of Files to be merged
        mergeFilesName = TempPath + '/merge_'+self.dataName+'_Files.txt'
        script=open(self.SCRIPTNAME,'w')
        
        script.write("#BSUB -J %s_Iter%d%s_%s_Merge -C 1\n" % (self.preName, self.i, self.folderSuffix, self.dataName))
        script.write("#BSUB -o %s/Iter%d%s/logs/Iter%d%s_%s_Merge.log \n" % (self.OutputPath,self.i, self.folderSuffix, self.i, self.folderSuffix, self.dataName))
        script.write("#BSUB -e %s/Iter%d%s/logs/Iter%d%s_%s_Merge.err \n" % (self.OutputPath,self.i, self.folderSuffix, self.i, self.folderSuffix, self.dataName))
        
        script.write("#BSUB -q %s \n" % self.QUEUE)
        script.write("\n")
        script.write("#   setup the environment \n")
        if self.ATHENACFG.atlasSetup() == "CMTHOME":
            script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))
        elif "single" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s --single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
        elif "nightlies" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 19.1.X rel1 --testarea=%s --single \n" % (self.ATHENACFG.TestArea()))
        else:
            script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
        script.write("cd %s/Iter%d%s/ \n" % (self.OutputPath,self.i,self.folderSuffix))
        script.write("DQHistogramMerge.py %s %s/TotalMonitoring.root True\n" %(mergeFilesName,self.dataName))
        script.write("cd - \n")
        for subJob in range(self.nCPUs):
            script.write("rm %s/Iter%d%s/%s/%02d/monitoring.root \n" % (self.OutputPath,self.i,self.folderSuffix,self.dataName,subJob))
        script.close()
        
        mergeFiles=open(mergeFilesName,"w")
        for subJob in range(self.nCPUs):
            mergeFiles.write("%s/Iter%d%s/%s/%02d/monitoring.root \n" %  (self.OutputPath,self.i,self.folderSuffix,self.dataName,subJob))
        mergeFiles.close()
                

    def send(self,runmode):
        os.system("chmod +x %s" % self.SCRIPTNAME)
        print ("----------------------------------------------")
        if runmode == "batch":
            os.system("bsub <%s" % self.SCRIPTNAME)
            
            print ("  Sending %s_Merge_%s_Iter%d%s job " % (self.preName, self.dataName, self.i, self.folderSuffix))
            
        elif runmode == "local":
            print ("  Running %s_Merge_%s_Iter%d%s job" % (self.preName, self.dataName, self.i, self.folderSuffix))
            os.system("sh %s | tee %s/Iter%d%s/logs/Iter%d%s_%s_Merge.log \n" % (self.SCRIPTNAME, self.OutputPath,self.i, self.folderSuffix, self.i, self.folderSuffix, self.dataName))
            
        print ("----------------------------------------------")
                       
         



# For Comparing the before and after monitoring files 
class compareMonitoringScript:
    def __init__(self,
             OutputPath,
             numIter,
             QUEUE,
             CMTDIR,
             ATHENACFG
             ):
        self.OutputPath = OutputPath
        self.numIter = numIter
        self.QUEUE=QUEUE
        self.CMTDIR=CMTDIR
        self.ATHENACFG=ATHENACFG

        
    def write(self):
        # Names of the Job and the Script
        self.SCRIPTNAME = self.OutputPath + '/MonitoringComparison.lsf'
        self.JOBNAME = 'MonitoringComparison.py'
        
        # Write the Script
        script=open(self.SCRIPTNAME,'w')
        script.write("#BSUB -J MonitoringComparision \n")
        script.write("#BSUB -o "+self.OutputPath+"/MonitoringComparision.log \n")
        script.write("#BSUB -e "+self.OutputPath+"/MonitoringComparision.err \n")
        script.write("#BSUB -q %s \n" % self.QUEUE)
        script.write("\n")
        script.write("#   setup the environment \n")
        if self.ATHENACFG.atlasSetup() == "CMTHOME":
            script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))
        elif "single" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s --single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
        else:
            script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s \n" % (self.ATHENACFG.AtlasSetupPath()),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()) 
        script.write("cd "+self.OutputPath+" \n")
        script.write("athena.py "+self.JOBNAME+" \n")
        script.write("cd - \n")
        script.close()
        
        # Write the Job
        job=open(self.OutputPath+"/"+self.JOBNAME,"w")
        job.write(" \n")
        job.write("# ==================================== \n")
        job.write("# The Reference File (Black)           \n")
        job.write('afterAlignmentFile = "'+self.OutputPath+'/Iter'+str(self.numIter-1)+'/TotalMonitoring.root" \n')
        job.write(" \n")
        job.write("# ==================================== \n")
        job.write("# The Monitored File (Red)             \n")
        job.write('beforeAlignmentFile = "'+self.OutputPath+'/Iter0/TotalMonitoring.root"\n')
        job.write("\n")
        job.write("# ==================================== \n")
        job.write("# The Output File                      \n")
        job.write('outputFile = "AlignmentOutput.root"    \n')
        job.write("\n")
        job.write("# ==================================== \n")
        job.write("include('InDetAlignmentMonitoring/makeComparision.py') \n")
        job.write(" \n")
        job.close()
        
    def send(self,runmode):
        os.system("chmod +x %s" % self.SCRIPTNAME)
        print ("----------------------------------------------")
        if runmode == "batch":
            os.system("bsub <%s" % self.SCRIPTNAME)
            
            print ("  Sending MonitoringComparision.lsf job ")
            
        elif runmode == "local":
            print ("  Running MonitoringComparision.lsf job")
            os.system("sh "+self.SCRIPTNAME+" | tee "+self.OutputPath+"/MonitoringComparison.log \n")
            
        print ("----------------------------------------------")

def HandleRunOptions():
    import sys
    from InDetAlignExample.NewInDet_SetupData import setupData
    # =====================================================================
    #  Default values
    # =====================================================================
    Def_RunNumber = 162882
    User_RunNumber = 0
    RunNumber = Def_RunNumber
    Def_ColEvents = 100
    User_ColEvents = 0
    Col_Events = Def_ColEvents
    Def_ColCPUs = 1
    User_ColCPUs = 0
    Col_CPUs = Def_ColCPUs

    argCurr = -1 # jut to make that first time it points to 0
    
    #print (' >>> user input has %s arguments' % argMax)
    for arg in sys.argv:
        argCurr += 1
        #print (".. %s .." % arg)
        if arg in ("-h", "--help"):  
            print (' >>> This is the help message of RunIterator.py'     )
            print (' NAME')
            print ('     RunIterator.py')
            print (' ')
            print (' SYNOPSIS')
            print ('     python RunIterator.py [-r RunNumber] [-e ColEvents]' )
            print (' ')
            print (' OPTIONS')
            print ('     The following options are recognized by RunIterator.py')
            print ('     -r RunNumber')
            print ('          The user specifies the run number. Default run number is %d ' % Def_RunNumber)
            print (' ')
            print ('     -e ColEvents')
            print ('          The user specifies the number of events for the collision data (default: %d)' % Def_ColEvents )
            print (' ')
            print ('     -q ColCPUs')
            print ('          The user specifies the number of cpus to be used for the collision data (default: %d)' % Def_ColCPUs )
            print (' ')
            print (' 22/September/2010')
            sys.exit()
        elif arg in ("-r", "--run"):    
            # print (' >>> The user wants to provide his run number... lets see. This argument is the %s argument ' % argCurr       )
            User_RunNumber = int(sys.argv[argCurr+1])
            if User_RunNumber >0:
                RunNumber = User_RunNumber
                # print ('>>> User run number = %s' % RunNumber)
        elif arg in ("-e", "--events"):    
            # print (' >>> The user wants to provide the number of events ... lets see. This argument is the %s argument ' % argCurr       )
            User_ColEvents = int(sys.argv[argCurr+1])
            if User_ColEvents >0:
                Col_Events = User_ColEvents
                # print ('>>> User events (collision data) = %s' % Col_Events)
        elif arg in ("-q", "--cpus"):    
            # print (' >>> The user wants to provide the number of cpus ... lets see. This argument is the %s argument ' % argCurr       )
            User_ColCPUs = int(sys.argv[argCurr+1])
            if User_ColCPUs >0:
                Col_CPUs = User_ColCPUs
                # print ('>>> User cpus (collision data) = %s' % Col_CPUs)
        elif arg in("-rtt","-RTT"):
            RTTConfig = True
    if 'rtt' in os.environ['USER']:
        RTTConfig = True    
    
    print ('>>> RunIterator >>> List of values:')
    if not RTTConfig:
        print (' RunNumber = %d' % RunNumber)
    if RTTConfig:
        print (' Detected RTT!! ')
    print (' # Events (col) = %d' % Col_Events)
    print (' # CPUS (col) = %d' % Col_CPUs)
    print ('>>> End of input options processing <<<'             )
    
    if RTTConfig:
        Data1 = setupData('Collision')
        Data1.setDataType('MC10_Singlemuons') 
        Col_CPUs = 1
        Col_Events = 10000
        Data1.setGlobalTag("OFLCOND-DR-BS7T-ANom-11")
    else:
        Data1 = setupData('IDTracks')
        Data1.setDataType("IDTracks")
    Data1.setRuns([RunNumber])
    Data1.setCPUs([Col_CPUs,Col_CPUs])
    Data1.setEvents([Col_Events,Col_Events])
    Data1.CreateFileList()
    return Data1
