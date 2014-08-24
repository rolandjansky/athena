#
# $Id$
#
# File: share/AuxDataTestTypelessRead_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading xAOD objects with aux data, w/o compile-time type info.
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
svcMgr.EventSelector.InputCollections        = [ "xaoddata.root" ]

#--------------------------------------------------------------
# Define output
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::CVec_v1#cvec"]
fullItemList+=["DMTest::CAuxContainer_v1#cvecAux."]
fullItemList+=["DMTest::CVec_v1#ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer_v1#ctrigAux."]
fullItemList+=["DMTest::C_v1#cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer_v1#cinfoAux."]

fullItemList+=["DMTest::CVec_v1#copy_cvec"]
fullItemList+=["DMTest::CAuxContainer_v1#copy_cvecAux."]
fullItemList+=["DMTest::CVec_v1#copy_ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer_v1#copy_ctrigAux."]
fullItemList+=["DMTest::C_v1#copy_cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer_v1#copy_cinfoAux."]

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile =   "xaoddata3.root"
# List of DO's to write out
Stream1.ItemList   += fullItemList

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import DMTest__xAODTestTypelessRead
topSequence += DMTest__xAODTestTypelessRead ("xAODTestTypelessRead",
                                             WritePrefix = "copy_")

# Note: can't autoload these.
theApp.getHandle().Dlls += [ "DataModelTestDataReadCnvPoolCnv" ]
theApp.getHandle().Dlls += [ "DataModelTestDataRead" ]
import PyCintex
PyCintex.loadDictionary("libDataModelTestDataReadDict")
PyCintex.loadDictionary("libDataModelTestDataCommonDict")


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

