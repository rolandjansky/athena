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

include ('DataModelRunTests/loadWriteDicts.py')


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestWriteCVec, \
     DMTest__xAODTestWriteCInfo, \
     DMTest__xAODTestDecor, \
     DMTest__MetaWriterAlg
from DataModelTestDataWrite.DataModelTestDataWriteConf import \
     DMTest__xAODTestWriteHVec, \
     DMTest__xAODTestWriteCView, \
     DMTest__xAODTestWriteCVecConst, \
     DMTest__xAODTestWrite, \
     DMTest__xAODTestWriteSymlinks
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec")
topSequence += DMTest__xAODTestWriteHVec ("xAODTestWriteHVec")
topSequence += DMTest__xAODTestWriteCView ("xAODTestWriteCView")
topSequence += DMTest__xAODTestWriteCInfo ("xAODTestWriteCInfo")
topSequence += DMTest__xAODTestWrite ("xAODTestWrite")
topSequence += DMTest__xAODTestWriteCVecConst ("xAODTestWriteCVecConst")
topSequence += DMTest__xAODTestDecor ("xAODTestDecor")
topSequence += DMTest__xAODTestWriteSymlinks ("xAODTestWriteSymlinks")
topSequence += DMTest__MetaWriterAlg ("MetaWriterAlg")


#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

# ItemList:
include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
fullItemList+=["DMTest::CVec#cvec"]
fullItemList+=["DMTest::CAuxContainer#cvecAux.-dVar2.-dtest"]
fullItemList+=["DMTest::CVecWithData#cvecWD"]
fullItemList+=["DMTest::CView#cview"]
fullItemList+=["DMTest::CAuxContainer#cvecWDAux."]
fullItemList+=["DMTest::GVec#gvec"]
fullItemList+=["DMTest::GAuxContainer#gvecAux."]
fullItemList+=["DMTest::CVec#ctrig"]
fullItemList+=["DMTest::CTrigAuxContainer#ctrigAux."]
fullItemList+=["DMTest::C#cinfo"]
fullItemList+=["DMTest::CInfoAuxContainer#cinfoAux."]
fullItemList+=["DMTest::HVec#hvec"]
fullItemList+=["DMTest::HAuxContainer#hvecAux."]
fullItemList+=["DMTest::HView#hview"]
fullItemList+=["DMTest::S2#S2"]

# Stream's output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
Stream1_Augmented = MSMgr.NewPoolStream ('Stream1', 'xaoddata.root', asAlg=True, noTag=True)
Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
# List of DO's to write out
Stream1.ItemList   += fullItemList
Stream1.MetadataItemList += [ 'DMTest::S1#MetaS1',
                              'DMTest::C#MetaC',
                              'DMTest::CInfoAuxContainer#MetaCAux.' ]
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_SPLITLEVEL='1'"]

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatStreamHelperTool
for tool in Stream1.HelperTools:
    if isinstance(tool, xAODMaker__EventFormatStreamHelperTool):
        tool.TypeNames += [
            'DataVector<DMTest::C_v1>',
            'DMTest::CVecWithData_v1',
            'DMTest::CAuxContainer_v1',
            'DMTest::CTrigAuxContainer_v1',
            'ViewVector<DataVector<DMTest::C_v1,DataModel_detail::NoBase> >',
            'DMTest::C_v1',
            'DMTest::CInfoAuxContainer_v1',
            'DataVector<DMTest::G_v1>',
            'DMTest::GAuxContainer_v1',
            'DMTest::G_v1',
            'DMTest::H_v1',
            'DataVector<DMTest::H_v1>',
            'ViewVector<DataVector<DMTest::H_v1,DataModel_detail::NoBase> >',
            'DMTest::HAuxContainer_v1',
        ]
        break


# Increment LBN every two events.
from McEventSelector import McEventSelectorConf
svcMgr+=McEventSelectorConf.McEventSelector('EventSelector',EventsPerLB=2)


# Avoid races when running tests in parallel.
FILECATALOG = 'xAODTestWrite_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
