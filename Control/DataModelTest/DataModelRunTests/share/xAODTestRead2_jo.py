#
# $Id$
#
# File: share/xAODTestRead2_jo.py
# Author: snyder@bnl.gov
# Date: May 2014
# Purpose: Test reading xAOD objects.
#          Read output of xAODTestRead_jo.py.
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
svcMgr.EventSelector.InputCollections        = [ "xaoddata2.root" ]

#--------------------------------------------------------------
# Define output
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::CVec#cvec"]
fullItemList+=["xAOD::AuxContainerBase#cvecAux."]

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
     DMTest__xAODTestReadCVec
from DataModelTestDataRead.DataModelTestDataReadConf import \
     DMTest__xAODTestReadCInfo, \
     DMTest__xAODTestReadCView, \
     DMTest__xAODTestReadHVec, \
     DMTest__xAODTestRead
topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec")
topSequence += DMTest__xAODTestReadCInfo ("xAODTestCInfo")
topSequence += DMTest__xAODTestRead ("xAODTestRead")
topSequence += DMTest__xAODTestReadCView ('xAODTestReadCView')
topSequence += DMTest__xAODTestReadHVec ("xAODTestReadHVec")
topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec_copy",
                                         CVecKey = "copy_cvec")
topSequence += DMTest__xAODTestReadCInfo ("xAODTestReadCInfo_copy",
                                          CInfoKey = "copy_cinfo")
topSequence += DMTest__xAODTestRead ("xAODTestRead_copy",
                                     CTrigReadKey = 'copy_ctrig',
                                     GVecReadKey = 'copy_gvec',
                                     CVecWDReadKey = 'copy_cvecWD')
topSequence += DMTest__xAODTestReadCView ("xAODTestReadCView_copy",
                                          CViewKey = "copy_cview")
topSequence += DMTest__xAODTestReadHVec ("xAODTestReadHVec_copy",
                                         HVecKey = "copy_hvec",
                                         HViewKey = "copy_hview")
topSequence += DMTest__xAODTestReadCVec ("xAODTestReadCVec_scopy",
                                         CVecKey = "scopy_cvec")
topSequence += DMTest__xAODTestReadCInfo ("xAODTestReadCInfo_scopy",
                                          CInfoKey = "scopy_cinfo")
topSequence += DMTest__xAODTestRead ("xAODTestRead_scopy",
                                     CTrigReadKey = 'scopy_ctrig',
                                     GVecReadKey = '',
                                     CVecWDReadKey = 'scopy_cvecWD')
topSequence += DMTest__xAODTestReadHVec ("xAODTestReadHVec_scopy",
                                         HVecKey = "scopy_hvec",
                                         HViewKey = "")

# Stream's output file
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata2x.root',asAlg=True, noTag=True)
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
Stream1.ItemList   += fullItemList # List of DO's to write out

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatStreamHelperTool
for tool in Stream1.HelperTools:
    if isinstance(tool, xAODMaker__EventFormatStreamHelperTool):
        tool.TypeNames += [
            'DataVector<DMTest::C_v1>',
            'DMTest::CAuxContainer_v1',
            'DMTest::C_v1',
        ]
        break

# Avoid races when running tests in parallel.
FILECATALOG = 'xAODTestRead2_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')

