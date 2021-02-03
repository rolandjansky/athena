#!/usr/bin/env python                                                                                                                                                                    
m_year = 2017
m_storingFolder = ""
m_recordsFileName = ""
m_athenaVersion = ""
m_testArea = ""
m_packagePath = ""
m_theUser = ""
m_savingFile = "acZmumu_history.txt"
m_reconmerge = "merge" # "deriv" "merge" "%"
m_workDirPlatform = ""

# options
m_minEvents = 10000
m_submitExec = False
m_firstRun = 0
m_lastRun = 999999
m_userLabel = ""
m_userRun = 0
m_dataType = "DESDM_MCP"
m_dataProject = "data17_13TeV"
m_userFiles = 0 # this means all the files
m_userFilesPerJob = 0 #this means let panda distribute the files
m_amitag = "%"
m_physicsType = "physics_Main"
m_mcDataSetName = "mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.ESD.e3601_s3126_r10201"
m_scriptName = "runzmumu_UserConstants.py"
m_userDataSet = "NONE"

###################################################################################################
def findListOfDataSets():
    import os
    import sys

    listOfDataSets = []
    theAMIsearchCommand = "ami list datasets %s.%%.%s.%s.%s.%s --order run_number --fields events,nfiles"  %(m_dataProject, m_physicsType, m_reconmerge, m_dataType, m_amitag)
    
    # case of using data set provided by the user
    if ("NONE" not in m_userDataSet):
        theAMIsearchCommand = "ami list datasets %s --fields events,nfiles"  %(m_userDataSet)

    print (' <acZmumu> AMI data set search command: \n  -->  %s' %(theAMIsearchCommand))
    amiReturn = os.popen(theAMIsearchCommand).readlines()

    for theLine in amiReturn:
        #if m_dataProject in theLine:
        lineWithContent = True
        if ("----------" in theLine): 
            lineWithContent = False # this way add the main line and the trailing lines. Remove the splitting lines
        if ("events" in theLine):
            lineWithContent = False # remove the header

        if (lineWithContent and "NONE" in m_userDataSet):
            theLine.rstrip() # remove trailing blank spaces
            listOfDataSets.append(theLine) # add this data set
        
        if ("NONE" not in m_userDataSet):
            tempstring = str(m_userDataSet[0:30])
            if (tempstring in theLine):
                listOfDataSets.append(m_userDataSet)
    
    # if user provides the data set name, it may happen (if errors) the data set is not found or does not exist
    if (len(listOfDataSets) == 0 and "NONE" not in m_userDataSet):
        print (' <acZmumu> ** WARNING ** user data set: %s not found ' %m_userDataSet)
        sys.exit(" >> STOP excution")

    return listOfDataSets

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
        # if folder does not exist, then make a new folder
        try:
            os.mkdir(folderName)
            print (" <acZmumu> New folder %s created successfully" %(folderName))
        except:
            print (" <acZmumu> problems creating folder %s. STOP execution. \n " %(folderName))
            folderReady = False
                   
    return folderReady
                   
###################################################################################################
def getYear ():
    theYear = m_year
    
    if ('data16' in m_dataProject):
        theYear = 2016

    if ('data17' in m_dataProject):
        theYear = 2017

    if ('data18' in m_dataProject):
        theYear = 2018

    return theYear

###################################################################################################
def preliminaries ():
    import os
    import sys
    global m_storingFolder
    global m_athenaVersion
    global m_testArea
    global m_theUser
    global m_packagePath
    global m_workDirPlatform

    m_athenaVersion, m_testArea, m_theUser, m_packagePath, m_workDirPlatform = getAthenaBasics () 
    m_year = getYear ()

    # check "run" folder exists
    folderName = "../../../../run"
    folderReady = checkOnFolder (folderName)
    if (not folderReady): exit()

    # reports folder
    folderName = "ReportsFolder"
    folderReady = checkOnFolder (folderName)
    if (not folderReady): exit()
 
    # folder of the current year
    folderName = "%s/%s" %(folderName, m_year)              
    folderReady = checkOnFolder (folderName)
    if (not folderReady): exit()

    # folder of the data project
    folderName = "%s/%s" %(folderName, m_dataProject)              
    folderReady = checkOnFolder (folderName)
    if (not folderReady): exit()

    m_storingFolder = folderName
    print (" <acZmumu> Storing output in folder: %s" %(m_storingFolder))

    # verify proper order of run numbers
    if (m_lastRun < m_firstRun):
        print (" ** ERROR ** last run %d < first run %d \n" %(m_lastRun, m_firstRun)) 
        sys.exit(" >> STOP excution due to bad run number ordering")

    return

###################################################################################################
def extractRunsAndProperties (listOfDataSets):

    # the list of data sets can contain dummy lines
    infoFromAMI = {}

    if (len(listOfDataSets)>0 and ("NONE" in m_userDataSet)):
        print (" <acZmumu> #data sets= %d" %(len(listOfDataSets)))
        # extract data set name
        # first is the data project
        lineindex = 0 # init line 
        for theLine in listOfDataSets: 
            lineindex += 1
            # lines with the main data (warning: there could be lines with left over content
            # Warning: there could be some dataset name left over in the next line
            if(m_dataProject in theLine):
                # usual procedure on each line as new data set is found
                tempString = theLine[1:] # already remove the first "|"
                theDataSet = tempString[1:tempString.find("|")] # start from character 1 to avoid blank space
                theDataSet.rstrip()
                # check if the dataset name spilled over the next line
                if ("--------" not in listOfDataSets[lineindex]):
                    # retrieve the remainings of the dataset name
                    theNextLine = listOfDataSets[lineindex]
                    tempString2 = theNextLine[1:]
                    tempString2 = tempString2[1:tempString2.find("|")]
                    tempString2.strip()
                    print (' --> dataset name spilled in next line: \"%s\"' %tempString2)
                    print (' --> original: \"%s\"' %theDataSet)
                    theDataSet = "%s%s" %(theDataSet[:-1],tempString2)
                    theDataSet.strip()

                print (' Data set: \"%s\"' %theDataSet)
                # remove the data project and the point behind
                tempString = tempString[tempString.find(m_dataProject) + len(m_dataProject) +1:]
                theRunNumber = int(tempString[:tempString.find(".")])
                print (' Run number: %d' %theRunNumber)
                # finding number events
                tempString = tempString[tempString.find("|")+1:]
                theNumberOfEvents = int(tempString[:tempString.find("|")])
                # finding number of files
                tempString = tempString[tempString.find("|")+1:]
                theNumberOfFiles = int(tempString[:tempString.find("|")])
                # only store runs with some events and files
                if (theNumberOfEvents > 1 and theNumberOfFiles > 0):
                    infoFromAMI[theRunNumber] = {}
                    infoFromAMI[theRunNumber]["dataset"] = theDataSet[:len(theDataSet)-1] # trick to remove a trailing blank space
                    infoFromAMI[theRunNumber]["events"] = theNumberOfEvents
                    infoFromAMI[theRunNumber]["nfiles"] = theNumberOfFiles
                    if (m_userFiles > 0): infoFromAMI[theRunNumber]["nfiles"] = m_userFiles
                    #print ('     --> run: %d stored in infoFromAMI with %d events and %d files ' %(theRunNumber, infoFromAMI[theRunNumber]["events"], infoFromAMI[theRunNumber]["nfiles"]) 
                if (m_dataProject not in theLine and theRunNumber in dict):
                    # this means this line has leftover content
                    tempString = theLine[1:] # already remove the first "|"
                    theDataSet = tempString[1:tempString.find("|")] # start from character 1 to avoid blank space
                    theDataSet = theDataSet[:theDataSet.find(" ")] # remove trailing blanks
                    infoFromAMI[theRunNumber]["dataset"] = "%s%s" %(infoFromAMI[theRunNumber]["dataset"],theDataSet)
                    continue
    else:
        if ("NONE" in m_userDataSet):
            print (" <acZmumu> ERROR ** list of data sets is empty. Stop Execution")
            exit ()

    print (" <acZmumu> extractRunsAndProperties completed ")

    return infoFromAMI

###################################################################################################                                                                                
def extractInfoFromFile ():
    import os
    import datetime
    global m_recordsFileName

    # current time and date
    now = datetime.datetime.now()

    # check if recordsfile exist
    
    recordsFileName = "%s/%s.log" %(m_storingFolder, "Zmumu_reports")
    if (os.path.isfile(recordsFileName)):
        print (" <acZmumu> Reading %s" %(recordsFileName))
    else:
        # create file
        recordsFile = open(recordsFileName, "w")
        recordsFile.write("# Zmumu records file for %s #  File created: %s " %(m_dataProject, now.strftime("%Y-%m-%d %H:%M")))
        recordsFile.close()

    # store it in the global variable
    m_recordsFileName = recordsFileName

    # init tuple
    infoFromRecordsFile = {}
    
    # redfile and dump into a list of lines
    linesInRecordsFile = open(recordsFileName,"r").readlines()

    runCount = 0
    for theLine in linesInRecordsFile:
        wordsInLine = theLine.split(":");
        print (" next line: %s" %(theLine))
        if ("#" in wordsInLine[0]): # comment line -> skip
            continue
        runNumber = int(wordsInLine[0])
        if m_dataProject in theLine:
            runCount += 1
            infoFromRecordsFile[runNumber] = theLine
            
    print (" <acZmumu> %d runs found in %s" %(runCount, recordsFileName))  

    return infoFromRecordsFile

###################################################################################################                                                                                
def crossCheckInfo(infoFromAMI, infoFromRecordsFile):
    # check if the runs from AMI are already known (stored in the infoFromRecordsFile).
    # If not --> flag them as new
    listOfNewRuns = []
    listOfPendingRuns = []

    if (len(infoFromAMI)>0):
        for runInAMI in infoFromAMI:
            runFound = False
            if (len(infoFromRecordsFile)>0):
                for runInFile in infoFromRecordsFile:
                    print (" <acZmumu> crosscheckInfo: ami run %d vs file run %d" %(runInAMI, runInFile))
                    if (runInAMI == runInFile):
                        runFound = True
            print (" <acZmumu> crosscheckInfo: ami run %d found in file list? %r" %(runInAMI,runFound))
            if (not runFound):
                listOfNewRuns.append(runInAMI)
            #else:
                # maybe run is found but the analysis is not completed
                # the logics must be formulated
                #if ("NEW" in infoFromRecordsFile[runInFile]["status"]):
                # this run is pending
                #listOfPendingRuns.append(runInAMI)

    print (" <acZmumu> List of new runs has: %d items " %len(listOfNewRuns)) 
    
    # sort run lists
    listOfNewRuns.sort()
    listOfPendingRuns.sort()

    return (listOfNewRuns, listOfPendingRuns)

###################################################################################################                                                                                
def submitGridJobs (infoFromAMI, listOfNewRuns, listOfPendingRuns):
    
    # submitting jobs with real data -> listOfNewRuns must be filled
    if (len(listOfNewRuns) > 0):
        submitGridJobsListOfRuns (infoFromAMI, listOfNewRuns, listOfPendingRuns)
    
    # submitting job when the user provides the data set
    if ("NONE" not in m_userDataSet):
        submitGridJobsUserDataSet () 

    return

###################################################################################################                                                                                
def submitGridJobsUserDataSet ():
    import os

    print (' <acZmumu> submitting grid job when user provides the data set name ')
    theCommand = getGridSubmissionCommand(0, infoFromAMI)
    if type(theCommand) is tuple:
        tempCommand = theCommand[0]
        theCommand = tempCommand

    if (m_submitExec): 
        print (' <acZmumu> m_submitExec = True --> job to be submmited');
        # move to the submission folder
        submissionPath = "%s/run" %(m_testArea)
        if (not os.path.exists(submissionPath)):
            print (" <acZmumu> \"run\" folder (submission path: %s) does not exist " %submissionPath)
            print ("          making the path available for you ;) ")
            theMkdirCommand = "mkdir %s" %submissionPath
            os.system(theMkdirCommand)
            
        os.chdir(submissionPath)
        print (" <acZmumu> path: %s" %(submissionPath))
        os.system(theCommand)

    return

###################################################################################################                                                                                
def submitGridJobsListOfRuns (infoFromAMI, listOfNewRuns, listOfPendingRuns):
    import os

    listOfSubmittedRuns = []
    listOfSubmittedDatasets = []

    # lets merge both list: new and pending
    listOfRunsToSubmit = [] 

    for runNumber in listOfNewRuns:
        listOfRunsToSubmit.append(runNumber)
    for runNumber in listOfPendingRuns:
        listOfRunsToSubmit.append(runNumber)

    # sort list
    listOfRunsToSubmit.sort()
    print (' <acZmumu> sorted list of runs to submit (%d)' %len(listOfRunsToSubmit))
    print (listOfRunsToSubmit)

    runcount = 0
    for runNumber in listOfRunsToSubmit:
        runcount += 1
        print ("\n <acZmumu> dealing with new run: %d  [%d/%d]" %(runNumber, runcount,len(listOfRunsToSubmit)))
        readyForSubmission = True
        theRunProperties = infoFromAMI[runNumber]
        runEvents = int(theRunProperties["events"])
        runEvents = int(infoFromAMI[runNumber]["events"])
        # check stats
        print ('   - Run number: %d, events: %d, min req: %d ' %(runNumber, runEvents, m_minEvents))
        if (runEvents < m_minEvents):
            readyForSubmission = False
            infoFromAMI[runNumber]["status"] = "LOW_STATS"
            infoFromAMI[runNumber]["attempt"] = 0

        # check if run number within range
        if (runNumber < m_firstRun):
            readyForSubmission = False
            infoFromAMI[runNumber]["status"] = "NOT_SELECTED"
            infoFromAMI[runNumber]["attempt"] = 0

        # check if run number within range
        if (runNumber > m_lastRun):
            readyForSubmission = False
            infoFromAMI[runNumber]["status"] = "NOT_SELECTED"
            infoFromAMI[runNumber]["attempt"] = 0

        # if ready for submission -> get command
        if (readyForSubmission):
            print ("\n <acZmumu> runNumber %d has %d events.  Is ready for submission? %r" %(runNumber, runEvents, readyForSubmission))
            infoFromAMI[runNumber]["status"] = "NEW"
            infoFromAMI[runNumber]["attempt"] = 0
            infoFromAMI[runNumber]["jeditaskid"] = 0
            theCommand, theDataset = getGridSubmissionCommand(runNumber, infoFromAMI)
            print  (" <acZmumu> GRID submission command: \n %s" %(theCommand))
            listOfSubmittedRuns.append(runNumber)
            listOfSubmittedDatasets.append(theDataset)

            print (" <acZmumu> submiting job number %d" %(len(listOfSubmittedRuns)))
            if (m_submitExec): 
                print (" <acZmumu> m_submitExec = True --> job to be submmited");
                # move to the submission folder
                submissionPath = "%s/run" %(m_testArea) 
                os.chdir(submissionPath)
                print (" <acZmumu> path: %s" %(submissionPath))
                os.system(theCommand)
                infoFromAMI[runNumber]["status"] = "SUBMITTED"
                infoFromAMI[runNumber]["jeditaskid"] = 1
                infoFromAMI[runNumber]["attempt"] = infoFromAMI[runNumber]["attempt"] + 1
            else:
                print (" <acZmumu> dry test for run %d" %runNumber);

    print (" <acZmumu> list of submitted runs has %d elements" %(len(listOfSubmittedRuns)))

    import json
    listOfSubmittedRuns.sort()
    filewithrunlist = open('acZmumu_listofsubmittedruns.txt', 'w')
    json.dump(listOfSubmittedRuns, filewithrunlist)        
    filewithrunlist.close() 


    listOfSubmittedDatasets.sort()
    print (' size of submitted datasets: %d' %len(listOfSubmittedDatasets))
    filewithdatasets= open("acZmumu_listofsubmitteddatasets.txt","w")
    for thedataset in listOfSubmittedDatasets:
        filewithdatasets.write("%s\n" %thedataset)
    filewithdatasets.close()

    return listOfSubmittedRuns

###################################################################################################                                                                                
def getAthenaBasics ():
    import os

    testArea = ""
    try:
        testArea = os.getenv("TestArea","")
        # make sure this points to the athena folder
        if ("athena" not in testArea.split("/")[-1]):
            tempTestArea = testArea.split("/")
            tempTestArea[-1] = "athena"
            # reform the string
            newTestArea = ""
            for tempword in tempTestArea:
                if (len(tempword)>0):
                    newTestArea = "%s/%s" %(newTestArea, tempword)
            testArea = newTestArea
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

    workdirplatform = ""
    try:
        workdirplatform = os.getenv("WorkDir_PLATFORM","")
    except:
        print (" <acZmumu> ERROR ** no WorkDir_PLATFORM defined --> job submission is not possible. STOP execution")
        exit()

    print (' == athenabasics == workdirplatform = %s' %workdirplatform)

    packagePath = ""
    try:
        thepwd = os.getcwd()
        thisfoldername = os.path.basename(thepwd)
        tempList = thepwd.split("athena")
        tempword = tempList[-1]
        tempList = tempword.split(thisfoldername)
        packagePath = tempList[0]
    except:
        packagePath = ""

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
        print (" <acZmumu> ERROR ** no voms initiated --> It is not possible to consult AMI. Stop execution")
        exit()

    return (athenaVersion, testArea, theUser, packagePath, workdirplatform)

###################################################################################################                                                                                
def getGridSubmissionCommand(runNumber, infoFromAMI):
    import os
    # build the command for submission

    #theScript = "%s/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/runzmumu_run2paper.py" %(m_testArea)
    #theScript = "%s/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/%s" %(m_testArea, m_scriptName)
    theScript = "%s%sshare/%s" %(m_testArea, m_packagePath, m_scriptName)

    theInput = "NONE"
    theDataSet = "NONE"
    if (runNumber>0):
        theDataSet = "%s" %(infoFromAMI[runNumber]["dataset"])
        theInput = "--inDS=%s" %(theDataSet)
    else:
        if ("NONE" not in m_userDataSet):
            theDataSet = "%s" %m_userDataSet
            theInput = "--inDS=%s" %m_userDataSet
    if ("NONE" in theInput):
        sys.exit(" <acZmumu> ** ERROR ** no input available for the grid submission command. ** STOP execution **")

    theOuput = "NONE"
    if (runNumber>0):
        theOutput = "--outDS=user.%s.%s_%s_%d_Zmumu_%s_%d " %(m_theUser, m_athenaVersion, m_dataProject, runNumber, m_userLabel, infoFromAMI[runNumber]["attempt"])
    else: 
        if ("NONE" not in m_userDataSet):
            theOutput = "--outDS=user.%s.%s_Zmumu_%s" %(m_theUser, m_athenaVersion, m_userLabel)
    if ("NONE" in theOutput):
        sys.exit(" <acZmumu> ** ERROR ** no output available for the grid submission command. ** STOP execution **")

    # warning: if one wants to limit the number of files per job just add to the options: --nFilesPerJob Nfiles

    theOptions = "NONE"
    if (runNumber>0):
        theOptions = "--nfiles %d --forceStaged" %(infoFromAMI[runNumber]["nfiles"])
    else: 
        if ("NONE" not in m_userDataSet):
            theOptions = " --forceStaged" # "--useShortLivedReplicas"
            
    if (m_userFilesPerJob > 0 ):
        theOptions = "%s --nFilesPerJob %d" % (theOptions, m_userFilesPerJob)

    if (m_userFiles > 0 ):
        theOptions = "%s --nFiles %d" % (theOptions, m_userFiles)

    theExtraOptions = "" 
    if (len(m_workDirPlatform)>0): 
        theExtraOptions = "--cmtConfig %s --excludedSite=ANALY_HPC2N,ANALY_RHUL_SL6,ANALY_JINR_MIG,ANALY_IHEP,ANALY_JINR,ANALY_CSCS-HPC" %m_workDirPlatform 
    else:
        theExtraOptions = "--excludedSite=ANALY_HPC2N,ANALY_RHUL_SL6,ANALY_JINR_MIG,ANALY_IHEP,ANALY_JINR,ANALY_CSCS-HPC"
    theExtraOptions = " " 

    theCommand = "pathena %s %s %s %s %s" %(theScript, theInput, theOutput, theOptions, theExtraOptions)
    print ('%s ' %theCommand)

    return theCommand,theDataSet

###################################################################################################                                                                                
def updateRecordsFile(listOfSubmittedRuns, infoFromAMI):

    import pickle

    print (' listOfSubmittedRuns: dimension:', len(listOfSubmittedRuns), '  list: ', listOfSubmittedRuns)

    if ("NONE" in m_userDataSet):
        print (" <acZmumu> updateRecordsFile --> %s " %(m_recordsFileName))
        fileToUpdate = open(m_recordsFileName, "a");
        fileToUpdate.write("\n")
        for runNumber in listOfSubmittedRuns:
            fileToUpdate.write("%d:%s\n" %(runNumber, infoFromAMI[runNumber]))
    
        fileToUpdate.close()

        # alternative method
        with open('acZmumu_listofsubmittedruns.txt', 'wb') as fp:
            pickle.dump(listOfSubmittedRuns, fp)
        

    return

###################################################################################################                                                                                
def welcomeBanner ():
    
    print ("\n \n")
    print ("        **************************")
    print ("        *  Automated Zmumu check *")
    print ("        **************************")
    print ("\n")
    print ("  config:")
    print ("  ** Exec: %r" %m_submitExec)
    print ("  ** data project: %s " %m_dataProject)
    print ("  ** min events: %d"  %m_minEvents)
    print ("  ** min Run: %d"  %m_firstRun)
    print ("  ** max Run: %d"  %m_lastRun)
    print ("  ** physics type: %s" %m_physicsType)
    print ("  ** data type: %s"  %m_dataType)
    print ("  ** set type: %s"  %m_reconmerge)
    if (m_userFiles == 0):
        print ("  ** use all available files ")
    if (m_userFiles > 0):
        print ("  ** user requested files: %d" %m_userFiles)
    if (m_userFilesPerJob > 0):
        print ("  ** user requested files per job: %d" %m_userFilesPerJob)
    print ("  ** AMI tag: %s" %m_amitag) 
    print ("  ** script: %s" %m_scriptName)
    if ("NONE" not in m_userDataSet):
        print ("  ** user data set: %s" %m_userDataSet)
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
    p_minEvents = m_minEvents
    p_firstRun = m_firstRun
    p_lastRun = m_lastRun
    p_userLabel = m_userLabel
    p_userRun = m_userRun
    p_dataType = m_dataType 
    p_userFiles = m_userFiles
    p_userFilesPerJob = m_userFilesPerJob
    p_amitag = m_amitag
    p_dataProject = m_dataProject
    p_physicsType = m_physicsType
    p_reconmerge = m_reconmerge
    p_scriptName = m_scriptName
    p_userDataSet = m_userDataSet

    parser = OptionParser()
    parser.add_option("--amiTag", dest="p_amitag", help="Name of the requested AMI tag (example: r10258_r10258_p3399). Wild card is also possible. Default %s" %(p_amitag), default = p_amitag)
    parser.add_option("--dataProject", dest="p_dataProject", help="data project of the data sets (examples: data17_13TeV). Default %s" %(p_dataProject), default = p_dataProject)
    parser.add_option("--dataSet", dest="p_userDataSet", help="User defined data set. Default %s" %(p_userDataSet), default = p_userDataSet)
    parser.add_option("--dataType", dest="p_dataType", help="User defined data type (examples: DAOD_ZMUMU, DESDM_MCP). Default %s" %(p_dataType), default = p_dataType)
    parser.add_option("--EXEC", dest="p_submitExec", help="Submit the Grid jobs. Default: no submission", action="store_true", default = False)
    parser.add_option("--firstRun", dest="p_firstRun", help="First run number (inclusive). Default %s" %(p_firstRun), default = p_firstRun)    
    parser.add_option("--lastRun", dest="p_lastRun", help="Last run number (inclusive). Default %s" %(p_lastRun), default = p_lastRun)
    parser.add_option("--minEvents", dest="p_minEvents", help="Minimum number of events. Default %s" %(p_minEvents), default = p_minEvents)
    parser.add_option("--nFiles", dest="p_userFiles", help="User defined number of files. Default %s = all the available files" %(p_userFiles), default = p_userFiles)
    parser.add_option("--nFilesPerJob", dest="p_userFilesPerJob", help="User defined number of files per job. Default %s = -> Panda decides" %(p_userFilesPerJob), default = p_userFilesPerJob)
    parser.add_option("--run", dest="p_userRun", help="Run number in case of targetting a single run. Default: %s" %(p_userRun), default = p_userRun)
    parser.add_option("--physicsType", dest="p_physicsType", help="Physics type to use (physics_Main, Hardprobes...) Default %s" %(p_physicsType), default = p_physicsType)
    parser.add_option("--script", dest="p_scriptName", help="Name of the python script to be executed. Default: %s" %p_scriptName, default = p_scriptName)
    parser.add_option("--setType", dest="p_reconmerge", help="Set type: recon, merge, deriv. Default: %s" %p_reconmerge, default = p_reconmerge)
    parser.add_option("--userLabel", dest="p_userLabel", help="User defined label. Default %s" %(p_userLabel), default = p_userLabel)

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
    m_dataType = config.p_dataType
    m_submitExec = config.p_submitExec
    m_minEvents = int(config.p_minEvents)
    m_firstRun = int(config.p_firstRun)
    m_lastRun = int(config.p_lastRun)
    m_userLabel = config.p_userLabel
    m_userRun = int(config.p_userRun)
    if (m_userRun > 0): # user provides a useful run number
        m_firstRun = m_userRun
        m_lastRun = m_userRun
    m_userFiles = int(config.p_userFiles)
    m_userFilesPerJob = int(config.p_userFilesPerJob)
    m_amitag = config.p_amitag
    m_dataProject = config.p_dataProject
    m_physicsType = config.p_physicsType
    m_scriptName = config.p_scriptName
    m_userDataSet = config.p_userDataSet
    m_reconmerge = config.p_reconmerge

    welcomeBanner ()
    preliminaries ()
    # extract info from AMI
    listOfDataSets = findListOfDataSets ()
    infoFromAMI = extractRunsAndProperties (listOfDataSets)
    #
    infoFromRecordsFile = extractInfoFromFile ()

    # now: crosschek the info from AMI and the records file flag new runs
    (listOfNewRuns, listOfPendingRuns) = crossCheckInfo(infoFromAMI, infoFromRecordsFile)

    # submit the necessary jobs
    listOfSubmittedRuns = submitGridJobs (infoFromAMI, listOfNewRuns, listOfPendingRuns)
    #print (" <acZmumu> main returned a list of submitted runs with %d elements" %(len(listOfSubmittedRuns)))

    # store new status
    #updateRecordsFile(listOfUsedRuns, infoFromAMI)

    endBanner ()
#
