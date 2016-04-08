###############################################################
#
# Job options file to read ESD and re-run full calo reconstruction starting 
#  from digits in ESD, thus producing reduced amount of cells and clusters
#  by default OFC iteration reconstruction will be applied, could be configured if need more through jobOptions
#
#==============================================================

# should specify : geometry of input ESD file, input ESD file list, and output ntuple name

Geometry = 'ATLAS-GEONF-08-00-00'

PoolESDInput =  [
"rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data09_calophys/ESD/data09_calophys.00127559.physics_RNDM.recon.ESD.f137/data09_calophys.00127559.physics_RNDM.recon.ESD.f137._lb0000._SFO-1._0001.1"
]

NtupleName = 'ntuple.root'


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

# FIXME  old global flags still use in LArDetDescr
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_pool()

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.makeRIO.Calo_setOn()

include( "PartPropSvc/PartPropSvc.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# the Tile, LAr and Calo detector description package
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
include( "TileConditions/TileConditions_jobOptions.py")

#Pool input
import AthenaPoolCnvSvc.ReadAthenaPool
# EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# Pool input (Change this to use a different file)
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'EventSelector'):
   import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = PoolESDInput

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.readInputFile('RecExPers/OKS_streamESD.py')

objKeyStore['inputFile'].removeItem(["CaloCellContainer#AllCalo"])
objKeyStore['inputFile'].removeItem(["CaloTowerContainer#CombinedTower"])
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#EMTopoCluster430"])
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#EMTopoCluster430_Data"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#EMTopoCluster430_Link"]) 
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#CaloCalTopoCluster"])
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#CaloCalTopoCluster"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#CaloCalTopoCluster"]) 
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#CaloTopoCluster"])
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#CaloTopoCluster"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#CaloTopoCluster"]) 
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEM"])
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEM7_11Nocorr"])
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEMFrwd"])
objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEMSofte"]) 
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEM7_11Nocorr_Data"]) 
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEMSofte_Data"]) 
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEM_Data"]) 
objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEM7_11Nocorr_Data"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEM7_11Nocorr_Link"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEM7_11Nocorr_Link"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEMSofte_Link"]) 
objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEM_Link"]) 

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArThinnedDigits.set_Value_and_Lock(False)

include("CaloRec/CaloRec_jobOptions.py")

topSequence.LArRawChannelBuilder.DataLocation = "LArDigitContainer_Thinned"


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO 
#increase the number of letter reserved to the alg/tool name from 18 to 30
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
svcMgr.MessageSvc.useColors = False
svcMgr.MessageSvc.defaultLimit=1000000

if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += ["AANT DATAFILE='%s' OPT='RECREATE'" % NtupleName]

#---------------------------------------------------------------------
# Perfmon
#--------------------------------------------------------------------

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring =False
#from PerfMonComps.JobOptCfg import PerfMonSvc
#svcMgr += PerfMonSvc()
#jobproperties.PerfMonFlags.doDetailedMonitoring = True


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10000
theApp.EvtSel = "EventSelector"

# CBNT
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream

theAANTupleStream=AANTupleStream(OutputName=NtupleName,ExistDataHeader=False )

include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
include("CBNT_Athena/CBNT_Audit_jobOptions.py")

# CBNT for EM calo cluster
include("CaloRec/CaloCluster_CBNT_jobOptions.py")
# CBNT for CaloTopo cluster
include("CaloRec/CaloTopoCluster_CBNT_jobOptions.py")
# CBNT for EMTopo clustr
include("CaloRec/EMTopoCluster_CBNT_jobOptions.py")

# CBNT for calo cells, with 3 sigma noise cut
from CaloRec.CaloRecConf import CBNTAA_CaloCell
CBNT_LArCell = CBNTAA_CaloCell("CBNT_LArCell" )
CBNT_AthenaAware += CBNT_LArCell 
from CaloIdentifier import SUBCALO
CBNT_LArCell.CaloNums=[SUBCALO.LAREM,SUBCALO.LARHEC,SUBCALO.LARFCAL]
# cell by cell info
CBNT_LArCell.MaxNCells = 200000
CBNT_LArCell.Suffix = "LAr"
CBNT_LArCell.SaveDetInfo = True
CBNT_LArCell.SaveTimeInfo = True
CBNT_LArCell.SaveId = True 
CBNT_LArCell.SaveQualityInfo = True
CBNT_LArCell.CellSigmaCut = 3.
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
from AthenaCommon.AppMgr import ToolSvc
ToolSvc+=LArBadChanTool()
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool
CBNT_LArCell.NoiseTool = theCaloNoiseTool

#CBNT for digits
from LArROD.LArRODConf import CBNTAA_LArDigits
theCBNTAA_LArDigits = CBNTAA_LArDigits("CBNTAA_LArDigits")
theCBNTAA_LArDigits.MaxChannels = 200000
theCBNTAA_LArDigits.NEvents=-1
theCBNTAA_LArDigits.DumpIterResults=True
theCBNTAA_LArDigits.SaveId=True
theCBNTAA_LArDigits.SaveSCAAddress = False
theCBNTAA_LArDigits.DumpCut=0
theCBNTAA_LArDigits.ContainerKey = "LArDigitContainer_Thinned"
CBNT_AthenaAware +=theCBNTAA_LArDigits


topSequence+=theAANTupleStream

