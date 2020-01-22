#
# $Id: DataModelTestWrite_jo.py,v 1.7 2009-03-22 16:14:20 ssnyder Exp $
#
# File: share/DataModelTestWrite_jo.py
# Author: snyder@bnl.gov
# Date: Nov 2005
# Purpose: Test writing (old-style) DataVector objects.
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

from DataModelTestDataWrite.DataModelTestDataWriteConf import DMTest__DMTestWrite
topSequence += DMTest__DMTestWrite ("DMTestWrite")

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DataVector<DMTest::B>#bvec"]
fullItemList+=["DataVector<DMTest::B>#b3"]
fullItemList+=["DMTest::BDer#bder"]
fullItemList+=["DataVector<DMTest::D>#dvec"]
fullItemList+=["DMTest::DDer#dder"]
fullItemList+=["DMTest::ELVec#elvec"]
fullItemList+=["DMTest::ELVec#elv_remap"]

# Stream's output file
from OutputStreamAthenaPool.CreateOutputStreams import createOutputStream
Stream1 = createOutputStream( "Stream1", noTag = True )
Stream1.OutputFile =   "SimplePoolFile.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

# Avoid races when running tests in parallel.
FILECATALOG = 'DataModelTestWrite_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
