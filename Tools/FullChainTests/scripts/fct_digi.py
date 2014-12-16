#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,datetime,time
import os, os.path
import glob,shutil
import getopt

# Usage String
usage='Usage: fct_digi.py <-c config_file> [-l (local), use this if want to use local machine] [-o outputFile][-e events] [-f inputhitsfile] '

print 'Starting fct_digi: ',datetime.datetime.now()

if len(sys.argv) < 2:
    print usage
    sys.exit(1)

# This section deals with the arguments -c, -e and -l
##########################################
try:
    opts, args = getopt.getopt(sys.argv[1:], "hc:e:o:f:l", ["help","config_file=","events=","output=","inputhitsfile=","local"])
except getopt.GetoptError, err:
    print 'Error! Incorrect arguments specified.'
    print usage
    sys.exit(2)
local = None
myInputEvents = None
myDir = None
inputhitsfile = "test"

print "inputhitsfile = " + inputhitsfile

for o, a in opts:
    if o in ("-h", "--help"):
        print usage
        sys.exit()
    elif o in ("-c", "--config_file"):
        config_file = a
    elif o in ("-o", "--output"):
        myDir = a
    elif o in ("-e", "--events"):
        myInputEvents = a
    elif o in ("-f", "--inputhitsfile"):
        inputhitsfile = a
    elif o in ("-l", "--local"):
        print "localset"
        local = ""
    else:
        assert False, "unhandled option"



############################################

print "inputhitsfile2 = " + inputhitsfile

# Specify the parent directory.

parentDir='/afs/cern.ch/atlas/offline/external/FullChainTest/rec_input/'

# Print where I am...

workdir=rttdir=os.getcwd()
print 'The script fct_digi.py is working in the directory '+rttdir

# Get the config file into the rtt directory

configcommand = 'get_files -data '+config_file
os.system(configcommand)

# Open config file and read config

config_file=open(config_file)

myinputHitsFile=config_file.readline()
myoutputRDOFile=config_file.readline()
#myoutputRDOFile=myoutputRDOFile[0:len(myoutputRDOFile)-1]
mymaxEvents=config_file.readline()
mymaxEvents=mymaxEvents[0:len(mymaxEvents)-1]
myskipEvents=config_file.readline()
myskipEvents=myskipEvents[0:len(myskipEvents)-1]
mygeometryVersion=config_file.readline()
mygeometryVersion=mygeometryVersion[0:len(mygeometryVersion)-1]
mydigiSeedOffset1=config_file.readline()
mydigiSeedOffset1=mydigiSeedOffset1[0:len(mydigiSeedOffset1)-1]
mydigiSeedOffset2=config_file.readline()
mydigiSeedOffset2=mydigiSeedOffset2[0:len(mydigiSeedOffset2)-1]
myminbiasHitsFile=config_file.readline()
myminbiasHitsFile=myminbiasHitsFile[0:len(myminbiasHitsFile)-1]
myjobConfig=config_file.readline()
myjobConfig=myjobConfig[0:len(myjobConfig)-1]
mytriggerConfig=config_file.readline()
mytriggerConfig=mytriggerConfig[0:len(mytriggerConfig)-1]
myDBRelease=config_file.readline()
myDBRelease=myDBRelease[0:len(myDBRelease)-1]
myconditionsTag=config_file.readline()
myconditionsTag=myconditionsTag[0:len(myconditionsTag)-1]

# Set up maxEvents - either from config or -e input if given 
if myInputEvents is not None:
    mymaxEvents='maxEvents='+myInputEvents

# Set up indputFile command if there is no -i specified
if myDir is None:
    myoutputRDOFile=myoutputRDOFile[0:len(myoutputRDOFile)-1]

# Set up indputRDOFile command if there is -i specified
else:
    myoutputRDOFile='outputRDOFile='+myDir
    
    
if local is not None:
    print "Fetching HITS files starting",datetime.datetime.now()
    workdir = os.getcwd()
    if "WORKDIR" in os.environ: workdir = os.environ["WORKDIR"]
    print "Working directory is",workdir
    os.chdir(workdir)

    noFiles=(int(mymaxEvents[10:len(mymaxEvents)])-1) / 10 + 1
#    noFiles=(int(mymaxEvents[10:len(mymaxEvents)])-1) / 25 + 1

    for i in range(1, noFiles+1):
        copycmd = ""
        if len(str(i))==1:
            copycmd = "cp " + inputhitsfile + "  HITS.094777._00000"+str(i)+".pool.root.1"
        if len(str(i))==2:
            copycmd = "cp " + inputhitsfile + "  HITS.094777._0000"+str(i)+".pool.root.1"
        if len(str(i))==3:
            copycmd = "cp " + inputhitsfile + "  HITS.094777._000"+str(i)+".pool.root.1"
        if len(str(i))==4:
            copycmd = "cp " + inputhitsfile + "  HITS.094777._00"+str(i)+".pool.root.1"
        os.system(copycmd)
        print copycmd

    
#    for i in range(1, noFiles+1):
#        copycmd = ""
#        if len(str(i))==1:
#            copycmd = "cp /afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/i686-slc5-gcc43-opt/AtlasProduction/Tools/FullChainTests/TopChainJob/CSC.005200.T1_McAtNlo_Jimmy.HITS.pool.root  HITS.094777._00000"+str(i)+".pool.root.1"
#        if len(str(i))==2:
#            copycmd = "cp /afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/i686-slc5-gcc43-opt/AtlasProduction/Tools/FullChainTests/TopChainJob/CSC.005200.T1_McAtNlo_Jimmy.HITS.pool.root  HITS.094777._0000"+str(i)+".pool.root.1"
#        if len(str(i))==3:
#            copycmd = "cp /afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/i686-slc5-gcc43-opt/AtlasProduction/Tools/FullChainTests/TopChainJob/CSC.005200.T1_McAtNlo_Jimmy.HITS.pool.root  HITS.094777._000"+str(i)+".pool.root.1"
#        if len(str(i))==4:
#            copycmd = "cp /afs/cern.ch/atlas/project/RTT/prod/Results/fct/chainstore/dev/i686-slc5-gcc43-opt/AtlasProduction/Tools/FullChainTests/TopChainJob/CSC.005200.T1_McAtNlo_Jimmy.HITS.pool.root  HITS.094777._00"+str(i)+".pool.root.1"         
#        os.system(copycmd)
#        print copycmd

    print "Fetching HITS files finished",datetime.datetime.now()
    print


# Set up inputFile command if there is no -l specified
if local is None:
    myDir=myinputHitsFile.replace('inputHitsFile=','')
    myDir=myDir[0:len(myDir)-1]
    myFiles=glob.glob(parentDir+myDir+'/*.root.*')
    if not myFiles:
        print 'Error! No files found in specified directory '+parentDir+myDir
        sys.exit(1)
    filelist=''
    for myFile in myFiles:
        filelist+=myFile+','
    filelist=filelist[0:len(filelist)-1]
    myinputHitsFile='inputHitsFile='+filelist

# Set up inputHitsFile command if there is -l specified
else:
    myFiles=glob.glob('HITS*.root.*')
    if not myFiles:
        print 'Error! No HITS files found in directory' 
        sys.exit(1)
    filelist=''
    for myFile in myFiles:
        filelist+=myFile+','
    filelist=filelist[0:len(filelist)-1]
    myinputHitsFile='inputHitsFile='+filelist

# The command
mycommand='Digi_trf.py '+myinputHitsFile+' '+myoutputRDOFile+' '+mymaxEvents+' '+myskipEvents+' '+mygeometryVersion+' '+mydigiSeedOffset1+' '+mydigiSeedOffset2+' '+myminbiasHitsFile+' '+myDBRelease+' '+myconditionsTag
#mycommand='Digi_trf.py '+myinputHitsFile+' '+myoutputRDOFile+' '+mymaxEvents+' '+myskipEvents+' '+mygeometryVersion+' '+mydigiSeedOffset1+' '+mydigiSeedOffset2+' '+myminbiasHitsFile+' '+myjobConfig+' '+mytriggerConfig+' '+myDBRelease+' '+myconditionsTag
    
# Execute the command
print 'The command I am using is: '
print mycommand
print
os.system(mycommand)

if local is not None:
    # Copy files back to RTT directory... 
    print
    print "Copying files back to RTT working directory ",datetime.datetime.now()
    print "workdir is "+workdir+", rttdir is "+rttdir
    print
    for filename in os.listdir(workdir):
        test = filename.startswith('HITS')
        if not test:
            try:
                shutil.copyfile(workdir+'/'+filename,rttdir+'/'+filename)
            except:
                print "Cannot copy file "+filename
    print "Finished copy ",datetime.datetime.now()
    print

print 
print 'Finished fct_digi: ',datetime.datetime.now()
print
