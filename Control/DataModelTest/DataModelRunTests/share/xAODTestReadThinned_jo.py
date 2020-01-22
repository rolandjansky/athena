#
# File: DataModelRunTests/share/xAODTestReadThinned_jo.py
# Author: snyder@bnl.gov
# Date: Aug 2019
# Purpose: Test thinning xAOD objects.
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

include ('DataModelRunTests/loadWriteDicts.py')

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "xaodthinned1.root" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestReadCVec, \
     DMTest__xAODTestReadCLinks
topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec")
topSequence += DMTest__xAODTestReadCLinks ("xAODTestReadCLinks")

topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec2",
                                         CVecKey = "cvec2",
                                         Brief = True)


# Avoid races when running tests in parallel.
if 'FILECATALOG' not in globals():
    FILECATALOG = 'xAODTestReadThinned_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')

