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
DetFlags.detdescr.Muon_setOn()

# Select geometry version
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-08-00-02"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
### Setup GeoModel ends ###

# the POOL converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
### Service
# Trigger::MuonRoiChainSvc
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__MuonRoiChainSvc
MuonRoiChainSvc = Trigger__MuonRoiChainSvc("Trigger::MuonRoiChainSvc")
ServiceMgr += MuonRoiChainSvc

### Tools
from AthenaCommon.AppMgr import ToolSvc
# Trigger::MuonRoiChainFindTool
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__MuonRoiChainFindTool
MuonRoiChainFindTool = Trigger__MuonRoiChainFindTool("Trigger::MuonRoiChainFindTool")
MuonRoiChainFindTool.LVL1_ROIName = "LVL1_ROI"
MuonRoiChainFindTool.MuCTPI_RDOName = "MUCTPI_RDO"
MuonRoiChainFindTool.tgcCoinDataContainerName = "TrigT1CoinDataCollection"
ToolSvc += MuonRoiChainFindTool

### Algorithms 
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Trigger::MuonRoiChainTestAlg
from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__MuonRoiChainTestAlg
MuonRoiChainTestAlg = Trigger__MuonRoiChainTestAlg("Trigger::MuonRoiChainTestAlg")
job += MuonRoiChainTestAlg


# The input POOL file and container name
ServiceMgr.EventSelector.InputCollections = [
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0008._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0108._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0109._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0112._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0113._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0123._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0124._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0135._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0137._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0143._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0144._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0146._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0149._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0155._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0160._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0161._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0163._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0165._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0169._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0173._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0176._0001.1",
    "/data/maxi41/zp/Data/142193/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189/data09_900GeV.00142193.physics_MuonswBeam.recon.ESD.f189._lb0177._0001.1"
    ]
# Number of events to be skipped
ServiceMgr.EventSelector.SkipEvents = 0
# Number of events to be processed
theApp.EvtMax = -1


# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "MuonRoiChainTestAlg DATAFILE='MuonRoiChainTestAlg.root' OPT='RECREATE'" ]
