#**********************************************************************
# MUON5.py 
#
# reductionConf flag MUON5 in Reco_tf.py 
#
# -- For training and analysing non-prompt lepton tagging algorithms --
# -- Saves electron, muon and taus for matching with track-jets and 
#    associated b-tagging containers                                 --
#
#**********************************************************************

#======================================================================
# IMPORT DERIVATION MODULES
#======================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon              import *
from DerivationFrameworkJetEtMiss.JetCommon            import *
from DerivationFrameworkEGamma.EGammaCommon            import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon              import *
from DerivationFrameworkFlavourTag.FlavourTagCommon    import *
from DerivationFrameworkHiggs.TruthCategories          import *

from AthenaCommon.GlobalFlags import globalflags

# Check if is MC
is_MC = (globalflags.DataSource()=='geant4')

if is_MC:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#======================================================================
# SET UP STREAM   
#======================================================================

streamName  = derivationFlags.WriteDAOD_MUON5Stream.StreamName
fileName    = buildFileName(derivationFlags.WriteDAOD_MUON5Stream)
MUON5Stream = MSMgr.NewPoolRootStream(streamName, fileName)
MUON5Stream.AddItem("xAOD::VertexContainer#SecVtxContainer_Muons")
MUON5Stream.AddItem("xAOD::VertexContainer#SecVtxContainer_Electrons")
MUON5Stream.AddItem("xAOD::VertexContainer#SecVtx_ConvVtxContainer_Electrons")
MUON5Stream.AddItem("xAOD::VertexContainer#RefittedPriVtx")
MUON5Stream.AcceptAlgs(["MUON5Kernel"])

#====================================================================
# CREATE PRIVATE SEQUENCE  
#====================================================================

MUON5Seq = CfgMgr.AthSequencer("MUON5Sequence")

#====================================================================
# RESTORE JET COLLECTIONS AND RUN BTAGGING
#====================================================================

# Replace jet collections dropped from primaries in r21
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets

# Replace missing collections
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig
if not hasattr(MUON5Seq,"Muons_decoratePromptLepton"):
    LepTagConfig.ConfigureAntiKt4PV0TrackJets(MUON5Seq,"MUON1")
    MUON5Seq += LepTagConfig.GetDecoratePromptLeptonAlgs()
    MUON5Seq += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs()

#======================================================================
# AUGMENTATION TOOLS
#======================================================================

augmentationTools = []

#======================================================================
# TAU TRUTH MATCHING TOOL
#======================================================================

if is_MC:
    from DerivationFrameworkTau.TauTruthCommon import *
    
#======================================================================
# TAU OVERLAPPING ELECTRON LLH DECORATORS 
#======================================================================

from TauAnalysisTools.TauAnalysisToolsConf             import TauAnalysisTools__TauOverlappingElectronLLHDecorator
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauOverlappingElectronLLHDecoratorWrapper

# Tau Overlapping Electron LLH Decorator
MUON5TauOrlElLLHDec = TauAnalysisTools__TauOverlappingElectronLLHDecorator(name                  = "MUON5TauOrlElLLHDec",
                                                                           ElectronContainerName = "Electrons")  
ToolSvc += MUON5TauOrlElLLHDec
   
# Tau Overlapping Electron LLH Decorator Wrapper
MUON5TauOrlElLLHDecWrapper = DerivationFramework__TauOverlappingElectronLLHDecoratorWrapper(name                               = "MUON5TauOrlElLLHDecWrapper",
                                                                                            TauOverlappingElectronLLHDecorator = MUON5TauOrlElLLHDec,
                                                                                            TauContainerName                   = "TauJets")
ToolSvc += MUON5TauOrlElLLHDecWrapper

augmentationTools.append(MUON5TauOrlElLLHDecWrapper)

#======================================================================
# BACKGROUND ELECTRON DECORATION TYPE/ORIGIN
#======================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/trunk/src/BkgElectronClassification.cxx

if is_MC:
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 

    MUON5BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name                  = "MUON5BkgElectronClassificationTool",
                                                                                         MCTruthClassifierTool = BkgElectronMCTruthClassifier)

    ToolSvc += MUON5BkgElectronClassificationTool

    augmentationTools.append(MUON5BkgElectronClassificationTool)

    printfunc ("BkgElectronClassificationTool: ", MUON5BkgElectronClassificationTool)

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper

MUON5ThinningHelper = ThinningHelper("MUON5ThinningHelper")

# Trigger Thinning Tool
MUON5ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_2e.*|HLT_2mu.*|HLT_tau.*'
MUON5ThinningHelper.AppendToStream(MUON5Stream)

#====================================================================
# TRACK THINNING
#====================================================================

do_track_thinning = False

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON5TrackThinningTool = DerivationFramework__TrackParticleThinning(name                    = "MUON5TrackThinningTool",
                                                                    StreamName              = streamName,
                                                                    SelectionString         = "abs(DFCommonInDetTrackZ0AtPV) < 10.0",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
if do_track_thinning:
    ToolSvc += MUON5TrackThinningTool
    thinningTools.append(MUON5TrackThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================

do_truth_thinning = False

truth_cond_ew    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep   = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top   = "((abs(TruthParticles.pdgId) == 6))"                                       # Top quark
truth_cond_b     = "((abs(TruthParticles.pdgId) == 5))"                                       # b quark
truth_cond_c     = "((abs(TruthParticles.pdgId) == 4))"                                       # c quark
truth_cond_final = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'          # stable particles

truth_expression = '('+truth_cond_ew+' || '+truth_cond_lep+' || '+truth_cond_top+' || '+truth_cond_b+' || '+truth_cond_c+') || ('+truth_cond_final+')'

# PreserveGeneratorDescendants only keeps particles that came directly from the event generator
# PreserveDescendants keeps all particles including those that come from Geant processes

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning

MUON5TruthTool = DerivationFramework__GenericTruthThinning(name                         = "MUON5TruthThinningTool",
                                                           StreamName                   = streamName,
                                                           ParticleSelectionString      = truth_expression,
                                                           PreserveDescendants          = True,
                                                           PreserveGeneratorDescendants = False,
                                                           PreserveAncestors            = True)

if is_MC and do_truth_thinning:
    ToolSvc += MUON5TruthTool
    thinningTools.append(MUON5TruthTool)    

#====================================================================
# CALO CLUSTER THINNING
#====================================================================
#  These tools thin any calorimeter clusters not associated to the 
#  objects specified below

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Egamma clusters associated with electrons
MUON5ElectronEgammaCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "MUON5ElectronEgammaCThinningTool",
                                                                             StreamName              = streamName,
                                                                             SGKey                   = "Electrons",
                                                                             CaloClCollectionSGKey   = "egammaClusters",
                                                                             SelectionString         = "Electrons.pt >= 4*GeV",
                                                                             ConeSize                = 0.4)
ToolSvc += MUON5ElectronEgammaCThinningTool
thinningTools.append(MUON5ElectronEgammaCThinningTool)

#====================================================================
# SKIMMING TOOL 
#====================================================================

# Apply skimming requirement: at least one electron, muon or tau
eleRequirements = '(Electrons.pt > 5*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.Loose || Electrons.DFCommonElectronsLHLoose)'
muRequirements  = '(Muons.DFCommonGoodMuon) && (Muons.muonType == 0) && (Muons.pt > 5*GeV) && (abs(Muons.eta) < 2.6)'
tauRequirements = '(TauJets.pt > 15*GeV) && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3)) && (abs(TauJets.eta) < 2.6)'

eSelection   = '(count('+eleRequirements+') >= 1)'
mSelection   = '(count('+muRequirements +') >= 1)'
tauSelection = '(count('+tauRequirements+') >= 1)'

lepSelection = eSelection+' || '+mSelection+' || '+tauSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

MUON5SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name       = "MUON5SkimmingTool",
                                                                expression = lepSelection)
ToolSvc += MUON5SkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

if is_MC:
    from DerivationFrameworkCore.LHE3WeightMetadata import *

DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON5Kernel",
                                                                       ThinningTools     = thinningTools,
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools     = [MUON5SkimmingTool])
# Add private sequence to DerivationFrameworkJob
DerivationFrameworkJob += MUON5Seq

#====================================================================
# SLIMMING TOOL
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

MUON5SlimmingHelper = SlimmingHelper("MUON5SlimmingHelper")

# Smart slimming containers
MUON5SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "TauMVATESJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "PrimaryVertices"]
                                        

# Append new b-tagging container to dictionary for saving
MUON5SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4Track'               : 'xAOD::BTaggingContainer',
                                          'BTagging_AntiKt4TrackAux'            : 'xAOD::BTaggingAuxContainer',
                                          'SecVtxContainer_Electrons'           : 'xAOD::VertexContainer',
                                          'SecVtxContainer_ElectronsAux'        : 'xAOD::VertexAuxContainer',
                                          'SecVtx_ConvVtxContainer_Electrons'   : 'xAOD::VertexContainer',
                                          'SecVtx_ConvVtxContainer_ElectronsAux': 'xAOD::VertexAuxContainer',
                                          'SecVtxContainer_Muons'               : 'xAOD::VertexContainer',
                                          'SecVtxContainer_MuonsAux'            : 'xAOD::VertexAuxContainer',
                                          'RefittedPriVtx'                      : 'xAOD::VertexContainer',
                                          'RefittedPriVtxAux'                   : 'xAOD::VertexAuxContainer',
                                          }
# Add extra variables to collections
MUON5SlimmingHelper.ExtraVariables = ["Muons.clusterLink.allAuthors.charge.extrapolatedMuonSpectrometerTrackParticleLink."
                                      "scatteringCurvatureSignificance.scatteringNeighbourSignificance",
                                      "Electrons.author.charge.topoetcone30.ptvarcone30",
                                      "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m."
                                      "ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt.TileStatus",
                                      "GSFTrackParticles.z0.vz.definingParametersCovMatrix",
                                      "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                      "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                      "PrimaryVertices.x.y.z.trackParticleLinks.vertexType.neutralParticleLinks",
                                      "TauJets.pantau_CellBasedInput_isPanTauCandidate.pantau_CellBasedInput_DecayMode."
                                      "ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased."
                                      "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n.pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn."
                                      "pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn.ele_match_lhscore.ele_olr_pass."
                                      "electronLink.ptDetectorAxis.etaDetectorAxis.phiDetectorAxis.mDetectorAxis",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m"]
MUON5SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraPromptVariablesForDxAOD(onlyBDT=False)
MUON5SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraImprovedPromptVariablesForDxAOD()
MUON5SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent

MUON5SlimmingHelper.AllVariables = ["egammaClusters", "CaloCalTopoClusters", "MuonClusterCollection", "TopoClusterIsoCentralEventShape", "TopoClusterIsoForwardEventShape", "AntiKt4PV0TrackJets", "BTagging_AntiKt4Track", "InDetTrackParticles","GSFConversionVertices","GSFTrackParticles"]

if is_MC:
    MUON5SlimmingHelper.AllVariables += ["TruthParticles", "TruthEvents", "TruthVertices", "AntiKt4TruthJets"]

    MUON5SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink.bkgTruthType.bkgTruthOrigin."
                                           "bkgTruthParticleLink.bkgMotherPdgId.deltaPhi1",
                                           "MuonTruthParticles.truthOrigin.truthType.truthParticleLink",
                                           "InDetTrackParticles.truthOrigin.truthType.truthParticleLink",
                                           "CombinedMuonTrackParticles.truthOrigin.truthType.truthParticleLink",
                                           "TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]
    
    MUON5SlimmingHelper.StaticContent = ["xAOD::TruthParticleContainer#TruthMuons",
                                         "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                         "xAOD::TruthParticleContainer#TruthElectrons",
                                         "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                         "xAOD::TruthParticleContainer#TruthNeutrinos",
                                         "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
                                         "xAOD::TruthParticleContainer#TruthTaus",
                                         "xAOD::TruthParticleAuxContainer#TruthTausAux."]    
    
MUON5SlimmingHelper.IncludeMuonTriggerContent  =True
MUON5SlimmingHelper.IncludeEGammaTriggerContent=True
MUON5SlimmingHelper.IncludeTauTriggerContent   =True

MUON5SlimmingHelper.AppendContentToStream(MUON5Stream)
