#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from CoolConvUtilities.AtlCoolLib import indirectOpen

class LArRunInfo:
    "Wrapper class to hold LAr run configuration information"
    def __init__(self,nSamples,gainType,latency,firstSample,format,runType):
        self._nSamples = nSamples
        self._gainType = gainType
        self._latency = latency
        self._firstSample = firstSample
        self._format = format
        self._runType = runType

    def nSamples(self):
        "Number of samples readout from FEB"
        return self._nSamples

    def gainType(self):
        "gainType: 0=auto,1=L,2=M,3=H,4=LM,5=LH,6=ML,7=MH,8=HL,9=HM,10=LMH,11=LHM,12=MLH,13=MHL,14=HLM,15=HML"
        return self._gainType

    def latency(self):
        "latency between l1 trigger and readout"
        return self._latency

    def firstSample(self):
        "firstsample"
        return self._firstSample

    def format(self):
        "format:0=Transparent, 1=Format 1, 2=Format 2"
        return self._format

    def runType(self):
        "runType: 0=RawData, 1=RawDataResult, 2=Result"
        return self._runType

    def stringFormat(self):
       if (self._format == 0) :
           return 'transparent'
       if (self._format == 1) :
           return 'Format1'
       if (self._format == 2) :
           return 'Format2'

    def stringRunType(self):
       if (self._runType ==0) :
           return 'RawData'
       if (self._runType ==1) :
           return 'RawDataResult'
       if (self._runType ==2) :
           return 'Result'


def getLArFormatForRun(run,readOracle=True,quiet=False,connstring=None):
    from AthenaCommon.Logging import logging
    mlog_LRF = logging.getLogger( 'getLArRunFormatForRun' )
    if connstring is None:
        from IOVDbSvc.CondDB import conddb
        connstring = "COOLONL_LAR/"+conddb.dbdata
    
    mlog_LRF.info("Connecting to database %s", connstring)
    print("run=",run)
    runDB=indirectOpen(connstring,oracle=readOracle)
    if (runDB is None):
        mlog_LRF.error("Cannot connect to database %s",connstring)
        raise RuntimeError("getLArFormatForRun ERROR: Cannot connect to database %s",connstring)
    format=None
    nSamples=None
    gainType=None
    runType=None
    latency=None
    firstSample=None
    try:
        folder=runDB.getFolder('/LAR/Configuration/RunLog')
        runiov=run << 32
        obj=folder.findObject(runiov,0)
        payload=obj.payload()
        format=payload['format']
        nSamples=ord(payload['nbOfSamples'])
        gainType=payload['gainType'] 
        runType=payload['runType']
        latency=ord(payload['l1aLatency'])
        firstSample=ord(payload['firstSample'])
    except Exception:
        mlog_LRF.warning("No information in /LAR/Configuration/RunLog for run %i", run)
        #mlog_LRF.warning(e)
        return None
    runDB.closeDatabase()
    mlog_LRF.info("Found info for run %d", run)
    return  LArRunInfo(nSamples,gainType,latency,firstSample,format,runType)

# command line driver for convenience
if __name__=='__main__':
    import sys
    if len(sys.argv)!=2:
        print("Syntax",sys.argv[0],'<run>')
        sys.exit(-1)
    run=int(sys.argv[1])
    myformat=getLArFormatForRun(run, connstring="COOLONL_LAR/CONDBR2")
    if (myformat is not None):
      print(" LAr run configuration: Nsamples:%d  GainType:%d  Latency:%d  FirstSample:%d  Format:%s  runType:%s" % (myformat.nSamples(),myformat.gainType(),myformat.latency(),myformat.firstSample(),myformat.stringFormat(),myformat.stringRunType()))
    else:
      print(" LAr run infomation not available")
