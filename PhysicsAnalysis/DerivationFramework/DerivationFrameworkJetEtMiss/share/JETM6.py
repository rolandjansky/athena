#====================================================================
# JETM6.py
# reductionConf flag JETM6 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkFlavourTag.HbbCommon import *

from DerivationFrameworkJetEtMiss.METCommon import *

#====================================================================
# TRUTH3
#====================================================================

if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addBosonsAndDownstreamParticles,addTopQuarkAndDownstreamParticles,addHFAndDownstreamParticles
  addStandardTruthContents()
  addBosonsAndDownstreamParticles()
  addTopQuarkAndDownstreamParticles()
  addHFAndDownstreamParticles(addB=True, addC=False, generations=0)
  import DerivationFrameworkCore.WeightMetadata
  import DerivationFrameworkCore.LHE3WeightMetadata

#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
electronTriggers = TriggerLists.single_el_Trig()
muonTriggers = TriggerLists.single_mu_Trig()
photonTriggers = TriggerLists.single_photon_Trig()
jetTriggers = TriggerLists.jetTrig()

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 400.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1 || count( AntiKt10TrackCaloClusterJets.pt > 400.*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5 ) >= 1 || count( AntiKt10UFOCSSKJets.pt > 400.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >= 1)'
if DerivationFrameworkIsMonteCarlo:
  jetSelection = '(count( AntiKt10LCTopoJets.pt > 180.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1 || count( AntiKt10TrackCaloClusterJets.pt > 180.*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5 ) >= 1 || count( AntiKt10UFOCSSKJets.pt > 180.*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5 ) >= 1)'

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHLoose)) >= 1',
                            '(count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5) >= 1 || count(AntiKt10UFOCSSKJets.pt > 150*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5) >=1)'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) >= 1',
                            '(count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5) >= 1 || count(AntiKt10UFOCSSKJets.pt > 150*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5) >=1 )'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '
gammatrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(photonTriggers)
gammaofflinesel = '(count(Photons.pt > 150*GeV) >= 1 && (count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5) >= 1 || count(AntiKt10UFOCSSKJets.pt > 150*GeV && abs(AntiKt10UFOCSSKJets.eta) < 2.5) >=1 ))'
photonSelection = ' ( (' + gammatrigsel + ') && (' + gammaofflinesel + ') ) '

lepSelection = '( ' + electronSelection + ' || ' + muonSelection + ' || ' + photonSelection + ' )'


expression = jetSelection + ' || '+ lepSelection


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM6TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM6TrigSkimmingTool",
                                                                    TriggerListOR  = jetTriggers+electronTriggers+muonTriggers+photonTriggers )
                                                                    #TriggerListOR  = triggers )
ToolSvc += JETM6TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM6OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM6OfflineSkimmingTool",
                                                                        expression = expression)
ToolSvc += JETM6OfflineSkimmingTool

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM6Stream )
JETM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM6Stream.AcceptAlgs(["JETM6MainKernel"])

#====================================================================
# CREATE PRIVATE SEQUENCE
#====================================================================

jetm6Seq = CfgMgr.AthSequencer("JETM6Sequence")
DerivationFrameworkJob += jetm6Seq

#====================================================================
# Trigger matching decorations
#====================================================================
photonTriggers_matching = ['HLT_g60_loose', 'HLT_g140_loose', 'HLT_g160_loose']

from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM6",
                                   ElectronTriggers=electronTriggers,
                                   MuonTriggers=muonTriggers,
                                   PhotonTriggers=photonTriggers_matching)

#====================================================================
# TRIGGER THINNING TOOL
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM6ThinningHelper = ThinningHelper( "JETM6ThinningHelper" )
JETM6ThinningHelper.TriggerChains = ''

JETM6ThinningHelper.TriggerChains += "|".join(electronTriggers)
JETM6ThinningHelper.TriggerChains += "|".join(muonTriggers)
JETM6ThinningHelper.TriggerChains += "|".join(photonTriggers)
JETM6ThinningHelper.TriggerChains += "|".join(jetTriggers)

JETM6ThinningHelper.AppendToStream( JETM6Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt4JetTPThinningTool",
                                                                            ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            SelectionString         = "AntiKt4EMTopoJets.pt > 20*GeV",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = False)
ToolSvc += JETM6Akt4JetTPThinningTool
thinningTools.append(JETM6Akt4JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt4PFlowJetTPThinningTool",
                                                                                 ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                                 JetKey                  = "AntiKt4EMPFlowJets",
                                                                                 SelectionString         = "AntiKt4EMPFlowJets.pt > 20*GeV",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 ApplyAnd                = False)
ToolSvc += JETM6Akt4PFlowJetTPThinningTool
thinningTools.append(JETM6Akt4PFlowJetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetTPThinningTool",
                                                                             ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                             JetKey                  = "AntiKt10LCTopoJets",
                                                                             SelectionString         = "AntiKt10LCTopoJets.pt > 100*GeV",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ApplyAnd                = False)
ToolSvc += JETM6Akt10JetTPThinningTool
thinningTools.append(JETM6Akt10JetTPThinningTool)

JETM6Akt10JetTPThinningToolTCC = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetTPThinningToolTCC",
                                                                                ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                                JetKey                  = "AntiKt10TrackCaloClusterJets",
                                                                                SelectionString         = "AntiKt10TrackCaloClusterJets.pt > 100*GeV",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ApplyAnd                = False)
ToolSvc += JETM6Akt10JetTPThinningToolTCC
thinningTools.append(JETM6Akt10JetTPThinningToolTCC)

JETM6Akt10JetCSSKUFOThinningToolTCC = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetCSSKUFOThinningTool",
                                                                                     ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                                     JetKey                  = "AntiKt10UFOCSSKJets",
                                                                                     SelectionString         = "AntiKt10UFOCSSKJets.pt > 100*GeV",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                     ApplyAnd                = False)
ToolSvc += JETM6Akt10JetCSSKUFOThinningToolTCC
thinningTools.append(JETM6Akt10JetCSSKUFOThinningToolTCC)


# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM6MuonTPThinningTool",
                                                                         ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6MuonTPThinningTool
thinningTools.append(JETM6MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM6ElectronTPThinningTool",
                                                                               ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6ElectronTPThinningTool
thinningTools.append(JETM6ElectronTPThinningTool)

# TrackParticles associated with photons
JETM6PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM6PhotonTPThinningTool",
                                                                             ThinningService         = JETM6ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6PhotonTPThinningTool
thinningTools.append(JETM6PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM6TauTPThinningTool",
                                                                        ThinningService = JETM6ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6TauTPThinningTool
thinningTools.append(JETM6TauTPThinningTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel(name = "JETM6TrigSkimKernel",
                                                         AugmentationTools = [] ,
                                                         SkimmingTools = [JETM6TrigSkimmingTool],
                                                         ThinningTools = [])

#======================================= 
# RECONSTRUCT TCC and CSSK UFO
#======================================= 

from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction, runUFOReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(jetm6Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles", outputTCCName="TrackCaloClustersCombinedAndNeutral")
runUFOReconstruction(jetm6Seq, ToolSvc, PFOPrefix="CSSK")

from JetRec.JetRecConf import PseudoJetGetter
csskufopjgetter = PseudoJetGetter("csskufoPJGetter", InputContainer="CSSKUFO", OutputContainer="CSSKUFOPJ", Label="UFO", SkipNegativeEnergy=True)
jtm+=csskufopjgetter
csskufogetters = [csskufopjgetter]+list(jtm.gettersMap["tcc"])[1:]

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt10TrackCaloClusterJets"]

replaceAODReducedJets(reducedJetList,jetm6Seq,"JETM6")

addStandardJets("AntiKt", 1.0, "UFOCSSK", ptmin=40000, ptminFilter=50000, algseq=jetm6Seq, outputGroup="JETM6", customGetters = csskufogetters, constmods=["CSSK"])

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM6MainKernel",
                                                          AugmentationTools = [TrigMatchAug] ,
                                                          SkimmingTools = [JETM6OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# GROOMED LARGE-R JETS
#====================================================================

OutputJets["JETM6"] = []

addDefaultTrimmedJets(jetm6Seq,"JETM6")
addTCCTrimmedJets(jetm6Seq,"JETM6")

from JetRecTools.ConstModHelpers import getConstModSeq
addCHSPFlowObjects()
pflowCSSKSeq = getConstModSeq(["CS","SK"], "EMPFlow")
addConstModJets("AntiKt", 1.0, "EMPFlow", ["CS", "SK"], jetm6Seq, "JETM6", ptmin=40000, ptminFilter=50000)

if DerivationFrameworkIsMonteCarlo:
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)
  addRecursiveSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, N=-1,  mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)
  addBottomUpSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.05, mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)

addSoftDropJets("AntiKt", 1.0, "UFOCSSK", beta=1.0, zcut=0.1, algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False, mods="tcc_groomed")
addRecursiveSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, N=-1,  mods="tcc_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)
addBottomUpSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.05, mods="tcc_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=False)

#====================================================================
# BTAGGING INFO FOR PFLOW JETS
#====================================================================

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'],Sequencer = jetm6Seq)

#====================================================================
# VR track-jets (b-tagging)
#====================================================================

addVRJets(jetm6Seq)

addVRJetsTCC(jetm6Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
             VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
             ghostArea = 0 , ptmin = 2000, ptminFilter = 2000,
             variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

#====================================================================
# add xbb taggers
#====================================================================

from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(jetm6Seq, ToolSvc)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM6SlimmingHelper = SlimmingHelper("JETM6SlimmingHelper")
JETM6SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4EMPFlowJets","AntiKt4TruthJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                        "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets",
                                        "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets",
                                        "AntiKtVR30Rmax4Rmin02TrackJets",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        ]
JETM6SlimmingHelper.AllVariables = [
  "TruthEvents",
  "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape",
  ]

JETM6SlimmingHelper.ExtraVariables  = ['CaloCalTopoClusters.calE.calEta.calM.calPhi.CENTER_MAG']
JETM6SlimmingHelper.ExtraVariables += ['Electrons.'+NewTrigVars["Electrons"],'Muons.'+NewTrigVars["Muons"],'Photons.'+NewTrigVars["Photons"]]
JETM6SlimmingHelper.ExtraVariables += [
    'HLT_xAOD__JetContainer_a4tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.BchCorrCell.CentroidR.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.Timing.eta.kinematics.m.phi.pt',
    'HLT_xAOD__JetContainer_a4tcemsubjesISFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.BchCorrCell.CentroidR.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.Timing.eta.kinematics.m.phi.pt',
    'HLT_xAOD__JetContainer_a10tclcwsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.BchCorrCell.CentroidR.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.Timing.eta.kinematics.m.phi.pt',
    ]

for truthc in [
  "TruthTopQuark",
  "TruthBoson",
  "TruthHF"
  ]:
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles")
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.")
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices")
  JETM6SlimmingHelper.StaticContent.append("xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux.")

for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthBottom"
    ]:
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

#====================================================================
# ORIGIN CORRECTED CLUSTERS
#====================================================================

addOriginCorrectedClusters(JETM6SlimmingHelper,writeLC=True,writeEM=True)

#====================================================================
# TRIGGER CONTENT
#====================================================================
 
JETM6SlimmingHelper.IncludeJetTriggerContent = True
JETM6SlimmingHelper.IncludeMuonTriggerContent = True
JETM6SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM6SlimmingHelper,
              ["JETM6","AntiKt10LCTopoJets","AntiKt10TruthJets","AntiKt10TrackCaloClusterJets","AntiKt10UFOCSSKJets"],
              ["AntiKt10LCTopoJets","AntiKt10TruthJets","AntiKt10TrackCaloClusterJets","AntiKt10UFOCSSKJets"], #smart slimming
              ["AntiKt10EMPFlowCSSKJets"] #veto jets
              )

# Add the MET containers to the stream
addMETOutputs(JETM6SlimmingHelper,["Diagnostic","AntiKt4EMPFlow","Track"])

JETM6SlimmingHelper.AppendContentToStream(JETM6Stream)
#JETM6Stream.RemoveItem("xAOD::TrigNavigation#*")
#JETM6Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")



def removeVars(coll, vars):
    """Terrible hack to force removing a limited set of variables.
    Intended to remove definingParametersCovMatrix if possible.. """
    items = JETM6Stream.GetItems()
    origS = None
    for i in items:
        if coll in i and '.' in i:
            origS = i
            break
    if origS is None:
        return
    existV = origS.split('.')
    cleanedV = []
    for v in existV:
        if v in vars:
            continue
        cleanedV.append(v)
    newS = '.'.join(cleanedV)
    JETM6Stream.RemoveItem( origS )
    JETM6Stream.AddItem( newS )

#removeVars('InDetTrackParticles', ['definingParametersCovMatrix',])

