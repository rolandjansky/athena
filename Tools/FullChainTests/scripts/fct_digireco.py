#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,datetime,time
import os, os.path
import glob,shutil
import getopt

#setup eos command. eos is an alias so it will not work in os.popen() as a command.
eos="/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select"

print "We will use eos command from : "+eos

# Usage String
usage='Usage: fct_digireco.py <-c config_file> [-i inputdataset] [-o outputfile] [-f firstfile] [-n neventsperfile] [-r runnumber]'

print 'Starting fct_digi: ',datetime.datetime.now()

if len(sys.argv) < 2:
    print usage
    sys.exit(1)

# This section deals with the arguments -c, -e, -o, -f, -n, -r
##########################################
try:
    opts, args = getopt.getopt(sys.argv[1:], "hc:i:o:f:n:r:e:", ["help","config_file=","events=","local"])
except getopt.GetoptError, err:
    print 'Error! Incorrect arguments specified.'
    print usage
    sys.exit(2)

myInputDataset='105200/valid1.105200.T1_McAtNlo_Jimmy.simul.HITS.e380_s610_tid094777'
myOutputFile = 'TopRecoLong'
myFirstFile=1
myEventsPerFile=2000
config_file=None
event_number=None

for o, a in opts:
    if o in ("-h", "--help"):
        print usage
        sys.exit()
    elif o in ("-c", "--config_file"):
        config_file = a
    elif o in ("-i", "--inputdataset"):
        myInputDataset = a
    elif o in ("-o", "--output"):
        myOutputFile = a
    elif o in ("-f", "--firstfile"):
        myFirstFile = a
    elif o in ("-n", "--neventsperfile"):
        myEventsPerFile = int(a)
    elif o in ("-r", "--runnumber"):
        rn = a
    elif o in ("-e", "--events"):
        event_number = a
    else:
        assert False, "unhandled option"

############################################
# Open config file and read config
from FullChainTests.readconfig import readconfig
a=readconfig(config_file)

# Reset event number
if event_number != None:
    a.setEventNumber(event_number)

# Specify the parent directory for input files
# castorinputdir="/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/fct/rec_input/"+myInputDataset+"/"
castorinputdir=myInputDataset+"/"
print 'castorinputdir = ' + castorinputdir

if len(os.popen(eos+" ls "+castorinputdir).readlines()) == 0:
    print 'Specified dataset does not exist'
    sys.exit(1)



print 'DataSet exists'
# use the day of the week
fuid=os.popen("echo $AtlasVersion").readline()


# Change to workernode directory, but keep track of RTT dir
workdir=rttdir=os.getcwd()
print 'The script fct_digireco.py is working in the directory '+rttdir
if "WORKDIR" in os.environ: workdir = os.environ["WORKDIR"]
print "Working directory is",workdir
os.chdir(workdir)

# Specify parent directory for DBRelease and check if the DBRelease is there
# Also look into the unvalidated caches
DBpath='/afs/cern.ch/atlas/www/GROUPS/DATABASE/pacman4/DBRelease/'
DBunvalidpath='/afs/cern.ch/atlas/www/GROUPS/DATABASE/pacman4/DBRelease-unvalidated/'
print a.DBRelease()
if a.DBRelease() in os.listdir(DBpath):
    print "linking DB Release to default cache"
    os.system('ln -s '+DBpath+a.DBRelease()+' .')
#elif 'DBRelease-'+a.DBRelease()+'.tar.gz' in os.listdir(DBunvalidpath):
elif a.DBRelease() in os.listdir(DBunvalidpath):
    print "linking DB Release to unvalidated cache"
    os.system('ln -s '+DBunvalidpath+a.DBRelease()+' .')
else:
    print 'DBRelease  '+a.DBRelease()+' does not exist!'
    sys.exit(1)


# Get files from castor
noFiles=(a.eventsPerJob()-1)/myEventsPerFile+1
myFiles=os.popen(eos+" ls "+castorinputdir).readlines()
print "Number of files: " + str(noFiles)
for i in range(int(myFirstFile), int(myFirstFile)+noFiles):
    copycmd="xrdcp root://eosatlas/"+castorinputdir+myFiles[i-1].rstrip('\n')+" ."
    print "print copycmd: " + copycmd
    os.system(copycmd)

print "Fetching input files finished",datetime.datetime.now()
print


myFiles=glob.glob('HITS*.root.*')
if not myFiles:
    print 'Error! No HITS files found in directory' 
    sys.exit(1)
filelist=''
myFiles.sort()
for myFile in myFiles:
    filelist+=myFile+','
filelist=filelist[0:len(filelist)-1]

# Handle file outputs
myRDOFile = myOutputFile+'.RDO.pool.root'
myESDFile = myOutputFile+'.ESD.pool.root'
myAODFile = myOutputFile+'.AOD.pool.root'
myHISTFile = myOutputFile+'HIST.root'

mycommand=a.digitTrf(filelist,myRDOFile)
print 'The command I am using for digi is: '
print mycommand
print
os.system(mycommand)

mycommand=a.recoTrf(myRDOFile,myOutputFile,rn)   
print 'The command I am using for reco is: '
print mycommand
print
os.system(mycommand)


# Get rid of unwanted files

myDelFiles=['RDO','ESD','HIST','NTUP_PIXELCALIB','NTUP_TRIG']
for delFile in myDelFiles:
    print "Removing all files containing ."+delFile+". in their file name"
    os.system("rm *."+delFile+".*")

# Direct castor archive

myArchiveCastor=['AOD']
for ff in myArchiveCastor:
    ff=ff.rstrip('\n')
    castorbase="/eos/atlas/atlascerngroupdisk/proj-sit/fct/rec_output/"+fuid+"/"+ff
    print "Copying files of type "+ff+" to castor location "+castorbase
    for aa in os.popen("ls *"+ff+"*.root").readlines():
        aa=aa.rstrip('\n')
        os.system("xrdcp "+aa+" root://eosatlas/"+castorbase)
        os.system("rm "+aa)

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
print 'Finished fct_digireco: ',datetime.datetime.now()
print
