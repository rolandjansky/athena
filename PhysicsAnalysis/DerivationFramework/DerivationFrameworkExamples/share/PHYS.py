#====================================================================
# DAOD_PHYS.py
# This defines DAOD_PHYS, which is a prototype for a possible new 
# unskimmed format for Run 3
# It requires the reductionConf flag PHYS in Reco_tf.py   
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


#====================================================================
# TRIGGER CONTENT   
#====================================================================
# See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
# Get single and multi mu, e, photon triggers
# Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el = TriggerAPI.getAllHLT(allperiods, triggerType=TriggerType.el,livefraction=0.9)
trig_mu = TriggerAPI.getAllHLT(allperiods, triggerType=TriggerType.mu,livefraction=0.9)
trig_g  = TriggerAPI.getAllHLT(allperiods, triggerType=TriggerType.g, livefraction=0.9)

trigger_names = []
for item in trig_el,trig_mu,trig_g:
   for triggers in item.keys(): trigger_names += item.keys()     

# Remove duplicates
trigger_names = list(dict.fromkeys(trigger_names))

# Create trigger matching decorations
PHYS_trigmatching_helper = TriggerMatchingHelper(matching_tool = "PHYSTriggerMatchingTool",
                                                 trigger_list = trigger_names)

#====================================================================
# SET UP STREAM   
#====================================================================

streamName = derivationFlags.WriteDAOD_PHYSStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSStream )
PHYSStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSStream.AcceptAlgs(["PHYSKernel"])

#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================

# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
PHYSThinningHelper = ThinningHelper( "PHYSThinningHelper" )
PHYSThinningHelper.AppendToStream( PHYSStream )

# Inner detector group recommendations for indet tracks in analysis
PHYS_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYSTrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYSTrackParticleThinningTool",
                                                                           ThinningService         = PHYSThinningHelper.ThinningSvc(),
                                                                           SelectionString         = PHYS_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ApplyAnd                = False)

ToolSvc += PHYSTrackParticleThinningTool

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
PHYSMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "PHYSMuonTPThinningTool",
                                                                        ThinningService         = PHYSThinningHelper.ThinningSvc(),
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd = False)
ToolSvc += PHYSMuonTPThinningTool

# TauJets thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSTauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSTauJetsThinningTool",
                                                                     ThinningService = PHYSThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks<6)")
ToolSvc += PHYSTauJetsThinningTool

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
PHYSTauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "PHYSTauTPThinningTool",
                                                                      ThinningService        = PHYSThinningHelper.ThinningSvc(),
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks<6)",
                                                                      ApplyAnd               = False,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += PHYSTauTPThinningTool

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
PHYSDiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYSDiTauTPThinningTool",
                                                                          ThinningService         = PHYSThinningHelper.ThinningSvc(),
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += PHYSDiTauTPThinningTool

#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYS"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PHYS"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")
   reducedJetList.append("AntiKt4TruthWZJets")
   reducedJetList.append("AntiKt4TruthJets")  
replaceAODReducedJets(reducedJetList,DerivationFrameworkJob,"PHYS")
addDefaultTrimmedJets(DerivationFrameworkJob,"PHYS",dotruth=DerivationFrameworkIsMonteCarlo)

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=DerivationFrameworkJob,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=DerivationFrameworkJob,algname="QGTaggerToolPFAlg")

#====================================================================
# Tau   
#====================================================================

# Schedule low-pt di-tau reconstruction (needs AntiKt2PV0TrackJets)
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt()

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYSDiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYSDiTauLowPtThinningTool",
                                                                        ThinningService = PHYSThinningHelper.ThinningSvc(),
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYSDiTauLowPtThinningTool

# ID tracks associated with low-pt ditau
PHYSDiTauLowPtTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYSDiTauLowPtTPThinningTool",
                                                                               ThinningService         = PHYSThinningHelper.ThinningSvc(),
                                                                               DiTauKey                = "DiTauJetsLowPt",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYSDiTauLowPtTPThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYSKernel",
                                                                       ThinningTools = [PHYSTrackParticleThinningTool,PHYSMuonTPThinningTool,
                                                                                        PHYSTauJetsThinningTool,PHYSTauTPThinningTool,
                                                                                        PHYSDiTauTPThinningTool,PHYSDiTauLowPtThinningTool,PHYSDiTauLowPtTPThinningTool],
                                                                       AugmentationTools = [PHYS_trigmatching_helper.matching_tool])


#====================================================================
# FLAVOUR TAGGING   
#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(DerivationFrameworkJob)
#addVRJetsTCC(DerivationFrameworkJob, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
#             VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
#             ghostArea = 0 , ptmin = 2000, ptminFilter = 2000,
#             variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")
# add xbb taggers
from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(DerivationFrameworkJob, ToolSvc)

FlavorTagInit(JetCollections  = [ 'AntiKt4EMTopoJets','AntiKt4EMPFlowJets'], Sequencer = DerivationFrameworkJob)

#====================================================================
# Truth collections
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles
   import DerivationFrameworkHiggs.TruthCategories 
   addStandardTruthContents()
   addMiniTruthCollectionLinks()
   addHFAndDownstreamParticles()

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYSSlimmingHelper = SlimmingHelper("PHYSSlimmingHelper")

PHYSSlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo_201810",
                                       "BTagging_AntiKt4EMPFlow_201810",
                                       "BTagging_AntiKt4EMPFlow_201903",
                                       "MET_Reference_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       "AntiKt4TruthWZJets",
                                       "AntiKt4TruthJets"
                                      ]


# Trigger content
PHYSSlimmingHelper.IncludeTriggerNavigation = False
PHYSSlimmingHelper.IncludeJetTriggerContent = False
PHYSSlimmingHelper.IncludeMuonTriggerContent = True
PHYSSlimmingHelper.IncludeEGammaTriggerContent = True
PHYSSlimmingHelper.IncludeJetTauEtMissTriggerContent = False
PHYSSlimmingHelper.IncludeTauTriggerContent = True
PHYSSlimmingHelper.IncludeEtMissTriggerContent = False
PHYSSlimmingHelper.IncludeBJetTriggerContent = False
PHYSSlimmingHelper.IncludeBPhysTriggerContent = False
PHYSSlimmingHelper.IncludeMinBiasTriggerContent = True

# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(PHYSSlimmingHelper,["PHYS"])

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   PHYSSlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                            'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                            'MET_TruthRegions':'xAOD::MissingETContainer','MET_TruthRegionsAux':'xAOD::MissingETAuxContainer',
                                            'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthWbosonWithDecayParticles':'xAOD::TruthParticleContainer','TruthWbosonWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthWbosonWithDecayVertices':'xAOD::TruthVertexContainer','TruthWbosonWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(PHYSSlimmingHelper)
   PHYSSlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices']

PHYSSlimmingHelper.ExtraVariables = ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2",
                                     "Electrons.TruthLink",
                                     "Muons.TruthLink",
                                     "Photons.TruthLink",
                                     "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                     "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID",
                                     "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID"]

# Add trigger matching
PHYS_trigmatching_helper.add_to_slimming(PHYSSlimmingHelper)

# Final construction of output stream
PHYSSlimmingHelper.AppendContentToStream(PHYSStream)

