#
# $Id$
#
# File: DataModelRunTests/share/ByteStreamTestWrite_jo.py
# Author: snyder@bnl.gov
# Date: Mar 2016
# Purpose: Test writing objects to bytestream.
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

include ('DataModelRunTests/loadWriteDicts.py')

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

bswrite = ['DMTest::CVec#cvec.-dVar2.-dtest',
           'DMTest::CView#cview',
           'DMTest::HVec#hvec',
           'DMTest::HView#hview',
           'DMTest::CVec#cvec2',
           ]

from TrigNavigation.TrigNavigationConf import HLT__Navigation
from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestWriteCVec, \
     DMTest__xAODTestDecor, \
     DMTest__DummyDecisionWriter
from DataModelTestDataWrite.DataModelTestDataWriteConf import \
     DMTest__xAODTestWriteHVec, \
     DMTest__xAODTestWriteCView, \
     DMTest__HLTResultWriter
topSequence += DMTest__DummyDecisionWriter ("DummyDecisionWriter",
                                            DecisionKey = 'xTrigDecision')
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec",
                                          CVecKey = 'HLT_DMTest__CVec_cvec')
topSequence += DMTest__xAODTestWriteCView ("xAODTestWriteCView",
                                           CVecKey = 'HLT_DMTest__CVec_cvec',
                                           CViewKey = 'HLT_DMTest__CView_cview')
topSequence += DMTest__xAODTestWriteHVec ("xAODTestWriteHVec",
                                          HVecKey = 'HLT_DMTest__HVec_hvec',
                                          HViewKey = 'HLT_DMTest__HView_hview')
topSequence += DMTest__xAODTestDecor ("xAODTestDecor",
                                      ReadPrefix = 'HLT_DMTest__CVec_',
                                      DoCInfo = False,
                                      DoCTrig = False)

# Making sure that no dyn vars are selected by default.
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec2",
                                          CVecKey = 'HLT_DMTest__CVec_cvec2')
topSequence += DMTest__HLTResultWriter \
               ("HLTResultWriter",
                Nav = HLT__Navigation (ClassesToPayload = bswrite,
                                       ClassesToPreregister = bswrite))

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# ItemList:
fullItemList = []
fullItemList+=["HLT::HLTResult#HLTResult_HLT"]

# Stream's output file
import os
try:
    os.remove('test.bs')
except OSError:
    pass
import os
try:
    os.remove('test.bs.writing')
except OSError:
    pass
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.BSRDOOutput = 'test.bs'
from ByteStreamCnvSvc import WriteByteStream
from StoreGate.StoreGateConf import StoreGateSvc
svcMgr += StoreGateSvc('InputMetaDataStore')
svcMgr += StoreGateSvc('MetaDataStore')
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
# List of DO's to write out
StreamBSFileOutput.ItemList   += fullItemList

include ('DataModelRunTests/commonTrailer.py')
