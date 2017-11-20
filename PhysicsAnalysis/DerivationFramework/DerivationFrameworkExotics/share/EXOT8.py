#********************************************************************
# EXOT8.py
# reductionConf flag EXOT8 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon            import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon    import *
from DerivationFrameworkJetEtMiss.METCommon            import *
from DerivationFrameworkEGamma.EGammaCommon            import *
from DerivationFrameworkMuons.MuonsCommon              import *
from DerivationFrameworkFlavourTag.HbbCommon           import *
from DerivationFrameworkCore.WeightMetadata            import *

from JetRec.JetRecFlags import jetFlags

exot8Seq = CfgMgr.AthSequencer("EXOT8Sequence")

#========================================================================================================================================
# Thinning Tools
#========================================================================================================================================
thinningTools=[]

#########################################
# Tracks associated with Muons
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "EXOT8MuonTPThinningTool",
                                                                         ThinningService        = "EXOT8ThinningSvc",
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         SelectionString        = "Muons.pt > 5*GeV",
                                                                         ConeSize               = 0)
ToolSvc += EXOT8MuonTPThinningTool
thinningTools.append(EXOT8MuonTPThinningTool)

#########################################
# Tracks associated with Electrons
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "EXOT8ElectronTPThinningTool",
                                                                               ThinningService        = "EXOT8ThinningSvc",
                                                                               SGKey                  = "Electrons",
                                                                               InDetTrackParticlesKey = "InDetTrackParticles",
                                                                               SelectionString        = "Electrons.pt > 25*GeV",
                                                                               ConeSize               = 0)
ToolSvc += EXOT8ElectronTPThinningTool
thinningTools.append(EXOT8ElectronTPThinningTool)

############################################
# Tracks associated with large-R jets (0.2)
############################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT8Ak10r2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "EXOT8Ak10r2JetTPThinningTool",
                                                                             ThinningService        = "EXOT8ThinningSvc",
                                                                             JetKey                 = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                             SelectionString        = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta > -2.7 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta < 2.7",
                                                                             InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT8Ak10r2JetTPThinningTool
thinningTools.append(EXOT8Ak10r2JetTPThinningTool)

#########################################
# truth thinning
#########################################
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT8TruthTool = DerivationFramework__MenuTruthThinning(name                         = "EXOT8TruthTool",
                                                        ThinningService              = "EXOT8ThinningSvc",
                                                        WritePartons                 = False,
                                                        WriteHadrons                 = False,
                                                        WriteBHadrons                = True,
                                                        WriteGeant                   = False,
                                                        GeantPhotonPtThresh          = -1.0,
                                                        WriteTauHad                  = False,
                                                        PartonPtThresh               = -1.0,
                                                        WriteBSM                     = True,
                                                        WriteBosons                  = True,
                                                        WriteBSMProducts             = True,
                                                        WriteBosonProducts           = False,
                                                        WriteTopAndDecays            = True,
                                                        WriteEverything              = False,
                                                        WriteAllLeptons              = True,
                                                        WriteStatus3                 = True,
                                                        PreserveGeneratorDescendants = True,
                                                        WriteFirstN                  = -1)

if globalflags.DataSource()=="geant4":
    ToolSvc += EXOT8TruthTool
    thinningTools.append(EXOT8TruthTool)

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__BJetRegressionVariables
EXOT8BJetRegressionVariables = DerivationFramework__BJetRegressionVariables(name = "EXOT8BJetRegressionVariables",
                                                                            ContainerName = "AntiKt4EMTopoJets",
                                                                            AssociatedTracks = "GhostTrack",
                                                                            MinTrackPtCuts = [0])

ToolSvc += EXOT8BJetRegressionVariables
augmentationTools.append(EXOT8BJetRegressionVariables)
print EXOT8BJetRegressionVariables
   
#========================================================================================================================================
# Triggers (https://indico.cern.ch/event/403233/contribution/4/material/slides/0.pdf)
#
# Not yet known: 25ns menu, L1_HT menu
#========================================================================================================================================
triggers = ["L1_J50",
            "L1_J75",
            "L1_J100",
            "L1_J120",
            "L1_2J50_XE40",

            "L1_3J40",
            "L1_3J50",
            "L1_3J75",

            "L1_4J15",
            "L1_4J20",
            "L1_3J25.0ETA23",
            "L1_4J15.0ETA25",
            "L1_J75_3J20",

            # Single Leptons
            "L1_EM22VHI",
            "L1_EM24VHI",
            "L1_EM26VHI",
            "L1_MU20",            
            ]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT8TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT8TriggerSkimmingTool",
                                                                     TriggerListAND = [],
                                                                     TriggerListOR  = triggers)
ToolSvc += EXOT8TriggerSkimmingTool

#========================================================================================================================================
# Event Skimming
#
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTaggingBenchmarks
#========================================================================================================================================

mv2calo = 0.1758475
mv2calo_tagger = "MV2c10_discriminant"

resolved_4jetsEMTopo = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) >= 4 && count(BTagging_AntiKt4EMTopo."+mv2calo_tagger+" > %s) >= 2" % mv2calo
resolved_4jet        = resolved_4jetsEMTopo

resolved_2jetsEMTopo = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) >= 2 && count(BTagging_AntiKt4EMTopo."+mv2calo_tagger+" > %s) >= 2" % mv2calo
resolved_2jet        = resolved_2jetsEMTopo

singleElectron       = "count((Electrons.Tight)        && (Electrons.pt > 25*GeV) && (abs(Electrons.eta) < 2.5)) >= 1"
singleMuon           = "count((Muons.DFCommonGoodMuon) && (Muons.pt     > 25*GeV) && (abs(Muons.eta)     < 2.5)) >= 1"
singleLepton         = "(%s) || (%s)" % (singleElectron, singleMuon)

boosted_1LargeR_R20  = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"
boosted_1LargeR      = "(%s)" % (boosted_1LargeR_R20)

mv2track = -10.0
mv2track_tagger = "MV2c10_discriminant"
boosted_2LargeR_R20  = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 2"
boosted_trackjet_R20 = "count(BTagging_AntiKt2Track."+mv2track_tagger+" > %s) >= 2" % mv2track

boosted_2LargeR      = "(%s) && (%s)" % (boosted_2LargeR_R20, boosted_trackjet_R20)

#eventSkim_zeroLepton   = "(%s) && ((%s) || (%s))" % (trigger, resolved_4jet, boosted_2LargeR)
eventSkim_zeroLepton   = "((%s) || (%s))" % (resolved_4jet, boosted_2LargeR)
eventSkim_singleLepton = "(%s) && ((%s) || (%s))" % (singleLepton, resolved_2jet, boosted_1LargeR)


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT8SkimmingTool_sl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_sl", expression = eventSkim_singleLepton)
ToolSvc += EXOT8SkimmingTool_sl

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT8SkimmingTool_zl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_zl", expression = eventSkim_zeroLepton)
ToolSvc += EXOT8SkimmingTool_zl

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
EXOT8ANDSkimmingTool_zl = DerivationFramework__FilterCombinationAND(name = "EXOT8ANDSkimmingTool_zl", FilterList = [EXOT8TriggerSkimmingTool, EXOT8SkimmingTool_zl])
ToolSvc += EXOT8ANDSkimmingTool_zl

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EXOT8SkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT8SkimmingTool", FilterList = [EXOT8ANDSkimmingTool_zl, EXOT8SkimmingTool_sl])
ToolSvc += EXOT8SkimmingTool


#========================================================================================================================================
# Create the derivation kernel alg
#========================================================================================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot8Seq

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT8"] = []
reducedJetList = [
    "AntiKt2PV0TrackJets", #flavour-tagged automatically
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot8Seq,"EXOT8")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot8Seq,"EXOT8")

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
# A wrapper function which does all the necessary steps
addVRJets(exot8Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet", 
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", 
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000, 
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")


#b-tagging

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ["AntiKtVR30Rmax4Rmin02TrackJets"], Sequencer = exot8Seq)

#====================================================================
# Apply jet calibration
#====================================================================
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot8Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot8Seq)

exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel_skim",SkimmingTools = [EXOT8SkimmingTool])
exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel", ThinningTools = thinningTools,
                                                                        AugmentationTools = augmentationTools)


#========================================================================================================================================
# Set up Stream
#========================================================================================================================================
streamName  = derivationFlags.WriteDAOD_EXOT8Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_EXOT8Stream )
EXOT8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT8Stream.AddItem("xAOD::JetContainer_v1#*")
EXOT8Stream.AddItem("xAOD::JetAuxContainer_v1#*")
EXOT8Stream.AcceptAlgs(["EXOT8Kernel"])


# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT8ThinningSvc", outStreams=[evtStream] )

# Add the containers to the output stream - slimming done here
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT8SlimmingHelper = SlimmingHelper("EXOT8SlimmingHelper")

EXOT8SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKt2Track",
                                        "PrimaryVertices",
                                        "Electrons",
                                        "Muons",
                                        "MET_Reference_AntiKt4EMTopo",
                                        ]

EXOT8SlimmingHelper.ExtraVariables = ["Electrons.charge", 
                                      "Muons.charge", 
                                      "AntiKt4EMTopoJets.DFCommonJets_TrackSumMass",
                                      "AntiKt4EMTopoJets.DFCommonJets_TrackSumPt",
                                      "AntiKt4EMTopoJets.TrackSumPt",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkPt0",
                                      "AntiKt4EMTopoJets.VecSumPtTrkPt0",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMTopoJets.VecSumPtTrkCleanPt0PV0",
                                      "BTagging_AntiKt4EMTopo.JetVertexCharge_discriminant",
                                      "BTagging_AntiKt4EMTopo.SV1_normdist",
                                      "BTagging_AntiKt4EMTopo.SV1_masssvx",
                                      "BTagging_AntiKt2Track.JetVertexCharge_discriminant",
                                      
                                      ]

EXOT8SlimmingHelper.AllVariables   = ["TruthParticles",
                                      "TruthEvents",
                                      "TruthVertices",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt2PV0TrackJets",
                                      "AntiKt4PV0TrackJets",
                                      "AntiKt4TruthJets",
                                      "AntiKt4TruthWZJets",
                                      "AntiKt10TruthJets",
                                      "CombinedMuonTrackParticles",
                                      "ExtrapolatedMuonTrackParticles",
                                      ]
                                      
EXOT8SlimmingHelper.StaticContent = [
                                     "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
                                    ]

# Add VR track-jet collection and its b-tagging container to output stream
EXOT8SlimmingHelper.AppendToDictionary = {
    "AntiKtVR30Rmax4Rmin02TrackJets"            :   "xAOD::JetContainer"        ,
    "AntiKtVR30Rmax4Rmin02TrackJetsAux"         :   "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track"       :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"    :   "xAOD::BTaggingAuxContainer",
}
                               
# Add all variabless for VR track-jets
EXOT8SlimmingHelper.AllVariables  += ["AntiKtVR30Rmax4Rmin02TrackJets"]
EXOT8SlimmingHelper.SmartCollections  += ["BTagging_AntiKtVR30Rmax4Rmin02Track"]

# Save certain b-tagging variables for VR track-jet
EXOT8SlimmingHelper.ExtraVariables += [
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP",
]


if globalflags.DataSource()=="geant4":
    EXOT8SlimmingHelper.StaticContent += [
                                     "xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.",
                                     ]

EXOT8SlimmingHelper.IncludeJetTriggerContent = True
EXOT8SlimmingHelper.IncludeBJetTriggerContent = True
EXOT8SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT8SlimmingHelper.IncludeMuonTriggerContent = True
EXOT8SlimmingHelper.AppendContentToStream(EXOT8Stream)
