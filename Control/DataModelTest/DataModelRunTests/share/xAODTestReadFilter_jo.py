#
# $Id$
#
# File: DataModelRuntests/share/xAODTestReadFilter_jo.py
# Author: snyder@bnl.gov
# Date: Sep 2016
# Purpose: Test reading xAOD objects data with renaming on input.
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

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "xaoddata.root" ]

from SGComps import AddressRemappingSvc
AddressRemappingSvc.addInputRename ('DMTest::CVec', 'cvec', 'cvec_renamed')
AddressRemappingSvc.addInputRename ('DMTest::CAuxContainer',
                                    'cvecAux.', 'cvec_renamedAux.')

#--------------------------------------------------------------
# Define output
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::CVec#cvec"]
fullItemList+=["xAOD::AuxContainerBase!#cvecAux."]
fullItemList+=["DMTest::C#cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer#cinfoAux."]

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatSvc
fmtsvc = xAODMaker__EventFormatSvc (FormatNames = 
                                    ['DataVector<DMTest::C_v1>',
                                     'DMTest::CAuxContainer_v1',
                                     'DMTest::C_v1',
                                     ])
ServiceMgr += fmtsvc

ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestWriteCInfo, \
     DMTest__xAODTestDecor
from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__xAODTestFilterCVec


topSequence += DMTest__xAODTestFilterCVec ('xAODTestFilterCVec')
topSequence += DMTest__xAODTestWriteCInfo ('xAODTestWriteCInfo',
                                           Offset = 111)
topSequence += DMTest__xAODTestDecor ('xAODTestDecor',
                                      DoCVec = False,
                                      DoCTrig = False)


# Stream's output file
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata_filt.root',asAlg=True,noTag=True)
Stream1_Augmented.AddMetaDataItem ('xAOD::EventFormat#EventFormat')
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
Stream1.ItemList   += fullItemList # List of DO's to write out


# Note: can't autoload these.
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
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

