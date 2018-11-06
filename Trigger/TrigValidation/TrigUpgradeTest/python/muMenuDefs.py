# 
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
# 
#  OutputLevel: INFO < DEBUG < VERBOSE 
#

from AthenaCommon.Include import include
from AthenaCommon.Constants import VERBOSE,DEBUG, INFO
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.DetFlags import DetFlags
import AthenaCommon.CfgMgr as CfgMgr
import AthenaCommon.CfgGetter as CfgGetter


from AthenaCommon.CfgGetter import getPublicTool, getPublicToolClone
from AthenaCommon import CfgMgr


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec = True 
InDetFlags.doPrimaryVertex3DFinding = False 
InDetFlags.doPrintConfigurables = False
InDetFlags.doResolveBackTracks = True 
InDetFlags.doSiSPSeededTrackFinder = True
InDetFlags.doTRTPhaseCalculation = True
InDetFlags.doTRTSeededTrackFinder = True
InDetFlags.doTruth = False
InDetFlags.init()

### PixelLorentzAngleSvc and SCTLorentzAngleSvc ###
include("InDetRecExample/InDetRecConditionsAccess.py")

from InDetRecExample.InDetKeys import InDetKeys
from TriggerJobOpts.TriggerFlags import TriggerFlags
from MuonRecExample.MuonRecFlags import muonRecFlags


# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence

### for Control Flow ###
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

doL2SA  = True
doL2CB  = True
doEFSA  = True
doL2ISO = True

TriggerFlags.doID   = True;
TriggerFlags.doMuon = True;


# ===========================================
#          SET PREPARATOR DATA               
# ===========================================
### Used the algorithms as Step2 "muComb step" ###

from TrigUpgradeTest.InDetSetup import makeInDetAlgs

(viewAlgs, eventAlgs) = makeInDetAlgs()

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
theFTF_Muon = TrigFastTrackFinder_Muon()
theFTF_Muon.OutputLevel = DEBUG
#theFTF_Muon.TracksName = "TrigFastTrackFinder_MuTracks"
theFTF_Muon.isRoI_Seeded = True
viewAlgs.append(theFTF_Muon)

### A simple algorithm to confirm that data has been inherited from parent view ###
### Required to satisfy data dependencies                                       ###
ViewVerify = CfgMgr.AthViews__ViewDataVerifier("muFastViewDataVerifier")
ViewVerify.DataObjects = [('xAOD::L2StandAloneMuonContainer','StoreGateSvc+MuonL2SAInfo')]
viewAlgs.append(ViewVerify)
 

### Used the algorithms as Step1 "muFast step" ###
### Load data from Muon detectors ###
#import MuonRecExample.MuonRecStandaloneOnlySetup
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonRecFlags.doTrackPerformance    = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel   = 5
muonRecFlags.doNSWNewThirdChain    = False
muonCombinedRecFlags.doCaloTrkMuId = False
muonCombinedRecFlags.printSummary = False
from RecExConfig.RecFlags import rec
from AthenaCommon.AlgSequence import AthSequencer
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

### Provide Muon_PrepDataAlgorithms ###
from TrigUpgradeTest.MuonSetup import makeMuonPrepDataAlgs
( eventAlgs_Muon, viewAlgs_MuL2SA, viewAlgs_MuEFSA ) = makeMuonPrepDataAlgs( doL2SA, doEFSA )


### ************* Step1  ************* ###

ServiceMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection = False

### set the EVCreator ###

l2MuViewNode = parOR("l2MuViewNode")

l2MuViewsMaker = EventViewCreatorAlgorithm("l2MuViewsMaker", OutputLevel=DEBUG)
l2MuViewsMaker.ViewFallThrough = True
l2MuViewsMaker.RoIsLink = "initialRoI" # -||-
l2MuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
l2MuViewsMaker.Views = "MUViewRoIs"
l2MuViewsMaker.ViewNodeName = l2MuViewNode.name()

### Define input data of L2MuonSA PRD provider algorithms  ###
### and Define EventViewNodes to run the algprithms        ###
for eventAlg_Muon in eventAlgs_Muon:
  if eventAlg_Muon.properties().has_key("RoIs"):
    eventAlg_Muon.RoIs = l2MuViewsMaker.InViewRoIs

### set up MuFastSteering ###
from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAMTConfig
muFastAlg = TrigL2MuonSAMTConfig("Muon")
muFastAlg.OutputLevel = DEBUG
muFastAlg.MuRoIs = l2MuViewsMaker.InViewRoIs
muFastAlg.RecMuonRoI = "RecMURoIs"
muFastAlg.MuonL2SAInfo = "MuonL2SAInfo"
muFastAlg.MuonCalibrationStream = "MuonCalibrationStream"
muFastAlg.forID = "forID"
muFastAlg.forMS = "forMS"

### To create BS->RDO data                                                                     ###
### viewAlgs_MuL2SA should has                                                                 ###
###      [CSCRdoTool, MDTRdoTool, MDTRawTool, RPCRdoTool, RPCRawTool, TGCRdoTool, TGCRawTool ] ###
### RPCRDO ###
if muonRecFlags.doCSCs():
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__CscDataPreparator
  L2CscDataPreparator = TrigL2MuonSA__CscDataPreparator(OutputLevel = DEBUG,
                                                        CscPrepDataProvider = viewAlgs_MuL2SA[0])
  ToolSvc += L2CscDataPreparator
   
  muFastAlg.DataPreparator.CSCDataPreparator = L2CscDataPreparator

### MDTRDO ###
if muonRecFlags.doMDTs():
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__MdtDataPreparator
  L2MdtDataPreparator = TrigL2MuonSA__MdtDataPreparator(OutputLevel = DEBUG,
                                                        MdtPrepDataProvider = viewAlgs_MuL2SA[1],
                                                        MDT_RawDataProvider = viewAlgs_MuL2SA[2])
  ToolSvc += L2MdtDataPreparator
  
  muFastAlg.DataPreparator.MDTDataPreparator = L2MdtDataPreparator


### RPCRDO ###
if muonRecFlags.doRPCs():
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__RpcDataPreparator
  L2RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator(OutputLevel = DEBUG,
                                                        RpcPrepDataProvider = viewAlgs_MuL2SA[3],
                                                        RpcRawDataProvider = viewAlgs_MuL2SA[4],
                                                        DecodeBS = DetFlags.readRDOBS.RPC_on())
  ToolSvc += L2RpcDataPreparator
   
  muFastAlg.DataPreparator.RPCDataPreparator = L2RpcDataPreparator

### TGCRDO ###
if muonRecFlags.doTGCs():
  from TrigL2MuonSA.TrigL2MuonSAConf import TrigL2MuonSA__TgcDataPreparator
  L2TgcDataPreparator = TrigL2MuonSA__TgcDataPreparator(OutputLevel         = DEBUG,
                                                        TgcPrepDataProvider = viewAlgs_MuL2SA[5])
  ToolSvc += L2TgcDataPreparator
   
  muFastAlg.DataPreparator.TGCDataPreparator = L2TgcDataPreparator

l2MuViewNode += muFastAlg

### set up MuFastHypo ###
from TrigMuonHypo.TrigMuonHypoConf import TrigMufastHypoAlg
trigMufastHypo = TrigMufastHypoAlg("TrigL2MufastHypoAlg")
trigMufastHypo.OutputLevel = DEBUG
trigMufastHypo.MuonL2SAInfoFromMuFastAlg = muFastAlg.MuonL2SAInfo


l2muFastSequence = seqAND("l2muFastSequence", [ l2MuViewsMaker, l2MuViewNode ])


from TrigMuonHypo.testTrigMuonHypoConfig import TrigMufastHypoToolFromName

muFastStep = MenuSequence( Sequence    = l2muFastSequence,
                           Maker       = l2MuViewsMaker,
                           Hypo        = trigMufastHypo,
                           HypoToolGen = TrigMufastHypoToolFromName )


### ************* Step2  ************* ###

### set up step2 ###
l2muCombViewNode = parOR("l2muCombViewNode")

l2muCombViewsMaker = EventViewCreatorAlgorithm("l2muCombViewsMaker", OutputLevel=DEBUG)
l2muCombViewsMaker.ViewFallThrough = True
l2muCombViewsMaker.RoIsLink = "roi" # -||-
l2muCombViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the consumer
l2muCombViewsMaker.Views = "EMCombViewRoIs"
l2muCombViewsMaker.ViewNodeName = l2muCombViewNode.name()

### Define input data of Inner Detector algorithms  ###
### and Define EventViewNodes to run the algprithms ###
TrackParticlesName = ""
for viewAlg in viewAlgs:
  l2muCombViewNode += viewAlg
  if viewAlg.properties().has_key("RoIs"):
    viewAlg.RoIs = l2muCombViewsMaker.InViewRoIs
  if viewAlg.properties().has_key("roiCollectionName"):
    viewAlg.roiCollectionName = l2muCombViewsMaker.InViewRoIs
  if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
    TrackParticlesName = viewAlg.TrackParticlesName
    #viewAlg.TrackName = theFTF_Muon.TracksName

### please read out TrigmuCombMTConfig file ###
### and set up to run muCombMT algorithm    ###
from TrigmuComb.TrigmuCombMTConfig import TrigmuCombMTConfig
muCombAlg = TrigmuCombMTConfig("Muon", theFTF_Muon.getName())
muCombAlg.OutputLevel = DEBUG
muCombAlg.L2StandAloneMuonContainerName = muFastAlg.MuonL2SAInfo
muCombAlg.TrackParticlesContainerName = TrackParticlesName
muCombAlg.L2CombinedMuonContainerName = "MuonL2CBInfo"

l2muCombViewNode += muCombAlg

### set up muCombHypo algorithm ###
from TrigMuonHypo.TrigMuonHypoConf import TrigmuCombHypoAlg
#trigmuCombHypo = TrigmuCombHypoAlg("L2muCombHypoAlg") # avoid to have "Comb" string in the name due to HLTCFConfig.py. 
trigmuCombHypo = TrigmuCombHypoAlg("TrigL2MuCBHypoAlg")
trigmuCombHypo.OutputLevel = DEBUG
trigmuCombHypo.MuonL2CBInfoFromMuCombAlg = muCombAlg.L2CombinedMuonContainerName 

l2muCombSequence = seqAND("l2muCombSequence", eventAlgs + [l2muCombViewsMaker, l2muCombViewNode ] )

from TrigMuonHypo.testTrigMuonHypoConfig import TrigmuCombHypoToolFromName
muCombStep = MenuSequence( Sequence    = l2muCombSequence,
                           Maker       = l2muCombViewsMaker,
                           Hypo        = trigmuCombHypo,
                           HypoToolGen = TrigmuCombHypoToolFromName )


### ************* Step3  ************* ###

### set the EVCreator ###    
efMuViewNode = parOR("efMuViewNode")

efMuViewsMaker = EventViewCreatorAlgorithm("efMuViewsMaker", OutputLevel=DEBUG)
efMuViewsMaker.ViewFallThrough = True
efMuViewsMaker.RoIsLink = "initialRoI" # -||-
efMuViewsMaker.InViewRoIs = "MURoIs" # contract with the consumer
efMuViewsMaker.Views = "EFMUViewRoIs"
efMuViewsMaker.ViewNodeName = efMuViewNode.name()


from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
ServiceMgr += Trk__TrackingVolumesSvc("TrackingVolumesSvc",BuildVolumesFromTagInfo = False)

theSegmentFinder = CfgGetter.getPublicToolClone("MuonSegmentFinder","MooSegmentFinder")
theSegmentFinder.DoSummary=True
CfgGetter.getPublicTool("MuonLayerHoughTool").DoTruth=False
theSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker",
                                                SegmentFinder=theSegmentFinder,
                                                MuonSegmentOutputLocation = "MooreSegments",
                                                UseCSC = muonRecFlags.doCSCs(),
                                                UseMDT = muonRecFlags.doMDTs(),
                                                UseRPC = muonRecFlags.doRPCs(),
                                                UseTGC = muonRecFlags.doTGCs(),
                                                doClusterTruth=False,
                                                UseTGCPriorBC = False,
                                                UseTGCNextBC  = False,
                                                doTGCClust = muonRecFlags.doTGCClusterSegmentFinding(),
                                                doRPCClust = muonRecFlags.doRPCClusterSegmentFinding(), OutputLevel=DEBUG )



theNCBSegmentFinderAlg=CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker_NCB",
                                                   SegmentFinder = getPublicToolClone("MooSegmentFinder_NCB","MuonSegmentFinder",
                                                                                      DoSummary=False,
                                                                                      Csc2dSegmentMaker = getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
                                                                                                                             segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
                                                                                                                                                              "CscSegmentUtilTool",
                                                                                                                                                              TightenChi2 = False, 
                                                                                                                                                              IPconstraint=False)),
                                                                                      Csc4dSegmentMaker = getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
                                                                                                                             segmentTool = getPublicTool("CscSegmentUtilTool_NCB")),
                                                                                      DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False),
                                                   MuonPatternCombinationLocation = "NCB_MuonHoughPatternCombinations", 
                                                   MuonSegmentOutputLocation = "NCB_MuonSegments", 
                                                   MuonSegmentCombinationOutputLocation = "NCB_MooreSegmentCombinations",
                                                   UseCSC = muonRecFlags.doCSCs(),
                                                   UseMDT = False,
                                                   UseRPC = False,
                                                   UseTGC = False,
                                                   UseTGCPriorBC = False,
                                                   UseTGCNextBC  = False,
                                                   doTGCClust = False,
                                                   doRPCClust = False)

from MuonRecExample.MuonStandalone import MuonTrackSteering
MuonTrackSteering.DoSummary=True
MuonTrackSteering.DoSummary=DEBUG
TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder" )
TrackBuilder.TrackSteering=CfgGetter.getPublicToolClone("MuonTrackSteering", "MuonTrackSteering")

from AthenaCommon.Include import include
include("InDetBeamSpotService/BeamCondSvc.py" )        
from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg, xAODMaker__TrackCollectionCnvTool, xAODMaker__RecTrackParticleContainerCnvTool

muonParticleCreatorTool = getPublicTool("MuonParticleCreatorTool")

muonTrackCollectionCnvTool = xAODMaker__TrackCollectionCnvTool( name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muonParticleCreatorTool )

muonRecTrackParticleContainerCnvTool = xAODMaker__RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muonParticleCreatorTool )

xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg", 
                                                          TrackParticleCreator = muonParticleCreatorTool,
                                                          TrackCollectionCnvTool=muonTrackCollectionCnvTool,
                                                          RecTrackParticleContainerCnvTool = muonRecTrackParticleContainerCnvTool,
                                                          TrackContainerName = "MuonSpectrometerTracks",
                                                          xAODTrackParticlesFromTracksContainerName = "MuonSpectrometerTrackParticles",
                                                          ConvertTrackParticles = False,
                                                          ConvertTracks = True)



theCandidateTool = getPublicToolClone("MuonCandidateTool_SA", "MuonCandidateTool", TrackBuilder="",ExtrapolationStrategy=1)
theMuonCandidateAlg=CfgMgr.MuonCombinedMuonCandidateAlg("MuonCandidateAlg",MuonCandidateTool=theCandidateTool)

muonparticlecreator = getPublicToolClone("MuonParticleCreator", "TrackParticleCreatorTool", UseTrackSummaryTool=False, UseMuonSummaryTool=True, KeepAllPerigee=True)
thecreatortool= getPublicToolClone("MuonCreatorTool_SA", "MuonCreatorTool", ScatteringAngleTool="", MuonSelectionTool="", FillTimingInformation=False, UseCaloCells=False, MakeSAMuons=True, MomentumBalanceTool="",  TrackParticleCreator=muonparticlecreator, OutputLevel=DEBUG)

themuoncreatoralg = CfgMgr.MuonCreatorAlg("MuonCreatorAlg", MuonCreatorTool=thecreatortool, CreateSAmuons=True, MakeClusters=False, TagMaps=[])

#Algorithms to views
efMuViewNode += theSegmentFinderAlg
#efMuViewNode += theNCBSegmentFinderAlg #The configuration still needs some sorting out for this so disabled for now.
efMuViewNode += TrackBuilder
efMuViewNode += xAODTrackParticleCnvAlg
efMuViewNode += theMuonCandidateAlg
efMuViewNode += themuoncreatoralg

#Setup MS-only hypo
from TrigMuonHypo.TrigMuonHypoConf import TrigMuonEFMSonlyHypoAlg
trigMuonEFSAHypo = TrigMuonEFMSonlyHypoAlg( "TrigMuonEFSAHypoAlg" )
trigMuonEFSAHypo.OutputLevel = DEBUG
trigMuonEFSAHypo.MuonDecisions = "Muons"

muonEFMSonlySequence = seqAND( "muonEFMSonlySequence", [efMuViewsMaker, efMuViewNode] )

from TrigMuonHypo.testTrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromName

muonEFSAStep = MenuSequence( Sequence    = muonEFMSonlySequence,
                             Maker       = efMuViewsMaker,
                             Hypo        = trigMuonEFSAHypo,
                             HypoToolGen = TrigMuonEFMSonlyHypoToolFromName )


### setup l2Muiso 
l2muIsoViewNode = parOR("l2muIsoViewNode")

l2muIsoViewsMaker = EventViewCreatorAlgorithm("l2muIsoViewsMaker", OutputLevel=DEBUG)
l2muIsoViewsMaker.ViewFallThrough = True
l2muIsoViewsMaker.RoIsLink = "roi" # -||-
l2muIsoViewsMaker.InViewRoIs = "MUIsoRoIs" # contract with the consumer
l2muIsoViewsMaker.Views = "MUIsoViewRoIs"
l2muIsoViewsMaker.ViewNodeName = l2muIsoViewNode.name()

from TrigmuIso.TrigmuIsoConfig import TrigmuIsoMTConfig
trigL2muIso = TrigmuIsoMTConfig("TrigL2muIso")
trigL2muIso.OutputLevel = DEBUG
trigL2muIso.MuonL2CBInfoName = muCombAlg.L2CombinedMuonContainerName
trigL2muIso.TrackParticlesName = TrackParticlesName
trigL2muIso.MuonL2ISInfoName = "MuonL2ISInfo"

l2muIsoViewNode += trigL2muIso

from TrigMuonHypo.TrigMuonHypoConf import TrigMuisoHypoAlg
trigmuIsoHypo = TrigMuisoHypoAlg("L2MuisoHypoAlg")
trigmuIsoHypo.OutputLevel = DEBUG
trigmuIsoHypo.MuonL2ISInfoName = trigL2muIso.MuonL2ISInfoName

from TrigMuonHypo.testTrigMuonHypoConfig import TrigMuisoHypoToolFromName
### Define a Sequence to run for muIso ### 
l2muIsoSequence = seqAND("l2muIsoSequence", [ l2muIsoViewsMaker, l2muIsoViewNode ] )

muIsoStep = MenuSequence( Sequence    = l2muIsoSequence,
                          Maker       = l2muIsoViewsMaker,
                          Hypo        = trigmuIsoHypo,
                          HypoToolGen = TrigMuisoHypoToolFromName )

   
def TMEF_TrkMaterialProviderTool(name='TMEF_TrkMaterialProviderTool',**kwargs):
    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    kwargs.setdefault("UseCaloEnergyMeasurement", False)
    return Trk__TrkMaterialProviderTool(name,**kwargs)
