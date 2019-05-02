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


# Float compression
SeqNANO = CfgMgr.AthSequencer("SeqNANO")
DerivationFrameworkJob += SeqNANO
SeqNANO += CfgMgr.DerivationFramework__FloatCompressionAlg("NANOFloatCompression", SGKeys=[
#"HLT_TrigSpacePointCountsCollection_spacepoints",
"AntiKt10LCTopoJetsAux.", "AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.", "AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.", "AntiKt4EMPFlowJetsAux.", "AntiKt4EMTopoJetsAux.", "AntiKt4LCTopoJetsAux.", "AntiKt4TruthJetsAux.", "AntiKt4TruthWZJetsAux.",
"LVL1EnergySumRoIAux.",
#"HLT_TrigVertexCountsCollection_vertexcounts",
#"TauTracks",
#"Electrons", 
#"HLT_xAOD__ElectronContainer_egamma_Electrons",
#"HLT_xAOD__TrigSpacePointCountsContainer_spacepoints",
"EventInfoAux.",
#"BTagging_AntiKt4EMTopo", 
#"HLT_xAOD__BTaggingContainer_HLTBjetFex",
#"LVL1EmTauRoIs",                           
#"LVL1EnergySumRoI",                        
#"AntiKt10LCTopoJets", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "AntiKt10TruthTrimmedPtFrac5SmallR20Jets", "AntiKt4EMPFlowJets", "AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets", "HLT_xAOD__JetContainer_EFJet", "HLT_xAOD__JetContainer_GSCJet", "HLT_xAOD__JetContainer_SplitJet", "HLT_xAOD__JetContainer_a10r_tcemsubjesFS", "HLT_xAOD__JetContainer_a10r_tcemsubjesISFS", "HLT_xAOD__JetContainer_a10tclcwsubjesFS", "HLT_xAOD__JetContainer_a10ttclcwjesFS", "HLT_xAOD__JetContainer_a4tcemsubjesFS", "HLT_xAOD__JetContainer_a4tcemsubjesISFS",
#"LVL1JetEtRoI",                            
"LVL1JetEtRoIAux.",                        
#"CombinedMuonTrackParticles", "ExtrapolatedMuonTrackParticles", "GSFTrackParticles", "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig", "InDetForwardTrackParticles", "InDetTrackParticles", "MuonSpectrometerTrackParticles",
#"HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts",
#"HLT_xAOD__TrigVertexCountsContainer_vertexcounts",
#"LVL1MuonRoIs",                            
#"egammaClusters",                          
#"HLT_xAOD__TrigTrackCountsContainer_trackcounts",
#"TrigConfKeys",                            
#"HLT_xAOD__PhotonContainer_egamma_Photons", "Photons",
#"StreamDAOD_MINI",                         
"Kt4EMPFlowEventShapeAux.", "Kt4EMTopoOriginEventShapeAux.", "Kt4LCTopoOriginEventShapeAux.",
"TrigNavigationAux.",                      
#"HLT_xAOD__TauJetContainer_TrigTauRecMerged", "TauJets",
#"TrigNavigation",                          
#"Kt4EMPFlowEventShape", "Kt4EMTopoOriginEventShape", "Kt4LCTopoOriginEventShape",
#"HLT_xAOD__TrigMissingETContainer_TrigEFMissingET", "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht", "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
"xTrigDecisionAux.",                       
#"TruthBSM", "TruthBoson", "TruthElectrons", "TruthMuons", "TruthNeutrinos", "TruthPhotons", "TruthTaus", "TruthTop",
#"METAssoc_AntiKt4EMPFlow", "METAssoc_AntiKt4EMTopo", "METAssoc_AntiKt4LCTopo",
#"McEventInfo",                             
#"EventInfo",                               
#"MET_Core_AntiKt4EMPFlow", "MET_Core_AntiKt4EMTopo", "MET_Core_AntiKt4LCTopo", "MET_Reference_AntiKt4EMPFlow", "MET_Reference_AntiKt4EMTopo", "MET_Reference_AntiKt4LCTopo", "MET_Track", "MET_Truth", "MET_TruthRegions",
#"HLT_xAOD__TrigBphysContainer_EFBMuMuFex", "HLT_xAOD__TrigBphysContainer_EFBMuMuXFex", "HLT_xAOD__TrigBphysContainer_EFMultiMuFex", "HLT_xAOD__TrigBphysContainer_EFTrackMass", "HLT_xAOD__TrigBphysContainer_L2BMuMuFex",
#"HLT_xAOD__TrigT2ZdcSignalsContainer_zdcsignals",
#"LVL1JetRoIs",                             
"BTagging_AntiKt4EMTopoAux.", "CombinedMuonTrackParticlesAux.", "ElectronsAux.", 
#"ExtrapolatedMuonTrackParticlesAux.", 
"GSFConversionVerticesAux.", "GSFTrackParticlesAux.", 
#"HLT_xAOD__BTaggingContainer_HLTBjetFexAux.", 
"HLT_xAOD__ElectronContainer_egamma_ElectronsAux.", "HLT_xAOD__JetContainer_EFJetAux.", "HLT_xAOD__JetContainer_GSCJetAux.", "HLT_xAOD__JetContainer_SplitJetAux.", "HLT_xAOD__JetContainer_a10r_tcemsubjesFSAux.", "HLT_xAOD__JetContainer_a10r_tcemsubjesISFSAux.", "HLT_xAOD__JetContainer_a10tclcwsubjesFSAux.", "HLT_xAOD__JetContainer_a10ttclcwjesFSAux.", "HLT_xAOD__JetContainer_a4tcemsubjesFSAux.", "HLT_xAOD__JetContainer_a4tcemsubjesISFSAux.", "HLT_xAOD__MuonContainer_MuonEFInfoAux.", "HLT_xAOD__MuonContainer_MuonEFInfo_FullScanAux.", "HLT_xAOD__PhotonContainer_egamma_PhotonsAux.", 
#"HLT_xAOD__TauJetContainer_TrigTauRecMergedAux.", 
"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrigAux.", "HLT_xAOD__TrigBphysContainer_EFBMuMuFexAux.", "HLT_xAOD__TrigBphysContainer_EFBMuMuXFexAux.", "HLT_xAOD__TrigBphysContainer_EFMultiMuFexAux.", "HLT_xAOD__TrigBphysContainer_EFTrackMassAux.", "HLT_xAOD__TrigBphysContainer_L2BMuMuFexAux.", "HLT_xAOD__TrigMissingETContainer_TrigEFMissingETAux.", "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mhtAux.", "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUCAux.", "HLT_xAOD__TrigSpacePointCountsContainer_spacepointsAux.", "HLT_xAOD__TrigT2MbtsBitsContainer_T2MbtsAux.", "HLT_xAOD__TrigT2ZdcSignalsContainer_zdcsignalsAux.", "HLT_xAOD__TrigTrackCountsContainer_trackcountsAux.", "HLT_xAOD__TrigVertexCountsContainer_vertexcountsAux.", "HLT_xAOD__VertexContainer_EFHistoPrmVtxAux.", "HLT_xAOD__VertexContainer_xPrimVxAux.", "InDetForwardTrackParticlesAux.", "InDetTrackParticlesAux.", "LVL1EmTauRoIsAux.", "LVL1JetRoIsAux.", "LVL1MuonRoIsAux.", "MET_Core_AntiKt4EMPFlowAux.", "MET_Core_AntiKt4EMTopoAux.", "MET_Core_AntiKt4LCTopoAux.", "MET_Reference_AntiKt4EMPFlowAux.", "MET_Reference_AntiKt4EMTopoAux.", "MET_Reference_AntiKt4LCTopoAux.", "MET_TrackAux.", "MET_TruthAux.", "MET_TruthRegionsAux.", "MuonSpectrometerTrackParticlesAux.", "MuonsAux.", "PhotonsAux.", "PrimaryVerticesAux.", "TauJetsAux.", "TauTracksAux.", "TruthBSMAux.", "TruthBosonAux.", "TruthElectronsAux.", "TruthEventsAux.", "TruthMuonsAux.", "TruthNeutrinosAux.", "TruthPhotonsAux.", "TruthTausAux.", "TruthTopAux.", "egammaClustersAux.",
#"GSFConversionVertices", "HLT_xAOD__VertexContainer_EFHistoPrmVtx", "HLT_xAOD__VertexContainer_xPrimVx", "PrimaryVertices",
#"HLT_TrigTrackCountsCollection_trackcounts",
#"HLT_TrigT2MbtsBitsContainer_T2Mbts",      
#"HLT_xAOD__MuonContainer_MuonEFInfo", "HLT_xAOD__MuonContainer_MuonEFInfo_FullScan", "Muons",
"METAssoc_AntiKt4EMPFlowAux.", "METAssoc_AntiKt4EMTopoAux.", "METAssoc_AntiKt4LCTopoAux.",
#"xTrigDecision",                           
#"TruthEvents"                           
], 
Vars = [ "AntiKt4EMTopoJetsAux.pt:7", "AntiKt4EMTopoJetsAux.eta:16", "AntiKt4EMTopoJetsAux.phi:15",

])
