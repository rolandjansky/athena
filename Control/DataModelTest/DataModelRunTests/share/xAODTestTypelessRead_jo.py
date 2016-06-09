
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
fullItemList+=["DMTest::CVec#cvec"]
fullItemList+=["DMTest::CAuxContainer#cvecAux."]
fullItemList+=["DMTest::CVecWithData#cvecWD"]
fullItemList+=["DMTest::CView#cview"]
fullItemList+=["DMTest::CAuxContainer#cvecWDAux."]
fullItemList+=["DMTest::CVec#ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer#ctrigAux."]
fullItemList+=["DMTest::C#cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer#cinfoAux."]
fullItemList+=["DMTest::HVec#hvec"]
fullItemList+=["DMTest::HAuxContainer#hvecAux."]
fullItemList+=["DMTest::HView#hview"]

fullItemList+=["DMTest::CVec#copy_cvec"]
fullItemList+=["DMTest::CAuxContainer#copy_cvecAux."]
fullItemList+=["DMTest::CVecWithData#copy_cvecWD"]
fullItemList+=["DMTest::CView#copy_cview"]
fullItemList+=["DMTest::CAuxContainer#copy_cvecWDAux."]
fullItemList+=["DMTest::CVec#copy_ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer#copy_ctrigAux."]
fullItemList+=["DMTest::C#copy_cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer#copy_cinfoAux."]
fullItemList+=["DMTest::HVec#copy_hvec"]
fullItemList+=["DMTest::HAuxContainer#copy_hvecAux."]
fullItemList+=["DMTest::HView#copy_hview"]

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatSvc
fmtsvc = xAODMaker__EventFormatSvc (FormatNames = 
                                    ['DataVector<DMTest::C_v1>',
                                     'DMTest::CVecWithData_v1',
                                     'DMTest::CAuxContainer_v1',
                                     'DMTest::CTrigAuxContainer_v1',
                                     'DMTest::C_v1',
                                     'DMTest::CInfoAuxContainer_v1'])
ServiceMgr += fmtsvc

ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]

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
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataReadCnvPoolCnv.so')


# Stream's output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata3.root',asAlg=True)
Stream1_Augmented.AddMetaDataItem ('xAOD::EventFormat#EventFormat')
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
# List of DO's to write out
Stream1.ItemList   += fullItemList


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
PoolSvc.WriteCatalog = "file:xAODTestTypelessRead_catalog.xml"
