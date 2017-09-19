#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Runs calorimeter clustering reconstruction - this part is taken from CaloHiveExOpts.py
# Additional configuration runs particle flow MT algorithms and sets up items needed for track to calorimeter extrapolation

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import MagFieldServices.SetupField

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Muon_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc
include('RecExCond/AllDet_detDescr.py')

#---------------------------------------------------------------------------------#
# MT-specific code
# Get number of processes and threads
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
nProc = jp.ConcurrencyFlags.NumProcs()

if nThreads >=1 :
   from AthenaCommon.AlgScheduler import AlgScheduler
   AlgScheduler.OutputLevel( INFO )
   AlgScheduler.ShowControlFlow( True )
   AlgScheduler.ShowDataDependencies( True )
   AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )

   # Support for the MT-MP hybrid mode
   if (nProc > 0) :

      from AthenaCommon.Logging import log as msg
      if (theApp.EvtMax == -1) : 
         msg.fatal('EvtMax must be >0 for hybrid configuration')
         sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

         if ( theApp.EvtMax % nProc != 0 ) :
            msg.warning('EvtMax[%s] is not divisible by nProcs[%s]: MP Workers will not process all requested events',theApp.EvtMax,nProc)

         chunkSize = int (theApp.EvtMax / nProc)

         from AthenaMP.AthenaMPFlags import jobproperties as jps 
         jps.AthenaMPFlags.ChunkSize= chunkSize
         
         msg.info('AthenaMP workers will process %s events each',chunkSize)

   ## force loading of data. make sure this alg is at the front of the
   ## AlgSequence
   #
   from SGComps.SGCompsConf import SGInputLoader
   topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)

# MT-specific code
#---------------------------------------------------------------------------------#

theApp.EvtMax = 5

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

#---------------------------------------------------------------------------------#
# NEW Conditions access infrastructure
#
from IOVSvc.IOVSvcConf import CondInputLoader
topSequence += CondInputLoader( "CondInputLoader", OutputLevel=DEBUG,  )

import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc()
# NEW Conditions access infrastructure
#---------------------------------------------------------------------------------#

# Make sure PerfMon is off
include( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" )

# Input file
dataFile="/data/hodgkinson/scratchFiles/mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s2832_r7968/ESD.08355655._001904.pool.root.1"
#dataFile="/eos/atlas/atlascerngroupdisk/phys-rig/MC15Samples/ESD/mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s2832_r7968/ESD.08355655._001904.pool.root.1"

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=[dataFile,dataFile]

# AutoConfiguration
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore
from RecExConfig.InputFilePeeker import inputFileSummary
objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])

#---------------------------------------------------------------------------------#
# Detector Description
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
from TileGeoModel.TileGeoModelConf import TileDetectorTool

ServiceMgr.GeoModelSvc.DetectorTools += [ LArDetectorToolNV(ApplyAlignments = True, GeometryConfig = "RECO"),
                                          TileDetectorTool(GeometryConfig = "RECO")
                                          ]


from CaloDetMgrDetDescrCnv import CaloDetMgrDDCnv

include( "TileConditions/TileConditions_jobOptions.py" )

include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from LArConditionsCommon import LArAlignable
ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True
# Detector Description
#---------------------------------------------------------------------------------#

import AthenaPoolCnvSvc.ReadAthenaPool  #Maybe better to break up to get rid of MetaData stuff

svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from GaudiAlg.GaudiAlgConf import EventCounter
topSequence+=EventCounter(Frequency=2)

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits=False

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.clusterCellGetterName = 'CaloRec.CaloCellGetter.CaloCellGetter'

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArDeadOTXCorr=False

include( "CaloRec/CaloTopoCluster_jobOptions.py" )

#PFlow

from eflowRec.eflowRecConf import PFLeptonSelector
PFLeptonSelector=PFLeptonSelector("PFLeptonSelector")
topSequence += PFLeptonSelector

from eflowRec.eflowRecConf import PFTrackSelector
PFTrackSelector=PFTrackSelector("PFTrackSelector")

from eflowRec.eflowRecConf import eflowTrackCaloDummyExtensionTool
TrackCaloExtensionTool=eflowTrackCaloDummyExtensionTool()

PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
TrackSelectionTool = InDet__InDetTrackSelectionTool()

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += TrackSelectionTool

TrackSelectionTool.CutLevel = "TightPrimary"
TrackSelectionTool.minPt = 500.0 

PFTrackSelector.trackSelectionTool = TrackSelectionTool

topSequence += PFTrackSelector

from eflowRec.eflowRecConf import PFClusterSelector
PFClusterSelector=PFClusterSelector("PFClusterSelector")

topSequence += PFClusterSelector

from eflowRec.eflowRecConf import PFAlgorithm
PFAlgorithm = PFAlgorithm("PFAlgorithm")

PFAlgorithm.SubtractionToolList  = []

from eflowRec.eflowRecConf import PFCellLevelSubtractionTool
PFCellLevelSubtractionTool = PFCellLevelSubtractionTool("PFCellLevelSubtractionTool")

from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
CellEOverPTool=eflowCellEOverPTool_mc12_JetETMiss()

PFCellLevelSubtractionTool.eflowCellEOverPTool=CellEOverPTool

from eflowRec.eflowRecFlags import jobproperties
if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
   PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = -1
else:
   PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = 1

from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
MatchingTool = PFTrackClusterMatchingTool("CalObjBldMatchingTool")
MatchingTool_Pull_02 = PFTrackClusterMatchingTool("MatchingTool_Pull_02")
MatchingTool_Pull_015 = PFTrackClusterMatchingTool("MatchingTool_Pull_015")

MatchingTool_Pull_015.TrackPositionType   = 'EM2EtaPhi' # str
MatchingTool_Pull_015.ClusterPositionType = 'PlainEtaPhi' # str
MatchingTool_Pull_015.DistanceType        = 'EtaPhiSquareDistance' # str
MatchingTool_Pull_015.MatchCut = 0.15*0.15 # float
PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_015 = MatchingTool_Pull_015

MatchingTool_Pull_02.TrackPositionType   = 'EM2EtaPhi' # str
MatchingTool_Pull_02.ClusterPositionType = 'PlainEtaPhi' # str
MatchingTool_Pull_02.DistanceType        = 'EtaPhiSquareDistance' # str
MatchingTool_Pull_02.MatchCut = 0.2*0.2 # float
PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = MatchingTool_Pull_02

PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = MatchingTool

if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
   MatchingTool.ClusterPositionType = 'PlainEtaPhi' # str
   MatchingTool.DistanceType        = 'EtaPhiSquareDistance'
   MatchingTool.MatchCut = 0.2*0.2 # float
   
PFAlgorithm.SubtractionToolList += [PFCellLevelSubtractionTool]

from eflowRec.eflowRecConf import PFRecoverSplitShowersTool
PFRecoverSplitShowersTool = PFRecoverSplitShowersTool("PFRecoverSplitShowersTool")

CellEOverPTool_Recover=eflowCellEOverPTool_mc12_JetETMiss("eflowCellEOverPTool_mc12_JetETMiss_Recover")

PFRecoverSplitShowersTool.eflowCellEOverPTool=CellEOverPTool_Recover

if jobproperties.eflowRecFlags.recoverIsolatedTracks == True:
   PFRecoverSplitShowersTool.RecoverIsolatedTracks = True
   
if jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction == False:
   PFRecoverSplitShowersTool.useUpdated2015ChargedShowerSubtraction = False

MatchingTool_Recover = PFTrackClusterMatchingTool()
MatchingTool_Recover.TrackPositionType   = 'EM2EtaPhi' # str
MatchingTool_Recover.ClusterPositionType = 'PlainEtaPhi' # str
MatchingTool_Recover.DistanceType        = 'EtaPhiSquareDistance' # str
MatchingTool_Recover.MatchCut = 0.2*0.2 # float
PFRecoverSplitShowersTool.PFTrackClusterMatchingTool = MatchingTool

PFAlgorithm.SubtractionToolList += [PFRecoverSplitShowersTool]

from eflowRec.eflowRecConf import PFMomentCalculatorTool
PFMomentCalculatorTool = PFMomentCalculatorTool("PFMomentCalculatorTool")

from CaloRec.CaloRecConf import CaloClusterMomentsMaker
PFClusterMomentsMaker = CaloClusterMomentsMaker("PFClusterMomentsMaker")

from CaloRec.CaloTopoClusterFlags import jobproperties

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theCaloNoiseTool

PFClusterMomentsMaker.MaxAxisAngle = 20*deg
PFClusterMomentsMaker.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute() 
PFClusterMomentsMaker.MinBadLArQuality = 4000
PFClusterMomentsMaker.CaloNoiseTool = theCaloNoiseTool
PFClusterMomentsMaker.UsePileUpNoise = True
PFClusterMomentsMaker.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
PFClusterMomentsMaker.OutputLevel = INFO
PFClusterMomentsMaker.MomentsNames = [
   "FIRST_PHI" 
   ,"FIRST_ETA"
   ,"SECOND_R" 
   ,"SECOND_LAMBDA"
   ,"DELTA_PHI"
   ,"DELTA_THETA"
   ,"DELTA_ALPHA" 
   ,"CENTER_X"
   ,"CENTER_Y"
   ,"CENTER_Z"
   ,"CENTER_MAG"
   ,"CENTER_LAMBDA"
   ,"LATERAL"
   ,"LONGITUDINAL"
   ,"FIRST_ENG_DENS" 
   ,"ENG_FRAC_EM" 
   ,"ENG_FRAC_MAX" 
   ,"ENG_FRAC_CORE" 
   ,"FIRST_ENG_DENS" 
   ,"SECOND_ENG_DENS"
   ,"ISOLATION"
   ,"EM_PROBABILITY"
   ,"ENG_POS"
   ,"ENG_BAD_CELLS"
   ,"N_BAD_CELLS"
   ,"BADLARQ_FRAC"
   ,"AVG_LAR_Q"
   ,"AVG_TILE_Q"
   ,"SIGNIFICANCE"
]

PFMomentCalculatorTool.CaloClusterMomentsMaker = PFClusterMomentsMaker

from eflowRec.eflowRecConf import PFClusterCollectionTool
PFClusterCollectionTool_default = PFClusterCollectionTool("PFClusterCollectionTool")

PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool_default

PFAlgorithm.BaseToolList = [PFMomentCalculatorTool]

from eflowRec.eflowRecConf import PFLCCalibTool
PFLCCalibTool = PFLCCalibTool("PFLCCalibTool")

from eflowRec.eflowLocalHadCal import eflowLocalHadCal
LocalHadCal = eflowLocalHadCal()
Calib = LocalHadCal.eflowCaloClusterLocalCalib("PFLCCalibTool")
CalibOO = LocalHadCal.eflowCaloClusterLocalCalibOO("PFLCCalibTool")
CalibOOPi0 = LocalHadCal.eflowCaloClusterLocalCalibOOPi0("PFLCCalibTool")
CalibDM = LocalHadCal.eflowCaloClusterLocalCalibDM("PFLCCalibTool")

from CaloRec.CaloTopoClusterFlags import jobproperties
if not (jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib()):
   #load local hadron calibration database, if not done so by CaloRec already
   from CaloRec import CaloClusterTopoCoolFolder

PFLCCalibTool.CaloClusterLocalCalib=Calib
PFLCCalibTool.CaloClusterLocalCalibOOCC=CalibOO
PFLCCalibTool.CaloClusterLocalCalibOOCCPi0=CalibOOPi0
PFLCCalibTool.CaloClusterLocalCalibDM=CalibDM

PFClusterCollectionTool_LCCalib = PFClusterCollectionTool("PFClusterCollectionTool_LCCalib")
PFLCCalibTool.eflowRecClusterCollectionTool=PFClusterCollectionTool_LCCalib
PFLCCalibTool.UseLocalWeight = False

PFAlgorithm.BaseToolList += [PFLCCalibTool]

topSequence += PFAlgorithm

from eflowRec.eflowRecConf import PFOChargedCreatorAlgorithm
PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithm("PFOChargedCreatorAlgorithm")

from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool        
PFlowTrackVertexAssociationTool = CP__TightTrackVertexAssociationTool(name="PFlowTightCPTool", dzSinTheta_cut=2.0, doPV=True)
PFOChargedCreatorAlgorithm.TrackVertexAssociationTool = PFlowTrackVertexAssociationTool

topSequence += [PFOChargedCreatorAlgorithm]

import AthenaPoolCnvSvc.WriteAthenaPool
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )
from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream

StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py")
StreamESD.ItemList+=CaloESDList

StreamESD.ItemList += [ "xAOD::PFOContainer#JetETMissChargedParticleFlowObjectsV2"]
StreamESD.ItemList += [ "xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsV2Aux."]

print StreamESD.ItemList

#---------------------------------------------------------------------------------#
# MT-specific code
if nThreads >=1 :
   include ( "PFlowHiveDeps.py" )

print "==========================================================================================\n"

#
## set which Algorithms can be cloned
#
# names of algs are:
#  SGInputLoader
#  xAODMaker::EventInfoCnvAlg
#  EventCounter
#  CaloCellMaker
#  CmbTowerBldr
#  CaloClusterMakerSWCmb
#  CaloTopoCluster
#  StreamESD

#  set algCardinality = 1 to disable cloning for all Algs
algCardinality = nThreads

if (algCardinality > 1):   
   for alg in topSequence:      
      name = alg.name()
      if name in ["CaloCellMaker","StreamESD"] :
         # suppress INFO message about Alg unclonability
         alg.Cardinality = 1
      else:
         alg.Cardinality = algCardinality

#Change input and output container names
topSequence.CaloTopoCluster.ClustersOutputName="CaloCalTopoClusterV2"
topSequence.PFClusterSelector.calClustersName="CaloCalTopoClusterV2"
           

