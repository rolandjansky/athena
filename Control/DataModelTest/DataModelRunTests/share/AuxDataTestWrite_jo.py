#
# $Id$
#
# File: share/AuxDataTestWrite_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test writing objects with aux data.
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
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

include ('DataModelRunTests/loadWriteDicts.py')



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

from DataModelTestDataWrite.DataModelTestDataWriteConf import DMTest__AuxDataTestWrite
topSequence += DMTest__AuxDataTestWrite ("AuxDataTestWrite")

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::BAuxVec#bauxvec"]
fullItemList+=["DMTest::BAuxStandalone#b"]

# Stream's output file
from OutputStreamAthenaPool.CreateOutputStreams import createOutputStream
Stream1 = createOutputStream( "Stream1", noTag = True )
Stream1.OutputFile =   "auxdata.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

# Avoid races when running tests in parallel.
FILECATALOG = 'AuxDataTestWrite_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
