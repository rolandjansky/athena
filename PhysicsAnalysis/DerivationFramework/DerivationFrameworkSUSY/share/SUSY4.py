#********************************************************************
# SUSY4.py
# reductionConf flag SUSY4 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents, addBSMAndDownstreamParticles, addHFAndDownstreamParticles, addPVCollection, addTopQuarkAndDownstreamParticles
  addStandardTruthContents()
  addBSMAndDownstreamParticles()
  addHFAndDownstreamParticles()
  addTopQuarkAndDownstreamParticles()
  addPVCollection()


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY4Stream )
SUSY4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY4Stream.AcceptAlgs(["SUSY4KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY4ThinningHelper = ThinningHelper( "SUSY4ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY4 = CfgMgr.AthSequencer("SeqSUSY4")
DerivationFrameworkJob += SeqSUSY4

#====================================================================
# Trigger navigation thinning
#====================================================================
# To be added.
SUSY4ThinningHelper.AppendToStream( SUSY4Stream )


#====================================================================
# THINNING TOOLS
#====================================================================

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
#SUSY4TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY4TPThinningTool",
#                                                                 ThinningService         = SUSY4ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY4TPThinningTool
#thinningTools.append(SUSY4TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY4MuonTPThinningTool",
                                                                         ThinningService         = SUSY4ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4MuonTPThinningTool
thinningTools.append(SUSY4MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                     = "SUSY4ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY4ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY4ElectronTPThinningTool
thinningTools.append(SUSY4ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY4PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY4PhotonTPThinningTool",
										 ThinningService	 = SUSY4ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4PhotonTPThinningTool
thinningTools.append(SUSY4PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY4TauTPThinningTool",
                                                                        ThinningService = SUSY4ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4TauTPThinningTool
thinningTools.append(SUSY4TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY4TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY4TruthThinningTool",
                                                       ThinningService              = SUSY4ThinningHelper.ThinningSvc(),
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
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = False,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY4TruthThinningTool
  thinningTools.append(SUSY4TruthThinningTool)


  from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__Truth3CollectionMaker

  SUSY4TRUTH3SherpaMETool = DerivationFramework__Truth3CollectionMaker(name = "SUSY4TRUTH3SherpaMETool",
                        NewCollectionName       = "SUSY4TRUTHSherpaME",
                        ParticleSelectionString = "(abs(TruthParticles.pdgId) < 100 || ((abs(TruthParticles.pdgId) >= 1000001) && (abs(TruthParticles.pdgId) <= 1000039))) && TruthParticles.status==3")
  ToolSvc += SUSY4TRUTH3SherpaMETool
  AugmentationTools.append(SUSY4TRUTH3SherpaMETool)



#====================================================================
# SKIMMING TOOL
#====================================================================

# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY4)

muonsRequirements = '(Muons.pt >= 15.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 15.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
jetRequirements = '(AntiKt4EMTopoJets.DFCommonJets_Calib_pt >= 40.*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.5)'

expression = '(((count('+electronsRequirements+') + count('+muonsRequirements+') >= 1) && (count('+jetRequirements+') >=3)) || ((count('+electronsRequirements+') + count('+muonsRequirements+') >= 2) && (count('+jetRequirements+') >=2)) )'

triggers = ["HLT_5j.*", "HLT_6j.*", "HLT_7j.*", "HLT_g45_loose_5j45_0eta240", "HLT_g45_loose_6j45_0eta240", "HLT_ht1000_L1J100"]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool, DerivationFramework__TriggerSkimmingTool, DerivationFramework__FilterCombinationOR
SUSY4ObjectSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY4ObjectSkimmingTool",
                                                                expression = expression)
ToolSvc += SUSY4ObjectSkimmingTool
# Multijet and Multijet + photon triggers
SUSY4TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY4TrigSkimmingTool",
                                                                  TriggerListOR = triggers)
ToolSvc += SUSY4TrigSkimmingTool
SUSY4SkimmingTool = DerivationFramework__FilterCombinationOR( name = "SUSY4SkimmingTool",
                                                              FilterList = [SUSY4ObjectSkimmingTool, SUSY4TrigSkimmingTool])
ToolSvc += SUSY4SkimmingTool


from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg

#====================================================================
# ISOLATION TOOL
#====================================================================
#Track selection
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool500.TrackSelectionTool.minPt= 500.
TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool500

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
import ROOT, cppyy
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt1000_'
                                                               )
Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool500,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt500_'
                                                               )
ToolSvc += Pt1000IsoTrackDecorator
ToolSvc += Pt500IsoTrackDecorator

AugmentationTools.append(Pt1000IsoTrackDecorator)
AugmentationTools.append(Pt500IsoTrackDecorator)

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
   SeqSUSY4 += CfgMgr.DerivationFramework__DerivationKernel("SUSY4KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY4")
                                                            )

   from DerivationFrameworkSUSY.SUSYWeightMetadata import *

#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY4 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY4KernelSkim",
    SkimmingTools = [SUSY4SkimmingTool]
)
# Calibrating jets in derivation framework not yet possible (no alg)
#from JetRec.JetRecCalibrationFinder import jrcf
#JetCalTool = jrcf.find("AntiKt", 0.4, "EMTopo", "aroj", "reco", "Kt4")
#ToolSvc += JetCalTool
#DerivationFrameworkJob += JetCalTool


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY4"] = []

reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY4, "SUSY4")

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
addDefaultTrimmedJets(SeqSUSY4, "SUSY4")


#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================

# COMMENTED OUT FOR R21
# fix for MET pflow summation
#updatePFlowJetsWithCPFO(SeqSUSY4)

# re-tag PFlow jets so they have b-tagging info.
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY4)



## Adding decorations for fJVT PFlow jets

                                                                                                                                                                                                            
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY4, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY4, algname='JetForwardJvtToolBDTAlg')

#SeqSUSY4 += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )

# COMMENTED OUT FOR R21
#overwriteMETPFlowWithFix(SeqSUSY4)


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY4 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY4KernelAug",
    AugmentationTools = AugmentationTools,
    ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY4SlimmingHelper = SlimmingHelper("SUSY4SlimmingHelper")
SUSY4SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "AntiKt4EMTopoJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        #"BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices", 
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", 
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810"]
SUSY4SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "InDetTrackParticles"] #InDetTrackParticles also in SmartCollections?
SUSY4SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant",
                                      "Electrons.Loose.Medium.Tight",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.JvtJvfcorr.JvtRpt.Jvt.TruthLabelID.BchCorrCell.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.DFCommonJets_jetClean_VeryLooseBadLLP",
                                      "AntiKt4EMPFlowJets.JvtJvfcorr.JvtRpt.Jvt.BchCorrCell.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
                                      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelID",
                                      "InDetTrackParticles.truthOrigin.truthType.truthMatchProbability.TrkIso*",
                                      "AntiKt4EMPFlowJets.EMFrac.HECFrac.LArQuality.HECQuality.FracSamplingMax.NegativeE.AverageLArQF.FracSamplingMaxIndex",
                                      "AntiKt4EMPFlowJets.pt.eta.btagging.btaggingLink.TruthLabelID.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostMuonSegment.GhostTrack.GhostTruthAssociationLink.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.ConeTruthLabelID.GhostAntiKt2TrackJet.PartonTruthLabelID.Jvt.HadronConeExclTruthLabelID",
                                      "BTagging_AntiKt4EMPFlow_201810.MV1_discriminant.MV1c_discriminant.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant.MVb_discriminant.MV1_discriminant.MSV_vertices.MV1c_discriminant.SV0_badTracksIP.SV0_vertices.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP.MV2c100_discriminant.MV2m_pu.MV2m_pc.MV2m_pb",
                                      "BTagging_AntiKt4EMPFlow_201903.MV1_discriminant.MV1c_discriminant.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant.MVb_discriminant.MV1_discriminant.MSV_vertices.MV1c_discriminant.SV0_badTracksIP.SV0_vertices.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP.MV2c100_discriminant.MV2m_pu.MV2m_pc.MV2m_pb",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.n_constituents.n_subjets.tau_1.tau_2.tau_3.ecf_1.ecf_2.ecf_3", # check R21 EDM
                                      "AntiKt10LCTopoJets.GhostTQuarksFinal.GhostTQuarksFinalCount.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostWBosons.GhostWBosonsCount.GhostZBosons.GhostZBosonsCount.GhostHBosons.GhostHBosonsCount"]
SUSY4SlimmingHelper.IncludeMuonTriggerContent   = False
SUSY4SlimmingHelper.IncludeEGammaTriggerContent = False
SUSY4SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY4SlimmingHelper.IncludeJetTriggerContent    = False
SUSY4SlimmingHelper.IncludeTauTriggerContent    = False
SUSY4SlimmingHelper.IncludeEtMissTriggerContent = False
SUSY4SlimmingHelper.IncludeBJetTriggerContent   = False

# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkHasTruth:
   SUSY4SlimmingHelper.AppendToDictionary = {
      'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets': 'xAOD::JetContainer', 'AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux': 'xAOD::JetAuxContainer',
      #'TruthTop':                   'xAOD::TruthParticleContainer', 'TruthTopAux':                   'xAOD::TruthParticleAuxContainer',
      #'TruthBSM':                   'xAOD::TruthParticleContainer', 'TruthBSMAux':                   'xAOD::TruthParticleAuxContainer',
      'TruthBoson':                 'xAOD::TruthParticleContainer', 'TruthBosonAux':                 'xAOD::TruthParticleAuxContainer',
      'SUSY4TRUTHSherpaME':         'xAOD::TruthParticleContainer', 'SUSY4TRUTHSherpaMEAux':         'xAOD::TruthParticleAuxContainer',
      'TruthBSMWithDecayParticles': 'xAOD::TruthParticleContainer', 'TruthBSMWithDecayParticlesAux': 'xAOD::TruthParticleAuxContainer',
      'TruthHFWithDecayParticles':  'xAOD::TruthParticleContainer', 'TruthHFWithDecayParticlesAux':  'xAOD::TruthParticleAuxContainer',
      'TruthBSMWithDecayVertices':  'xAOD::TruthVertexContainer',   'TruthBSMWithDecayVerticesAux':  'xAOD::TruthVertexAuxContainer',
      'TruthHFWithDecayVertices':   'xAOD::TruthVertexContainer',   'TruthHFWithDecayVerticesAux':   'xAOD::TruthVertexAuxContainer',
      'TruthPrimaryVertices':       'xAOD::TruthVertexContainer',   'TruthPrimaryVerticesAux':       'xAOD::TruthVertexAuxContainer'
      }
else:
   SUSY4SlimmingHelper.AppendToDictionary = {
      'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets': 'xAOD::JetContainer', 'AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux': 'xAOD::JetAuxContainer'
      }


# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
if DerivationFrameworkHasTruth:
  SUSY4SlimmingHelper.AllVariables += [
                                      "TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", 
                                      #"TruthTop", # Remove TruthTop as it overlaps with addTopQuarkAndDownstreamParticles()
                                      #"TruthBSM", # Remove TruthBSM  as it overlaps with TruthBSMWithDecayParticles
                                      "TruthBoson", "SUSY4TRUTHSherpaME",
                                      "TruthBSMWithDecayParticles", "TruthHFWithDecayParticles", "TruthBSMWithDecayVertices", "TruthHFWithDecayVertices",
                                      "TruthPrimaryVertices"
                                       ]


#==============================================================================
# Extra info for quark-gluon tagging 
#==============================================================================
truthjetalg='AntiKt4TruthJets'
if not DerivationFrameworkHasTruth:
	  truthjetalg=None
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqSUSY4,algname="QGTaggerToolPFAlg",truthjetalg=truthjetalg) 
SUSY4SlimmingHelper.ExtraVariables += ['AntiKt4EMPFlowJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta']

SUSY4SlimmingHelper.AppendContentToStream(SUSY4Stream) # AppendContentToStream must always go last
