#
# $Id$
#
# File: share/xAODTestRead3_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading objects with xAOD data.
#          Read output of xAODTestTypelessRead_jo.py.
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

include ('DataModelRunTests/loadReadDicts.py')

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
svcMgr.EventSelector.InputCollections        = [ "xaoddata3.root" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application:
#--------------------------------------------------------------

from AthenaServices.AthenaServicesConf import AthReadAlg
topSequence += AthReadAlg ('cvecCnv',
                           Key = 'DMTest::CVec/cvec')

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestReadCVec
from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__xAODTestReadCInfo, \
     DMTest__xAODTestReadCView, \
     DMTest__xAODTestReadHVec, \
     DMTest__xAODTestRead
topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec")
topSequence += DMTest__xAODTestReadCInfo ("xAODTestReadCInfo")
topSequence += DMTest__xAODTestRead ("xAODTestRead")
topSequence += DMTest__xAODTestReadCView ('xAODTestReadCView')
topSequence += DMTest__xAODTestReadHVec ("xAODTestReadHVec")
topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec_copy",
                                     CVecKey = "copy_cvec")
topSequence += DMTest__xAODTestReadCInfo ("xAODTestReadCInfo_copy",
                                          CInfoKey = "copy_cinfo")
topSequence += DMTest__xAODTestRead ("xAODTestRead_copy",
                                     ReadPrefix = "copy_")
topSequence += DMTest__xAODTestReadCView ("xAODTestReadCView_copy",
                                          CViewKey = "copy_cview")
topSequence += DMTest__xAODTestReadHVec ("xAODTestReadHVec_copy",
                                         HVecKey = "copy_hvec",
                                         HViewKey = "copy_hview")


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

# Avoid races when running tests in parallel.
if 'FILECATALOG' not in globals():
    FILECATALOG = 'xAODTestRead3_catalog.xml'
include ('DataModelRunTests/setCatalog.py')
