#----------------------------------------------------------------------------------------------------
#
#    Script to run InDetAlignExample with parallelization
#    (local or lxbatch mode) & (physic data or cosmics)
#    Overhauled for improving efficiency
#
#    Hideyuki Oide [started 13-07-2016]
#
#----------------------------------------------------------------------------------------------------

import os, string, time, datetime
import sys


print "Output saved in:", config.OutputPath

# AW: moved that one to python dir such that it can be imported from anywhere
from InDetAlignExample.NewInDet_IteratorClasses import *
from InDetAlignExample.IteratorManager import *

#----------------------------------------------------------------------------------------------------
def write_info_init( config ):
    
    info=open(config.OutputPath+"/info.txt",'w')
    info.write("----------------------------------------------\n")
    info.write("\t\t%s      \n" % datetime.date.today() )
    info.write("----------------------------------------------\n")
    #info.write("Release %s\n" % ATHENAREL)
    info.write("Output stored in %s\n\n" % config.OutputPath)
    print "Info stored in: " +config.OutputPath+"/info.txt"
    
    if config.runMode == 'batch':
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

#----------------------------------------------------------------------------------------------------
def make_output_paths( config ):
    
    dirnames = [ "%s/%s"     % (config.OutputPath, config.iter_dir( iteration ) ) for iteration in range(config.FirstIteration, config.FirstIteration+config.Iterations) ]
    dirnames.append( "%s/%s" % (config.OutputPath, config.common_dir() ) )
    
    for dirname in dirnames:
        if os.path.isdir( dirname ):
            countdir=0
            while True:
                newdir="%s-%s-%d" % (dirname, datetime.date.today(), countdir)
                if not os.path.isdir("%s-%s-%d" % (dirname, datetime.date.today(), countdir)):
                    print "rename to", newdir
                    os.rename(dirname, newdir)
                    print "WARNING: %s directory exists" % (dirname)
                    print "Renamed to %s" % (newdir)
                    break
                countdir += 1
        
        os.mkdir(dirname)
        os.mkdir(dirname+'/logs/')
        os.system("get_files -jo InDetAlignExample/NewInDetAlignLevels.py >/dev/null")
        os.system("cp NewInDetAlignLevels.py %s" % dirname)
    
    return dirnames[-1]



    
#----------------------------------------------------------------------------------------------------
def configure_reco_options( subJob, data, dataFiles, iteration ):
    global config 
    
    RecoOptions = {}
    # Get the Reconstruction Options
    RecoOptions["ClusteringAlgo"] = config.ClusteringAlgo
    if config.ErrorScaling[iteration]:
        RecoOptions["errorScalingTag"] = config.errorScalingTag

    if not config.doDetailedSplitting:
        RecoOptions["inputFiles"]          = dataFiles.getCPU(subJob)
        RecoOptions["numberOfEvents"]      = data.getEventsPerCPU(iteration)
    else:
        RecoOptions["inputFiles"]          = dataFiles.getInputFiles(subJob)
        RecoOptions["numberOfEvents"]      = dataFiles.getNumEvents(subJob)
        RecoOptions["SkipEvents"]          = dataFiles.getSkipEvents(subJob)
        
    if len(data.getGlobalTag()) != 0:
        RecoOptions["globalTag"]           = data.getGlobalTag()
    if len(data.getDetDescrVersion()) != 0:
        RecoOptions["detectorDescription"] = data.getDetDescrVersion()
    
    return RecoOptions


#----------------------------------------------------------------------------------------------------
def configure_and_submit_subjob( subJob, data, dataFiles, iteration, preJOBNAME, alignLevels ):
    global config
    
    RecoOptions = configure_reco_options( subJob, data, dataFiles, iteration )
    
    JOBNAME                                = "%s_Part%02d.py" % (preJOBNAME,subJob)
    RecoScript                             = "InDetAlignExample/jobOption_RecExCommon.py"
    ConditionsScript                       = "InDetAlignExample/jobOption_ConditionsOverrider.py"

    config.extraOptions["doReadBS"]        = data.getByteStream()
    if (len(data.getProjectName())>0): 
        config.extraOptions["projectName"]        = data.getProjectName()
            
    config.extraOptions["Cosmics"]                = data.getCosmics()
    config.extraOptions["DigitalClustering"]      = data.getDigitalClustering()
    config.extraOptions["PtCut"]                  = data.getPtMin()
    
    filesForSolve                      = RecoOptions["inputFiles"]
    solveDoReadBS                      = config.extraOptions["doReadBS"]
        
    currentjob = IteratorManager(config            = config,
                                 dataName          = data.getName(),
                                 iter              = iteration,
                                 part              = subJob,
                                 JOBNAME           = JOBNAME,
                                 RecoOptions       = RecoOptions,
                                 RecoScript        = RecoScript,
                                 ConditionsScript  = ConditionsScript,
                                 AlignmentLevels   = alignLevels,
                                 ATHENACFG         = ATHENACFG,
                                 inputCoolFiles    = []
                                 )
    
    # Write the job
    currentjob.createDirectories()
    currentjob.writeJO()
    currentjob.writeScript()
    
    if config.is_dryrun == False:
        #import os
        #os.system( "sleep 15" )
        currentjob.send(config.runMode)
    
    return filesForSolve, solveDoReadBS
    

#----------------------------------------------------------------------------------------------------
def distribute_jobs_to_batch( data, config, folderName, info, alignLevels ):
    print "----------------------------------------------"
    print " Number of CPUs used to process the sample " + data.getName() + ": " + str(data.getCPUs(config.FirstIteration))
    print "----------------------------------------------"
    
    info.write(" Number of CPUs used to process the sample %s : %s\n\n" % (data.getName(), data.getCPUs(config.FirstIteration)))
    
    if data.getEvents(config.FirstIteration)!=-1:
        print "Number of events per CPU: " +  str(data.getEventsPerCPU(config.FirstIteration))
        info.write("Number of events per CPU: %d\n\n" % data.getEventsPerCPU(config.FirstIteration))
        
    else:
        print "Number of events per CPU: All" 
        info.write("Number of events per CPU: All" )
        
    OutputPaths = config.OutputPath
    os.mkdir(folderName+'/'+data.getName()+'/')
    
    print " Processing..."
    
    # check how many jobs must be sent
    # if users requests many, but the input file has less files, then the number of
    # jobs must be capped
    numberOfSubJobs = data.getCPUs(config.FirstIteration)
    if (data.getNFilesInList() < numberOfSubJobs): 
        numberOfSubJobs = data.getNFilesInList()
        print " >>> User requested ",data.getCPUs(config.FirstIteration), " jobs but files has fewer entries. numberOfSubJobs= ", numberOfSubJobs     
        
    # Get the Input file
    dataFiles = SortCpus(numberOfSubJobs
                         ,""
                         ,data.getFileList()
                         ,config.OutputLevel
                         ,doDetailedSplitting=config.doDetailedSplitting
                         ,nEventsPerFile=data.getEventsPerCPU(config.FirstIteration))
    
    preJOBNAME="%s_%s_%s" % (config.preName, data.getName(), config.folderSuffix)
    
    
    # Loop over subjobs     
    print "distribute_jobs_to_batch(): numberOfSubJobs = " , numberOfSubJobs
    for subJob in range(0, numberOfSubJobs):
        filesForSolve, solveDoReadBS = configure_and_submit_subjob( subJob, data, dataFiles, config.FirstIteration, preJOBNAME, alignLevels )
    
    return preJOBNAME, filesForSolve, solveDoReadBS



#----------------------------------------------------------------------------------------------------
def solve_each_data( data, config, info, folderName, filesForSolve, solveDoReadBS, JOBNAME ):
    
    print "----------------------------------------------"
    print "  Solving dataset %s, Iter %s"%(data.getName(),config.FirstIteration)
    print "----------------------------------------------"
    
    info.write('\n')
    info.write("----------------------------------------------\n")
    info.write("  Solving dataset %s, Iter %d\n" %(data.getName(),config.FirstIteration))
    info.write("----------------------------------------------\n")
    #alignLevels = config.OutputPath+'/Iter'+repr(config.FirstIteration)+"/NewInDetAlignLevels.py"
    alignLevels = folderName+"/NewInDetAlignLevels.py"
    PrefixName="Iter%d%s_%s" % (config.FirstIteration, config.folderSuffix, data.getName()) 
    
    print " Solving logs stored in %s/Iter%d%s/logs/%s_Solve.log" % (config.OutputPath, config.FirstIteration, config.folderSuffix, PrefixName)
    
    # Get the reconstruction options
    RecoOptions = {}
    RecoOptions["numberOfEvents"] = 1
    RecoOptions["inputFiles"] = filesForSolve
    config.extraOptions["doReadBS"] = solveDoReadBS
    
    if len(data.getGlobalTag()) != 0:
        RecoOptions["globalTag"] = data.getGlobalTag()
        
    if len(data.getDetDescrVersion()) != 0:
        RecoOptions["detectorDescription"] = data.getDetDescrVersion()
            
    RecoScript = "InDetAlignExample/jobOption_RecExCommon.py"
    ConditionsScript = "InDetAlignExample/jobOption_ConditionsOverrider.py"
    constantsFile = ""
    bowingdb = ""
                
    poolfiles = [constantsFile]
    #coolfiles = [RecoOptions["inputDbs"][0],bowingdb]

    currentjob = IteratorManager(config            = config,
                                 dataName          = data.getName(),
                                 iter              = config.FirstIteration,
                                 part              = -1,
                                 JOBNAME           = JOBNAME,
                                 RecoOptions       = RecoOptions,
                                 RecoScript        = RecoScript,
                                 ConditionsScript  = ConditionsScript,
                                 ATHENACFG         = ATHENACFG,
                                 AlignmentLevels   = alignLevels,
                                 inputCoolFiles    = [],
                                 nCPUs             = data.getCPUs(config.FirstIteration),
                                 data              = data
                                 )
    
    # Write the job
    currentjob.createDirectories()
    currentjob.writeJO()
    currentjob.writeScript()
    
    if config.is_dryrun == False:
        currentjob.send(config.runMode) # submit only the last one 
    
    
    return currentjob

#----------------------------------------------------------------------------------------------------
def do_solve( DataToRun, config, info, preJOBNAME, folderName, filesForSolve, solveDoReadBS ):
    
    DataToSolve = list(DataToRun) # in case there is more than one set to run
    
    data = DataToSolve[0]
    
    if (len(DataToRun)>1): 
        # As only one solving is sent for when combining several data sets, name it Total
        JOBNAME    ="%s_R_%s_%s_Solve.py" % (config.preName,"Total",config.folderSuffix)
        SCRIPTNAME ="%s_R_%s_%sSolve.lsf" % (config.preName,"Total",config.folderSuffix)
    else:
        JOBNAME    ="%s_R_%s_%s_Solve.py" % (config.preName,data.getName(),config.folderSuffix)
        SCRIPTNAME ="%s_R_%s_%s_Solve.lsf" % (config.preName,data.getName(),config.folderSuffix)
        
            
    DataToSolve = list(DataToRun) # Restore data set the list
    
    # need to figure out the list of matrices from the individual Accumulate jobs
    for data in DataToSolve:
        currentjob = solve_each_data( data, config, info, folderName, filesForSolve, solveDoReadBS, JOBNAME )
        #end loop over DatatoSolve
        

#----------------------------------------------------------------------------------------------------
def do_wait( DataToRun, config ):
    
    print "---------------------------------------------------------------\n"
    print " WARNING: Skipping the solving because the flag doSolve is OFF\n"
    print "---------------------------------------------------------------\n"
    
    DataToMerge = list(DataToRun)
    print "\n Waiting for Accumulate job completion. Please be patient."
    while len(DataToMerge):
        for data in DataToMerge:
            batchjobs = "%s_Iter%d_%s_%s_Part" % (config.preName,config.FirstIteration, config.folderSuffix, data.getName())
            if os.popen('bjobs -w').read().find(batchjobs)!=-1: #check if accumulate jobs are running
                #print "Waiting for ",batchjobs
                time.sleep(60)
                continue
            DataToMerge.remove(data) #completed
            #print len(DataToMerge)
            time.sleep(30)


#----------------------------------------------------------------------------------------------------
def do_monitoring( data, config ):
    global ATHENACFG
    
    OutputPaths = config.OutputPath+'/'
    MERGEJOBNAME    ="%s_%s_Merge_%s.py"  % (config.preName, data.getName(), config.folderSuffix)
    MERGESCRIPTNAME ="%s_%s_Merge_%s.lsf" % (config.preName, data.getName(), config.folderSuffix)
    
    print "MERGEJOBNAME = " , MERGEJOBNAME
    print "MERGESCRIPTNAME = " , MERGESCRIPTNAME
    
    """
    monitoringMerge = IteratorManager(config            = config,
                                      dataName          = data.getName(),
                                      iter              = config.FirstIteration,
                                      part              = -1,
                                      ATHENACFG         = ATHENACFG,
                                      JOBNAME           = MERGEJOBNAME,
                                      RecoScript        = MERGESCRIPTNAME,
                                      nCPUs             = data.getCPUs(config.FirstIteration),
                                      )
    monitoringMerge.writeJO()
    monitoringMerge.writeScript()
    """

    monitoringMerge = MergingManager(config        = config
                                     ,iter         = config.FirstIteration
                                     ,dataName     = data.getName()
                                     ,nCPUs        = data.getCPUs(config.FirstIteration)
                                     ,ATHENACFG    = ATHENACFG
                                     ,SCRIPTNAME   = MERGESCRIPTNAME
                                     ,JOBNAME      = MERGEJOBNAME)
    monitoringMerge.write()
    
    
    if config.is_dryrun == False:
        monitoringMerge.send(config.runMode)



#----------------------------------------------------------------------------------------------------
def iteration_core( DataToRun, config ):
    
    info=open(config.OutputPath+"/info.txt",'a')
    
    # Make OutputPaths
    folderName = make_output_paths( config )
    
    alignLevels = folderName+"/NewInDetAlignLevels.py"
    
    
    # Distribute the reconstruction job to LSF
    for data in DataToRun:
        # Create Status Checker
        write_status_checker( config, data.getName() )
        
        preJOBNAME, filesForSolve, solveDoReadBS = distribute_jobs_to_batch( data, config, folderName, info, alignLevels )

    #  Solving the system
    if config.doSolve:
        do_solve( DataToRun, config, info, preJOBNAME, folderName, filesForSolve, solveDoReadBS )
    else:
        do_wait( DataToRun, config )


    # histogram merging step    
    for data in DataToRun:
        if "doMonitoring" in config.extraOptions and config.extraOptions["doMonitoring"]==True:
            do_monitoring( data, config )





# =====================================================================
#
#  Main Script
#
# =====================================================================
    
# extract release configuration

ATHENACFG = getAthenaConfig(config.ASetupOptions)


print
if not os.path.isdir(config.OutputPath):
    os.mkdir(config.OutputPath)
    
write_info_init( config )

# check that user requires to run some iterations:
if (config.Iterations == 0):
    print " ------------------------------------------------- \n -- WARNING -- user requests Iterations = 0 !!! -- \n ------------------------------------------------- \n" 
    
#  Loop over iterations
iteration_core( DataToRun, config )

