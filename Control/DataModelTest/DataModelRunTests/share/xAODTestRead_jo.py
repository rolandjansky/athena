#
# $Id$
#
# File: share/xAODTestRead_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading xAOD objects data.
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

fullItemList+=["DMTest::CVec_v1#scopy_cvec"]
fullItemList+=["xAOD::ShallowAuxContainer#scopy_cvecAux."]
fullItemList+=["DMTest::CVec_v1#scopy_ctrig"]
fullItemList+=["xAOD::ShallowAuxContainer#scopy_ctrigAux."]
fullItemList+=["DMTest::C_v1#scopy_cinfo"]
fullItemList+=["xAOD::ShallowAuxContainer#scopy_cinfoAux."]
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__xAODTestRead, \
     DMTest__xAODTestDecor, \
     DMTest__xAODTestClearDecor, \
     DMTest__xAODTestShallowCopy


topSequence += DMTest__xAODTestRead ('xAODTestRead',
                                     WritePrefix = 'copy_')

topSequence += DMTest__xAODTestDecor ('AuxDataTestDecor1',
                                      DecorName = 'dInt100',
                                      Offset = 100)

topSequence += DMTest__xAODTestShallowCopy ('xAODTestShallowCopy',
                                            WritePrefix = 'scopy_')

topSequence += DMTest__xAODTestDecor ('AuxDataTestDecor1_scopy',
                                      ReadPrefix = 'scopy_',
                                      DecorName = 'dInt150',
                                      Offset = 300)

# Stream's output file
Stream1 = AthenaPoolOutputStream( "Stream1", asAlg = True )
Stream1.OutputFile =   "xaoddata2.root"
Stream1.ItemList   += fullItemList # List of DO's to write out

topSequence += DMTest__xAODTestClearDecor ('xAODTestClearDecor')
topSequence += DMTest__xAODTestClearDecor ('xAODTestClearDecor_scopy',
                                           ReadPrefix = 'scopy_')

topSequence += DMTest__xAODTestDecor ('xAODTestDecor2',
                                      DecorName = 'dInt200',
                                      Offset = 200)

topSequence += DMTest__xAODTestDecor ('AuxDataTestDecor2_scopy',
                                      ReadPrefix = 'scopy_',
                                      DecorName = 'dInt250',
                                      Offset = 600)
# Stream's output file
Stream2 = AthenaPoolOutputStream( "Stream2", asAlg = True )
Stream2.OutputFile =   "xaoddata2b.root"
Stream2.ItemList   += fullItemList # List of DO's to write out

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

