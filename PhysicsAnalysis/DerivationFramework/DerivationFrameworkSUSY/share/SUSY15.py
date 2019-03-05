#********************************************************************
# SUSY15.py
# reductionConf flag SUSY15 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY15Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY15Stream )
SUSY15Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY15Stream.AcceptAlgs(["SUSY15KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY15ThinningHelper = ThinningHelper( "SUSY15ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY15 = CfgMgr.AthSequencer("SeqSUSY15")
DerivationFrameworkJob += SeqSUSY15


#====================================================================
# Vertex dissolving (variation of VrtSecInclusive)
#====================================================================
from VrtSecInclusive.TrackRandomizer import TrackRandomizer
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive

def setupVSI( vsiInstance ):
    vsiInstance.OutputLevel                            = INFO
    vsiInstance.do_PVvetoCut                           = True
    vsiInstance.do_d0Cut                               = False
    vsiInstance.do_z0Cut                               = False
    vsiInstance.do_d0errCut                            = False
    vsiInstance.do_z0errCut                            = False
    vsiInstance.do_d0signifCut                         = False
    vsiInstance.do_z0signifCut                         = False
    vsiInstance.doTRTPixCut                            = True
    vsiInstance.DoSAloneTRT                            = False
    vsiInstance.ImpactWrtBL                            = True
    vsiInstance.doPVcompatibilityCut                   = False
    vsiInstance.RemoveFake2TrkVrt                      = True
    vsiInstance.CheckHitPatternStrategy                = 'ExtrapolationAssist' # Either 'Classical', 'Extrapolation' or 'ExtrapolationAssist'
    vsiInstance.doReassembleVertices                   = True
    vsiInstance.doMergeByShuffling                     = True
    vsiInstance.doMergeFinalVerticesDistance           = True
    vsiInstance.doAssociateNonSelectedTracks           = True
    vsiInstance.doFinalImproveChi2                     = False
    vsiInstance.DoTruth                                = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
    vsiInstance.FillHist                               = True
    vsiInstance.FillIntermediateVertices               = False
    vsiInstance.CutPixelHits                           = 0
    vsiInstance.CutSctHits                             = 2
    vsiInstance.TrkA0ErrCut                            = 200000
    vsiInstance.TrkZErrCut                             = 200000
    vsiInstance.a0TrkPVDstMinCut                       = 2.0    # track d0 min
    vsiInstance.a0TrkPVDstMaxCut                       = 300.0  # track d0 max: default is 1000.0
    vsiInstance.zTrkPVDstMinCut                        = 0.0    # track z0 min: default is 0.0, just for clarification
    vsiInstance.zTrkPVDstMaxCut                        = 1500.0 # track z0 max: default is 1000.0
    vsiInstance.twoTrkVtxFormingD0Cut                  = 2.0
    vsiInstance.TrkPtCut                               = 1000
    vsiInstance.SelVrtChi2Cut                          = 5.
    vsiInstance.CutSharedHits                          = 2
    vsiInstance.TrkChi2Cut                             = 50
    vsiInstance.TruthTrkLen                            = 1
    vsiInstance.SelTrkMaxCutoff                        = 2000
    vsiInstance.mergeByShufflingAllowance              = 10.
    vsiInstance.associatePtCut                         = 1000.
    vsiInstance.associateMinDistanceToPV               = 2.
    vsiInstance.associateMaxD0Signif                   = 5.
    vsiInstance.associateMaxZ0Signif                   = 5.
    vsiInstance.MergeFinalVerticesDist                 = 1.
    vsiInstance.MergeFinalVerticesScaling              = 0.
    vsiInstance.improveChi2ProbThreshold               = 0.0001
    vsiInstance.doAugmentDVimpactParametersToMuons     = False
    vsiInstance.doAugmentDVimpactParametersToElectrons = False
    return

# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              AtlasMagFieldSvc    = "AtlasFieldSvc"
                                             )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO

TrackRandomizingSuffices = [ "0p5", "1p0", "2p0", "3p0", "4p0" ]
RandomizingSigmas        = [ 0.5, 1.0, 2.0, 3.0, 4.0 ]

# Temporary flag

doDissolvedVertexing = False

#------------------------------------------------------------------------------
if doDissolvedVertexing:

  for suffix, sigma in zip( TrackRandomizingSuffices, RandomizingSigmas ):
    randomizer = TrackRandomizer("TrackRandomizer_" + suffix)
    vsi        = VrtSecInclusive("VrtSecInclusive_Random_" + suffix)

    randomizer.outputContainerName = suffix
    randomizer.shuffleStrength = sigma

    setupVSI( vsi )
    vsi.TrackLocation           = "InDetTrackParticlesRandomized" + suffix
    vsi.AugmentingVersionString = "_Randomized" + suffix
    vsi.VertexFitterTool        = InclusiveVxFitterTool
    vsi.Extrapolator            = ToolSvc.AtlasExtrapolator

    SeqSUSY15 += randomizer
    SeqSUSY15 += vsi

  MSMgr.GetStream("StreamDAOD_SUSY15").AddItem( [ 'xAOD::TrackParticleContainer#InDetTrackParticles*',
                                                  'xAOD::TrackParticleAuxContainer#InDetTrackParticles*',
                                                  'xAOD::VertexContainer#VrtSecInclusive*',
                                                  'xAOD::VertexAuxContainer#VrtSecInclusive*'] )
  print "List of items for the DAOD_RPVLL output stream:"
  print MSMgr.GetStream("StreamDAOD_SUSY15").GetItems()

# end of vertex dissolving
#------------------------------------------------------------------------------


from DerivationFrameworkSUSY.SUSY15TriggerList import triggers_met, triggers_jet, triggers_lep, triggers_photon

triggers = triggers_met + triggers_jet + triggers_lep + triggers_photon

#SUSY15ThinningHelper.TriggerChains = '(' + ' | '.join(triggers) + ')' #triggerRegEx

# for now adding an unconfigured thinning helper (?) /CO
SUSY15ThinningHelper.AppendToStream( SUSY15Stream )

### Thinning for now taken from SUSY 2 with minor modifications as indicated /CO

#====================================================================
# THINNING TOOLS
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
SUSY15TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY15TPThinningTool",
                                                                 ThinningService         = SUSY15ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "(InDetTrackParticles.pt > 1*GeV) && ((InDetTrackParticles.numberOfPixelHits + InDetTrackParticles.numberOfSCTHits) > 1)",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15TPThinningTool
thinningTools.append(SUSY15TPThinningTool)


# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY15MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "SUSY15MuonTPThinningTool",
                                                                         ThinningService         = SUSY15ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15MuonTPThinningTool
thinningTools.append(SUSY15MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY15ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                 	= "SUSY15ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY15ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 BestMatchOnly          = False,
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY15ElectronTPThinningTool
thinningTools.append(SUSY15ElectronTPThinningTool)
# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY15PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY15PhotonTPThinningTool",
										 ThinningService	 = SUSY15ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15PhotonTPThinningTool
thinningTools.append(SUSY15PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY15TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name           = "SUSY15TauTPThinningTool",
                                                                        ThinningService = SUSY15ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY15TauTPThinningTool
thinningTools.append(SUSY15TauTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY15AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                   = "SUSY15AKt4JetTPThinningTool",
                                                                            ThinningService         = SUSY15ThinningHelper.ThinningSvc(),
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            SelectionString         = "AntiKt4EMTopoJets.pt > 20*GeV && abs(AntiKt4EMTopoJets.eta) < 2.1",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15AKt4JetTPThinningTool
thinningTools.append(SUSY15AKt4JetTPThinningTool)


#====================================================================
# THINNING FOR RANDOMIZED TRACKS
#====================================================================
# Set up your thinning tools (you can have as many as you need).
# Note how the thinning service (which must be passed to the tools) is accessed


if doDissolvedVertexing:

  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__VsiTrackThinningTool

  #-----
  SUSY15ThinningTools = []
  for suffix in TrackRandomizingSuffices:
    thinningTool = DerivationFramework__VsiTrackThinningTool( name = ("VsiTrackThinningTool" + suffix),
                                                              ThinningService = SUSY15ThinningHelper.ThinningSvc(),
                                                              TrackContainerName = ("InDetTrackParticlesRandomized"+suffix),
                                                              VertexContainerName = ("VrtSecInclusive_SecondaryVertices_Randomized"+suffix) )
    ToolSvc += thinningTool
    thinningTools.append( thinningTool )
  #-----


  from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
  DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SUSY15Kernel",
                                                                         ThinningTools = SUSY15ThinningTools )


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY15TruthThinningTool = DerivationFramework__MenuTruthThinning(name             = "SUSY15TruthThinningTool",
                                                       ThinningService              = SUSY15ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = True,
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = True,
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = True,
                                                       WriteBosons                  = True,
                                                       WriteBosonProducts           = True,
                                                       WriteBSMProducts             = True,
                                                       WriteTopAndDecays            = True,
                                                       WriteEverything              = True,
                                                       WriteAllLeptons              = True,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY15TruthThinningTool
  thinningTools.append(SUSY15TruthThinningTool)

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

  # --- Decorate Truth Particles with some the track parameters:
  # --- The decorator tool (needed by the decorator)
  from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValTruthDecoratorTool
  IDPV_TruthDecoratorTool = InDetPhysValTruthDecoratorTool()
  ToolSvc += IDPV_TruthDecoratorTool
  # --- and the augmentation tool
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles as TrkParam4TruthPart
  TrkParam4Truth = TrkParam4TruthPart(
                                      name="TrkParam4Truth",
                                      OnlyDressPrimaryTracks = False
                                     )

  # WARNING !!! this crashes on R21: ERROR SG::ExcStoreLocked: Attempted to modify auxiliary data in a locked store: `::d0'
  # commented out until the problem is fixed
  #ToolSvc += TrkParam4Truth
  #AugmentationTools.append(TrkParam4Truth)


  from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__LongLivedTruthJetKinematics
  TruthJetKinematicsToolRHadron = DerivationFramework__LongLivedTruthJetKinematics(name = "LongLivedTruthJetsRHadron",
    OutputContainer = "AntiKt4LLP_RHadronTruthJets",
    LLPType = 1,
  )
  ToolSvc += TruthJetKinematicsToolRHadron
  AugmentationTools.append(TruthJetKinematicsToolRHadron)

  TruthJetKinematicsToolChargino = DerivationFramework__LongLivedTruthJetKinematics(name = "LongLivedTruthJetsChargino",
    OutputContainer = "AntiKt4LLP_CharginoTruthJets",
    LLP_PDGIDS = [1000024],
  )
  ToolSvc += TruthJetKinematicsToolChargino
  AugmentationTools.append(TruthJetKinematicsToolChargino)

  TruthJetKinematicsToolNeutralino = DerivationFramework__LongLivedTruthJetKinematics(name = "LongLivedTruthJetsNeutralino",
    OutputContainer = "AntiKt4LLP_NeutralinoTruthJets",
    LLP_PDGIDS = [1000022],
  )
  ToolSvc += TruthJetKinematicsToolNeutralino
  AugmentationTools.append(TruthJetKinematicsToolNeutralino)

  TruthJetKinematicsToolGluino = DerivationFramework__LongLivedTruthJetKinematics(name = "LongLivedTruthJetsGluino",
    OutputContainer = "AntiKt4LLP_GluinoTruthJets",
    LLP_PDGIDS = [1000021],
  )
  ToolSvc += TruthJetKinematicsToolGluino
  AugmentationTools.append(TruthJetKinematicsToolGluino)

  TruthJetKinematicsToolSquark = DerivationFramework__LongLivedTruthJetKinematics(name = "LongLivedTruthJetsSquark",
    OutputContainer = "AntiKt4LLP_SquarkTruthJets",
    LLP_PDGIDS = [1000001, 1000002, 1000003, 1000004, 1000005, 1000006, 2000001, 2000002, 2000003, 2000004, 2000005, 2000006],
  )
  ToolSvc += TruthJetKinematicsToolSquark
  AugmentationTools.append(TruthJetKinematicsToolSquark)



#=============================================================================================
# SKIMMING - skimming on triggers listed in python/SUSY15TriggerList.py
#=============================================================================================

expression_trigger = "(" + " || ".join(triggers) + ")"

expression = expression_trigger

print "expression = '" + expression + "'"
print

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY15SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY15SkimmingTool", expression = expression)

ToolSvc += SUSY15SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():

   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqSUSY15 += CfgMgr.DerivationFramework__DerivationKernel("SUSY15KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY15")
                                                            )

   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY15 += CfgMgr.DerivationFramework__DerivationKernel(
        "SUSY15KernelSkim",
        SkimmingTools = [SUSY15SkimmingTool],
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY15)

#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY15"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY15, "SUSY15")


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY15 += CfgMgr.DerivationFramework__DerivationKernel(
        "SUSY15KernelAug",
        AugmentationTools = AugmentationTools,
        ThinningTools = thinningTools,
)


#==============================================================================
# VrtSecInclusive IP Augmentor
#==============================================================================

from VrtSecInclusive.IPAugmentor import IPAugmentor

IPAugmentor = IPAugmentor("VsiIPAugmentor")
IPAugmentor.doAugmentDVimpactParametersToMuons     = True
IPAugmentor.doAugmentDVimpactParametersToElectrons = True
IPAugmentor.VertexFitterTool=InclusiveVxFitterTool

SeqSUSY15 += IPAugmentor


#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY15SlimmingHelper = SlimmingHelper("SUSY15SlimmingHelper")
SUSY15SlimmingHelper.SmartCollections = [
                                         "Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "MET_Reference_AntiKt4EMPFlow",
                                         "AntiKt4EMTopoJets",
                                         "AntiKt4EMPFlowJets",
                                         "BTagging_AntiKt4EMTopo",
                                         "BTagging_AntiKt4EMPFlow",
                                         "InDetTrackParticles",
                                         ]

SUSY15SlimmingHelper.AllVariables = [
                                     "TruthParticles",
                                     "TruthEvents",
                                     "TruthVertices",
                                     "MET_Truth",
                                     # additions for DV specific content
                                     "PrimaryVertices",
                                     "VrtSecInclusive_SecondaryVertices",
                                     "VrtSecInclusive_SecondaryVertices_Leptons",
                                     "VrtSecInclusive_All2TrksVertices", # only filled for debug, by default off
                                     ]

SUSY15SlimmingHelper.ExtraVariables = [ "BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant.BTagTrackToJetAssociator",
                                        "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.msInnerMatchChi2.msInnerMatchDOF.EnergyLossSigma.MeasEnergyLoss.MeasEnergyLossSigma.ParamEnergyLoss.ParamEnergyLossSigma.ParamEnergyLossSigmaMinus.ParamEnergyLossSigmaPlus",
					"AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.Timing.DFCommonJets_jetClean_VeryLooseBadLLP",
					"GSFTrackParticles.chiSquared.hitPattern.patternRecoInfo.numberDoF.numberOfPixelHoles.numberOfPixelSharedHits.numberOfSCTSharedHits.vx.vy.vz.z0.d0.definingParametersCovMatrix.truthOrigin.truthType.beamlineTiltX.beamlineTiltY",
					"InDetTrackParticles.truthOrigin.truthType.hitPattern.patternRecoInfo.vx.vy.vz.beamlineTiltX.beamlineTiltY.radiusOfFirstHit",
					"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
					"MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.decayVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType.charge",
					"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                        "TruthParticles.px.py.pz.m.e.status.pdgId.charge.barcode.prodVtxLink.decayVtxLink.truthOrigin.truthType",
					"Electrons.bkgMotherPdgId.bkgTruthOrigin",
                                        "InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV",
                                        "InDetTrackParticles.is_selected_Leptons.is_associated_Leptons.is_svtrk_final_Leptons.pt_wrtSV_Leptons.eta_wrtSV_Leptons.phi_wrtSV_Leptons.d0_wrtSV_Leptons.z0_wrtSV_Leptons.errP_wrtSV_Leptons.errd0_wrtSV_Leptons.errz0_wrtSV_Leptons.chi2_toSV_Leptons",
                                        "Electrons.svLinks.d0_wrtSVs.z0_wrtSVs.pt_wrtSVs.eta_wrtSVs.phi_wrtSVs.d0err_wrtSVs.z0err_wrtSVs",
                                        "Muons.svLinks.d0_wrtSVs.z0_wrtSVs.pt_wrtSVs.eta_wrtSVs.phi_wrtSVs.d0err_wrtSVs.z0err_wrtSVs",
                                        "MET_LocHadTopo.source.name.mpx.mpy.sumet",
                                        "MET_Track.source.name.mpx.mpy.sumet",
                                        "MuonSegments.x.y.z.chamberIndex.sector.etaIndex.nPhiLayers.nTrigEtaLayers.nPrecisionHits.t0.clusterTime",
]
SUSY15SlimmingHelper.IncludeMuonTriggerContent = True
SUSY15SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY15SlimmingHelper.IncludeEtMissTriggerContent = True

SUSY15Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_RHadronTruthJets")
SUSY15Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_RHadronTruthJetsAux.")
SUSY15Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_CharginoTruthJets")
SUSY15Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_CharginoTruthJetsAux.")
SUSY15Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_NeutralinoTruthJets")
SUSY15Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_NeutralinoTruthJetsAux.")
SUSY15Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_GluinoTruthJets")
SUSY15Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_GluinoTruthJetsAux.")
SUSY15Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_SquarkTruthJets")
SUSY15Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_SquarkTruthJetsAux.")


# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY15SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  SUSY15SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]

SUSY15SlimmingHelper.AppendContentToStream(SUSY15Stream)
