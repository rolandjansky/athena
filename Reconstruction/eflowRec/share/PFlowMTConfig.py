from eflowRec.eflowRecConf import PFLeptonSelector
PFLeptonSelector=PFLeptonSelector("PFLeptonSelector")
topSequence += PFLeptonSelector

from eflowRec.eflowRecConf import PFTrackSelector
PFTrackSelector=PFTrackSelector("PFTrackSelector")

from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool
TrackCaloExtensionTool=eflowTrackCaloExtensionTool()

PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
TrackSelectionTool = InDet__InDetTrackSelectionTool()

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += TrackSelectionTool

TrackSelectionTool.CutLevel = "TightPrimary"
TrackSelectionTool.minPt = 500.0 

PFTrackSelector.trackSelectionTool = TrackSelectionTool

topSequence += PFTrackSelector

from eflowRec.eflowRecConf import PFAlgorithm
PFAlgorithm = PFAlgorithm("PFAlgorithm")

from eflowRec.eflowRecConf import PFClusterSelectorTool
PFClusterSelectorTool = PFClusterSelectorTool("PFClusterSelectorTool")

PFAlgorithm.PFClusterSelectorTool = PFClusterSelectorTool

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
PFRecoverSplitShowersTool.PFTrackClusterMatchingTool = MatchingTool_Recover

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


from eflowRec.eflowRecFlags import jobproperties

if jobproperties.eflowRecFlags.useCalibHitTruth:

   PFMomentCalculatorTool.UseCalibHitTruth=True

   from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterMomentsMaker2
   PFCalibClusterMomentsMaker = CaloCalibClusterMomentsMaker2("PFCalibClusterMomentsMaker")
   PFCalibClusterMomentsMaker.MomentsNames = ["ENG_CALIB_TOT"
                                              ,"ENG_CALIB_OUT_L"
                                              ,"ENG_CALIB_EMB0"
                                              ,"ENG_CALIB_EME0"
                                              ,"ENG_CALIB_TILEG3"
                                              ,"ENG_CALIB_DEAD_TOT"
                                              ,"ENG_CALIB_DEAD_EMB0"
                                              ,"ENG_CALIB_DEAD_TILE0"
                                              ,"ENG_CALIB_DEAD_TILEG3"
                                              ,"ENG_CALIB_DEAD_EME0"
                                              ,"ENG_CALIB_DEAD_HEC0"
                                              ,"ENG_CALIB_DEAD_FCAL"
                                              ,"ENG_CALIB_DEAD_LEAKAGE"
                                              ,"ENG_CALIB_DEAD_UNCLASS"
                                              ,"ENG_CALIB_FRAC_EM"
                                              ,"ENG_CALIB_FRAC_HAD"
                                              ,"ENG_CALIB_FRAC_REST"]

   PFCalibClusterMomentsMaker.CalibrationHitContainerNames = ["LArCalibrationHitInactive"
                                                              ,"LArCalibrationHitActive"
                                                              ,"TileCalibHitActiveCell"
                                                              ,"TileCalibHitInactiveCell"]
   PFCalibClusterMomentsMaker.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial"
                                                                ,"TileCalibHitDeadMaterial"]

   PFMomentCalculatorTool.CaloCalibClusterMomentsMaker2=PFCalibClusterMomentsMaker

   
                                           
from eflowRec.eflowRecConf import PFClusterCollectionTool
PFClusterCollectionTool_default = PFClusterCollectionTool("PFClusterCollectionTool")

PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool_default

PFAlgorithm.BaseToolList = [PFMomentCalculatorTool]

from eflowRec.eflowRecConf import PFLCCalibTool
PFLCCalibTool = PFLCCalibTool("PFLCCalibTool")

from eflowRec.PFLocalHadCal import PFLocalHadCal
LocalHadCal = PFLocalHadCal()
Calib = LocalHadCal.PFCaloClusterLocalCalib("PFLCCalibTool")
CalibOO = LocalHadCal.PFCaloClusterLocalCalibOO("PFLCCalibTool")
CalibOOPi0 = LocalHadCal.PFCaloClusterLocalCalibOOPi0("PFLCCalibTool")
CalibDM = LocalHadCal.PFCaloClusterLocalCalibDM("PFLCCalibTool")

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

topSequence += PFOChargedCreatorAlgorithm

from eflowRec.eflowRecConf import PFONeutralCreatorAlgorithm
PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithm("PFONeutralCreatorAlgorithm")
if jobproperties.eflowRecFlags.useCalibHitTruth:
   PFONeutralCreatorAlgorithm.UseCalibHitTruth=True
   
topSequence += PFONeutralCreatorAlgorithm

if jobproperties.eflowRecFlags.usePFEGammaPFOAssoc:

   from eflowRec.eflowRecConf import PFEGammaPFOAssoc
   PFEGammaPFOAssoc=PFEGammaPFOAssoc("PFEGammaPFOAssoc")
   topSequence += PFEGammaPFOAssoc
