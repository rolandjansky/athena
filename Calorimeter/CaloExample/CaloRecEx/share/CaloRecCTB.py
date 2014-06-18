###############################################################
#
# Job options file to run Digitization for CTB Calo only
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DataSource.set_geant4() 
DetFlags.detdescr.all_setOn()
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.makeRIO.Calo_setOn()

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

include( "PartPropSvc/PartPropSvc.py" )

#
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )

#
# Pool input
#
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.EventSelector.InputCollections = [
"ctb_CaloDigit.pool.root"
]

# the Tile, LAr and Calo detector description package
from AtlasGeoModel import SetGeometryVersion

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.AtlasVersion = "ATLAS-CTB-01"

CTBforTTCellMap=True
from AtlasGeoModel import GeoModelInit

#configure LAr to read from OFLP200
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArDBConnection="<dbConnection>COOLONL_LAR/OFLP200</dbConnection>"

include("LArDetDescr/LArDetDescr_H8_joboptions.py")
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='CTB'
DetDescrVersion = "CTB"

# difference is in the IDET tag which is not defined from the jo above and which is found in the input file
ServiceMgr.GeoModelSvc.IgnoreTagDifference = True

# conditions
include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
include( "TileConditions/TileTBConditions_jobOptions.py" )

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')

# calo reco
# produces cells and TB 3x3 clusters

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doEmCluster = False
jobproperties.CaloRecFlags.doCaloTopoCluster = False
jobproperties.CaloRecFlags.doCaloEMTopoCluster = False
jobproperties.CaloRecFlags.doTileMuId = False

from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFit"

include ("CaloRec/CaloRec_jobOptions.py")

from TileL2Algs.TileL2AlgsConf import TileL2Builder
TileL2Builder = TileL2Builder("TileL2Builder")
ToolSvc += TileL2Builder
TileL2Builder.TileRawChannelContainer = "TileRawChannelFit"

from CaloRec.CaloRecConf import CaloClusterMaker
LArTBClusterMaker = CaloClusterMaker("LArTBClusterMaker")
LArTBClusterMaker.ClustersOutputName="LArClusterTBEM"
from LArClusterRec.LArClusterRecConf import LArTBClusterBuilder
theLArTBClusterBuilder = LArTBClusterBuilder("theLArTBClusterBuilder")
theLArTBClusterBuilder.EtaSize=3.
theLArTBClusterBuilder.PhiSize=3.
theLArTBClusterBuilder.CellContainers="AllCalo"
theLArTBClusterBuilder.EnergyThreshold=-1000000.*MeV
theLArTBClusterBuilder.ClusterNbr=1
theLArTBClusterBuilder.LArCaloRegion="BARREL"
ToolSvc += theLArTBClusterBuilder
LArTBClusterMaker.ClusterMakerTools = [ ToolSvc.theLArTBClusterBuilder.getFullName() ]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += LArTBClusterMaker


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      =  INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 20
theApp.EvtSel = "EventSelector"

# Use auditors
theApp.Dlls += [ "GaudiAud" ]
#
# write out a summary of the time spent
#
from GaudiSvc.GaudiSvcConf import AuditorSvc
svcMgr += AuditorSvc()
from GaudiSvc.GaudiSvcConf import ChronoStatSvc
svcMgr += ChronoStatSvc()

from GaudiSvc.GaudiSvcConf import MessageSvc
svcMgr += MessageSvc( defaultLimit = 100000000 )

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream("Stream1")
Stream1.OutputFile = "ctb_CaloESD.pool.root"
Stream1.ForceRead=TRUE
Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["McEventCollection#*"]    
Stream1.ItemList += ["CaloCalibrationHitContainer#*"]

from AthenaCommon.Logging import logging
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )
include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py")

Stream1.ItemList += CaloESDList

theKey = "LArClusterTBEM"
Stream1.ItemList+=["CaloClusterContainer#"+theKey]
Stream1.ItemList+=["CaloCellLinkContainer#"+theKey+"_Link"]
Stream1.ItemList+=["CaloShowerContainer#"+theKey+"_Data"]
