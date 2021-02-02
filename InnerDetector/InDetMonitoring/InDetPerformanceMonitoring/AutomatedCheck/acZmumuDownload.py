#!/usr/bin/env python
#m_taskNameTemplate = "user.martis.21.0.97_data18_13TeV_%d_Zmumu_Run2Data18_jpsi_v04_0"
#m_taskNameTemplate = "user.martis.21.0.97_data18_13TeV_%d_Zmumu_Run2Data18_paperplots_aligned_v08_0"
#m_taskNameTemplate = "user.martis.21.0.97_data16_13TeV_%d_Zmumu_Run2Data16_AlignmentCampaingApril20_default_1_0"
#m_taskNameTemplate = "user.martis.21.0.97_data16_13TeV_%d_Zmumu_Run2Data16_AlignmentCampaingApril20_step01_v2_0"
m_taskNameTemplate = "user.martis.21.0.97_data16_13TeV_%d_Zmumu_Run2Data16_AlignmentCampaingApril20_step02_v0_0"


m_mainFolder = "/afs/cern.ch/work/m/martis/public/athena/ZmumuNtuples"
m_originFolder = ""
m_ntuplesName = "ZmumuValidationOut"
#
m_storingFolder = ""
m_recordsFileName = ""
m_outputList = []
m_athenaVersion = ""
m_testArea = ""
m_theUser = ""
m_scriptName = "runzmumu_UserConstants.py"
m_savingFile = "acZmumu_history.txt"
m_reconmerge = "merge" #"%"

# options
m_submitExec = False
m_firstRun = 0
m_lastRun = 999999
m_userLabel = ""
m_userRun = 0
m_dataType = "DESDM_MCP"
m_userFiles = 0 # this means all the files
m_amitag = "%"
m_targetFolder = "data18_13TeV"
m_updateFiles = False
m_listOfRuns = []
m_listOfPendingRuns = []
m_runListFile = "NONE"
m_monitoring = False
m_useJobStatus = True
m_datasetTemplate = ""
###################################################################################################
def GetListOfRuns():
    import os
    import json

    myListOfRuns = []

    if (len(m_listOfRuns) > 0):
        myListOfRuns = []
        for run in m_listOfRuns:
            myListOfRuns.append(int(run))

    if (m_firstRun != 0):
        myListOdRuns = []
        for run in range (m_firstRun, m_lastRun+1):
            myListOfRuns.append(int(run))

    # case a file with a list of runs from acZmumu.py was given
    if ("NONE" not in m_runListFile):
        print (" <acZmumu> extracting run list from file: %s " %m_runListFile)
        with open(m_runListFile) as theinputfile: runListFromFile = json.load(theinputfile)
        # print "----> runListFromFile: ",runListFromFile
        # add this list to the list of runs given with option "--runList"
        for runNumber in runListFromFile:
            myListOfRuns.append(runNumber)

    # sort the list and remove duplicates
    myListOfRuns.sort()
    from collections import OrderedDict
    myListOfRuns = list(OrderedDict.fromkeys(myListOfRuns))
    print (" -- List of sorted runs (duplicates removed) contains",len(myListOfRuns)," elements: \n", myListOfRuns)
    
    return myListOfRuns

###################################################################################################
def processRuns(myListOfRuns):
    import os
    global m_outputList

    print (" -- origin folder: %s" %m_originFolder)
    print (" -- storing folder: %s" %m_storingFolder)

    runCount = 0;
    for theRun in myListOfRuns:
        runCount = runCount + 1;
        print ("\n -- processing run [%d/%d]: %d " %(runCount, len(myListOfRuns), theRun))
        os.chdir(m_storingFolder)
        #datasetName = getDataSetName(m_datasetTemplate, theRun)
        datasetName = getDataSetName(m_taskNameTemplate, theRun)
        print (" -- dataset: %s" %datasetName)

        # one thing to find out is which is the final name of the stored file. 
        # with that one can check if the file already exists
        # so, one can skip the downloading.
        # Otherwise redownload is possible if --update is given
        finalNtupleFile = getNtupleName (m_storingFolder, theRun, m_ntuplesName)
        print (" -- to store it as: %s" %finalNtupleFile)

        downloadDataSet = CheckIfDowloadIsNecessary (finalNtupleFile)

        if (downloadDataSet):
            downloadOK = True # assume the files are going to be properly downloaded
            theCommand = "rucio download %s > acztemp" %datasetName
            print (" -- execute command: %s" %theCommand)
            # download the dataset
            os.system(theCommand)
            datasetFolder = "%s%s%s" %(os.getcwd(), os.sep, datasetName)
            numberOfJobs = getNumberOfJobs(datasetFolder)
            print (" -- run %d was split in %d jobs" %(theRun, numberOfJobs))
            if (numberOfJobs == 0):
                print (" -- warning -- Run: %d does not exist or has 0 files" %theRun)
                downloadOK = False

            if (downloadOK):
                os.chdir(datasetFolder)
                # copy the merging script into the dataset folder
                theCommand = "cp %s%s%s %s%s." %(m_mainFolder, os.sep, "HistMergerGrid.py", datasetFolder, os.sep)
                print (" -- execute command: %s" %theCommand)
                os.system(theCommand)
        
                #exectute the merging command
                theCommand = "python HistMergerGrid.py %d" % numberOfJobs
                print (" -- execute command: %s" %theCommand)
                os.system(theCommand)

                #move the merged file into the storing folder
                theCommand = "mv mergedOutput/Run_ZmumuValidationOut.root %s" % finalNtupleFile
                print (" -- execute command: %s" %theCommand)
                os.system(theCommand)

                #remove the individual files of the dataset
                theCommand = "rm -rf %s" % datasetFolder
                print (" -- execute command: %s" %theCommand)
                os.system(theCommand)

                #prepare the list
                m_outputList.append("      s_fileNames.push_back(\"%s%sRun_%d_%s.root\");" %(m_storingFolder, os.sep, theRun, m_ntuplesName))
        else:
            #prepare the list
            m_outputList.append("      s_fileNames.push_back(\"%s%sRun_%d_%s.root\");" %(m_storingFolder, os.sep, theRun, m_ntuplesName))

    return

###################################################################################################
def getDataSetName(nameTemplate, run):

    if ("/" in nameTemplate[-1]):
        nameTemplate = nameTemplate[:-1]

    nametemplatelong = nameTemplate + "_ZmumuValidationUserSel/" 
    theName = nametemplatelong % run
    if (m_monitoring):
        theName = theName.replace("ZmumuValidationUserSel","CombinedMonitoring")

    return theName

###################################################################################################
def postProcessing (myListOfRuns):
    # in case of downloading the monitoring files, then merge them all together in one
    import os

    if (m_monitoring):
        # copy the merging script into the dataset folder                                                                                                                       
        theCommand = "cp %s%s%s %s%s." %(m_mainFolder, os.sep, "HistMergerMonitoringGrid.py", m_storingFolder, os.sep)
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)

        # move to the storing folder
        os.chdir(m_storingFolder)

        # now save the Zmumu ntuples in a temp folder
        # 1- create the save folder
        theCommand = "mkdir ../tempStorage" 
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)
        # 2- move the files
        theCommand = "mv Run_*_ZmumuValidationOut.root ../tempStorage" 
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)

        # time to merge the files together
        theCommand = "python HistMergerMonitoringGrid.py %d" % len(myListOfRuns)
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)

        # mv the merged file and rename
        theCommand = "mv mergedMonitoring/TotalMonitoring.root Run_ALL_TotalMonitoring.root" 
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)

        # remove the merging script
        theCommand = "rm HistMergerMonitoringGrid.py"
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)

        # delete the merging folder
        theCommand = "rm -rf mergedMonitoring"
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)
        
        # restore the Zmumu ntuples
        theCommand = "mv ../tempStorage/* ."
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)
        
        # delete the temporary folder
        theCommand = "rm -rf ../tempStorage"
        print (" -- postProcessing -- execute command: %s" %theCommand)
        os.system(theCommand)

    return

###################################################################################################
def getNtupleName (storingFolder, run, ntuplesName):
    import os

    if (m_monitoring):
        ntuplesName = "TotalMonitoring"

    theName = "%s%sRun_%d_%s.root" %(storingFolder, os.sep, run, ntuplesName)
    return theName

###################################################################################################
def getNumberOfJobs(theFolder):
    import os
    import glob 

    filesInFolder = glob.glob("%s%s*" %(theFolder, os.sep))
    theNumberOfJobs = len(filesInFolder)

    os.system("rm acztemp")
            
    return theNumberOfJobs

################################################################################################### 
def CheckIfDowloadIsNecessary (targetFile):
    import os

    isDownloadNecessary = True
    
    # check if file exist
    if (os.path.exists(targetFile)):
        isDownloadNecessary = False
        print (" -- File %s already exists " % targetFile)
        if (m_updateFiles):
            isDownloadNecessary = True
            print (" -- update files option is used --> File %s will be updated" % targetFile)

    return isDownloadNecessary

###################################################################################################
def checkOnFolder (folderName):
    import os
    folderReady = True

    # first check if folders exist
    if (os.path.exists(folderName)):
        print (" <acZmumu> reports folder already exists: %s " %(folderName))
        # check if folder is writeable
        if (not os.access(folderName, os.W_OK)):
            print (" <acZmumu> ** ERROR ** Folder %s is not writeable STOP execution. " %(folderName)) 
            folderReady = False
        # end of folder not writeable            
    else:
        print (" <acZmumu> folder %s does not exist --> mkdir " %(folderName))
        # if folder does not exist, then make a new folder
        # but check the path 
        tempFolders = folderName.replace("%s/" %os.getcwd(),'') # remove the pwd
        tempFolders = tempFolders.split(os.sep)
        print (" <acZmumu> list of folders to test recursively: ", tempFolders)
        tempdirname = m_mainFolder
        # make sure one is sitting in home folder
        os.chdir(m_mainFolder)

        for tempdir in tempFolders:
            if len(tempdir) == 0: continue
            tempdirname = "%s%s%s" %(tempdirname, os.sep, tempdir)
            # move to home folder 
            print (" <acZmumu> current folder: %s " % os.getcwd())
            print (" <acZmumu> test on destination folder: %s " %(tempdirname))
            if (os.path.exists(tempdirname)):
                print (" <acZmumu> destination folder already exists: %s " %(tempdirname))
                os.chdir(tempdirname)
            else:
                try:
                    print (" <acZmumu> pwd = %s " % os.getcwd())
                    print (" <acZmumu> executing: os.mkdir(%s)" %tempdir)
                    os.mkdir(tempdir)
                    os.chdir(tempdir)
                    folderReady = True
                    print (" <acZmumu> New folder %s created successfully" %(tempdir))
                except:
                    print (" <acZmumu> problems creating folder %s. STOP execution. \n " %(tempdir))
                    folderReady = False
                    aczExit()
                   
    return folderReady
                   
###################################################################################################
def printOutputList():
    #import urllib2

    print ("\n\n -- Output list for userFiles.C -- ")
    print ("      // Number of ready files: %d    # pending %d " %(len(m_outputList),len(m_listOfPendingRuns) ))
    for textLine in m_outputList:
        print (textLine)
    print (" -- End of list -- %d files -- \n\n" %len(m_outputList))
    
    if (len(m_listOfPendingRuns)>0):
        print ("  -- List of pending runs -- Total (%d) -- " %len(m_listOfPendingRuns))
        print (m_listOfPendingRuns)

    return

###################################################################################################
def preliminaries ():
    import os
    global m_storingFolder
    global m_originFolder

    # get the origin folder (from where the command was launched)
    m_originFolder = os.getcwd()

    # check if main folder exists, and if so move there
    folderReady = checkOnFolder (m_mainFolder)
    if (not folderReady): aczExit()
    os.chdir(m_mainFolder)
    
    # check if the target folder exist, if not try to create it
    m_storingFolder = "%s%s%s" %(m_originFolder, os.sep, m_targetFolder)
    #m_storingFolder = "%s" %(m_targetFolder)
    print (" <acZmumu> storingFolder = %s " %m_storingFolder)
    folderReady = checkOnFolder (m_storingFolder)
    if (not folderReady): aczExit()

    print (" <acZmumu> Storing output in folder: %s" %(m_storingFolder))
    print (" <acZmumu> dataset Template: %s" %(m_datasetTemplate)) 

    return

###################################################################################################
def aczExit():
    import os
    
    #return to the origin folder and exit
    os.chdir(m_originFolder)
    exit()
    return

###################################################################################################                                                                                
def getAthenaBasics ():
    import os

    testArea = ""
    try:
        testArea = os.getenv("TestArea","")
    except:
        print (" <acZmumu> ERROR ** no Athena TestArea defined --> job submission is not possible. STOP execution")
        exit()

    theUser = ""
    try:
        theUser = os.getenv("USER","")
    except:
        print (" <acZmumu> ERROR ** no USER defined --> job submission is not possible. STOP execution")
        exit()

    athenaVersion = ""
    try:
        athenaVersion = os.getenv("AthenaExternals_VERSION","")
    except:
        print (" <acZmumu> ERROR ** no Athena TestArea defined --> job submission is not possible. STOP execution")
        exit()

    # voms proxy must be initiated
    goodVoms = True
    vomsInfoReturn = os.popen("voms-proxy-info").readlines()
    if (len(vomsInfoReturn)>0):
        lineCount = 0
        for theLine in vomsInfoReturn:
            lineCount += 1
            if ("Proxy not found" in theLine):
                goodVoms = False
    else: 
        goodVoms = False
        
    if (not goodVoms):
        print (" <acZmumu> ERROR ** no voms initiated --> It is not possible to download with rucio. Stop execution")
        exit()

    return (athenaVersion, testArea, theUser)

###################################################################################################                                                                                
def retrievePandaInfo (myListOfRuns):
    
    import os
    import json
    
    global m_listOfPendingRuns

    listOfCompletedRuns = []
    listOfFinishedRuns = [] # this is a subset of the CompletedRuns
    listOfPendingRuns = []
    listOfUnknownRuns = []

    ndays = 90 # retrieve the grid jobs of the past ndays

    if (m_useJobStatus):
        statusFileName = "azPandaStatus.txt"
        # 0 step: check if an old status files exists --> delete
        if (os.path.exists(statusFileName)):
            theCommand = "rm %s" %statusFileName
            os.system(theCommand)
        
        # 1st get panda cookie
        theCommand ="cern-get-sso-cookie -u https://bigpanda.cern.ch/ -o bigpanda.cookie.txt"
        print (" <acZmumu> getting panda cookie with command: %s " %theCommand)
        os.system(theCommand)

        # 2nd retrieve job status in json format
        theCommand ="curl -b bigpanda.cookie.txt -H 'Accept: application/json' -H 'Content-Type: application/json' \"https://bigpanda.cern.ch/tasks/?taskname=user.%s*&days=%d&json\">%s" %(os.getenv("USER","martis"), ndays, statusFileName) 
        print (" <acZmumu> retrieve job status with comand: \n --> %s " %theCommand)
        os.system(theCommand)
    
        with open(statusFileName) as theinputfile: jobStatusList = json.load(theinputfile)
        jobcount = 0
        #print " List of Keys: ", jobStatusList[0].keys()
        for jobstatus in jobStatusList:
            jobTaskName = jobstatus['taskname']
            jobStatus = jobstatus['status']
            #print "  >>>>  jobTaskName: %s   status: %s"  %(jobTaskName,jobStatus)
            jobDsinfo = jobstatus['dsinfo']
            #print "        dsinfo: ", jobDsinfo
            #print "        nfiles: ", jobDsinfo['nfiles']
            #print "        nfinis: ", jobDsinfo['nfilesfinished']
            for run in myListOfRuns:
                thisTaskName = m_taskNameTemplate % run
                if thisTaskName in jobTaskName: # in case the jobTaskName ends with "/"
                    #print "  >>>>  jobTaskName: %s   status: %s"  %(jobTaskName,jobStatus)
                    runCompleted = False
                    if ('done' in jobstatus['status']):
                        runCompleted = True
                    if ('finished' in jobstatus['status']):
                        runCompleted = True
                        listOfFinishedRuns.append(run)
                    if ('failed' in jobstatus['status']):
                        runCompleted = False
                        listOfFinishedRuns.append(run)
                    if (jobDsinfo['nfiles'] == jobDsinfo['nfilesfinished']):
                        print (" >>> Adding run %d to the list of completed ones" %(run))
                        runCompleted = True
                    if (m_updateFiles):
                        runCompleted = True # this really means to download as it is
                        print (" Run %d to be downloaded as user request :)" %run)
                    # end of cheking job status
                    if (runCompleted):
                        # print "  --> bingo for run %d " %run
                        listOfCompletedRuns.append(run)
                    else:
                        #print "  --> Run %d is PENDING" %run
                        listOfPendingRuns.append(run)
                #else:
                    #if ((run not in listOfUnknownRuns) and (run not in listOfCompletedRuns) and (run not in listOfPendingRuns)):
                        #listOfUnknownRuns.append(run)

    if (not m_useJobStatus):
        for run in myListOfRuns:
            listOfCompletedRuns.append(run)

    # prepare the list of pending runs
    for run in listOfPendingRuns:
        m_listOfPendingRuns.append(run)

    # sort lists of runs
    listOfCompletedRuns.sort()
    listOfPendingRuns.sort()
    listOfUnknownRuns.sort()
    listOfFinishedRuns.sort()

    print (" <acZmumu> List of completed, pending and unknown runs contains in total %d + %d + %d = %d runs" %(len(listOfCompletedRuns), len(listOfPendingRuns), len(listOfUnknownRuns), len(listOfCompletedRuns)+len(listOfPendingRuns)+len(listOfUnknownRuns)))
    print ("\n  >> list of completed runs: [", len(listOfCompletedRuns),"] -> ", listOfCompletedRuns)
    print ("\n  >> list of pending runs: [", len(listOfPendingRuns),"] -> ", listOfPendingRuns)
    print ("\n  >> list of unknown runs: [", len(listOfUnknownRuns),"] -> ", listOfUnknownRuns)
    print ("\n  >> list of finished runs: [", len(listOfFinishedRuns),"] -> ",listOfFinishedRuns)

    return listOfCompletedRuns

###################################################################################################                                                                                
def welcomeBanner ():
    
    print ("\n \n")
    print ("        **************************")
    print ("        *  Automated Zmumu check *")
    print ("        **************************")
    print ("\n")
    print ("  config:")
    print ("  ** Exec: %r" %m_submitExec)
    print ("  ** min Run: %d"  %m_firstRun)
    print ("  ** max Run: %d"  %m_lastRun)
    print ("  ** data type: %s"  %m_dataType)
    if (m_userFiles == 0):
        print ("  ** use all available files ")
    if (m_userFiles > 0):
        print ("  ** user requested files: %d" %m_userFiles)
    print ("  ** AMI tag: %s" %m_amitag) 
    print ("  ** main folder: %s" %m_mainFolder)
    print ("  ** target folder: %s" %m_targetFolder)
    print ("  ** update files: %r" %m_updateFiles)
    if ("NONE" not in m_runListFile):
            print ("  ** run list file: %s" %m_runListFile)
    print ("  ** merge monitoring histos: %r" %m_monitoring)
    print ("  ** task name template: %s" %m_taskNameTemplate)
    print ("\n")

    return
###################################################################################################                                                                                
def endBanner ():
    
    print ("\n")
    print ("   GAME OVER. INSERT COIN \n")

    return
###################################################################################################
def optParsing():
    from optparse import OptionParser

    p_submitExec = False
    p_firstRun = m_firstRun
    p_lastRun = m_lastRun
    p_userLabel = m_userLabel
    p_dataType = m_dataType 
    p_userFiles = m_userFiles
    p_targetFolder = m_targetFolder
    p_updateFiles = m_updateFiles
    p_listOfRuns = m_listOfRuns
    p_runListFile = m_runListFile
    p_monitoring = m_monitoring
    p_taskNameTemplate = m_taskNameTemplate

    parser = OptionParser()
    parser.add_option("--dataType", dest="p_dataType", help="User defined data type (examples: DAOD_ZMUMU, DESDM_MCP). Default %s" %(p_dataType), default = p_dataType)
    parser.add_option("--EXEC", dest="p_submitExec", help="Submit the Grid jobs. Default: no submission", action="store_true", default = False)
    parser.add_option("--firstRun", dest="p_firstRun", help="First run number (inclusive). Default %s" %(p_firstRun), default = p_firstRun)    
    parser.add_option("--lastRun", dest="p_lastRun", help="Last run number (inclusive). Default %s" %(p_lastRun), default = p_lastRun)
    parser.add_option("--monitor", dest="p_monitoring", help="Download and merge monitoring files. Default %r" % p_monitoring, action="store_true", default = m_monitoring)  
    parser.add_option("--nFiles", dest="p_userFiles", help="User defined number of files. Default %s = all the available files" %(p_userFiles), default = m_userFiles)
    parser.add_option("--runList", dest="p_listOfRuns", help="List of runs (comma sepparated) %s" %(p_listOfRuns), default = p_listOfRuns)
    parser.add_option("--runListFile", dest="p_runListFile", help="File containign the list of runs (comma sepparated) %s" %(p_runListFile), default = p_runListFile)
    parser.add_option("--taskName", dest="p_taskNameTemplate", help="taks name template. Be ware of run number ", default = p_taskNameTemplate)
    parser.add_option("--targetFolder", dest="p_targetFolder", help="Destination folder of tje chained ntuples. Default %s" %(p_targetFolder), default = p_targetFolder)
    parser.add_option("--userLabel", dest="p_userLabel", help="User defined label. Default %s" %(p_userLabel), default = p_userLabel)
    parser.add_option("--update", dest="p_updateFiles", help="Download all files. If file already exists then it is updates", action="store_true", default = p_updateFiles)

    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    return config

###################################################################################################                                                                                
def saveCommandLine (theCommandLine):
    import os
    import datetime
    global m_savingFile

    # current time and date
    now = datetime.datetime.now()
    fileReady = False

    # first check if file exist                                                                                                                                                  
    if (os.path.exists(m_savingFile)):
        fileReady = True
    
    # file already exists
    if (fileReady):
        theOutputFile = open(m_savingFile,"a")
    else:
        theOutputFile = open(m_savingFile,"w")

    # write
    theOutputFile.write("%s => python %s\n" %(now.strftime("%Y-%m-%d %H:%M:%S"),theCommandLine))
    # close
    theOutputFile.close()    

    return

##########################################################                                                                                                                        
#             Main code                                  #                                                                                                                         
##########################################################                                                                                                                              
if __name__ == '__main__':
    import sys

    saveCommandLine (' '.join(sys.argv))
    config = optParsing()
    m_targetFolder= config.p_targetFolder
    m_updateFiles = config.p_updateFiles
    m_monitoring  = config.p_monitoring
    m_firstRun = int(config.p_firstRun)
    m_lastRun = int(config.p_lastRun)
    m_taskNameTemplate = config.p_taskNameTemplate

    if(len(config.p_listOfRuns)>0): 
        m_listOfRuns = config.p_listOfRuns.split(",")

    m_runListFile = config.p_runListFile
        
    welcomeBanner ()
    getAthenaBasics ()
    preliminaries ()

    listOfRuns = GetListOfRuns ()
    listOfRuns = retrievePandaInfo (listOfRuns)
    print (" # runs to download: %d" %len(listOfRuns))

    processRuns (listOfRuns)
    postProcessing (listOfRuns)
    printOutputList ()

    endBanner ()
    aczExit()
#
    

