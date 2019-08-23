#====================================================================
# DAOD_PHYS2.py
# This defines DAOD_PHYS2, which is a prototype for a possible new 
# unskimmed format for Run 3
# It is a clone of DAOD_PHYS but also includes lossy compression
# In the end it will be merged back into DAOD_PHYS
# It requires the reductionConf flag PHYS2 in Reco_tf.py   
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
PHYS2_trigmatching_helper = TriggerMatchingHelper(matching_tool = "PHYS2TriggerMatchingTool",
                                                 trigger_list = trigger_names)

#====================================================================
# SET UP STREAM   
#====================================================================

streamName = derivationFlags.WriteDAOD_PHYS2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYS2Stream )
PHYS2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYS2Stream.AcceptAlgs(["PHYS2Kernel"])

#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================

# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
PHYS2ThinningHelper = ThinningHelper( "PHYS2ThinningHelper" )
PHYS2ThinningHelper.AppendToStream( PHYS2Stream )

# Inner detector group recommendations for indet tracks in analysis
PHYS2_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYS2TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYS2TrackParticleThinningTool",
                                                                           ThinningService         = PHYS2ThinningHelper.ThinningSvc(),
                                                                           SelectionString         = PHYS2_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ApplyAnd                = False)

ToolSvc += PHYS2TrackParticleThinningTool

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
PHYS2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "PHYS2MuonTPThinningTool",
                                                                        ThinningService         = PHYS2ThinningHelper.ThinningSvc(),
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd = False)
ToolSvc += PHYS2MuonTPThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYS2Kernel",
                                                                       ThinningTools = [PHYS2TrackParticleThinningTool,PHYS2MuonTPThinningTool],
                                                                       AugmentationTools = [PHYS2_trigmatching_helper.matching_tool])
#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYS2"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PHYS2"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")
   reducedJetList.append("AntiKt4TruthWZJets")
   reducedJetList.append("AntiKt4TruthJets")  
replaceAODReducedJets(reducedJetList,DerivationFrameworkJob,"PHYS2")
addDefaultTrimmedJets(DerivationFrameworkJob,"PHYS2",dotruth=DerivationFrameworkIsMonteCarlo)

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=DerivationFrameworkJob,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=DerivationFrameworkJob,algname="QGTaggerToolPFAlg")




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
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
   addStandardTruthContents()

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYS2SlimmingHelper = SlimmingHelper("PHYS2SlimmingHelper")

PHYS2SlimmingHelper.SmartCollections = ["Electrons", 
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
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",      
                                       "AntiKt4TruthWZJets",
                                       "AntiKt4TruthJets"
                                      ]


# Trigger content
PHYS2SlimmingHelper.IncludeTriggerNavigation = False
PHYS2SlimmingHelper.IncludeJetTriggerContent = False
PHYS2SlimmingHelper.IncludeMuonTriggerContent = True
PHYS2SlimmingHelper.IncludeEGammaTriggerContent = True
PHYS2SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
PHYS2SlimmingHelper.IncludeTauTriggerContent = True
PHYS2SlimmingHelper.IncludeEtMissTriggerContent = False
PHYS2SlimmingHelper.IncludeBJetTriggerContent = False
PHYS2SlimmingHelper.IncludeBPhysTriggerContent = False
PHYS2SlimmingHelper.IncludeMinBiasTriggerContent = True

# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(PHYS2SlimmingHelper,["PHYS2"])

# Truth containers
PHYS2SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                        }
PHYS2SlimmingHelper.AllVariables = ["MET_Truth",
                                   "MET_TruthRegions",
                                   "TruthElectrons",
                                   "TruthMuons",
                                   "TruthPhotons",
                                   "TruthTaus",
                                   "TruthNeutrinos",
                                   "TruthBSM",
                                   "TruthTop",
                                   "TruthBoson",
                                   "TruthWbosonWithDecayParticles",
                                   "TruthWbosonWithDecayVertices"]

PHYS2SlimmingHelper.ExtraVariables = ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2",
                                     "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.weights.crossSection",
                                     "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                     "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID",
                                     "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID"]

# Float compression
from DerivationFrameworkExamples.PHYS_MCCompressionList import PHYS_MCCompressionList
from DerivationFrameworkExamples.PHYS_DataCompressionList import PHYS_DataCompressionList
PHYS2_compression_list = []
if (DerivationFrameworkIsMonteCarlo): PHYS2_compression_list = PHYS_MCCompressionList 
else: PHYS2_compression_list = PHYS_DataCompressionList
PHYS2_SeqCompress = CfgMgr.AthSequencer("PHYS2_SeqCompress")
DerivationFrameworkJob += PHYS2_SeqCompress
PHYS2_SeqCompress += CfgMgr.DerivationFramework__FloatCompressionAlg("CompressFloatCompression", SGKeys=PHYS2_compression_list)

# Add trigger matching
PHYS2_trigmatching_helper.add_to_slimming(PHYS2SlimmingHelper)

# Final construction of output stream
PHYS2SlimmingHelper.AppendContentToStream(PHYS2Stream)

