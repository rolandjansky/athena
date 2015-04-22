#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,datetime,time
import os, os.path
import glob,shutil
import getopt

# Usage String
usage='Usage: fct_reco.py <-c config_file> [-l (local), use this if want to use local machine] [-i inputFileName] [-o outputFileName] [-e nEvents]'

if len(sys.argv) < 2:
    print usage
    sys.exit(1)

# This section deals with the arguments -c, -l, -i and -e
##########################################
try:
    opts, args = getopt.getopt(sys.argv[1:], "hc:e:i:o:l", ["help","config_file=","events=","input=","output=","local"])
except getopt.GetoptError, err:
    print 'Error! Incorrect arguments specified.'
    print usage
    sys.exit(2)
myDir = None
myDirOut = None
myInputEvents = None
local = None
for o, a in opts:
    if o in ("-h", "--help"):
        print usage
        sys.exit()
    elif o in ("-c", "--config_file"):
        config_file = a
    elif o in ("-l", "--local"):
        local = ""
    elif o in ("-i", "--input"):
        myDir = a
    elif o in ("-o", "--output"):
        myDirOut = a
    elif o in ("-e", "--events"):
        myInputEvents = a
    else:
        assert False, "unhandled option"

############################################

# Specify the parent directory.

parentDir='/afs/cern.ch/atlas/offline/external/FullChainTest/rec_input/'

# Print where I am to check config file opening...
workdir=rttdir=os.getcwd()
print 'The script fct_reco.py is working in the directory '+rttdir

# Get the config file into the working directory

configcommand = 'get_files -data '+config_file
os.system(configcommand)

# Open config file and read config

config_file=open(config_file)

myinputRDOFile=config_file.readline()
myoutputESDFile=config_file.readline()
myoutputESDFile=myoutputESDFile[0:len(myoutputESDFile)-1]
myoutputAODFile=config_file.readline()
#myoutputAODFile=myoutputAODFile[0:len(myoutputAODFile)-1]
myntupleFile=config_file.readline()
myntupleFile=myntupleFile[0:len(myntupleFile)-1]
mymaxEvents=config_file.readline()
mymaxEvents=mymaxEvents[0:len(mymaxEvents)-1]
myskipEvents=config_file.readline()
myskipEvents=myskipEvents[0:len(myskipEvents)-1]
mygeometryVersion=config_file.readline()
mygeometryVersion=mygeometryVersion[0:len(mygeometryVersion)-1]
mytriggerConfig=config_file.readline()
mytriggerConfig=mytriggerConfig[0:len(mytriggerConfig)-1]
myDBRelease=config_file.readline()
myDBRelease=myDBRelease[0:len(myDBRelease)-1]
myconditionsTag=config_file.readline()
myconditionsTag=myconditionsTag[0:len(myconditionsTag)-1]

if local is not None:
    if "WORKDIR" in os.environ: workdir = os.environ["WORKDIR"]
    print "Working directory is",workdir
    os.chdir(workdir)


# Set up outputFile command if there is no -o specified
if myDirOut is None:
    myoutputAODFile=myoutputAODFile[0:len(myoutputAODFile)-1]
# Set up outputAODFile command if there is -o specified
else:
    myoutputAODFile='outputAODFile='+myDirOut

# Set up indputFile command if there is no -i specified
if myDir is None:
    myDir=myinputRDOFile.replace('inputRDOFile=','')
    myDir=myDir[0:len(myDir)-1]
    myFiles=glob.glob(parentDir+myDir+'/*.root.*')
    if not myFiles:
        print 'Error! No files found in specified directory '+parentDir+myDir
        sys.exit(1)
    filelist=''
    for myFile in myFiles:
        filelist+=myFile+','
    filelist=filelist[0:len(filelist)-1]
    myinputRDOFile='inputRDOFile='+filelist

# Set up indputRDOFile command if there is -i specified
else:
    myinputRDOFile='inputRDOFile='+myDir

# Set up maxEvents - either from config or -e input if given 
if myInputEvents is not None:
    mymaxEvents='maxEvents='+myInputEvents

# The command
mycommand='Reco_trf.py '+myinputRDOFile+' '+myoutputESDFile+' '+myoutputAODFile+' '+myntupleFile+' '+mymaxEvents+' '+myskipEvents+' '+mygeometryVersion+' '+mytriggerConfig+' '+myDBRelease+' '+myconditionsTag

# Execute the command
print 'The command I am using is: '
print mycommand
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
print 'Finished fct_reco: ',datetime.datetime.now()
print 
