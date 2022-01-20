#====================================================================
# DAOD_HDBS3.py
# This defines DAOD_HDBS3, an skimmed DAOD format for Run 3.
# It contains the variables and objects needed for the HZa Z->2l, a->jet 
# of physics analyses in ATLAS.
# It requires the reductionConf flag HDBS3 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import *
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HDBS3Stream.StreamName
fileName = buildFileName(derivationFlags.WriteDAOD_HDBS3Stream)
HDBS3Stream = MSMgr.NewPoolRootStream( streamName, fileName)
HDBS3Stream.AcceptAlgs(["HDBS3Kernel"])

### Thinning and augmentation tools lists
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HDBS3ThinningHelper = ThinningHelper( "HDBS3ThinningHelper" )
HDBS3ThinningHelper.AppendToStream( HDBS3Stream )
thinningTools       = []
AugmentationTools   = []

# Special sequence 
SeqHDBS3 = CfgMgr.AthSequencer("SeqHDBS3")

#====================================================================
# Truth collections
#====================================================================
if DerivationFrameworkHasTruth:
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection, addTruthJets
   import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   HDBS3TruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "HDBS3TruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += HDBS3TruthCharmTool
   from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqHDBS3 += CfgMgr.DerivationFramework__CommonAugmentation("HDBS3TruthCharmKernel",AugmentationTools=[HDBS3TruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqHDBS3)
   # Add standard truth
   addStandardTruthContents(SeqHDBS3,prefix='HDBS3_')
   addTruthJets()  # +++
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   HDBS3TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="HDBS3TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += HDBS3TruthNavigationDecorator
   SeqHDBS3.HDBS3_MCTruthNavigationDecoratorKernel.AugmentationTools = [HDBS3TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqHDBS3)
   addPVCollection(SeqHDBS3)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"
   # Add sumOfWeights metadata for LHE3 multiweights =======
   from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# TRIGGER CONTENT   
#====================================================================
# See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
# Get single and multi mu, e, photon triggers
# Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
# Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
# Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)

# Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
trigger_names_full_tau = list(set(trig_tau+trig_txe))

# Now reduce the list...
from RecExConfig.InputFilePeeker import inputFileSummary
trigger_names_notau = []
trigger_names_tau = []
for trig_item in inputFileSummary['metadata']['/TRIGGER/HLT/Menu']:
    if not 'ChainName' in trig_item: continue
    if trig_item['ChainName'] in trigger_names_full_notau: trigger_names_notau += [ trig_item['ChainName'] ]
    if trig_item['ChainName'] in trigger_names_full_tau:   trigger_names_tau   += [ trig_item['ChainName'] ]

# Create trigger matching decorations
trigmatching_helper_notau = TriggerMatchingHelper(name='HDBS3TriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='HDBS3TriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)

#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

#### Inner detector group recommendations for indet tracks in analysis
HBDS3_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV" 
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HDBS3TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HDBS3TrackParticleThinningTool",
                                                                           ThinningService         = HDBS3ThinningHelper.ThinningSvc(),
                                                                           SelectionString         = HBDS3_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ApplyAnd                = False)

ToolSvc += HDBS3TrackParticleThinningTool
thinningTools.append(HDBS3TrackParticleThinningTool)


####Jets +++?
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HDBS3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HDBS3JetTPThinningTool",
                                                                         ThinningService        = HDBS3ThinningHelper.ThinningSvc(),
                                                                         JetKey                 = "AntiKt4EMPFlowJets",
                                                                         ##SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS3JetTPThinningTool
thinningTools.append(HDBS3JetTPThinningTool)

#### Tracks associated with Muons +++?
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HDBS3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HDBS3MuonTPThinningTool",
                                                                           ThinningService        = HDBS3ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS3MuonTPThinningTool
thinningTools.append(HDBS3MuonTPThinningTool)

####Tracks associated with Electrons +++?
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HDBS3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HDBS3ElectronTPThinningTool",
                                                                                 ThinningService        = HDBS3ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HDBS3ElectronTPThinningTool
thinningTools.append(HDBS3ElectronTPThinningTool)


#### TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
HDBS3TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "HDBS3TauJetsThinningTool",
                                                                     ThinningService = HDBS3ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += HDBS3TauJetsThinningTool
thinningTools.append(HDBS3TauJetsThinningTool)


#====================================================================
# JET/MET   
#====================================================================

OutputJets["HDBS3"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets","AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if DerivationFrameworkHasTruth:
   OutputJets["HDBS3"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")
   OutputJets["HDBS3"].append("AntiKt10TruthSoftDropBeta100Zcut10Jets")

replaceAODReducedJets(reducedJetList,SeqHDBS3,"HDBS3")
add_largeR_truth_jets = DerivationFrameworkHasTruth and not hasattr(SeqHDBS3,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(SeqHDBS3,"HDBS3",dotruth=add_largeR_truth_jets)
add_largeR_truth_SD_jets = DerivationFrameworkHasTruth and not hasattr(SeqHDBS3,'jetalgAntiKt10TruthSoftDropBeta100Zcut10')
addDefaultUFOSoftDropJets(SeqHDBS3,"HDBS3",dotruth=add_largeR_truth_SD_jets)

# Add large-R jet truth labeling
if (DerivationFrameworkHasTruth):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqHDBS3,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqHDBS3,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqHDBS3,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqHDBS3, algname='JetForwardPFlowJvtToolAlg')

#====================================================================
# PromptLeptonVeto
#====================================================================
import LeptonTaggers.LeptonTaggersConfig as PLVConfig

# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqHDBS3, "PHYS")
PLVConfig.ConfigureAntiKt4PV0TrackJets(SeqHDBS3, "HDBS3")

# add decoration
SeqHDBS3 += PLVConfig.GetDecoratePromptLeptonAlgs(addSpectators=False)


#====================================================================
# Add our sequence to the top sequence
#====================================================================
# Ideally, this should come at the end of the job, but the tau additions
# below make it such that we need it here
DerivationFrameworkJob += SeqHDBS3


#====================================================================
# Tau   
#====================================================================

# Schedule low-pt di-tau reconstruction (needs AntiKt2PV0TrackJets)
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt()

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
HDBS3DiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "HDBS3DiTauLowPtThinningTool",
                                                                        ThinningService = HDBS3ThinningHelper.ThinningSvc(),
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += HDBS3DiTauLowPtThinningTool
thinningTools.append(HDBS3DiTauLowPtThinningTool)

#====================================================================
# SKIMMING
#====================================================================

Trigger2L=[
 "HLT_mu20_iloose_L1MU15",
 "HLT_mu40",
 "HLT_e24_lhmedium_L1EM20VH",
 "HLT_e60_lhmedium",
 "HLT_e120_lhloose",
 "HLT_mu26_ivarmedium",
 "HLT_e26_lhtight_nod0_ivarloose",
 "HLT_e60_lhmedium_nod0",
 "HLT_e140_lhloose_nod0",
 "HLT_mu50"]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HDBS3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "HDBS3TriggerSkimmingTool",
                                                                    TriggerListOR = Trigger2L)
                                                                    
                                                                    
ToolSvc += HDBS3TriggerSkimmingTool


from DerivationFrameworkHDBS.DerivationFrameworkHDBSConf import DerivationFramework__SkimmingToolHDBS3
HDBS3SelectionSkimmingTool = DerivationFramework__SkimmingToolHDBS3 ( name                    = "HDBS3SelectionSkimmingTool",
                                                                    FilterType              = "2L",
                                                                    MuonContainerKey        = "Muons",
                                                                    MuonPtCut               = 10.0*Units.GeV,
                                                                    ElectronContainerKey    = "Electrons",
                                                                    ElectronPtCut           = 10.0*Units.GeV,
                                                                    InvariantMassCut_min    = 50.0*Units.GeV,
                                                                    InvariantMassCut_max    = 106.0*Units.GeV
                                                                                                       )
ToolSvc += HDBS3SelectionSkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HDBS3Kernel",
                                                        AugmentationTools = AugmentationTools,
                                                        SkimmingTools = [HDBS3TriggerSkimmingTool,HDBS3SelectionSkimmingTool],            ##--SkimmingTools = [SkimmingToolPHYS],
                                                        ThinningTools = thinningTools)


#====================================================================
# FLAVOUR TAGGING   
#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 

largeRJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
]

addVRJets(SeqHDBS3, largeRJetCollections)
addVRJets(SeqHDBS3, largeRJetCollections, training='201903')

from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(SeqHDBS3, ToolSvc)

FlavorTagInit(JetCollections  = [ 'AntiKt4EMTopoJets','AntiKt4EMPFlowJets'], Sequencer = SeqHDBS3)

#====================================================================
# TC-LVT Vertices 
#====================================================================

from SoftBVrtClusterTool.SoftBVrtConfig import addSoftBVrt
addSoftBVrt(SeqHDBS3,'Loose')
addSoftBVrt(SeqHDBS3,'Medium')
addSoftBVrt(SeqHDBS3,'Tight')

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HDBS3SlimmingHelper = SlimmingHelper("HDBS3SlimmingHelper")

HDBS3SlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo_201810",
                                       "BTagging_AntiKt4EMPFlow_201810",
                                       "BTagging_AntiKt4EMPFlow_201903",
                                       "MET_Baseline_AntiKt4EMTopo",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                       "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                       "AntiKt4TruthJets"                                      
                                      ]
                                      
                                      
#HDBS3SlimmingHelper.AllVariables = ["TruthParticles"]    +++ "AntiKt4TruthJets" 

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]

HDBS3SlimmingHelper.StaticContent = StaticContent

# Trigger content
HDBS3SlimmingHelper.IncludeTriggerNavigation = True
HDBS3SlimmingHelper.IncludeJetTriggerContent = False
HDBS3SlimmingHelper.IncludeMuonTriggerContent = True
HDBS3SlimmingHelper.IncludeEGammaTriggerContent = True
HDBS3SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
HDBS3SlimmingHelper.IncludeTauTriggerContent = False
HDBS3SlimmingHelper.IncludeEtMissTriggerContent = False
HDBS3SlimmingHelper.IncludeBJetTriggerContent = False
HDBS3SlimmingHelper.IncludeBPhysTriggerContent = False
HDBS3SlimmingHelper.IncludeMinBiasTriggerContent = False

# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(HDBS3SlimmingHelper,["PHYS"])

# Truth containers
if DerivationFrameworkHasTruth:
   HDBS3SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                            'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                            'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                            'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthCharm':'xAOD::TruthParticleContainer','TruthCharmAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                            'AntiKt10TruthSoftDropBeta100Zcut10Jets':'xAOD::JetContainer', 'AntiKt10TruthSoftDropBeta100Zcut10JetsAux':'xAOD::JetAuxContainer'
                                           }

 
HDBS3SlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "AntiKt10TruthSoftDropBeta100Zcut10Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "AntiKt4EMPFlowJets.EMFrac.HECFrac.LArQuality.HECQuality.FracSamplingMax.NegativeE.AverageLArQF.FracSamplingMaxIndex.HadronConeExclTruthLabelID.GhostTrack",
                                      "TruthPrimaryVertices.t.x.y.z"]

# Saves the BDT output scores for the PLV algorithms.
# Can specify just electrons or just muons by adding 'name="Electrons"' or 'name="Muons"' as the argument.
HDBS3SlimmingHelper.ExtraVariables += PLVConfig.GetExtraPromptVariablesForDxAOD(addSpectators=False, onlyBDT=True)

# Add trigger matching
trigmatching_helper_notau.add_to_slimming(HDBS3SlimmingHelper)
trigmatching_helper_tau.add_to_slimming(HDBS3SlimmingHelper)

# Final construction of output stream
HDBS3SlimmingHelper.AppendContentToStream(HDBS3Stream)

