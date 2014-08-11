#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import time
from PyCool import cool, coral

from CoolRunQuery.utils.AtlRunQueryLookup import InitDetectorMaskDecoder

def main( runNum=None ):
    if runNum == None:
        print 'ERROR no runNumber given'
        sys.exit(-1)

    print 'runInfo from python folder'
    since = ( runNum << 32 )
    until = ( (runNum+1) << 32 )-1
    dbSvc = cool.DatabaseSvcFactory.databaseService()
    RunCtrlDB = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO'
    RunCtrlDb = dbSvc.openDatabase( RunCtrlDB )
    try:
        RunCtrlDb = dbSvc.openDatabase( RunCtrlDB )
    except Exception,e:
        print 'Problem opening database', e
        sys.exit(-1)
    
    folder_runCtrl = RunCtrlDb.getFolder( '/TDAQ/RunCtrl/EOR_Params' )
    chList = folder_runCtrl.listChannels()
    if not chList:
        print 'ERROR : no data in /TDAQ/RunCtrl/EOR_Params'

    for Id in chList:
        objs = folder_runCtrl.browseObjects( since, until, cool.ChannelSelection(Id) )
        for obj in objs:
            payl = obj.payload()
            #print payl
            RunNumber    = payl[ 'RunNumber' ]
            SORTime      = payl[ 'SORTime' ]
            EORTime      = payl[ 'EORTime' ]
            iovstart     = obj.since()
            iovend       = obj.until()
            nLB          = iovend - ( iovstart + 1 )
            ProjectTag   = payl[ 'FilenameTag' ]
            DetectorMask = payl[ 'DetectorMask' ]
        objs.close()
        break
    RunCtrlDb.closeDatabase()

    ts1 = SORTime/1000000000L
    ts2 = EORTime/1000000000L
    
    #--- Detector mask
    mask=DetectorMask

    dName, NotInAll, vetoedbits = InitDetectorMaskDecoder()

    res=''
    found = False
    for i in range(64):
        if ( mask & (1 << i) ) :
            if "SCT BA" in dName[i] :
                found = True
                res += "SCTBA,"
            if "SCT BC" in dName[i] :
                found = True
                res += "SCTBC,"
            if "SCT EA" in dName[i] :
                found = True
                res += "SCTEA,"
            if "SCT EC" in dName[i] :
                found = True
                res += "SCTEC,"

    if found:
        if len(res)>1:
            res = res[:-1]
        else:
            res += 'NONE'
        
    #print RunNumber, ProjectTag, ts1, ts2, nLB, res
    fw = open( 'runInfo.txt', 'w' )
    fw.write( str(RunNumber) + ' ' + str(ProjectTag) + ' ' + str(ts1) + ' ' + str(ts2) + ' ' + str(nLB) + ' ' + res )
    fw.close()
     
if __name__ == "__main__":
    try:
        runNumber = int( sys.argv[1] )
    except:
        print "run number %s could not be converted to int"%(sys.argv[1])
        sys.exit(-1)
        
    main(runNum = runNumber)
