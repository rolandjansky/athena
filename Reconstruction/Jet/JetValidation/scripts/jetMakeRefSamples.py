#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## **************************************************** 
## Create reference ESD/xAOD files from a given input.
##
## This scripts run a series of transform from a given input
## file (typically RDO or RAW) to perform the steps
##  RDO/RAW --> ESD
##  ESD --> AOD
## Each step produces a file which is expected to be similar to
## those in /afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/
##
## This script is intended to be

import os, os.path
import sys
import shlex, subprocess
import argparse

parser = argparse.ArgumentParser(description='''
Creates reference ESD/AOD by running a series of Reco_tf.py from a given input
file (typically RDO or RAW).
Performs typically 2 jobs :
  RDO/RAW --> ESD
  ESD --> AOD
Each step produces a file which is expected to be similar to the reference
files in /afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/.

 Ex:
 %s MC             # -> create reference files from /afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/RDO
 %s MC MyDir       # -> same but creates files under MyDir/
 %s ESD.root MyDir # -> create an AOD reference file from ESD.root under MyDir/
'''%((os.path.basename(sys.argv[0]),)*3),
                                 formatter_class=argparse.RawDescriptionHelpFormatter,
                                 )

parser.add_argument('sampleType', type=str, help='full path to a file. Or "mc" or "data" : will start with a reference RDO or RAW file from the JetEtmiss afs space' )
parser.add_argument('outputDir', type=str, nargs='?', default='jetRefFiles/')

parser.add_argument('-n', '--noRun', action='store_true', help='Do not run the Reco_tf commands, just print them.')
parser.add_argument( '--maxEvents', type=int, default=50, help="Propaged to the maxEvents of the Reco_tf commands")
parser.add_argument( '--oneStep', action='store_true' , help="stop after 1st step processed")
parser.add_argument( '--overWrite', action='store_true' , help="force to overWrite the output dir")


arg=parser.parse_args()
## **************************************************** 

#print arg.sampleType , arg.outputDir, arg.noRun

maxEvents=arg.maxEvents
skipEvents=0
logFile="output.log"

#Geometry/conditions tags:
#    Matched what was done for the official production of the sample at the time of this writing
#    geometryVersion=ATLAS-GEO-20-00-01
#    conditionsTagMC=OFLCOND-MC12-SDR-06
#    conditionsTagData=COMCOND-BLKPA-RUN1-01
#    http://www-f9.ijs.si/atlpy/atlprod/prodrequest/?q=mc12_8TeV.117050.PowhegPythia_P2011C_ttbar
#    http://panda.cern.ch/server/pandamon/query/?reqid=1162571&mode=showtask0
#    https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/CoolProdTags
specifyCondAndGeo= False # true to use the tags below, false to autoconfigure everything
geometryVersion="ATLAS-GEO-20-00-01"
conditionsTagMC="OFLCOND-MC12-SDR-06"
conditionsTagData="COMCOND-BLKPA-RUN1-01"
IsCOMMON=True


# Default input files
#defaultMC="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/HITS/mc12_8TeV.CurrentRef.HITS.pool.root"
#defaultMC="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/MC/HITS/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.simul.HITS.e1728_s1581.039772.pool.root.1"
defaultMC="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/RDO/mc12_8TeV.CurrentRef.RDO.pool.root"
defaultData="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/RAW/data12_8TeV.CurrentRef.RAW"
#defaultData="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/RAW/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1"


def makeDirAndCd(dir):
    if not os.path.exists(dir):        
        os.mkdir(dir)
    elif not arg.overWrite:
        print 
        print "ERROR !!!"
        print " Directory ",dir,'already exists. Not overwriting it (use --overWrite option if needed)'
        sys.exit(1)

    os.chdir(dir)


def runStep(inputFile):
    if not os.path.exists(inputFile) and not arg.noRun:
        print 'ERROR input file ', inputFile , ' missing'
        sys.exit(2)

    preExec = ''
    transform="Reco_tf.py"

    inputFileBase = os.path.basename(inputFile)

    if 'HITS' in inputFileBase:
        inputType = 'HITS'
        outputType = "RDO"
        #transform="Digi_trf.py"
        preExec=""

    elif "RDO" in inputFileBase:
        inputType="RDO"
        inputDataType=inputType
        outputType="ESD"
        preExec='''from RecExConfig.RecFlags import rec;rec.doTrigger=False;rec.doBTagging=False'''

    elif "RAW" in inputFileBase:
        inputType="RAW"
        inputDataType="BS" #bytestream
        outputType="ESD"
        preExec='''from RecExConfig.RecFlags import rec;rec.doTrigger=False'''

    elif 'ESD' in inputFileBase:
        inputType="ESD"
        inputDataType=inputType
        outputType="AOD"
        preExec='''from RecExConfig.RecFlags import rec;rec.doTrigger=False;from JetValidation.RTTConfig import scheduleRTTJetTests;rec.UserExecs = ["scheduleRTTJetTests()"]'''

    else:
        print "ERROR RunStep: Input file does not appear to be a supported type (RAW, HITS, RDO, ESD)"
        print '   -> got', inputFileBase
        sys.exit(3)

    print "Starting ",inputType, ' to ', outputType


    runDir = arg.outputDir + '/' + inputType+'to'+outputType + '/'
    if not os.path.exists(runDir):
        os.mkdir(runDir)
    outputFile = runDir + inputFileBase.replace(inputType, outputType)
    if arg.overWrite and os.path.exists(outputFile):
        os.remove(outputFile)
        
    outputLog = runDir+'log'
    
    if specifyCondAndGeo:
        # from older sh script
        fullComand="${transform} preExec=${preExec} input${inputDataType}File=${inputDir}/${inputFile} output${outputType}File=${outputFile} maxEvents=${maxEvents} skipEvents=${skipEvents} geometryVersion=$geometryVersion conditionsTag=$conditionsTag autoConfiguration=everything" 
    else:
        comandArgs = [ '--preExec='+preExec,
                       '--input%sFile=%s'%(inputDataType, inputFile),
                       '--output%sFile=%s'%(outputType, outputFile),
                       '--maxEvents='+str(maxEvents),
                       '--skipEvents='+str(skipEvents),
                       '--autoConfiguration=everything',
                       #'--postExec=saxasxa'
                       ]

    print 'Running : '
    print transform, ' '.join(comandArgs)
    print
    
    if arg.noRun :
        res = 0
    else:
        logfile = open(outputLog, 'w')
        res = subprocess.call( [transform] + comandArgs, stdout=logfile, stderr=logfile)
        logfile.close()

    if res != 0:
        print 'ERROR RunStep: Transform appears to have failed - exiting. Check ', outputLog
        sys.exit(4)



    print 'Done %s to %s step'%(inputType, outputType)
    return outputFile


def runChain(inputFile):
    print 'runChain ', inputFile
    while any( [t in os.path.basename(inputFile) for t in ['HITS','RDO','RAW','ESD'] ] ):
        print 'runStep at ',os.path.basename(inputFile)
        inputFile = runStep(inputFile)
        if arg.oneStep:
            return


if arg.sampleType.lower() == 'mc':
    inputFile=defaultMC
elif arg.sampleType.lower() == 'data':
    inputFile=defaultData
else:
    # assume sampleType is a path
    inputFile = os.path.abspath(arg.sampleType)

if not os.path.exists(inputFile):
    print 'ERROR Input path does not exist ', inputFile
    sys.exit(5)
 

arg.outputDir = os.path.abspath(arg.outputDir)
   
makeDirAndCd( arg.outputDir )

runChain(inputFile)

sys.exit(0)
