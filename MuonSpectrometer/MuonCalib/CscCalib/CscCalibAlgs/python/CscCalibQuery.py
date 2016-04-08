#!/afs/cern.ch/sw/lcg/external/Python/2.5.4p2/slc4_ia32_gcc34/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#/usr/bin/python
import os
import sys
import re
import glob
import subprocess
import pickle


#This script looks for new CSC calibration files on castor and runs on them. It:
#1: Checks to see if a job is currently running by looking for a calibrationRunning### file in the script directory. Quits if anything is running.
#2: If no job is currently running, sees if there are any new calibration directories that we haven't yet processed. Castor is checked and its contents are checked against a Castor Contents local text file. This file isn't nesscary (there is a redundency later on to prevent duplicate calib jobs), but speeds things up.
#3: If there are new  directories, we check to see if any have at least numFilesToRun files. If they don't, we don't update Castor Contents, but we  don't run on them either (its assumed they are being staged and we'll pick them up next time CscCalibQuery is run)
#4: If we find a valid calibration directory with enough files, we check to see if a directory with calibration output for the new calibration run exists. If it does, we don't run on it again.
#5: If it is indeed a new calibration directory, a bash script is created and submitted to a batch queue. The bash script:
#   a) Runs athena calibration algorithm
#   b) Produces web page with calibration monitoring plots
#   c) Emails mailing list about state of calibration (any problems)
#   d) Generates mysql database file for merging into COOL
#   e) (Disabled) automatically adds to database
#...

#minimum number of calibration file in castor to start calibration
numFilesToRun=3
responsiblePerson = "youzhou@email.arizona.edu"
maillist = responsiblePerson

CoolMergeByDefault = False

#Utility functions################################


def updateList(oldListPath,newList):
  print 'updating file list'
  #update the old file list
  outFile = open(oldListPath, 'w')
  outFile.write(newList)
  outFile.close()

#runs calibration on input file.
#It creates a bash script which can setup the job, run it, and do post job
#processing. The bash script is submitted to lxbatch
def runCalib(calType, runNumber,workDir,castorCopyCmd):
  #print 'running calib'
  #initialize based on calibration type
  scriptDir = '${HOME}/CSC/run/'
  if calType == 'pulser':
    calibScript =  scriptDir + 'CscCalcSlopeMon.py'
    dbScript = scriptDir + 'cscWritePSlopeCool.py'
    webDir = '${HOME}/www/csc/pulser'
    runListFile = "pulserRunList.pickle"
    #extractScript = scriptDir +'CscExtractPulser.py'
  elif( calType == 'ped'):
    calibScript = scriptDir + 'CscCalcPedMon.py'
    dbScript = scriptDir + 'cscWritePedRefCool.py'
    onlDbFile = scriptDir + 'online.cal'
    webDir = '${HOME}/www/csc/ped'
    webPageUrl = 'https://atlas-csc-calib.web.cern.ch/atlas-csc-calib/ped/pedRun_' +runNumber
    runListFile = "pedRunList.pickle"
    #extractScript = scriptDir + 'CscExtractPed.py'

  outputDir = workDir +"/CalibResults"
  bsDir = workDir +"/Bytestream"

  print 'outputDir = ' + outputDir


  #Setup finished email message
  emailMessage = 'Finished ' + calType + ' calibration on run number ' + runNumber
  emailMessage += '. Output in ' + outputDir + '.'
  emailMessage += '\\nAnd website at:\\n'
  emailMessage += webPageUrl
  
  #Setup finished email subjects
  goodEmailSubject = '[CSC CALIB PROC]: SUCCESS with ' + calType + 'calib run' + runNumber
  badEmailSubject = '[CSC CALIB PROC]: PROBLEMS with ' + calType + 'calib run' + runNumber

  #Prepare bash script for batch system
  bashFilePath = workDir+'/CscCalib_' + calType + '_' + runNumber + '.sh'

  bsubCmd ='cd ' + outputDir + ';bsub -q 2nd -R "type==SLC5_64&&mem>420" ' + bashFilePath

  bashFileContents = "#!/bin/bash\n"
  bashFileContents += "#To resubmit this job, submit it to the atlasmuonqueu like so:\n#"
  bashFileContents += bsubCmd +"\n"
  bashFileContents += "source ~/CSC/CscSetup.sh\n"
  bashFileContents += "\n"
  bashFileContents += "resultDir=\"" + outputDir + "\"\n"
  bashFileContents += 'bytestreamDir="' + bsDir + '"\n'
  bashFileContents += 'maillist="' + maillist + '"\n'
  bashFileContents += 'webSiteDir="' + webDir + '"\n'

  calFilePrefix= "${resultDir}/" + runNumber
  inputPattern = "${bytestreamDir}/*.data"

  calibCommand = 'echo "Running calibration"\n' \
      + 'mkdir ${resultDir}\n' \
      + 'athena.py -c "outputPre=\'' + calFilePrefix \
      + '\';inputOnlCalibFile=\'' +onlDbFile \
      + '\';inputPat=\'' + inputPattern \
      + '\';reportPrefix=\'' + emailMessage \
      + '\';" ' \
      +  calibScript


  goodEmailCommand = ' mail -s "' + goodEmailSubject + '" $maillist < ' + calFilePrefix + "CalibReport.txt"
  badEmailCommand = ' mail -s "' + badEmailSubject + '" $maillist < ' + calFilePrefix + "CalibReport.txt"
  
  #For reference tag, we actually want the IOV to start just after the LAST run number
  #Get old run numbers
  infile = open(runListFile,"r")
  runList = pickle.load(infile)
  runList.sort()
  print "got runs"
  print runList
  infile.close()


  if(runNumber in runList):
    print "Mailing message"
    message =["mail","-s",\
        '"New castor run directory found for previously processed run ' + str(runNumber) + '"',\
        responsiblePerson,\
        "<",\
        "runAlreadyProcessed.mail"]
    print message
    subprocess.call(message)
    sys.exit()


  highestRun = runList[-1]

  isRunNumberConflict = False

  
  if(highestRun > runNumber):
    #Something odd happening. The Input run number is lower than the last run number
    #this script (thinks it) processed
    #Notify someone important, and don't add to cool when done...
    subprocess.call(["mail","-s",\
        "[CSC CALIB PROC] Wrong run number ordering on run " + str(runNumber) \
        + "! Human intervension required!",\
        responsiblePerson,\
        "<",\
        "runNumberConflict.mail"]\
        )
    isRunNumberConflict = True
  else:
    #No problem, update run list
    runList += [runNumber]
    outfile = open(runListFile,"w")
    pickle.dump(runList,outfile)
    outfile.close()

    
  #Label that we're working, so that other instances of CscCalibQuery won't run
  subprocess.call(['touch','/afs/cern.ch/user/m/muoncali/CSC/run/runningCalibration' + runNumber])

  #Command to create .db file
  DbCommand = 'athena.py -c "input=\'' + calFilePrefix+'.cal\';output=\'' \
      + calFilePrefix + '.db\';IOVRunStart=int(\'' + highestRun + '\')" ' + dbScript

  UploadCommand = ''
  #Noexec prevents actual execution of cool 
  #UploadCommand = '/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py ' \
  #    + "--batch --noexec --comment=\'Automated UPD1 update from " + calType  \
  #    + ' run ' + runNumber +'\' ' \
  #    + calFilePrefix + '.db'   \
  #    + ' COMP200 ATONR_COOL ATLAS_COOLONL_CSC_W PASSWORD '
  #UploadCommand += "\n"





  #Command to upload .db file to database
  UploadCommand += '/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py ' \
      + "--batch "
  if(not CoolMergeByDefault or isRunNumberConflict):
    UploadCommand += " --noexec "
    
  UploadCommand += "--comment=\'Automated reference update from " + calType  \
    + ' run ' + runNumber + ' to IOV starting at ' + highestRun + "' " \
    + calFilePrefix + '.db'   \
    + ' COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_CSC_W WCOOLOFL4CSC17 '

  WebSiteCommand = '\nfs sa ${resultDir} webserver:afs read\n'
  WebSiteCommand += 'cd $resultDir\n'
  WebSiteCommand += 'ln -s ${resultDir} ${webSiteDir}/pedRun_' + runNumber + '\n'
  WebSiteCommand += 'MakeCscPedSite.exe ' + runNumber + '.root ' + runNumber + '.cal_online\n'
  
  t1 = '\t'

  #Run calibration. If no problems detected, go ahead and upload.
  bashFileContents += '\ncd ' + workDir + '\n' \
      + "#Copying files from castor#\n" \
      + castorCopyCmd \
      + '\n'\
      + "#Running calibration (and calib monitoring)\n"\
      + "#, resulting in .cal file and status report\n"\
      + calibCommand +'\n' \
      + "\n"\
      +  "#Athena job to transform *.cal file to *.db file.\n"\
      +  DbCommand + '\n' \
      +  "#Python utility to upload *.db file to database. When entering by\n"\
      +  "#hand, I recomend removing '--batch' flag so you can check puposed\n"\
      +  "#operations before submision to database\n"\
      +  UploadCommand + '\n' \
      + '#Check if AllCalibMonGood file was created, which means that\n'\
      + '#this run passed acceptable criteria in the calibration monitoring\n'\
      + 'if [ -a AllCalibMonGood ]; then\n' \
      + t1 + "#Email list that the calibration looks good\n"\
      + t1 + goodEmailCommand +'\n' \
      + t1 + "################################################################\n"\
      + t1 + "#Execute next two commands if you want to submit database entry#\n"\
      + t1 + "#Useful if these steps were skipped due to suspicious behaviour#\n"\
      + t1 + "#during calibration.                                           #\n"\
      + t1 + "###############################################################\n"\
      + t1 + "\n"\
      + t1 + '\n'\
      + 'else\n' \
      + t1 + "#Suspicious behaviour in calibration. Notify mail list of this fact\n"\
      + t1 + badEmailCommand + '\n' \
      + 'fi\n'\
      + '\n'\
      + '#Always create website'\
      + WebSiteCommand + '\n' \
      + 'rm -rf $bytestreamDir\n' \
      + 'rm -rf ' + scriptDir +"runningCalibration" + runNumber +'\n'

  #Write bashfile
  print "Printing bash file to: " +bashFilePath
  bashFile = open(bashFilePath, 'w')
  bashFile.write(bashFileContents)
  bashFile.close()

  #Submit script
  os.system('chmod +x ' + bashFilePath)
  bsubsMessage = os.popen(bsubCmd).read()

  #Send alert email
  emailMessage = 'Starting ' + calType + ' calibration on run number ' + runNumber
  emailSubject = '[CSC CALIB PROC]: ' +  emailMessage
  emailMessage += '\nbsubs output:\n' + bsubsMessage
  os.system('echo "' + emailMessage + '" | mail -s "' + emailSubject + '" ' + maillist)

#########################################
#Main program#############################
#########################################

print 'running'  
#first command line argument should be the calibration type, pulser or ped.
try:
  calType = sys.argv[1]
  if calType == 'pulser':
    calibFileDir = '/castor/cern.ch/user/l/lampen/CalibRunTest/slope/'
    oldListFilePath = '/afs/cern.ch/user/m/muoncali/CSC/run/pulserList.txt'
    outputDir = '/afs/cern.ch/user/m/muoncali/w0/CSC/runs/pulser/pulser'
  elif calType == 'ped':
    calibFileDir = '/castor/cern.ch/grid/atlas/DAQ/muon/csc/'
    oldListFilePath = '/afs/cern.ch/user/m/muoncali/CSC/run/pedDirList.txt'

    #oldListFilePath = '/afs/cern.ch/user/l/lampen/Calib/dev/rel_0/MuonSpectrometer/MuonCalib/CscCalib/CscCalibAlgs/python/pedList.txt'
    #outputDir = '/afs/cern.ch/user/l/lampen/Calib/dev/rel_0/MuonSpectrometer/MuonCalib/CscCalib/CscCalibAlgs/python/'
    outputDir = '/afs/cern.ch/user/m/muoncali/w0/CSC/runs/ped/ped'
    #data11_calib.00172807.calibration_pedCSC.daq.RAW._lb0000._CSC-EB._0001.data
    #calibRe = re.compile('data10_calib.*calibration_ped\.daq\.RAQ.*\.data')
    #calibRe = re.compile('data10_.*pedCSC.*\.data')
    calibRe = re.compile('data1.*_calib.*calibration_pedCSC\.daq\.RAW.*\.data')
  else:
    print 'Need to specify pulser or ped'
    os._exit(0)
except:
  print 'Need to specify pulser or ped'
  os._exit(0) 


#First, see if a calibration is already running
#If already running, stop
testFile = glob.glob("/afs/cern.ch/user/m/muoncali/CSC/run/runningCalibration*")
if(testFile):
  print 'already running: ' + str(testFile)
  sys.exit()


#Get the current list of directories in castor
print 'rfdir ' + calibFileDir
currentLs = os.popen('rfdir ' + calibFileDir).read()

os.popen('touch testing')

#Get the old list of files from castor
inFile = open(oldListFilePath, 'r')
oldLs = inFile.read()

inFile.close()

print 'checking for changes'

import datetime
now = datetime.datetime.now()
today = now.day

#Check if there are any changes between the lists
if(oldLs != currentLs):
  print 'There has been a change'
  currentDirList = currentLs.split('\n')
  oldDirList = oldLs.split('\n')

  updateRunList = True
  runningDir = ""
  #Run on any new directories
  for Dir in currentDirList:
    #print 'Checking ' + Dir
    if Dir not in oldDirList:
      print "**********************************************************"
      splitDir = Dir.split()
      day = int(splitDir[-3])
      DirName =splitDir[-1]   #last ' ' delimited word in line is Dirname

      print splitDir
      timediff = today - day
      if(timediff > 7 or timediff < -23):
        print timediff

        print "Found old dir " + DirName + ", but its over at least a week old, so we're skipping it"

        continue



      print "day is " + str(day)

      #print 'Dirname is ' + DirName
      cmd = 'rfdir ' + calibFileDir + DirName
      fileList = os.popen(cmd).read().split('\n')

      nFiles = len(fileList) -1
      print "found " + str(nFiles) + " files"

      runNumber = DirName

      #prepare output directory
      outputDirFull = outputDir + 'Run_' + runNumber
      print 'outputDirFull is ' + outputDirFull

      #Loop through files in directory. 
      #Start building castor copy cmd
      #If any files don't match the calibration file
      #requirement, mark this directory as something not interested in.
      ThisCalibDir = False
      castorCopyCmd = "mkdir ${bytestreamDir}"
      for file in fileList:
        fileName = (file.split(' '))[-1]  #last ' ' delimited word in line is fileName
        if(fileName != ""):
          print "fileName: " +fileName
          ThisCalibDir = re.match(calibRe,fileName)
          if(nFiles < numFilesToRun):
            print "only " +str(nFiles) + " files. Breaking."
            if(ThisCalibDir):
              print "There is a calib dir, but it only has " + str(nFiles) \
                  + " file. Will not process."
              updateRunList = False
            break;
          if(ThisCalibDir):
            fullPath = calibFileDir + DirName + '/' + fileName 
            castorCopyCmd += "\nxrdcp root://castoratlas/" + fullPath + " ${bytestreamDir}";
            #print 'found ' + fullPath
          else:
            break

      print "found " + str(nFiles) + " files"

      #only run if have enough files
      if(nFiles >= numFilesToRun):
        #Comment next 4 lines out if you want to run on multiple runs at once
        if(runningDir):
          updateRunList = False
          print "Found new calibration directory to run on (" +DirName + "), but already running on " + runningDir
          continue
        print 'running on ' + DirName

        if(nFiles > numFilesToRun):
          print 'WARNING! Found ' + str(nFiles) + ' calib files, when only ' + str(numFilesToRun) + ' were expected!'


        if( os.path.exists(outputDirFull)):
          print "There is already a directory for run number " + runNumber
          print "Will not run on this pedestal run"
          continue


        if(ThisCalibDir):
          print castorCopyCmd
          #updateList(oldListFilePath,currentLs)
          #os.makedirs(outputDirFull+"/Bytestream")
          os.makedirs(outputDirFull+"/CalibResults")

          runCalib(calType, runNumber, outputDirFull, castorCopyCmd)

          print('Launched job, but will continue to check if we have more runs to run on later')
          runningDir = DirName
          updateRunList = False
          continue


  if( updateRunList):
    #We update the run list so long as there are no runs on it that we expect to run on in the future.
    #The safest thing to do is to wait until all runs are processed.  
    print "No unprocessed or currently being processed calibration runs. Will update run list."
    updateList(oldListFilePath,currentLs)
  else:
    print "NOT updating run list"

else:
  print 'No changes between old and new runs'
  pass
