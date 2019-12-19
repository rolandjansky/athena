#
# $Id$
#
# File: share/AuxDataTestRead3_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading objects with aux data.
#          Read output of AuxDataTestTypelessRead_jo.py.
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
import AthenaPoolCnvSvc.WriteAthenaPool
import AthenaPoolCnvSvc.ReadAthenaPool

include ('DataModelRunTests/loadReadDicts.py')

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "auxdata3.root" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import DMTest__AuxDataTestRead
topSequence += DMTest__AuxDataTestRead ("AuxDataTestRead")
topSequence += DMTest__AuxDataTestRead ("AuxDataTestRead_copy",
                                        ReadPrefix = "copy_")

# Avoid races when running tests in parallel.
FILECATALOG = 'AuxDataTestRead3_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
