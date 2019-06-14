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

PHYS_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYSTrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYSTrackParticleThinningTool",
                                                                           ThinningService         = PHYSThinningHelper.ThinningSvc(),
                                                                           SelectionString         = PHYS_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ApplyAnd                = False)

ToolSvc += PHYSTrackParticleThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYSKernel",
                                                                       ThinningTools = [PHYSTrackParticleThinningTool],
                                                                       AugmentationTools = [PHYS_trigmatching_helper.matching_tool])
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

#updateJVT_xAODColl("AntiKt4EMTopo")
#addAntiKt4LowPtJets(DerivationFrameworkJob,"PHYS")

#====================================================================
# FLAVOUR TAGGING   
#====================================================================
# alias for VR
#BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

addDefaultTrimmedJets(DerivationFrameworkJob,"PHYS",dotruth=True)

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(DerivationFrameworkJob)
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
PHYSSlimmingHelper = SlimmingHelper("PHYSSlimmingHelper")

PHYSSlimmingHelper.SmartCollections = ["Electrons", 
                                       "Photons", 
                                       "Muons", 
                                       "PrimaryVertices", 
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4LCTopoJets", 
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4LCTopo",
                                       "MET_Reference_AntiKt4EMPFlow",
                                       "TauJets",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",      
                                       "AntiKt4TruthWZJets",
                                       "AntiKt4TruthJets",
                                       "BTagging_AntiKt2Track"
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
                                        }
PHYSSlimmingHelper.AllVariables = ["MET_Truth",
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

PHYSSlimmingHelper.ExtraVariables = ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2",
                                     "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.weights.crossSection",
                                     "AntiKt2PV0TrackJets.pt.eta.phi.m"]

# Add trigger matching
PHYS_trigmatching_helper.add_to_slimming(PHYSSlimmingHelper)

# Final construction of output stream
PHYSSlimmingHelper.AppendContentToStream(PHYSStream)

