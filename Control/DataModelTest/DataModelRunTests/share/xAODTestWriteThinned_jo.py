#
# File: DataModelRunTests/share/xAODTestWriteThinned_jo.py
# Author: snyder@bnl.gov
# Date: Aug 2019
# Purpose: Test thinning xAOD objects.
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

include ('DataModelRunTests/loadWriteDicts.py')


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestThinCVec, DMTest__xAODTestWriteCLinks, \
     DMTest__xAODTestWriteCVec
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec")
topSequence += DMTest__xAODTestWriteCLinks ("xAODTestWriteCLinks")
topSequence += DMTest__xAODTestThinCVec ("xAODTestThinCVec", Stream = 'Thinned1')

# Test combining thinning from two independent algorithms.
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec2",
                                          CVecKey = "cvec2")
topSequence += DMTest__xAODTestThinCVec ("xAODTestThinCVec2a", Stream = 'Thinned1',
                                         CVecKey = "cvec2",
                                         Mask = 5)
topSequence += DMTest__xAODTestThinCVec ("xAODTestThinCVec2b", Stream = 'Thinned1',
                                         CVecKey = "cvec2",
                                         Mask = 6)


#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::CVec#cvec"]
fullItemList+=["DMTest::CAuxContainer#cvecAux.-dVar2.-dtest"]
fullItemList+=["DMTest::CVec#cvec2"]
fullItemList+=["DMTest::CAuxContainer#cvec2Aux.-dVar2.-dtest"]
fullItemList+=["DMTest::CLinks#clinks"]
fullItemList+=["DMTest::CLinksAuxInfo#clinksAux."]
fullItemList+=["DMTest::CLinksContainer#clinksContainer"]
fullItemList+=["DMTest::CLinksAuxContainer#clinksContainerAux."]
fullItemList+=["DMTest::CLinksContainer#clinksContainer2"]
fullItemList+=["DMTest::CLinksAuxContainer#clinksContainer2Aux."]
fullItemList+=["DMTest::CLinksAOD#clinksAOD"]

# Stream's output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
Thinned1_Augmented = MSMgr.NewPoolStream ('Thinned1', 'xaodthinned1.root', asAlg=True, noTag=True)
Thinned1 = Thinned1_Augmented.GetEventStream()
Thinned1.WritingTool.SubLevelBranchName = '<key>'
# List of DO's to write out
Thinned1.ItemList   += fullItemList

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatStreamHelperTool
for tool in Thinned1.HelperTools:
    if isinstance(tool, xAODMaker__EventFormatStreamHelperTool):
        tool.TypeNames += [
            'DataVector<DMTest::C_v1>',
            'DMTest::CAuxContainer_v1',
            'DMTest::C_v1',
            'DMTest::CLinks_v1',
            'DataVector<DMTest::CLinks_v1>',
            'DMTest::CLinksAuxInfo_v1',
            'DMTest::CLinksAuxContainer_v1',
        ]
        break


ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]


# Avoid races when running tests in parallel.
FILECATALOG = 'xAODTestWriteThinned_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')

