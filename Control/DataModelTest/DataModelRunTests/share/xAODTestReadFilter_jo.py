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

include ('DataModelRunTests/loadReadDicts.py')

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

from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatStreamHelperTool
for tool in Stream1_Augmented.GetEventStream().HelperTools:
    if isinstance(tool, xAODMaker__EventFormatStreamHelperTool):
        tool.TypeNames += [
            'DataVector<DMTest::C_v1>',
            'DMTest::CAuxContainer_v1',
            'DMTest::C_v1',
        ]
        break



Stream1 = Stream1_Augmented.GetEventStream()
Stream1.WritingTool.SubLevelBranchName = '<key>'
Stream1.ItemList   += fullItemList # List of DO's to write out


# Avoid races when running tests in parallel.
FILECATALOG = 'xAODTestReadFilter_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')

