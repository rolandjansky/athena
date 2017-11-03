#
# $Id$
#
# File: share/CondReader_jo.py
# Author: snyder@bnl.gov
# Date: Jul 2017
# Purpose: Test conditions handling.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataWriteCnvPoolCnv.so')


#--------------------------------------------------------------
# Conditions setup.
#--------------------------------------------------------------

from IOVSvc.IOVSvcConf import CondSvc 
svcMgr += CondSvc()

from AthenaCommon.AlgSequence import AthSequencer 
condSeq = AthSequencer("AthCondSeq") 

import StoreGate.StoreGateConf as StoreGateConf 
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore") 

from IOVDbSvc.CondDB import conddb
conddb.addFolder ('condtest.db', '/DMTest/TestAttrList <tag>tag AttrList_noTag</tag>',
                  className='AthenaAttributeList')
conddb.addFolder ('condtest.db', '/DMTest/S2 <tag>tag S2_noTag</tag>',
                  className='DMTest::S2')


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------


from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads >= 1:
  from AthenaCommon.AlgScheduler import AlgScheduler
  AlgScheduler.ShowDataDependencies (True)
  AlgScheduler.setDataLoaderAlg ('SGInputLoader')

  from SGComps.SGCompsConf import SGInputLoader
  topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )


from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__CondReaderAlg, DMTest__CondAlg1
topSequence += DMTest__CondReaderAlg()


from AthenaCommon.AlgSequence import AthSequencer             
condSequence = AthSequencer("AthCondSeq")             
condSequence += DMTest__CondAlg1()


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = 3

# No stats printout
ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoPrintOutTable = FALSE
ChronoStatSvc.PrintUserTime       = FALSE
ChronoStatSvc.StatPrintOutTable   = FALSE

# Increment LBN every two events.
from McEventSelector import McEventSelectorConf
svcMgr+=McEventSelectorConf.McEventSelector('EventSelector',EventsPerLB=2)
