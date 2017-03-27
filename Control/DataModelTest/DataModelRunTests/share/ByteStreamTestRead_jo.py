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

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from TrigNavigation.TrigNavigationConf import HLT__Navigation
from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__HLTResultReader, \
     DMTest__xAODTestReadCView, \
     DMTest__xAODTestReadCVec, \
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


# Note: can't autoload these.
import ROOT
import cppyy
cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
ROOT.DMTest.setConverterLibrary ('libDataModelTestDataReadCnvPoolCnv.so')
ROOT.DMTest.setTrigConverterLibrary ('libDataModelTestDataReadSerCnv.so')

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.BSRDOInput = ['test.bs']
from ByteStreamCnvSvc import ReadByteStream
svcMgr.EventSelector.Input = ['test.bs']
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    'HLT::HLTResult/HLTResult_HLT',
    ]

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

