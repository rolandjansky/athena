#
# $Id$
#
# File: DataModelRunTests/share/xAODTestReadRemap_jo.py
# Author: snyder@bnl.gov
# Date: Aug 2016
# Purpose: Test reading xAOD objects data with remapping on input.
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
svcMgr.EventSelector.InputCollections        = [ "xaoddata_filt.root" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestReadCVec
from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__xAODTestReadCInfo


topSequence += DMTest__xAODTestReadCVec ('xAODTestReadCVec')
topSequence += DMTest__xAODTestReadCInfo ('xAODTestReadCInfo')


# Avoid races when running tests in parallel.
FILECATALOG = 'xAODTestReadFilterRead_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
