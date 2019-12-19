#
# $Id$
#
# File: DataModelRunTests/share/xAODTestSymlinks2_jo.py
# Author: snyder@bnl.gov
# Date: Apr 2017
# Purpose: Test syminks and hive.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

include ('DataModelRunTests/loadReadDicts.py')


#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "xaoddata.root" ]


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
     DMTest__xAODTestReadSymlink
topSequence += DMTest__xAODTestReadSymlink ("xAODTestReadSymlink", Key='cinfo')


# Avoid races when running tests in parallel.
if 'FILECATALOG' not in globals():
  FILECATALOG = 'xAODTestSymlinks2_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')


