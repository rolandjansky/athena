## ---------------------------------------------------------
##                  PhotonAnalysisUtils
## ---------------------------------------------------------

# Author: Marcello Fanti
# Milano, August 2007


MessageSvc.OutputLevel = ERROR
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
MessageSvc.warningLimit = 9999999
MessageSvc.infoLimit = 9999999
MessageSvc.debugLimit = 9999999
MessageSvc.verboseLimit = 9999999

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = WARNING

from AthenaCommon.AppMgr import ServiceMgr

from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags

if PAUflags.DoReRunJetFinding():
	from PhotonAnalysisUtils.LowPtJetFinder import LowPtJetFinder
	mygetter2 = LowPtJetFinder(theJob)


# here if you want to set which process you are analysing...

from PhotonAnalysisUtils.PAUanalysisCodes import PAUanalysisCodes
#PAUanalysis.AnalysisCode = PAUanalysisCodes["gluongluon"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["VBF"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["WHprod"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["ZHprod"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["ttHprod"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["QCDgammagamma"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["QCDgammajet"]
#PAUanalysis.AnalysisCode = PAUanalysisCodes["QCDjetjet"]

##########################################################################################################
#  Configure the extrapolators
##########################################################################################################
from PhotonAnalysisUtils.PAU_AtlasExtrapolator import PAU_AtlasExtrapolator
theAtlasExtrapolator = PAU_AtlasExtrapolator()

from PhotonAnalysisUtils.PAU_ExtrapolateToCaloTool import PAU_ExtrapolateToCaloTool
exToCalo = PAU_ExtrapolateToCaloTool(theAtlasExtrapolator = theAtlasExtrapolator)
exToCalo.OutputLevel = INFO
##########################################################################################################


##########################################################################################################
#  Configure the InDetConversionFinder
##########################################################################################################
from PhotonAnalysisUtils.VertexFitterTool import VertexFitterTool
InDetConversionVxFitterTool = VertexFitterTool(theAtlasExtrapolator)

from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
theAtlasTrackSummaryTool = AtlasTrackSummaryTool("theAtlasTrackSummaryTool")
ToolSvc += theAtlasTrackSummaryTool

# Helper Tool
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
InDetConversionHelper = InDet__ConversionFinderUtils(name = "ConversionFinderUtils")
ToolSvc += InDetConversionHelper


# Distance of minimum approach utility
#
from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
InDetConversionTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name = 'InDetConversionTrkDistanceFinder')
ToolSvc += InDetConversionTrkDistanceFinder 


# Single track conversion tool
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__SingleTrackConversionTool
InDetSingleTrackConversion = InDet__SingleTrackConversionTool(name                       = "SingleTrackConversionTool",
                                                              ConversionFinderHelperTool = InDetConversionHelper,
                                                              #TrackSummaryTool           = InDetTrackSummaryTool,
                                                              TrackSummaryTool           = theAtlasTrackSummaryTool,
                                                              MinInitialHitRadius        = 70.,
                                                              MinRatioOfHLhits           = 0.95)#0.5)
ToolSvc += InDetSingleTrackConversion


from PhotonAnalysisUtils.PAU_InDetConversionFinderTools import PAU_InDetConversionFinderTools
InDetConversionFinderTools = PAU_InDetConversionFinderTools(InDetConversionHelper            = InDetConversionHelper,
							    InDetSingleTrackConversion       = InDetSingleTrackConversion,
							    theAtlasExtrapolator             = theAtlasExtrapolator,
							    theAtlasTrackSummaryTool         = theAtlasTrackSummaryTool,
							    InDetConversionTrkDistanceFinder = InDetConversionTrkDistanceFinder,
							    InDetConversionVxFitterTool      = InDetConversionVxFitterTool)
############################################################################################################


# --------------------------------------------------------------------
# PAU Photon Selector
from PhotonAnalysisUtils.PhotonSelector import PhotonSelector
PAUphotonSelector = PhotonSelector(CaloCellContainerName      = CaloCells,
				   AtlasExtrapolator          = theAtlasExtrapolator,
				   DetailContainerName        = PhotonsDetailsContainerName,
				   exToCalo                   = exToCalo,
				   InDetConversionFinderTools = InDetConversionFinderTools,
				   InDetConversionHelper      = InDetConversionHelper)

PAUphotonSelector.OutputLevel                 = WARNING
#uncomment to access ESD containers: PAUphotonSelector.ContainersAccessMode = "ESD"
#PAUphotonSelector.doRecovery = True
PAUphotonSelector.PhotonContainer             = PhotonContainerMerged
PAUphotonSelector.ElectronContainer           = ElectronContainer
PAUphotonSelector.PhotonDetailContainerName   = PhotonsDetailsContainerName
PAUphotonSelector.ElectronDetailContainerName = ElectronDetailContainer

PAUphotonSelector.UsePAUconversionFlagTool    = usePAUconversionFlagTool
PAUphotonSelector.RemovePhotonWithoutEgDetail = PAUflags.DoReRunEgamma()
PAUphotonSelector.JetCollection               = PAUflags.JetCollectionName()

# --- Photon selection:
#PAUphotonSelector.GammaTypeNames = [ "unconv" ] # default is [ "unconv" , "conv1" , "conv2" ] 
#PAUphotonSelector.PtIsolationCut = ...    # Pt cut in MeV (default is 4000; negative value for not using isolation tool)
#PAUphotonSelector.PhotonIDbyCuts = ...    # photon ID (default==0 use isem(PhotonTight); >0 use G.Unal's itune ; -1 use likelihood)
#PAUphotonSelector.PhotonIDlogLHcut = ...  # set cut for photon ID loglikelihood (default==9.4; works if PhotonIDbyCuts==-1)
# --- Event selection:
#PAUphotonSelector.PtCut1 = ...  # inclusive selection: pt cut (MeV) for leading photon (default is 40 GeV)
#PAUphotonSelector.PtCut2 = ...  # inclusive selection: pt cut (MeV) for sub-leading photon (default is 25 GeV)
# --- printout
#PAUphotonSelector.DumpReco = True    # dump reco e/gammas in file dumpReco.txt (~ 0.8 K/evt)
#PAUphotonSelector.FileEventSummary = "myEventSummary.txt" # comment out to switch off event summary
#PAUphotonSelector.FileEventDetails = "myEventDetails.txt" # comment out to switch off event details
#PAUphotonSelector.DumpTriggerDecision = True    # dump at INFO level, default is False
# --------------------------------------------------------------------


# --------------------------------------------------------------------
# PAU Truth Utilities
from PhotonAnalysisUtils.TruthTools import TruthTools
PAUtruthTool = TruthTools(MCParticleContainer = MCParticleContainer,
			  MCEventCollection   = MCEventCollection,
			  PAUdoESD            = PAUdoESD,
			  TruthJetsCollection = TruthJetsCollection)
PAUtruthTool.OutputLevel = WARNING

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtruthMatchingTool
ToolSvc += PAUtruthMatchingTool()
ToolSvc.PAUtruthMatchingTool.OutputLevel = INFO
# --------------------------------------------------------------------


# --------------------------------------------------------------------
# Other PAU tools

# Topo Cluster Tool
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtopoClusterTool
ToolSvc += PAUtopoClusterTool()
ToolSvc.PAUtopoClusterTool.DeltaRMatch = 0.1
ToolSvc.PAUtopoClusterTool.EtThreshold = 2000
ToolSvc.PAUtopoClusterTool.EtaMax = 4.8


# Hybrid Primary Vertex Tool
from PhotonAnalysisUtils.HybridPrimaryVertex import HybridPrimaryVertex
PAUhybridPrimaryVertexTool = HybridPrimaryVertex(theAtlasExtrapolator        = theAtlasExtrapolator,
						 InDetSingleTrackConversion  = InDetSingleTrackConversion,
						 InDetConversionVxFitterTool = InDetConversionVxFitterTool,
						 exToCalo                    = exToCalo,
						 InDetConversionFinderTools  = InDetConversionFinderTools)
PAUhybridPrimaryVertexTool.OutputLevel = INFO


# H->gamma gamma fitter tool
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhggFitter
ToolSvc += PAUhggFitter()
ToolSvc.PAUhggFitter.ExtrapolationTool=theAtlasExtrapolator


# Calorimeter isolation tool base
from egammaCaloTools.egammaIsoPtCorrectionToolBase import egammaIsoPtCorrectionToolBase
EMCaloIsoPtCorrectionTool = egammaIsoPtCorrectionToolBase("egammaIsoPtCorrection")
ToolSvc += EMCaloIsoPtCorrectionTool

# Calorimeter isolation tool
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUcaloIsolationTool
PAUcaloisolationtool = PAUcaloIsolationTool()
PAUcaloisolationtool.DoAreaCorrections = PAUflags.DoReRunJetFinding()
PAUcaloisolationtool.JetAreaEtaBins = PAUflags.JetAreaEtaBins()
PAUcaloisolationtool.UseOldPtCorrections = PAUflags.UseOldPtCorrections()
PAUcaloisolationtool.UseOldAreaCorrections = True
PAUcaloisolationtool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool
ToolSvc += PAUcaloisolationtool

# Calorimeter isolation tool for truth (hadron-level) isolation
PAUTruthCaloisolationtool = PAUcaloIsolationTool("PAUTruthCaloIsolationTool")
PAUTruthCaloisolationtool.DoAreaCorrections = PAUflags.DoReRunJetFinding()
PAUTruthCaloisolationtool.JetCollectionName = "Kt5TruthJets"
PAUTruthCaloisolationtool.JetAreaEtaBins = PAUflags.JetAreaEtaBins()
PAUTruthCaloisolationtool.UseOldPtCorrections = PAUflags.UseOldPtCorrections()
PAUTruthCaloisolationtool.UseOldAreaCorrections = True
PAUTruthCaloisolationtool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool
ToolSvc += PAUTruthCaloisolationtool

# Calorimeter isolation tool with new ED corrections
PAUNewCaloisolationtool = PAUcaloIsolationTool("PAUNewcaloisolationtool")
PAUNewCaloisolationtool.DoAreaCorrections = PAUflags.DoReRunJetFinding()
PAUNewCaloisolationtool.JetCollectionName = "Kt5TopoJets"
PAUNewCaloisolationtool.JetAreaEtaBins    = PAUflags.JetAreaEtaBins()
PAUNewCaloisolationtool.UseOldPtCorrections = PAUflags.UseOldPtCorrections()
PAUNewCaloisolationtool.UseOldAreaCorrections = False
PAUNewCaloisolationtool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool
PAUNewCaloisolationtool.OutputLevel       = WARNING
ToolSvc += PAUNewCaloisolationtool

# Calorimeter isolation tool for truth (hadron-level) isolation with new ED corrections
PAUNewTruthCaloisolationtool = PAUcaloIsolationTool("PAUNewTruthCaloIsolationTool")
PAUNewTruthCaloisolationtool.DoAreaCorrections = PAUflags.DoReRunJetFinding()
PAUNewTruthCaloisolationtool.JetCollectionName = "Kt5TruthJets"
PAUNewTruthCaloisolationtool.JetAreaEtaBins    = PAUflags.JetAreaEtaBins()
PAUNewTruthCaloisolationtool.UseOldPtCorrections = PAUflags.UseOldPtCorrections()
PAUNewTruthCaloisolationtool.UseOldAreaCorrections = False
PAUNewTruthCaloisolationtool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool
PAUNewTruthCaloisolationtool.OutputLevel       = WARNING
ToolSvc += PAUNewTruthCaloisolationtool

# Trigger/reconstruction matching tool
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUtrigRecoMatch
ToolSvc+= PAUtrigRecoMatch()
ToolSvc.PAUtrigRecoMatch.TrigHLTRecoDeltaR = 0.4;
ToolSvc.PAUtrigRecoMatch.TrigL1RecoDeltaR  = 0.4;

# PAU dumper tool
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUdumper
ToolSvc += PAUdumper()
ToolSvc.PAUdumper.DumpNtuple          = (PAUflags.DumpNtuple() and PAUflags.DumpPAURecoTree())
ToolSvc.PAUdumper.DumpElectronTrigger = PAUflags.DumpElectronTrigger()
ToolSvc.PAUdumper.ElectronToPhoton    = PAUflags.ElectronToPhoton()
ToolSvc.PAUdumper.DumpMETTrigger      = PAUflags.DumpMETTrigger()

# User Data dumper
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhggUserDataTool
ToolSvc += PAUhggUserDataTool()
ToolSvc.PAUhggUserDataTool.DumpNtuple = (PAUflags.DumpNtuple() and PAUflags.DumpHggUserDataTree())

#Load conversion geometry tool (needs VertexPointEstimator)
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
PAUVtxPointEstimator = InDet__VertexPointEstimator(name = "PAUVtxPointEstimator",
						   MinDeltaR = [-5.,-25.,-50.],
						   MaxDeltaR = [5.,10.,10.],
						   MaxPhi    = [0.05, 0.5, 0.5])
ToolSvc += PAUVtxPointEstimator

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUconvGeoTool
ToolSvc += PAUconvGeoTool( name = "PAUconvGeoTool" )
ToolSvc.PAUconvGeoTool.DistanceTool = ToolSvc.InDetConversionTrkDistanceFinder
ToolSvc.PAUconvGeoTool.VertexPointEstimator = ToolSvc.PAUVtxPointEstimator

#Load SecVxHelper to get neutral parameters
from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils
PAUconvParamsUtil = InDet__InDetJetFitterUtils(name = "PAUconvParamsUtil",
					    LinearizedTrackFactory = None,
					    Extrapolator           = None)
ToolSvc += PAUconvParamsUtil

# --------------------------------------------------------------------


# --------------------------------------------------------------------
# Configure ToT tool
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAU_ToT_tool
ToTTool = PAU_ToT_tool(	name = "ToTTool" )
ToolSvc += ToTTool
#ToTTool.OutputLevel = VERBOSE
# --------------------------------------------------------------------


# --------------------------------------------------------------------
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
bunchCrossingTool = BunchCrossingTool()
# --------------------------------------------------------------------


# --------------------------------------------------------------------
# Finally, configure the algorithm
#
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUanalysis
theJob += PAUanalysis()

# Options
theJob.PAUanalysis.OutputLevel               = WARNING
theJob.PAUanalysis.UseTrigger                = PAUflags.UseTrigger()
theJob.PAUanalysis.OnlyTriggerDecision       = False
theJob.PAUanalysis.doElectronTrigger         = PAUflags.DumpElectronTrigger()
theJob.PAUanalysis.doMETTrigger              = PAUflags.DumpMETTrigger()
theJob.PAUanalysis.EnforceTriggerLogicalFlow = PAUflags.EnforceTriggerLogicalFlow()
theJob.PAUanalysis.MuonContainerName         = MuonContainer
theJob.PAUanalysis.CaloCellContainer         = CaloCells
theJob.PAUanalysis.doTrack                   = PAUflags.DumpTracks()
theJob.PAUanalysis.FilteringTracks           = PAUflags.FilteringTracks()
theJob.PAUanalysis.doEtCone                  = PAUflags.DumpAdditionalEtCones()
theJob.PAUanalysis.doPartonCones             = PAUflags.DumpAdditionalPartonEtCones()
theJob.PAUanalysis.doCell                    = PAUflags.DumpCells()
theJob.PAUanalysis.doFilteredCell            = PAUflags.DumpFilteredCells()
theJob.PAUanalysis.doCluster                 = PAUflags.DumpClusters()
theJob.PAUanalysis.doUEDCluster              = PAUflags.DumpUEDClusters()
theJob.PAUanalysis.PAUdoESD                  = PAUdoESD
theJob.PAUanalysis.UseRealData               = PAUflags.UseRealData()
theJob.PAUanalysis.ProductionRelease         = PAUflags.ProductionRelease.StoredValue
theJob.PAUanalysis.DumpHistograms            = PAUflags.DumpHistograms()
theJob.PAUanalysis.DumpTimeDifference        = True
theJob.PAUanalysis.ElectronToPhoton          = PAUflags.ElectronToPhoton()
theJob.PAUanalysis.SkimDecisionContainer     = "StreamESD_SkimDecisionsContainer"
theJob.PAUanalysis.JetAreaJetsToRemove       = PAUflags.JetAreaJetsToRemove()
theJob.PAUanalysis.JetAreaEtaBins            = PAUflags.JetAreaEtaBins()
theJob.PAUanalysis.DoPtMiss                  = PAUflags.DoPtMiss()

# check on the topo cluster container
from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput ('CaloClusterContainer', 'EMTopoCluster430'):
	theJob.PAUanalysis.EMTopoClusterContainer = 'EMTopoCluster430'
elif cfgKeyStore.isInInput ('CaloClusterContainer', 'EMTopoSW35'):
	theJob.PAUanalysis.EMTopoClusterContainer = 'EMTopoSW35'
        
# AlgTools
theJob.PAUanalysis.ExtrapolateToCaloTool     = exToCalo
theJob.PAUanalysis.PAUcaloIsolationTool      = PAUcaloisolationtool
theJob.PAUanalysis.PAUTruthCaloIsolationTool = PAUTruthCaloisolationtool
theJob.PAUanalysis.PAUNewcaloIsolationTool      = PAUNewCaloisolationtool
theJob.PAUanalysis.PAUNewTruthCaloIsolationTool = PAUNewTruthCaloisolationtool
theJob.PAUanalysis.PAUtruthTool              = PAUtruthTool
theJob.PAUanalysis.PAUtruthMatchingTool      = ToolSvc.PAUtruthMatchingTool
theJob.PAUanalysis.PAUhggFitterTool          = ToolSvc.PAUhggFitter
theJob.PAUanalysis.PAUhybridPrimaryVertexTool= PAUhybridPrimaryVertexTool
theJob.PAUanalysis.PAUtopoClusterTool        = ToolSvc.PAUtopoClusterTool
theJob.PAUanalysis.PAUtrigRecoMatchTool      = ToolSvc.PAUtrigRecoMatch
theJob.PAUanalysis.PAUhggUserDataTool        = ToolSvc.PAUhggUserDataTool
theJob.PAUanalysis.BunchCrossingTool         = bunchCrossingTool
theJob.PAUanalysis.ConversionGeometryTool    = PAUconvGeoTool
theJob.PAUanalysis.ConversionUtils           = PAUconvParamsUtil
theJob.PAUanalysis.PAU_ToT_tool              = ToTTool

# theJob.PAUanalysis.DumpTruth                 = 2   # dump truth tree in file dumpTruth.txt (BIG! ~ 200 K/evt)
# theJob.PAUanalysis.UsePAUtruthMatchingTool   = True

# Trigger Signatures
if ( TriggerNames == 'R14' ) :
    theJob.PAUanalysis.PhotonTriggerSignatures = [ "g20" ,
                                                   "g20i" ,
                                                   "2g20" ,
                                                   "g25" ,
                                                   "g105" ,
                                                   "g10" ]
    theJob.PAUanalysis.ElectronTriggerSignatures = [ "e20" ,
                                                     "e20i" ,
                                                     "e25" ,
                                                     "e105" ,
                                                     "e10" ]
    
if ( TriggerNames == 'R15' ) :
    theJob.PAUanalysis.PhotonTriggerSignatures = [ "g3_nocut",   
                                                   "g3_loose",   
                                                   "g5_loose",
						   "g7_loose",   
                                                   "g10_loose",
						   "g11_etcut",
						   "g15_loose",
						   "g15i_loose",
						   "g17_etcut",
						   "g20_loose" ,
						   "g20_tight" ,
						   "g25_loose" ,
						   "g30_loose" ,
						   "g30_tight" ,
						   "g40_loose" ,
						   "g40_tight" ,
						   "g50_loose",
                                                   "em105_passHLT",
                                                   "2g3_nocut" ,
                                                   "2g5_loose" ,
						   "2g7_loose" ,
						   "2g10_nocut" ,
						   "2g10_loose" ,
						   "2g15_loose" ,
                                                   "2g20_loose" ,
						   "g3_NoCut_unpaired_iso" ,
						   "g3_NoCut_unpaired" ,
						   "g5_NoCut_cosmic" 
                                                   ]
    
    theJob.PAUanalysis.ElectronTriggerSignatures = [ "e3_NoCut" , 
                                                     "e3_loose" ,
						     "e3_medium" ,
                                                     "e5_loose" , 
                                                     "e5_medium" , 
                                                     "e10_NoCut" ,
						     "e10_loose" , 
                                                     "e10_medium" ,
                                                     "e10_tight" ,
                                                     "e15_loose" ,
						     "e15_medium" ,
						     "e15_tight" ,
                                                     "e20_loose" ,
						     "e20_medium" ,
                                                     "em105_passHLT",
                                                     "2e3_loose",
						     "2e3_medium",
                                                     "2e3_tight",
						     "2e5_medium"
                                                     ]

    theJob.PAUanalysis.METTriggerSignatures = [ "xe20_noMu" ,
						"xe25_noMu",
						"xe30_noMu",
						"xe35_noMu",
						"xe40_noMu",
						"xe20_tight_noMu",
						"xe25_tight_noMu",
						"xe30_tight_noMu",
						"xe35_tight_noMu",
						"j50_jetNoCut",
						"j75_jetNoCut",
						"j95_jetNoCut",
						"je135_noEF",
						"je195_noEF",
						"je255_noEF",
						"je300_noEF",
						"je350_noEF",
						"j20_jetNoEF",
						"j30_jetNoEF",
						"j35_jetNoEF",
						"j50_jetNoEF",
						"j75_jetNoEF",
						"j95_jetNoEF"
                                               ]
# --------------------------------------------------------------------
