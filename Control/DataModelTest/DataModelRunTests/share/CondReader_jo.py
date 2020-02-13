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

include ('DataModelRunTests/loadReadDicts.py')


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
conddb.addFolder ('condtest.db', '/DMTest/TestAttrList <tag>AttrList_noTag</tag>',
                  className='AthenaAttributeList')
conddb.addFolder ('condtest.db', '/DMTest/S2 <tag>S2_noTag</tag>',
                  className='DMTest::S2')
conddb.addFolder ('condtest.db', '/DMTest/RLTest <tag>RL_noTag</tag>',
                  className='AthenaAttributeList')
conddb.addFolder ('condtest.db', '/DMTest/TSTest <tag>TS_noTag</tag>',
                  className='AthenaAttributeList')


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 30


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------


from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads >= 1:
  from AthenaCommon.AlgScheduler import AlgScheduler
  AlgScheduler.ShowDataDependencies (True)


from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__CondReaderAlg, DMTest__CondAlg1, DMTest__CondAlg2
topSequence += DMTest__CondReaderAlg (RLTestKey = '/DMTest/RLTest',
                                      TSTestKey = '/DMTest/TSTest',
                                      S3Key = 'scond3')


from AthenaCommon.AlgSequence import AthSequencer             
condSequence = AthSequencer("AthCondSeq")             
condSequence += DMTest__CondAlg1()
condSequence += DMTest__CondAlg2()


# Increment LBN every three events, TS each event.
from McEventSelector import McEventSelectorConf
svcMgr+=McEventSelectorConf.McEventSelector('EventSelector',
                                            EventsPerLB=3,
                                            TimeStampInterval=1)

from PoolSvc import PoolSvcConf
PoolSvc = PoolSvcConf.PoolSvc()
PoolSvc.ReadCatalog = ["file:CondWriter_catalog.xml"]

include ('DataModelRunTests/commonTrailer.py')
