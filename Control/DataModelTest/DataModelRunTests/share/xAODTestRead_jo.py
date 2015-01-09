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
fullItemList+=["DMTest::GVec_v1#gvec"]
fullItemList+=["DMTest::GAuxContainer_v1#gvecAux."]
fullItemList+=["DMTest::CVec_v1#ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer_v1#ctrigAux."]
fullItemList+=["DMTest::C_v1#cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer_v1#cinfoAux."]

fullItemList+=["DMTest::CVec_v1#copy_cvec"]
fullItemList+=["DMTest::CAuxContainer_v1#copy_cvecAux."]
fullItemList+=["DMTest::GVec_v1#copy_gvec"]
fullItemList+=["DMTest::GAuxContainer_v1#copy_gvecAux."]
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

# from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatSvc
# fmtsvc = xAODMaker__EventFormatSvc (FormatNames = 
#                                     ['DataVector<DMTest::C_v1>',
#                                      'DMTest::CAuxContainer_v1',
#                                      'DMTest::CTrigAuxContainer_v1',
#                                      'DMTest::C_v1',
#                                      'DMTest::CInfoAuxContainer_v1'])
# ServiceMgr += fmtsvc

ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


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
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata2.root',asAlg=True)
#Stream1_Augmented.AddMetaDataItem ('xAOD::EventFormat_v1#EventFormat')
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
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
Stream2_Augmented = MSMgr.NewPoolStream ('Stream2','xaoddata2b.root',asAlg=True)
#Stream2_Augmented.AddMetaDataItem ('xAOD::EventFormat_v1#EventFormat')
Stream2 = Stream2_Augmented.GetEventStream()
Stream2.WritingTool.SubLevelBranchName = '<key>'
Stream2.ItemList   += fullItemList # List of DO's to write out

# Note: can't autoload these.
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataReadCnvPoolCnv.so')

theApp.CreateSvc += ['xAODMaker::EventFormatSvc']


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

