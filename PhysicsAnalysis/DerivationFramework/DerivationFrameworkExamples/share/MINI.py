#====================================================================
# DAOD_MINI.py
# This defines DAOD_MINI, which is a prototype for a possible new 
# unskimmed format for Run 3
# It requires the reductionConf flag MINI in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import *


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MINIKernel")

#====================================================================
# JET/MET   
#====================================================================

OutputJets["MINI"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]

reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["MINI"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")
   reducedJetList.append("AntiKt4TruthWZJets")
   reducedJetList.append("AntiKt4TruthJets")  
   replaceAODReducedJets(reducedJetList,DerivationFrameworkJob,"MINI")

updateJVT_xAODColl("AntiKt4EMTopo")
addAntiKt4LowPtJets(DerivationFrameworkJob,"MINI")

#====================================================================
# FLAVOUR TAGGING   
#====================================================================
# alias for VR
#BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

addDefaultTrimmedJets(DerivationFrameworkJob,"MINI",dotruth=True)

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
# SET UP STREAM   
#====================================================================

streamName = derivationFlags.WriteDAOD_MINIStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MINIStream )
MINIStream = MSMgr.NewPoolRootStream( streamName, fileName )
MINIStream.AcceptAlgs(["MINIKernel"])

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
MINISlimmingHelper = SlimmingHelper("MINISlimmingHelper")

MINISlimmingHelper.SmartCollections = ["Electrons", 
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
                                       "AntiKt4TruthJets"
                                      ]


#MINISlimmingHelper.ExtraVariables = [ "BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex",
#                                      "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx.-vxTrackAtVertex",
#                                      "BTagging_AntiKt4EMPFlowSecVtx.-vxTrackAtVertex"
#                                    ]

# Trigger content
MINISlimmingHelper.IncludeJetTriggerContent = True
MINISlimmingHelper.IncludeMuonTriggerContent = True
MINISlimmingHelper.IncludeEGammaTriggerContent = True
MINISlimmingHelper.IncludeJetTauEtMissTriggerContent = True
MINISlimmingHelper.IncludeJetTriggerContent = True
MINISlimmingHelper.IncludeTauTriggerContent = True
MINISlimmingHelper.IncludeEtMissTriggerContent = True
MINISlimmingHelper.IncludeBJetTriggerContent = True
MINISlimmingHelper.IncludeBPhysTriggerContent = True
MINISlimmingHelper.IncludeMinBiasTriggerContent = True

# Add the jet containers to the stream (defined in JetCommon if import needed)
addJetOutputs(MINISlimmingHelper,["MINI"])

# Truth containers
MINISlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
MINISlimmingHelper.AllVariables = ["MET_Truth",
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
MINISlimmingHelper.ExtraVariables = ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2",
                                     "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.weights.crossSection"]

# Final construction of output stream
MINISlimmingHelper.AppendContentToStream(MINIStream)

