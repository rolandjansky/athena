#
# File: share/CondUpdater_jo.py
# Author: Frank Winklmeier

# Date: Aug 2018
# Purpose: Test reading of conditions that are written during runtime
#

#--------------------------------------------------------------
# Create conditions DB
#--------------------------------------------------------------

## Cleanup previous file
import os
if os.path.isfile("condtest_rw.db"):
  os.remove("condtest_rw.db")

## Write some initial IOVs and values
os.system("dmtest_condwriter.py --r=0 --ls=0 --lu=4  'sqlite://;schema=condtest_rw.db;dbname=OFLP200' AttrList_noTag 10")
os.system("dmtest_condwriter.py --rs=0 --ls=5 'sqlite://;schema=condtest_rw.db;dbname=OFLP200' AttrList_noTag 20")

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

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
conddb.addFolder ('condtest_rw.db', '/DMTest/TestAttrList <tag>AttrList_noTag</tag>',
                  className='AthenaAttributeList', extensible=True)

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


from DataModelTestDataCommon.DataModelTestDataCommonConf import (DMTest__CondReaderAlg,
                                                                 DMTest__CondWriterExtAlg,
                                                                 DMTest__CondAlg1)

## Setup writer alg that writes new conditions on given LB
cmds = { 6 : "dmtest_condwriter.py --rs=0 --ls=8  'sqlite://;schema=condtest_rw.db;dbname=OFLP200' AttrList_noTag 42" }

topSequence += DMTest__CondWriterExtAlg( Commands = cmds )
topSequence += DMTest__CondReaderAlg( S2Key = "")

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
condSequence += DMTest__CondAlg1()

svcMgr.MessageSvc.Format = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit  = 0

# Increment LBN every two events.
from McEventSelector import McEventSelectorConf
svcMgr+=McEventSelectorConf.McEventSelector('EventSelector',EventsPerLB=2)

# This is how we currently configure the IOV(Db)Svc in the HLT
svcMgr.IOVSvc.updateInterval = "RUN"
#svcMgr.IOVSvc.preLoadData = True
svcMgr.IOVSvc.forceResetAtBeginRun = False
svcMgr.IOVDbSvc.CacheAlign = 0  # VERY IMPORTANT to get unique queries for folder udpates (see Savannah #81092)
svcMgr.IOVDbSvc.CacheRun = 0
svcMgr.IOVDbSvc.CacheTime = 0


include ('DataModelRunTests/commonTrailer.py')
