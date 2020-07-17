#-------------------------------------------------------------------------------
SKIPEVENTS = 0
#-------------------------------------------------------------------------------
from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from StoreGate.StoreGateConf import StoreGateSvc
from StoreGate.StoreGateConf import ActiveStoreSvc
from SGComps.SGCompsConf import ProxyProviderSvc
#-------------------------------------------------------------------------------
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
#-------------------------------------------------------------------------------
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaPoolServices.AthenaRootStreamerSvcConf import AthenaRootStreamerSvc
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import AthenaPoolAddressProviderSvc
from AthenaServices.AthenaServicesConf import MetaDataSvc
#-------------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#-------------------------------------------------------------------------------
from AthenaCommon.Logging import logging
logger = logging.getLogger("MergingTest")
#-------------------------------------------------------------------------------

# Needed for muon cabling config / athfile - atleast for the import below
athenaCommonFlags.FilesInput = athenaCommonFlags.PoolRDOInput()

globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)
globalflags.ConditionsTag.set_Value_and_Lock(runArgs.conditionsTag)


#-------------------------------------------------------------------------------
# Shared configuration
#-------------------------------------------------------------------------------
theApp.EventLoop = "TrigSim::MergingEventLoopMgr/MergingEventLoopMgr"

from TrigSimTransforms.TrigSimTransformsConf import TrigSim__MergingEventLoopMgr
svcMgr += TrigSim__MergingEventLoopMgr("MergingEventLoopMgr")
svcMgr.MergingEventLoopMgr.IncidentSvc      = svcMgr.IncidentSvc

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.dcs.all_setOn()
DetFlags.makeRIO.all_setOff()

DetFlags.readRDOPool.all_setOn()
DetFlags.readRIOPool.all_setOn()
DetFlags.readRDOBS.all_setOff()

DetFlags.readRDOPool.LVL1_setOff()
DetFlags.readRIOPool.LVL1_setOff()
DetFlags.readRDOBS.LVL1_setOn()
DetFlags.readRIOBS.LVL1_setOn()

include("RecExCond/AllDet_detDescr.py")
# svcMgr.GeoModelSvc.AutomaticGeometryVersion = True
svcMgr.GeoModelSvc.IgnoreTagDifference = True

import MuonCnvExample.MuonCablingConfig

# PixelLorentzAngleSvc and SCTLorentzAngleSvc (see JIRA ATR-15109/ATEAM-339)
from InDetRecExample.InDetJobProperties import InDetFlags
include("InDetRecExample/InDetRecConditionsAccess.py")

#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# BS conf.
#-------------------------------------------------------------------------------
# Store Gate
svcMgr += StoreGateSvc("BSMetaDataStore")
svcMgr += StoreGateSvc("BSStoreGate")

svcMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")
svcMgr.ByteStreamInputSvc.EventStore        = svcMgr.BSStoreGate
svcMgr.ByteStreamInputSvc.MetaDataStore     = svcMgr.BSMetaDataStore
svcMgr.EventSelector.Input      = athenaCommonFlags.BSRDOInput()

svcMgr += EventSelectorByteStream("BSEventSelector")
svcMgr.BSEventSelector.ByteStreamInputSvc = "ByteStreamInputSvc"
svcMgr.BSEventSelector.SkipEvents = SKIPEVENTS

svcMgr += ByteStreamCnvSvc()
svcMgr.ByteStreamCnvSvc.IsSimulation = True

svcMgr += ROBDataProviderSvc()

svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]


svcMgr += ByteStreamAddressProviderSvc()

from AthenaKernel import StoreID
svcMgr.ByteStreamAddressProviderSvc.StoreID = StoreID.UNKNOWN

"""
# LVL1
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::TriggerTower>/TriggerTowers" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::JEPBSCollection/JEPBSCollection" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::JEPRoIBSCollection/JEPRoIBSCollection" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "LVL1::CPBSCollection/CPBSCollection" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "DataVector<LVL1::CPMRoI>/CPMRoIs" ]

# LVL2
svcMgr.ByteStreamAddressProviderSvc.TypeNames   += [ "HLT::HLTResult/HLTResult_L2" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames   += [ "HLT::HLTResult/HLTResult_EF" ]

svcMgr.ByteStreamAddressProviderSvc.TypeNames   += [ "CTP_RDO/CTP_RDO" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames   += [ "MuCTPI_RDO/MuCTPI_RDO" ]
"""
svcMgr.ByteStreamAddressProviderSvc.TypeNames   += [ "MuCTPI_RDO/MuCTPI_RDO" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames   += \
        ['CTP_RDO/CTP_RDO',
         'DataVector<LVL1::CMMCPHits>/CMMCPHits',
         'DataVector<LVL1::CMMEtSums>/JEMEtSums',
         'DataVector<LVL1::CMMJetHits>/CMMJetHits',
         'DataVector<LVL1::CPMHits>/CPMHits',
         'DataVector<LVL1::CPMRoI>/CPMRoIs',
         'DataVector<LVL1::CPMTower>/CPMTowers',
         'DataVector<LVL1::JEMEtSums>/JEMEtSums',
         'DataVector<LVL1::JEMHits>/JEMHits',
         'DataVector<LVL1::JEMRoI>/JEMRoIs',
         'DataVector<LVL1::JetElement>/JetElements',
         'DataVector<LVL1::TriggerTower>/TriggerTowers',
         'HLT::HLTResult/HLTResult_L2',
         'HLT::HLTResult/HLTResult_EF',
         'LVL1::CMMRoI/CMMRoIs',
#         'LVL1::CPBSCollection/CPBSCollection',
#         'LVL1::JEPBSCollection/JEPBSCollection',
#         'LVL1::JEPRoIBSCollection/JEPRoIBSCollection',
        ]


svcMgr += ProxyProviderSvc()
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]

svcMgr.MergingEventLoopMgr.SecondaryEventStore       = svcMgr.BSStoreGate
svcMgr.MergingEventLoopMgr.SecondaryEventSelector    = "BSEventSelector"
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# RDO conf
#-------------------------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.MergingEventLoopMgr.PrimaryEventStore    = svcMgr.StoreGateSvc
svcMgr.MergingEventLoopMgr.PrimaryEventSelector = "EventSelector"

svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolRDOInput()
svcMgr.PoolSvc.AttemptCatalogPatch = True

#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Setup merging
#-------------------------------------------------------------------------------
svcMgr.MergingEventLoopMgr.OverwritePrimaryTriggerInfo = True
svcMgr.MergingEventLoopMgr.ShareSecondaryProxies = True
#svcMgr.MergingEventLoopMgr.SharePrimaryProxies = True
#-------------------------------------------------------------------------------



#-------------------------------------------------------------------------------
# AthenaOutputStreamTool
#-------------------------------------------------------------------------------
#http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Control/AthenaExamples/AthExThinning/share/WriteSlimmedData_jobOptions.py
#import AthenaPoolCnvSvc.WriteAthenaPool
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_SPLITLEVEL ='0'" ]
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "STREAM_MEMBER_WISE = '1'" ]
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '32000'" ]
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'TTree=POOLContainer'; TREE_AUTO_FLUSH = '0'" ]

from OutputStreamAthenaPool.CreateOutputStreams import AthenaPoolOutputStream
ostream = AthenaPoolOutputStream("StreamRDO", athenaCommonFlags.PoolRDOOutput(), True)

ostream.TakeItemsFromInput = True

ostream.ItemList += [n.replace("/", "#") for n in svcMgr.ByteStreamAddressProviderSvc.TypeNames]



#-------------------------------------------------------------------------------




#-------------------------------------------------------------------------------
# Print some post output post config
#-------------------------------------------------------------------------------
print(svcMgr)

print(topSequence)

print(DetFlags.Print())

svcMgr.MessageSvc.debugLimit = 500000000


svcMgr.InputMetaDataStore.Dump = True

