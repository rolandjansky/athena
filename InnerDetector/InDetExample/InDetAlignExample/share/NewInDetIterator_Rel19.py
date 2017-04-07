#################################################################
#    Script to run InDetAlignExample with parallelization       #
#      (local or lxbatch mode) & (physic data or cosmics)       #
#             Vicente Lacuesta [started 12-03-2008]             #
#################################################################

import os, string, time, datetime
import sys

# ==============================================================
#  End of User Options
# ==============================================================

# =====================================================================
# =====================================================================
# =====================================================================
# =====================================================================
#
# Don not edit the lines below unless you know what you're doing!
#

print "Output saved in:",OutputPath

# AW: moved that one to python dir such that it can be imported from anywhere
from InDetAlignExample.NewInDet_IteratorClasses import *

# =====================================================================
#
#  Main Script
#
# =====================================================================


# extract release configuration

ATHENACFG = getAthenaConfig(ASetupOptions)


print
if not os.path.isdir(OutputPath):
    os.mkdir(OutputPath)

info=open(OutputPath+"/info.txt",'w')
info.write("----------------------------------------------\n")
info.write("\t\t%s      \n" % datetime.date.today() )
info.write("----------------------------------------------\n")
#info.write("Release %s\n" % ATHENAREL)
info.write("Output stored in %s\n\n" % OutputPath)
print "Info stored in: " +OutputPath+"/info.txt"

if runMode == 'batch':
    print "Alignment Algorithm will run in Lxbatch"
    info.write("Alignment Algorithm run in Lxbatch\n")
elif runMode == ' local':
    print "Alignment Algorithm will run on local machine"
    info.write("Alignment Algorithm run on local machine\n")
    info.write("----------------------------------------------\n")
    print "System Info"
    info.write("----------------------------------------------\n")
    info.write("System Info\n")
    os.system('grep processor /var/log/dmesg | grep MHz')
    info.write("----------------------------------------------\n")
    info.write(os.popen('grep processor /var/log/dmesg | grep MHz','r').read())
    info.write("\n")
    os.system('dmesg | grep Brought')
    info.write(os.popen('dmesg | grep Brought','r').read())
    info.write("\n")
    os.system('cat /proc/meminfo | grep MemTotal')
    info.write(os.popen('cat /proc/meminfo | grep MemTotal','r').read())
    info.write("\n")
    info.write("----------------------------------------------\n")
    info.write("\n")

print
info.write("\n")
StartTime=time.time()   # Start the total time counter
info.close()

# check that user requires to run some iterations:
if (Iterations == 0):
    print " ------------------------------------------------- \n -- WARNING -- user requests Iterations = 0 !!! -- \n ------------------------------------------------- \n" 

#  Loop over iterations
for iteration in range(FirstIteration,Iterations+FirstIteration):
    IterStartTime=time.time()
    if (iteration == 0) and not inputAlignmentPoolFile:
        ReadAlignmentConstants = False
    else:
        ReadAlignmentConstants = True

    if (iteration == 0 ) and not inputBowingCoolFile:
	    ReadBowingParameter = False
    else:
        ReadBowingParameter = True

    print '\n'
    print " ---> Iteration "+repr(iteration)
    print '\n'
    info=open(OutputPath+"/info.txt",'a')
    info.write('\n')
    info.write("---> Iteration "+repr(iteration))
    info.write('\n')
    # Protect existing directories
    if os.path.isdir("%s/Iter%d%s" % (OutputPath, iteration, folderSuffix)):
        countdir=0
        while os.path.isdir("%s/Iter%d%s-%s-%d" % (OutputPath, iteration, folderSuffix,datetime.date.today(), countdir)):
            countdir += 1
        os.rename("%s/Iter%d%s" % (OutputPath, iteration, folderSuffix),("%s/Iter%d%s-%s-%d" % (OutputPath, iteration, folderSuffix, datetime.date.today(), countdir)))

        print "WARNING: %s/Iter%d%s directory exists" % (OutputPath, iteration, folderSuffix)
        print "Renamed to %s/Iter%d%s-%s-%d" % (OutputPath, iteration, folderSuffix, datetime.date.today(), countdir)

    # Make OutputPaths
    folderName = OutputPath+'/Iter'+repr(iteration)+folderSuffix
    os.mkdir(folderName)
    os.mkdir(folderName+'/logs/')
    print " <NewInDetIterator> Storing all files of this iteration in folder ", folderName 

    # Settup up a local copy of the alignment levels
    os.system("get_files -jo InDetAlignExample/NewInDetAlignLevels.py >/dev/null")
    os.system("cp NewInDetAlignLevels.py %s" % folderName)
    alignLevels = folderName+"/NewInDetAlignLevels.py"
    
    for data in DataToRun:
        print "----------------------------------------------"
        print " Number of CPUs used to process the sample " + data.getName() + ": " + str(data.getCPUs(iteration))
        print "----------------------------------------------"

        info.write(" Number of CPUs used to process the sample %s : %s\n\n" % (data.getName(), data.getCPUs(iteration)))

        if data.getEvents(iteration)!=-1:
            print "Number of events per CPU: " +  str(data.getEventsPerCPU(iteration))
            info.write("Number of events per CPU: %d\n\n" % data.getEventsPerCPU(iteration))
            
        else:
            print "Number of events per CPU: All" 
            info.write("Number of events per CPU: All" )

        OutputPaths = OutputPath
        os.mkdir(folderName+'/'+data.getName()+'/')

        print " Processing..."
        
        # check how many jobs must be sent
        # if users requests many, but the input file has less files, then the number of
        # jobs must be capped
        numberOfSubJobs = data.getCPUs(iteration)
        if (data.getNFilesInList() < numberOfSubJobs): 
            numberOfSubJobs = data.getNFilesInList()
            print " >>> User requested ",data.getCPUs(iteration), " jobs but files has fewer entries. numberOfSubJobs= ", numberOfSubJobs     

        # Get the Input file
        dataFiles = SortCpus(numberOfSubJobs
                      ,""
                      ,data.getFileList()
                      ,OutputLevel
                      ,doDetailedSplitting=doDetailedSplitting
                      ,nEventsPerFile=data.getEventsPerCPU(iteration))

        preJOBNAME="%s_Iter%d%s_%s" % (preName,iteration,folderSuffix,data.getName())
        
        # Loop over subjobs     
        for subJob in range(0, numberOfSubJobs):
            RecoOptions = {}
            # Get the Reconstruction Options
            RecoOptions["ClusteringAlgo"] = ClusteringAlgo
            if ErrorScaling[iteration]:
                RecoOptions["errorScalingTag"] = errorScalingTag

            JOBNAME="%s_Part%02d.py" % (preJOBNAME,subJob)
            RecoScript = "InDetAlignExample/jobOption_RecExCommon.py"
            ConditionsScript = "InDetAlignExample/jobOption_ConditionsOverrider.py"

            if not doDetailedSplitting:
                RecoOptions["inputFiles"] = dataFiles.getCPU(subJob)
                RecoOptions["numberOfEvents"] = data.getEventsPerCPU(iteration)
            else:
                RecoOptions["inputFiles"] = dataFiles.getInputFiles(subJob)
                RecoOptions["numberOfEvents"] = dataFiles.getNumEvents(subJob)
                RecoOptions["SkipEvents"] = dataFiles.getSkipEvents(subJob)

            extraOptions["doReadBS"] = data.getByteStream()
            if (len(data.getProjectName())>0): 
                extraOptions["projectName"] = data.getProjectName()
            extraOptions["Cosmics"] = data.getCosmics()
            extraOptions["DigitalClustering"] = data.getDigitalClustering()
            extraOptions["PtCut"] = data.getPtMin()
                
            if subJob == 0:
                filesForSolve = RecoOptions["inputFiles"]
                solveDoReadBS = extraOptions["doReadBS"]
            if len(data.getGlobalTag()) != 0:
                RecoOptions["globalTag"] = data.getGlobalTag()
            if len(data.getDetDescrVersion()) != 0:
                RecoOptions["detectorDescription"] = data.getDetDescrVersion()
    
        
    
            # Get the Alignment Options
            AlignmentOptions = {}
            if AlignmentMethod == "GX2":
                AlignmentOptions["runLocal"] = False
                AlignmentOptions["solveLocal"] = False
                AlignmentOptions["solvingOption"] = 1
                # AlignmentOptions["ModCut"] = 6 # This is not needed anymore (Salva, 19/May/2015)
            else:
                AlignmentOptions["runLocal"] = True
                AlignmentOptions["solvingOption"] = 0
    
    
            AlignmentOptions["readConstantsFromPool"] = ReadAlignmentConstants
            constantsFile = ""
            bowingdb= ""
            if ReadAlignmentConstants:
                if iteration == 0:
                    constantsFile = inputAlignmentPoolFile 
                    AlignmentOptions["inputPoolFiles"] = [constantsFile]
                else:
                    constantsFile = str(OutputPaths)+"/Iter"+str(iteration-1)+folderSuffix+"/Iter"+str(iteration-1)+"_AlignmentConstants.root"
                    AlignmentOptions["inputPoolFiles"] = [constantsFile]

            if ReadBowingParameter:
                    if iteration == 0:
                        bowingdb = inputBowingCoolFile
                        AlignmentOptions["inputBowingDatabase"] = inputBowingCoolFile
                    else:
                        bowingdb = str(OutputPaths)+"/Iter"+str(iteration-1)+folderSuffix+"/mycool.db"
                        AlignmentOptions["inputBowingDatabase"] = bowingdb

            AlignmentOptions["alignTRT"] = AlignTRT[iteration]
            if AlignTRT[iteration]:
                AlignmentOptions["trtAlignmentLevel"] = TRTAlignmentLevel[iteration]
                AlignmentOptions["trtAlignBarrel"]    = AlignTRTBarrel[iteration]
                AlignmentOptions["trtAlignEndcaps"]   = AlignTRTEndcaps[iteration]
                AlignmentOptions["trtAlignmentLevel"] = TRTAlignmentLevel[iteration]
                AlignmentOptions["trtAlignmentLevelBarrel"] = TRTAlignmentLevelBarrel[iteration]
                AlignmentOptions["trtAlignmentLevelEndcaps"] = TRTAlignmentLevelEndcaps[iteration]

                
            AlignmentOptions["alignSCT"] = AlignSCT[iteration]
            #if AlignSCTBarrel[iteration]:
            AlignmentOptions["sctAlignmentLevel"] = SCTAlignmentLevel[iteration]
    
            AlignmentOptions["sctAlignBarrel"] = AlignSCTBarrel[iteration]
            #if AlignSCTBarrel[iteration]:
            AlignmentOptions["sctAlignmentLevelBarrel"] = SCTAlignmentLevelBarrel[iteration]
    
            AlignmentOptions["sctAlignEndcaps"] = AlignSCTEndcaps[iteration]
            #if AlignSCTEndcaps[iteration]:
            AlignmentOptions["sctAlignmentLevelEndcaps"] = SCTAlignmentLevelEndcaps[iteration]
    
            AlignmentOptions["alignPixel"] = AlignPixel[iteration]
            #if AlignSCTBarrel[iteration]:
            AlignmentOptions["pixelAlignmentLevel"] = PixelAlignmentLevel[iteration]
    
            AlignmentOptions["pixelAlignBarrel"] = AlignPixelBarrel[iteration]
            #if AlignPixelBarrel[iteration]:
            AlignmentOptions["pixelAlignmentLevelBarrel"] = PixelAlignmentLevelBarrel[iteration]
    
            AlignmentOptions["pixelAlignEndcaps"] = AlignPixelEndcaps[iteration]
            #if AlignPixelEndcaps[iteration]:
            AlignmentOptions["pixelAlignmentLevelEndcaps"] = PixelAlignmentLevelEndcaps[iteration]
    
    
            AlignmentOptions["runSolving"] = False
            AlignmentOptions["runAccumulate"] = True
            AlignmentOptions["WriteTFile"] = useTFiles
                            
    
            poolfiles = RecoOptions["inputFiles"]
            poolfiles.append(constantsFile)

            #coolfiles = RecoOptions["inputDbs"]
            #coolfiles.append("")


            #print " <NewInDetIterator> create a job: Outputpath = ", OutputPaths
            #print "                                     JOBNAME = ", JOBNAME
            currentjob = manageJob(OutputPath = OutputPaths,
                                   dataName = data.getName(),
                                   iter = iteration,
                                   folderSuffix = folderSuffix,
                                   part = subJob,
                                   JOBNAME = JOBNAME,
                                   preName = preName,
                                   RecoOptions = RecoOptions,
                                   extraOptions = extraOptions,
                                   AlignmentOptions = AlignmentOptions,
                                   RecoScript = RecoScript,
                                   ConditionsScript = ConditionsScript,
                                   AlignmentLevels = alignLevels,
                                   #MonitoringScript = MonitoringScript,
                                   QUEUE = QUEUE,
                                   CMTDIR = CMTDIR,
                                   ATHENACFG = ATHENACFG,
                                   inputPoolFiles = poolfiles,
                                   inputCoolFiles = []
                          )
    
            # Write the job
            currentjob.createDirectories()
            currentjob.writeJO()
            currentjob.writeScript()
            
            currentjob.send(runMode)
    #  End j loop
    #if runMode == "batch":
        # Wait for signal
    #   currentjob.wait()
    #print " -- SALVA -- Accumulate jobs already submitted. Waiting for completion -- "
    #rep = ''
    #while not rep in [ 'c', 'Q' ]:
    #    rep = raw_input( '[RunIterator]% enter "c" to continue: ' )
    #    if 1 < len(rep):
    #        rep = rep[0]

            
    #  Solving the system
    if doSolve:
        DataToSolve = list(DataToRun) # in case there is more than one set to run
        print "\n Waiting for Accumulate job completion. Please be patient. "
        while len(DataToSolve):
            for data in DataToSolve:
                batchjobs = "%s_Iter%d%s_%s_Part" % (preName,iteration,folderSuffix,data.getName())
                #print "            preJOBNAME ", preJOBNAME
                if os.popen('bjobs -w').read().find(preJOBNAME)!=-1: # check if accumulate jobs still running 
                    time.sleep(30) # wait for a while before asking again
                    continue
                DataToSolve.remove(data) # this data set is already completed 
                time.sleep(30) # wait a bit longer just to be sure reco step has already finished
                print " --> DataToSolve --> jobs running for ",len(DataToRun)," data types "
               
        print " >> Accumulate jobs were completed at ", time.strftime("%H:%M:%S")        
        info.write("\n Accumulate jobs were completed at \n")

        if (len(DataToRun)>1): 
            # As only one solving is sent for when combining several data sets, name it Total
            JOBNAME="%s_R%s_Iter%d%s_Solve.py" % (preName,"Total",iteration,folderSuffix)
            SCRIPTNAME="%s_R%s_Iter%d%sSolve.lsf" % (preName,"Total",iteration, folderSuffix)
        else:
            JOBNAME="%s_R%s_Iter%d%s_Solve.py" % (preName,data.getName(),iteration, folderSuffix)
            SCRIPTNAME="%s_R%s_Iter%d%sSolve.lsf" % (preName,data.getName(),iteration,folderSuffix)
            
            
        DataToSolve = list(DataToRun) # Restore data set the list
        
        # Obtain the aligment options
        AlignmentOptions = {}
            
        if AlignmentMethod == "GX2":
            AlignmentOptions["runLocal"] = False
            AlignmentOptions["solveLocal"] = False
        else:
            AlignmentOptions["runLocal"] = True
            AlignmentOptions["solvingOption"] = 0

        AlignmentOptions["readConstantsFromPool"] = ReadAlignmentConstants
    
        AlignmentOptions["inputPoolFiles"] = [constantsFile]

        AlignmentOptions["inputBowingDatabase"] = bowingdb
        
        AlignmentOptions["alignTRT"] = AlignTRT[iteration]
        if AlignTRT[iteration]:
            AlignmentOptions["trtAlignmentLevel"] = TRTAlignmentLevel[iteration]
            AlignmentOptions["trtAlignBarrel"]    = AlignTRTBarrel[iteration]
            AlignmentOptions["trtAlignEndcaps"]   = AlignTRTEndcaps[iteration]
            AlignmentOptions["trtAlignmentLevel"] = TRTAlignmentLevel[iteration]
            AlignmentOptions["trtAlignmentLevelBarrel"] = TRTAlignmentLevelBarrel[iteration]
            AlignmentOptions["trtAlignmentLevelEndcaps"] = TRTAlignmentLevelEndcaps[iteration]

        AlignmentOptions["alignSCT"] = AlignSCT[iteration]
        #if AlignSCTBarrel[iteration]:
        AlignmentOptions["sctAlignmentLevel"] = SCTAlignmentLevel[iteration]
    
        AlignmentOptions["sctAlignBarrel"] = AlignSCTBarrel[iteration]
        #if AlignSCTBarrel[iteration]:
        AlignmentOptions["sctAlignmentLevelBarrel"] = SCTAlignmentLevelBarrel[iteration]
        
        AlignmentOptions["sctAlignEndcaps"] = AlignSCTEndcaps[iteration]
        #if AlignSCTEndcaps[iteration]:
        AlignmentOptions["sctAlignmentLevelEndcaps"] = SCTAlignmentLevelEndcaps[iteration]
        
        AlignmentOptions["alignPixel"] = AlignPixel[iteration]
        #if AlignSCTBarrel[iteration]:
        AlignmentOptions["pixelAlignmentLevel"] = PixelAlignmentLevel[iteration]

        AlignmentOptions["pixelAlignBarrel"] = AlignPixelBarrel[iteration]
        #if AlignPixelBarrel[iteration]:
        AlignmentOptions["pixelAlignmentLevelBarrel"] = PixelAlignmentLevelBarrel[iteration]
    
        AlignmentOptions["pixelAlignEndcaps"] = AlignPixelEndcaps[iteration]
        #if AlignPixelEndcaps[iteration]:
        AlignmentOptions["pixelAlignmentLevelEndcaps"] = PixelAlignmentLevelEndcaps[iteration]

            
        # need to figure out the list of matrices from the individual Accumulate jobs
        tfiles = [] # set an empty list
        while len(DataToSolve):
            for data in DataToSolve:
                print "----------------------------------------------"
                print "  Solving dataset %s, Iter %s"%(data.getName(),iteration)
                print "----------------------------------------------"
            
                info.write('\n')
                info.write("----------------------------------------------\n")
                info.write("  Solving dataset %s, Iter %d\n" %(data.getName(),iteration))
                info.write("----------------------------------------------\n")
            
                #alignLevels = OutputPaths+'/Iter'+repr(iteration)+"/NewInDetAlignLevels.py"
                alignLevels = folderName+"/NewInDetAlignLevels.py"
                PrefixName="Iter%d%s_%s" % (iteration, folderSuffix, data.getName()) 

                print " Solving logs stored in %s/Iter%d%s/logs/%s_Solve.log" % (OutputPaths, iteration, folderSuffix, PrefixName)
        
                # Get Vectors, Matricies and Hitmaps
                if not useTFiles:
                    tdata = [data]
                    matrices, vectors, hitmaps = mergeMatrix(OutputPaths, iteration, tdata)
                    AlignmentOptions["inputMatrixFiles"] =  matrices
                    AlignmentOptions["inputVectorFiles"] = vectors
                    AlignmentOptions["inputHitmapFiles"] = hitmaps
                    AlignmentOptions["WriteTFile"] = False
                    if len(hitmaps) == 0:
                        AlignmentOptions["readHitmaps"] = False
                else:
                    tdata = [data]
                    tfiles = mergeTFiles(OutputPaths, iteration, folderSuffix, tdata) + tfiles
                    AlignmentOptions["inputTFiles"] = tfiles
                    AlignmentOptions["WriteTFile"] = True           
            
        
                AlignmentOptions["runSolving"] = True
                AlignmentOptions["runAccumulate"] = False

                # Get the reconstruction options
                RecoOptions = {}
                RecoOptions["numberOfEvents"] = 1
                RecoOptions["inputFiles"] = filesForSolve
                extraOptions["doReadBS"] = solveDoReadBS

                if len(data.getGlobalTag()) != 0:
                    RecoOptions["globalTag"] = data.getGlobalTag()
    
                if len(data.getDetDescrVersion()) != 0:
                    RecoOptions["detectorDescription"] = data.getDetDescrVersion()

                RecoScript = "InDetAlignExample/jobOption_RecExCommon.py"
                ConditionsScript = "InDetAlignExample/jobOption_ConditionsOverrider.py"
                constantsFile = ""
                bowingdb = ""
                if ReadAlignmentConstants:  
                    if iteration == 0:
                        constantsFile = inputAlignmentPoolFile 
                        AlignmentOptions["inputPoolFiles"] = [inputAlignmentPoolFile]
                    else:
                        constantsFile = str(OutputPaths)+"/Iter"+str(iteration-1)+folderSuffix+"/Iter"+str(iteration-1)+"_AlignmentConstants.root"
                        AlignmentOptions["inputPoolFiles"] = [constantsFile]
                
                if ReadBowingParameter:
                    if iteration == 0:
                        bowingdb = inputBowingCoolFile
                        AlignmentOptions["inputBowingDatabase"] = inputBowingCoolFile
                    else:
                        bowingdb = str(OutputPaths)+"/Iter"+str(iteration-1)+folderSuffix+"/mycool.db"
                        AlignmentOptions["inputBowingDatabase"] = bowingdb

                poolfiles = [RecoOptions["inputFiles"][0],constantsFile]
                #coolfiles = [RecoOptions["inputDbs"][0],bowingdb]

                # only one solution job submitted
                print " -- tfiles list = ", tfiles 
                
                #form
                #OriginalLBIBLTweak = extraOptions["applyLBibldistTweak"]
                #if OriginalLBIBLTweak:
                #    print "-- Turning Off the LB IBLDistortion Tweak During Solving --"
                #    extraOptions["applyLBibldistTweak"] = False
                #else:
                #    print "-- The LB IBLDistortion tweak was ", extraOptions["applyLBibldistTweak"], " since accumulation"

                currentjob = manageJob(OutputPath = OutputPaths,
                                       dataName = data.getName(),
                                       iter = iteration,
                                       folderSuffix = folderSuffix,
                                       part = -1,
                                       JOBNAME = JOBNAME,
                                       preName = preName,
                                       RecoOptions = RecoOptions,
                                       extraOptions = extraOptions,
                                       AlignmentOptions = AlignmentOptions,
                                       RecoScript = RecoScript,
                                       ConditionsScript = ConditionsScript,
                                       AlignmentLevels = alignLevels,
                                       #MonitoringScript = MonitoringScript,
                                       QUEUE = QUEUE,
                                       CMTDIR = CMTDIR,
                                       ATHENACFG = ATHENACFG,
                                       inputPoolFiles = poolfiles,
                                       inputCoolFiles = []
                                       )

                # Write the job
                currentjob.createDirectories()
                currentjob.writeJO()
                currentjob.writeScript()
                if (len(DataToSolve) == 1):
                    if (len(tfiles)>0): # there are tfiles ready to be used 
                        print " -- Submitting Solve job: ",currentjob.JOBNAME," to queue ", currentjob.QUEUE
                        currentjob.send(runMode) # submit only the last one 
                    else:
                        print " -- WARNING -- No tfiles are found !!! Solve job is not submitted "
                            
                DataToSolve.remove(data)
            #end loop over DatatoSolve
        #end while
        
        if runMode == "batch":
            #Don't wait for the solve. 
            currentjob.wait()
        #Restoring the LBibldistTweak for the next Iteration.
        #formd
        #extraOptions["applyLBibldistTweak"] = OriginalLBIBLTweak
        
    else:
        print "---------------------------------------------------------------\n"
        print " WARNING: Skipping the solving because the flag doSolve is OFF\n"
        print "---------------------------------------------------------------\n"
        
        DataToMerge = list(DataToRun)
        print "\n Waiting for Accumulate job completion. Please be patient."
        while len(DataToMerge):
            for data in DataToMerge:
                batchjobs = "%s_Iter%d%s_%s_Part" % (preName,iteration,folderSuffix, data.getName())
                if os.popen('bjobs -w').read().find(batchjobs)!=-1: #check if accumulate jobs are running
                    #print "Waiting for ",batchjobs
                    time.sleep(60)
                    continue
                DataToMerge.remove(data) #completed
                #print len(DataToMerge)
                time.sleep(30)


    # histogram merging step    
    for data in DataToRun:
        if "doMonitoring" in extraOptions and extraOptions["doMonitoring"]==True:
            OutputPaths = OutputPath+'/'
            MERGEJOBNAME="%s_Iter%d%s_%s_Merge.py" % (preName,iteration,folderSuffix,data.getName())
            MERGESCRIPTNAME="%s_Iter%d%s_%s_Merge.lsf" % (preName,iteration, folderSuffix, data.getName())
            monitoringMerge = mergeScript(OutputPath = OutputPaths
                          ,iter = iteration
                          ,preName = preName
                          ,folderSuffix = folderSuffix
                          ,dataName = data.getName()
                          ,nCPUs = data.getCPUs(iteration)
                          ,QUEUE = QUEUE
                          ,CMTDIR = CMTDIR
                          ,ATHENACFG = ATHENACFG
                          ,SCRIPTNAME = MERGESCRIPTNAME
                          ,JOBNAME = MERGEJOBNAME)

            monitoringMerge.write()
            monitoringMerge.send(runMode)

#   print "  Iteration %d finished: %5.3f seconds \n" % (iteration,(time.time()-IterStartTime))
#   print "----------------------------------------------"
#
#   info.write("\n")
#   info.write("----------------------------------------------\n")
#   info.write("  Iteration %d finished: %5.3f  seconds\n" % (iteration, (time.time()-IterStartTime)))
#   info.write("----------------------------------------------\n")
#
#   lastIteration = iteration
#   os.system("gzip %s/Iter%d/logs/Iter*.log" % (OutputPath, iteration))
#
#
## =======================
## Post Processing
## =======================
##if MonitoringScript == True:
#if False:
#   print
#   print "Comparing the Monitoring Files"
#   print
#   info.write('\n')
#   info.write("Comparing the Monitoring Files \n" )
#   info.write("\n")
#   compareMonitoring = compareMonitoringScript(OutputPath = OutputPath
#                           ,numIter = Iterations
#                           ,QUEUE = QUEUE
#                           ,CMTDIR = CMTDIR
#                           ,ATHENACFG = ATHENACFG)
#                           
#   compareMonitoring.write()
#   compareMonitoring.send(runMode)
#
#   print
#   print "Processed %d iterations !!!" % Iterations
#   print "  %5.3f  seconds" % (time.time()-StartTime)
#
#info=open(OutputPath+"/info.txt",'a')
#info.write('\n')
#info.write("Processed %d iterations !!!\n" % Iterations)
#info.write("  %5.3f  seconds \n" % (time.time()-StartTime))
#
