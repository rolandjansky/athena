#
# $Id$
#
# File: share/AuxDataTestTypelessRead_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading objects with aux data, w/o compile-time type info.
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
svcMgr.EventSelector.InputCollections        = [ "auxdata.root" ]

#--------------------------------------------------------------
# Define output
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::BAuxVec#bauxvec"]
fullItemList+=["DMTest::BAuxStandalone#b"]
fullItemList+=["DMTest::BAuxVec#copy_bauxvec"]
fullItemList+=["DMTest::BAuxStandalone#copy_b"]

# Stream's output file
from OutputStreamAthenaPool.CreateOutputStreams import createOutputStream
Stream1 = createOutputStream( "Stream1", noTag = True )
Stream1.OutputFile =   "auxdata3.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import DMTest__AuxDataTestTypelessRead
topSequence += DMTest__AuxDataTestTypelessRead ("AuxDataTestTypelessRead",
                                                WritePrefix = "copy_")

# Avoid races when running tests in parallel.
FILECATALOG = 'AuxDataTestTypelessRead_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
