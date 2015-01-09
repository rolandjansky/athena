#
# $Id$
#
# File: share/xAODTestWrite_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test writing xAOD objects.
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



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

from DataModelTestDataWrite.DataModelTestDataWriteConf import DMTest__xAODTestWrite
topSequence += DMTest__xAODTestWrite ("xAODTestWrite")

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataWriteDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataWriteCnvPoolCnv.so')

theApp.CreateSvc += ['xAODMaker::EventFormatSvc']

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

# from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatSvc
# fmtsvc = xAODMaker__EventFormatSvc (FormatNames = 
#                                     ['DataVector<DMTest::C_v1>',
#                                      'DMTest::CAuxContainer_v1',
#                                      'DMTest::CTrigAuxContainer_v1',
#                                      'DMTest::C_v1',
#                                      'DMTest::CInfoAuxContainer_v1',
#                                      'DataVector<DMTest::G_v1>',
#                                      'DMTest::GAuxContainer_v1',
#                                      'DMTest::G_v1'])
# ServiceMgr += fmtsvc

# Stream's output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata.root', asAlg=True)
#Stream1_Augmented.AddMetaDataItem ('xAOD::EventFormat_v1#EventFormat')
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
# List of DO's to write out
Stream1.ItemList   += fullItemList
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]


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
