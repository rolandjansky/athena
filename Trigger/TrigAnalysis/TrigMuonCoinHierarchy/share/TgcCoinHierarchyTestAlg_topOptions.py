# Get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

# Set output level of MessageSvc
MessageSvc.OutputLevel = INFO
MessageSvc.infoLimit = 10000000
MessageSvc.Format = "% F%45W%S%7W%R%T %0W%M"

# Set output level of AthenaEventLoopMgr
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = WARNING

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

### Setup GeoModel starts ### 
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data() # data or geant4

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.TGC_setOn()

# Select geometry version
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-10-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
### Setup GeoModel ends ###

# the POOL converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
### Service
# Trigger::TgcCoinHierarchySvc
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__TgcCoinHierarchySvc
TgcCoinHierarchySvc = Trigger__TgcCoinHierarchySvc("Trigger::TgcCoinHierarchySvc")
TgcCoinHierarchySvc.loopOverContainedROTS = True
ServiceMgr += TgcCoinHierarchySvc

### Tools
from AthenaCommon.AppMgr import ToolSvc
# Trigger::TgcCoinHierarchyClassifyTool
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__TgcCoinHierarchyClassifyTool
TgcCoinHierarchyClassifyTool = Trigger__TgcCoinHierarchyClassifyTool("Trigger::TgcCoinHierarchyClassifyTool")
# TgcCoinHierarchyClassifyTool.tgcCoinDataContainerNamePrior = "TrigT1CoinDataCollectionPriorBC"
# TgcCoinHierarchyClassifyTool.tgcCoinDataContainerNameCurr  = "TrigT1CoinDataCollection"
# TgcCoinHierarchyClassifyTool.tgcCoinDataContainerNameNext  = "TrigT1CoinDataCollectionNextBC"
# TgcCoinHierarchyClassifyTool.tgcPrepDataContainerNamePrior = "TGC_MeasurementsPriorBC"
# TgcCoinHierarchyClassifyTool.tgcPrepDataContainerNameCurr  = "TGC_Measurements"
# TgcCoinHierarchyClassifyTool.tgcPrepDataContainerNameNext  = "TGC_MeasurementsNextBC"
ToolSvc += TgcCoinHierarchyClassifyTool
# Trigger::TgcCoinHierarchyFindTool
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__TgcCoinHierarchyFindTool
TgcCoinHierarchyFindTool = Trigger__TgcCoinHierarchyFindTool("Trigger::TgcCoinHierarchyFindTool")
#TgcCoinHierarchyFindTool.DummyHitChannel = False
ToolSvc += TgcCoinHierarchyFindTool

# TGC cabling service setting (MuonTGC_Cabling will be used with this setting)
from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc
ServiceMgr += TGCcablingServerSvc()
theApp.CreateSvc += [ "TGCcablingServerSvc" ]
ServiceMgr.TGCcablingServerSvc.Atlas=True
ServiceMgr.TGCcablingServerSvc.forcedUse=True
ServiceMgr.TGCcablingServerSvc.useMuonTGC_CablingSvc=True
from TGC_CondCabling.TGC_CondCablingConf import TGCCablingDbTool
ToolSvc += TGCCablingDbTool()
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitMC('TGC','/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA')

print ServiceMgr

### Algorithms 
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Trigger::TgcCoinHierarchyTestAlg
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__TgcCoinHierarchyTestAlg
TgcCoinHierarchyTestAlg = Trigger__TgcCoinHierarchyTestAlg("Trigger::TgcCoinHierarchyTestAlg")
TgcCoinHierarchyTestAlg.OutputLevel = INFO
TgcCoinHierarchyTestAlg.showFraction = True
TgcCoinHierarchyTestAlg.runEvent = False
TgcCoinHierarchyTestAlg.runEventHistos = True
TgcCoinHierarchyTestAlg.runTgcPrepData = False
TgcCoinHierarchyTestAlg.runTgcCoinData = False
TgcCoinHierarchyTestAlg.runMuon = False
TgcCoinHierarchyTestAlg.runTrack = False
job += TgcCoinHierarchyTestAlg


# The input POOL file and container name
ServiceMgr.EventSelector.InputCollections = [
  "/data/maxi91/temp/oda/data10_7TeV.00159179.physics_MuonswBeam.00-06-69/ESD.pool.root"    
]
# Number of events to be skipped
ServiceMgr.EventSelector.SkipEvents = 0 
# Number of events to be processed
theApp.EvtMax = -1
#theApp.EvtMax = 100000

# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "TgcCoinHierarchyTestAlg DATAFILE='TgcCoinHierarchyTestAlg.root' OPT='RECREATE'" ]
