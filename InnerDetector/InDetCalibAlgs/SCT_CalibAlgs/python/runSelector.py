#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import time
import subprocess

import PyJobTransforms.trfArgClasses as trfArgClasses

def main( runNum = None, procType = None, forceSkipQueue = 0, Stream = None ):
#def main( runNum = None, procType = None, forceSkipQueue = 0 ):
    #===== Run number =====
    if runNum == None:
        print 'ERROR no run number given'
        return False
    Run0 = runNum
    #===== procType =====
    if procType == None:
        print 'ERROR no process type given'
        return False
    if Stream == None:
        print 'ERROR no stream given'
        return False
    for iType in procType:
        if not iType in ['doNoisyStrip','doNoiseOccupancy','doDeadChip','doDeadStrip','doHV','doBSErrorDB','doRawOccupancy','doEfficiency','doLorentzAngle']:
            print 'ERROR process type does not match any possible candidates'
            return False
        else :
            pType = iType

    #run RunQuery only if the stream _is not_ cosmics
    
    print 'STREAM'
    print Stream
    #===== Check the stable beam flag in Run0 =====
    #--- RunQuery
    runQuery  = 'AtlRunQuery.py '
    runQuery += '--run \"' + str(Run0) + '\" '

    if 'cos' not in Stream:
        runQuery += '--lhc \"stablebeams TRUE\" '

    runQuery += '--partition \"ATLAS\" '
    runQuery += '--detmaskin \"240A\" '


    if 'eV' in Stream:
        runQuery += '--projecttag \"data*_*eV\" '
    elif 'hi' in Stream:
        runQuery += '--projecttag \"data*_hi\" '
    else:
        runQuery += '--projecttag \"data*_cos\" '
    # if 'cos' not in Stream:
    #     runQuery += '--projecttag \"data*_*eV\" '
    # else:
    #     runQuery += '--projecttag \"data*_cos\" '

    if pType == 'doNoisyStrip':
        runQuery += '--streams \"*calibration_SCTNoise 10000+\" '
        runQuery += '--show \"streams *calibration_SCTNoise\" '

    if pType == 'doDeadChip' or pType == 'doDeadStrip':

        if 'cos' not in Stream:
            runQuery += '--streams \"*express 200000+\" '
            runQuery += '--show \"streams *express\" '
        else:
            runQuery += '--streams \"*IDCosmic 200000+\" '
            runQuery += '--show \"streams *IDCosmic\" '


    runQuery += '--show run --show projecttag --show events --show time --show \"lhc\" '
    runQuery += '--noroot --nohtml --verbose'
    print runQuery
    os.system(runQuery)
    

    print Run0

#    --- Check stable beam flag if stream _is not_ cosmics
    if 'cos' not in Stream:
    
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

        else: 
            print "ERROR problem in access to /data/MyLBCollection.xml file produced by AtlRunQuery.py --- probably due to AtlRunQuery crash..."
            sys.exit( -1 )
            
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

#        if 'cos' not in Stream:
        if 'cos' not in Stream:
            runQuery += '--lhc \"stablebeams TRUE\" '
        runQuery += '--partition \"ATLAS\" '
        runQuery += '--detmaskin \"240A\" '

        if 'eV' in Stream:
            runQuery += '--projecttag \"data*_*eV\" '
        elif 'hi' in Stream:
            runQuery += '--projecttag \"data*_hi\" '
        else:
            runQuery += '--projecttag \"data*_cos\" '
        # if 'cos' not in Stream:
        #     runQuery += '--projecttag \"data*_*eV\" '
        # else:
        #     runQuery += '--projecttag \"data*_cos\" '

        runQuery += '--streams \"*calibration_SCTNoise 10000+\" '
        runQuery += '--show \"streams *calibration_SCTNoise\" '
        runQuery += '--show run --show projecttag --show events --show time --show \"lhc\" '
        runQuery += '--noroot --nohtml --verbose'
        print runQuery
        os.system(runQuery)

        #--- Check Run0 and RunLast
        runList=[]
        runDict={}
        runNum=''
        runPro=''
        if os.path.exists("./data/QueryResult.txt") :
            f = open('./data/QueryResult.txt')
            line = f.readline()
            while line:
                if 'Run:' in line and runNum=='':
                    runNum = line.split(':')[1]
                    runNum = runNum.replace("\n",'').replace(' ','')
                if 'Project tag:' in line and runPro=='':
                    runPro = line.split('\'')[1]
                if runNum!='' and runPro!='':
                    if 'data17_cos' in runPro or 'data17_13TeV' in runPro or 'data17_5TeV' in runPro or 'data17_900GeV' in runPro or 'data17_hi' in runPro:
                        runList.append(runNum)
                        runDict[runNum] = runPro
                    runNum=''
                    runPro=''
                line = f.readline()
                
            f.close()


            RunWait = 0
            for iRun in range( int(RunLast)+1, int(Run0) ):
                if str(iRun) in runList:
                    RunWait += 1
                    print "Run %s: this run has to be processed/uploaded ---> job in pending state " %(iRun)


            if RunWait == 0:
                Wait = False
                print "Run %s : checked the last run uploaded ---> job will be launched" %(Run0)

        # if os.path.exists("./data/MyLBCollection.xml") :
        #     f = open('./data/MyLBCollection.xml')
        #     line = f.readline()
        #     while line:
        #         if "Metadata" in line and "RunList" in line :
        #             #--- Check if there are runs waiting for processing/upload
        #             RunWait = 0
        #             for iRun in range( int(RunLast)+1, int(Run0) ):
        #                 if str(iRun) in line:
        #                     RunWait += 1
        #                     print "Run %s : this run has to be processed/uploaded ---> job in pending state " %(iRun)

        #             if RunWait == 0:
        #                 Wait = False
        #                 print "Run %s : checked the last run uploaded ---> job will be launched" %(Run0)

        #         line = f.readline()
        #     f.close()

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
