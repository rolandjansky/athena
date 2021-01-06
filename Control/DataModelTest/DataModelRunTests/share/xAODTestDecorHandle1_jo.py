#
# $Id$
#
# File: DataModelRunTests/share/xAODTestDecorHandle1_jo.py
# Author: snyder@bnl.gov
# Date: Apr 2017
# Purpose: Test decoration handles and hive.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

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


from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestWriteCVec, \
     DMTest__xAODTestWriteCInfo, \
     DMTest__xAODTestDecor, \
     DMTest__xAODTestReadDecor, \
     DMTest__xAODTestShallowCopy
from DataModelTestDataWrite.DataModelTestDataWriteConf import \
     DMTest__xAODTestWrite
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec")
topSequence += DMTest__xAODTestWriteCInfo ("xAODTestWriteCInfo")
topSequence += DMTest__xAODTestWrite ("xAODTestWrite")
topSequence += DMTest__xAODTestDecor ("xAODTestDecor")
topSequence += DMTest__xAODTestReadDecor ("xAODTestReadDecor")

topSequence += DMTest__xAODTestShallowCopy ("xAODTestShallowCopy",
                                            CVecWDReadKey = '',
                                            CTrigReadKey = '')
topSequence += DMTest__xAODTestReadDecor ("xAODTestReadDecorSCopy",
                                          ReadPrefix = "scopy_")

include ('DataModelRunTests/commonTrailer.py')
