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
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

include ('DataModelRunTests/loadReadDicts.py')

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from TrigNavigation.TrigNavigationConf import HLT__Navigation
from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestReadCVec
from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__HLTResultReader, \
     DMTest__xAODTestReadCView, \
     DMTest__xAODTestReadHVec


topSequence += DMTest__HLTResultReader \
               ("HLTResultReader",
                Nav = HLT__Navigation (ClassesToPayload = [],
                                       ClassesToPreregister = []))


topSequence += DMTest__xAODTestReadCVec ('xAODTestReadCVec',
                                         CVecKey = 'HLT_DMTest__CVec_cvec')
topSequence += DMTest__xAODTestReadCView ('xAODTestReadCView',
                                          CViewKey = 'HLT_DMTest__CView_cview')
topSequence += DMTest__xAODTestReadHVec ('xAODTestReadHVec',
                                         HVecKey = 'HLT_DMTest__HVec_hvec',
                                         HViewKey = 'HLT_DMTest__HView_hview')
topSequence += DMTest__xAODTestReadCVec ('xAODTestReadCVec2',
                                         CVecKey = 'HLT_DMTest__CVec_cvec2')


from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.BSRDOInput = ['test.bs']
from ByteStreamCnvSvc import ReadByteStream
svcMgr.EventSelector.Input = ['test.bs']
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    'HLT::HLTResult/HLTResult_HLT',
    ]

include ('DataModelRunTests/commonTrailer.py')
