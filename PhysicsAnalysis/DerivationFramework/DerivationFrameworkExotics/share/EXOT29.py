###  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# EXOT29.py
# reductionConf flag EXOT29 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from VrtSecInclusive.VrtSecInclusive_Configuration import setupVSI

### Set up stream
streamName = derivationFlags.WriteDAOD_EXOT29Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT29Stream )
EXOT29Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT29Stream.AcceptAlgs(["EXOT29KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT29ThinningHelper = ThinningHelper( "EXOT29ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqEXOT29 = CfgMgr.AthSequencer("SeqEXOT29")
DerivationFrameworkJob += SeqEXOT29


#====================================================================
# Rerun VrtSecInclusive in derivation step 
#====================================================================
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive

# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              AtlasMagFieldSvc    = "AtlasFieldSvc"
                                             )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO




# Re-run the vertexing during derivation production)
# Will write out (additional) vertex container(s), each with a suffix added to their names 
rerunVertexing = True
LeptonsModSuffix = "LeptonsMod_LRTR3_1p0"
keepOriginalVertexContainer = True

#------------------------------------------------------------------------------
if rerunVertexing:

  # make Pixel and SCT conditions available
  include ("InDetRecExample/PixelConditionsAccess.py") # include all pixel condtions avaliable in AOD /DT
  include ("InDetRecExample/SCTConditionsAccess.py")


  # setup lepton-only vertexing for displaced HNL analysis. 
  vsi_lepMod = setupVSI( LeptonsModSuffix ) # setup default vsi configuration 
  vsi_lepMod.twoTrkVtxFormingD0Cut = 1.0 # loosen d0 cut to 1 mm
  vsi_lepMod.doSelectTracksWithLRTCuts = True # apply addtional track cuts inspired by LRT Run 3 optimizations
  vsi_lepMod.doSelectTracksFromMuons    = True # do leptons-only vertexing
  vsi_lepMod.doRemoveCaloTaggedMuons    = True # do remove calo-tagged muons from track selection
  vsi_lepMod.doSelectTracksFromElectrons  = True # do leptons-only vertexing


  vsi_lepMod.VertexFitterTool        = InclusiveVxFitterTool
  vsi_lepMod.Extrapolator            = ToolSvc.AtlasExtrapolator

  SeqEXOT29 += vsi_lepMod

  MSMgr.GetStream("StreamDAOD_EXOT29").AddItem( [ 'xAOD::TrackParticleContainer#InDetTrackParticles*',
                                                  'xAOD::TrackParticleAuxContainer#InDetTrackParticles*',
                                                  'xAOD::VertexContainer#VrtSecInclusive*'+LeptonsModSuffix+'*',
                                                  'xAOD::VertexAuxContainer#VrtSecInclusive*'+LeptonsModSuffix+'*'] )
  print "List of items for the DAOD_RPVLL output stream:"
  print MSMgr.GetStream("StreamDAOD_EXOT29").GetItems()

  # end of vertex rerunning
#------------------------------------------------------------------------------

# for now adding an unconfigured thinning helper (?) /CO
EXOT29ThinningHelper.AppendToStream( EXOT29Stream )

### Thinning taken from SUSY 15 

#====================================================================
# THINNING TOOLS
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
EXOT29TPThinningTool = DerivationFramework__TrackParticleThinning(name = "EXOT29TPThinningTool",
                                                                 ThinningService         = EXOT29ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "(InDetTrackParticles.pt > 1*GeV) && ((InDetTrackParticles.numberOfPixelHits + InDetTrackParticles.numberOfSCTHits) > 1)",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT29TPThinningTool
thinningTools.append(EXOT29TPThinningTool)


# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT29MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "EXOT29MuonTPThinningTool",
                                                                         ThinningService         = EXOT29ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT29MuonTPThinningTool
thinningTools.append(EXOT29MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT29ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                  = "EXOT29ElectronTPThinningTool",
                                                                                 ThinningService        = EXOT29ThinningHelper.ThinningSvc(),
                                                                                 SGKey              = "Electrons",
                                                                                 BestMatchOnly          = False,
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT29ElectronTPThinningTool
thinningTools.append(EXOT29ElectronTPThinningTool)
# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT29PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name       = "EXOT29PhotonTPThinningTool",
                     ThinningService   = EXOT29ThinningHelper.ThinningSvc(),
                     SGKey       = "Photons",
                     InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT29PhotonTPThinningTool
thinningTools.append(EXOT29PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EXOT29TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name           = "EXOT29TauTPThinningTool",
                                                                        ThinningService = EXOT29ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT29TauTPThinningTool
thinningTools.append(EXOT29TauTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT29AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                   = "EXOT29AKt4JetTPThinningTool",
                                                                            ThinningService         = EXOT29ThinningHelper.ThinningSvc(),
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            SelectionString         = "AntiKt4EMTopoJets.pt > 20*GeV && abs(AntiKt4EMTopoJets.eta) < 2.1",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT29AKt4JetTPThinningTool
thinningTools.append(EXOT29AKt4JetTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkHasTruth:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  EXOT29TruthThinningTool = DerivationFramework__MenuTruthThinning(name             = "EXOT29TruthThinningTool",
                                                       ThinningService              = EXOT29ThinningHelper.ThinningSvc(),
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
  ToolSvc += EXOT29TruthThinningTool
  thinningTools.append(EXOT29TruthThinningTool)

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
# SKIMMING - skimming on lepton-only DV object selection
#=============================================================================================
# Require at least one lepton-only DV in the event to pass skimming selection
DVRequirements = '(VrtSecInclusive_SecondaryVertices_{}.mass > 0)'.format(LeptonsModSuffix) # mass in [MeV]
objectSelection = '(count('+DVRequirements+') >= 1)'
expression = objectSelection

print "expression = '" + expression + "'"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT29SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT29SkimmingTool", expression = expression)

ToolSvc += EXOT29SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# EXOT skimming selection
#==============================================================================
SeqEXOT29 += CfgMgr.DerivationFramework__DerivationKernel(
        "EXOT29KernelSkim",
        SkimmingTools = [EXOT29SkimmingTool],
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqEXOT29)

## Adding decorations for fJVT PFlow jets                                                                                                                                                                   
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqEXOT29, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqEXOT29, algname='JetForwardJvtToolBDTAlg')

#==============================================================================
# Augment after skim
#==============================================================================
SeqEXOT29 += CfgMgr.DerivationFramework__DerivationKernel(
        "EXOT29KernelAug",
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

SeqEXOT29 += IPAugmentor


#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT29SlimmingHelper = SlimmingHelper("EXOT29SlimmingHelper")
EXOT29SlimmingHelper.SmartCollections = [
                                         "Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "MET_Reference_AntiKt4EMPFlow",
                                         "AntiKt4EMTopoJets",
                                         "AntiKt4EMTopoJets_BTagging201810",
                                         "AntiKt4EMPFlowJets",
                                         "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903",
                                         "BTagging_AntiKt4EMTopo_201810",
                                         "BTagging_AntiKt4EMPFlow_201810",
                                         "BTagging_AntiKt4EMPFlow_201903",
                                         "InDetTrackParticles",
                                         ]

EXOT29SlimmingHelper.AllVariables = [
                                     "TruthParticles",
                                     "TruthEvents",
                                     "TruthVertices",
                                     "MET_Truth",
                                     # additions for DV specific content
                                     "PrimaryVertices",
                                     "VrtSecInclusive_SecondaryVertices_Leptons",
                                     "VrtSecInclusive_All2TrksVertices", # only filled for debug, by default off
                                     ]


EXOT29SlimmingHelper.ExtraVariables = [ "BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant.BTagTrackToJetAssociator",
                                        "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.msInnerMatchChi2.msInnerMatchDOF.EnergyLossSigma.MeasEnergyLoss.MeasEnergyLossSigma.ParamEnergyLoss.ParamEnergyLossSigma.ParamEnergyLossSigmaMinus.ParamEnergyLossSigmaPlus",
                                        "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.Timing.DFCommonJets_jetClean_LooseBadLLP.DFCommonJets_jetClean_VeryLooseBadLLP.DFCommonJets_jetClean_SuperLooseBadLLP",
                                        "GSFTrackParticles.chiSquared.hitPattern.patternRecoInfo.numberDoF.numberOfPixelHoles.numberOfPixelSharedHits.numberOfSCTSharedHits.vx.vy.vz.z0.d0.definingParametersCovMatrix.truthOrigin.truthType.beamlineTiltX.beamlineTiltY.radiusOfFirstHit.truthMatchProbability",
                                        "InDetTrackParticles.truthOrigin.truthType.chiSquared.hitPattern.patternRecoInfo.vx.vy.vz.beamlineTiltX.beamlineTiltY.radiusOfFirstHit.truthMatchProbability",
                                        "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType.patternRecoInfo",
                                        "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType.patternRecoInfo",
                                        "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
                                        "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.decayVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType.charge",
                                        "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                        "TruthParticles.px.py.pz.m.e.status.pdgId.charge.barcode.prodVtxLink.decayVtxLink.truthOrigin.truthType",
                                        "Electrons.bkgMotherPdgId.bkgTruthOrigin",
                                        "InDetTrackParticles.is_selected_Leptons.is_associated_Leptons.is_svtrk_final_Leptons.pt_wrtSV_Leptons.eta_wrtSV_Leptons.phi_wrtSV_Leptons.d0_wrtSV_Leptons.z0_wrtSV_Leptons.errP_wrtSV_Leptons.errd0_wrtSV_Leptons.errz0_wrtSV_Leptons.chi2_toSV_Leptons",
                                        "GSFTrackParticles.is_selected_Leptons.is_associated_Leptons.is_svtrk_final_Leptons.pt_wrtSV_Leptons.eta_wrtSV_Leptons.phi_wrtSV_Leptons.d0_wrtSV_Leptons.z0_wrtSV_Leptons.errP_wrtSV_Leptons.errd0_wrtSV_Leptons.errz0_wrtSV_Leptons.chi2_toSV_Leptons",
                                        "Electrons.svLinks.d0_wrtSVs.z0_wrtSVs.pt_wrtSVs.eta_wrtSVs.phi_wrtSVs.d0err_wrtSVs.z0err_wrtSVs",
                                        "Muons.svLinks.d0_wrtSVs.z0_wrtSVs.pt_wrtSVs.eta_wrtSVs.phi_wrtSVs.d0err_wrtSVs.z0err_wrtSVs",
                                        "MET_LocHadTopo.source.name.mpx.mpy.sumet",
                                        "MET_Track.source.name.mpx.mpy.sumet",
                                        "MuonSegments.x.y.z.chamberIndex.sector.etaIndex.nPhiLayers.nTrigEtaLayers.nPrecisionHits.t0.clusterTime",
                                        "Electrons.Reta.Rphi.Rhad1.Rhad.weta2.Eratio.f3.deltaEta1.deltaPhiRescaled2.wtots1.LHLoose.ptcone30.ptcone20",
]

# Include dvtrack variables from re-running of VSI 
original_dvtrack_vars = "is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV".split(".")
LeptonsMod_dvtrack_vars = [v + "_" +  LeptonsModSuffix for v in original_dvtrack_vars]
EXOT29SlimmingHelper.ExtraVariables += [ "InDetTrackParticles." + ".".join(LeptonsMod_dvtrack_vars) ]
EXOT29SlimmingHelper.ExtraVariables += [ "GSFTrackParticles." + ".".join(LeptonsMod_dvtrack_vars) ]

EXOT29SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent

if (not rerunVertexing or keepOriginalVertexContainer):
  EXOT29SlimmingHelper.AllVariables += [
                                        "VrtSecInclusive_SecondaryVertices"
                                       ]
  EXOT29SlimmingHelper.ExtraVariables += [ "InDetTrackParticles." + ".".join(original_dvtrack_vars) ]


EXOT29SlimmingHelper.IncludeMuonTriggerContent = True
EXOT29SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT29SlimmingHelper.IncludeEtMissTriggerContent = True

EXOT29Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_RHadronTruthJets")
EXOT29Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_RHadronTruthJetsAux.")
EXOT29Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_CharginoTruthJets")
EXOT29Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_CharginoTruthJetsAux.")
EXOT29Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_NeutralinoTruthJets")
EXOT29Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_NeutralinoTruthJetsAux.")
EXOT29Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_GluinoTruthJets")
EXOT29Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_GluinoTruthJetsAux.")
EXOT29Stream.AddItem("xAOD::JetContainer#AntiKt4LLP_SquarkTruthJets")
EXOT29Stream.AddItem("xAOD::ShallowAuxContainer#AntiKt4LLP_SquarkTruthJetsAux.")


# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkHasTruth:

  EXOT29SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  EXOT29SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]

EXOT29SlimmingHelper.AppendContentToStream(EXOT29Stream)
