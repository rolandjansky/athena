#
# $Id: DataModelTestRead_jo.py,v 1.9 2009-03-22 16:14:20 ssnyder Exp $
#
# File: share/DataModelTestRead_jo.py
# Author: snyder@bnl.gov
# Date: Nov 2005
# Purpose: Test DataVector backwards compatibility.
#          We read old-style DataVectors as new-style,
#          then write them out again new-style.
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

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "SimplePoolFile.root" ]

#--------------------------------------------------------------
# Define output
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DataVector<DMTest::B>#bvec"]
fullItemList+=["DataVector<DMTest::B>#b3"]
fullItemList+=["DMTest::BDer#bder"]
# The auxdata-related changes in AthContainers expose a bug in root 5.34;
# I/O crashes when trying to write objects of these types.
# This appears to be resolved in root 6.
# For now, just avoid doing this.
#fullItemList+=["DataVector<DMTest::D>#dvec"]
#fullItemList+=["DMTest::DDer#dder"]
fullItemList+=["DMTest::ELVec#elvec"]
fullItemList+=["DMTest::ELVec#elv_remap"]

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile2.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import DMTest__DMTestRead
topSequence += DMTest__DMTestRead ("DMTestRead")

# Note: can't autoload these.
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
getattr(ROOT, 'ElementLinkVector_p1<unsigned int>') #root6 pb workaround
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataReadCnvPoolCnv.so')


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = 3

# No stats printout
ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoPrintOutTable = FALSE
ChronoStatSvc.PrintUserTime       = FALSE
ChronoStatSvc.StatPrintOutTable   = FALSE

#svcMgr.ExceptionSvc.Catch = "None"

# Explicitly specify the output file catalog
# to avoid races when running tests in parallel.
PoolSvc = Service( "PoolSvc" )
PoolSvc.WriteCatalog = "file:DataModelTestRead_catalog.xml"
