#
# $Id: DataModelTestRead2_jo.py,v 1.4 2009-03-22 16:14:20 ssnyder Exp $
#
# File: share/DataModelTestRead2_jo.py
# Author: snyder@bnl.gov
# Date: Nov 2005
# Purpose: Test DataVector backwards compatibility.
#          Read back new-style DataVectors.
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
svcMgr.EventSelector.InputCollections        = [ "SimplePoolFile2.root" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import DMTest__DMTestRead
topSequence += DMTest__DMTestRead ("DMTestRead")


# Avoid races when running tests in parallel.
FILECATALOG = 'DataModelTestRead2_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
