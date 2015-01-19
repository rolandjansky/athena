#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import time

import PyJobTransforms.trfArgClasses as trfArgClasses

def main( runNum = None, procType = None, forceSkipQueue = 0 ):
    #===== Run number =====
    if runNum == None:
        print 'ERROR no run number given'
        return False
    Run0 = runNum
    #===== procType =====
    if procType == None:
        #    if procType == trfArgClasses.argList():
        print 'ERROR no process type given'
        return False
    for iType in procType:
        if not iType in ['doNoisyStrip','doNoiseOccupancy','doDeadChip','doDeadStrip','doHV','doBSErrorDB','doRawOccupancy','doEfficiency','doLorentzAngle']:
            print 'ERROR process type does not match any possible candidates'
            return False
        else :
            pType = iType

    #===== Check the stable beam flag in Run0 =====
    #--- RunQuery
    runQuery  = 'AtlRunQuery.py '
    runQuery += '--run \"' + str(Run0) + '\" '
    runQuery += '--lhc \"stablebeams TRUE\" '
    runQuery += '--partition \"ATLAS\" '
    runQuery += '--detmaskin \"240A\" '
    runQuery += '--projecttag \"data*_*eV\" '
    if pType == 'doNoisyStrip':
        runQuery += '--streams \"*SCTNoise 10000+\" '
        runQuery += '--show \"streams *SCTNoise\" '
    if pType == 'doDeadChip' or pType == 'doDeadStrip':
        runQuery += '--streams \"*express 200000+\" '
        runQuery += '--show \"streams *express\" '

    runQuery += '--show run --show events --show time --show \"lhc\" '
    runQuery += '--noroot --nohtml --verbose'
    print runQuery
    os.system(runQuery)
    
    print Run0
    #--- Check stable beam flag
    StableBeam = False
    if os.path.exists('./data/MyLBCollection.xml'):
        f = open('./data/MyLBCollection.xml')
        line = f.readline()
        while line:
            if "Metadata" in line and "RunList" in line and str(Run0) in line:
                StableBeam = True
                print "Run %s : run selection passed ---> job will be launched" %(Run0)
            line = f.readline()
        f.close()

    if not StableBeam:
        print "Run %s : run selection didn't pass Stable Beam check--- job will be finished" %(Run0)
        return False
    
    #===== Read last run uploaded : only for NoisyStrip =====
    #--- Only for NoisyStrip
    Wait = True
    if pType != 'doNoisyStrip' or forceSkipQueue == 1 :
        Wait = False
                    
    #--- Check if Run0 is next to the last run uploaded
    while Wait:
        #--- Read last run uploaded
        RunLast = 0
        if os.path.exists('/afs/cern.ch/user/s/sctcalib/scratch0/lastRun') :
            f = open('/afs/cern.ch/user/s/sctcalib/scratch0/lastRun')
            line = f.readline()
            while line:
                RunLast = line
                line = f.readline()
            f.close()
            print "Run %s : the last run uploaded" %(RunLast)[:-1]

#        --- Check last run is before Run0
        if int(RunLast) > int(Run0) :
            print "Run %s : taken later than the current run ---> job will not be run" %(RunLast)[:-1]
            return False
                    
        #--- RunQuery
        runQuery  = 'AtlRunQuery.py '
        runQuery += '--run \"' + str(RunLast[:-1]) + '+\" '
        runQuery += '--lhc \"stablebeams TRUE\" '
        runQuery += '--partition \"ATLAS\" '
        runQuery += '--detmaskin \"240A\" '
        runQuery += '--projecttag \"data*_*eV\" '
        runQuery += '--streams \"*SCTNoise 10000+\" '
        runQuery += '--show \"streams *SCTNoise\" '
        runQuery += '--show run --show events --show time --show \"lhc\" '
        runQuery += '--noroot --nohtml --verbose'
        print runQuery
        os.system(runQuery)
        
        #--- Check Run0 and RunLast
        if os.path.exists("./data/MyLBCollection.xml") :
            f = open('./data/MyLBCollection.xml')
            line = f.readline()
            while line:
                if "Metadata" in line and "RunList" in line :
                    #--- Check if there are runs waiting for processing/upload
                    RunWait = 0
                    for iRun in range( int(RunLast)+1, int(Run0) ):
                        if str(iRun) in line:
                            RunWait += 1
                            print "Run %s : this run has to be processed/uploaded ---> job in pending state " %(iRun)

                    if RunWait == 0:
                        Wait = False
                        print "Run %s : checked the last run uploaded ---> job will be launched" %(Run0)

                line = f.readline()
            f.close()

        #--- Wait for RunLast update
        if Wait:
            print 'Run %s : waiting for previous run(s) processed/uploaded' %(Run0)
            time.sleep(5)

    #--- Start processing
    print 'Run %s : job starts running now' %(Run0)
    return True

#===== Execute     
if __name__ == "__main__":
    try :
        runNumber = int( sys.argv[1] )
    except :
        print "Failed to read run number" %(sys.argv[1])
        sys.exit( -1 )
    try :
        processType = str( sys.argv[2] )
    except :
        print "Failed to read proces type" %(sys.argv[2])
        sys.exit( -1 )
    try :
        skipQueue = int( sys.argv[3] )
    except :
        print "Setting skipqueue to default (wait until previous runs are analysed)"
        skipQueue = 0

    main( runNum = runNumber, procType = processType, forceSkipQueue = skipQueue )
