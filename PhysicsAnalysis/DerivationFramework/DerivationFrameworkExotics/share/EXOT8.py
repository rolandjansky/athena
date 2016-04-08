#
# EXOT8.py
# reductionConf flag EXOT8 in Reco_tf.py   
#
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon            import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon    import *
from DerivationFrameworkJetEtMiss.METCommon            import *
from DerivationFrameworkEGamma.EGammaCommon            import *
from DerivationFrameworkMuons.MuonsCommon              import *

from DerivationFrameworkCore.WeightMetadata import *

exot8Seq = CfgMgr.AthSequencer("EXOT8Sequence")

from JetRec.JetRecFlags import jetFlags

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
                                                                         ConeSize               = 0.4)
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
                                                                               ConeSize               = 0.4)
ToolSvc += EXOT8ElectronTPThinningTool
thinningTools.append(EXOT8ElectronTPThinningTool)

#########################################
# Tracks associated with R=0.4 jets
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT8Ak4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "EXOT8Ak4JetTPThinningTool",
                                                                          ThinningService        = "EXOT8ThinningSvc",
                                                                          JetKey                 = "AntiKt4LCTopoJets",
                                                                          SelectionString        = "AntiKt4LCTopoJets.pt > 50*GeV && AntiKt4LCTopoJets.eta > -2.7 && AntiKt4LCTopoJets.eta < 2.7",
                                                                          InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT8Ak4JetTPThinningTool
thinningTools.append(EXOT8Ak4JetTPThinningTool)

############################################
# Tracks associated with large-R jets (0.2)
############################################
EXOT8Ak10r2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "EXOT8Ak10r2JetTPThinningTool",
                                                                             ThinningService        = "EXOT8ThinningSvc",
                                                                             JetKey                 = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                             SelectionString        = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta > -2.7 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta < 2.7",
                                                                             InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT8Ak10r2JetTPThinningTool
thinningTools.append(EXOT8Ak10r2JetTPThinningTool)

#########################################
# clusters thinning associated R=0.4 jets
#########################################
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT8Ak4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                   = "EXOT8Ak4CCThinningTool",
                                                                     ThinningService        = "EXOT8ThinningSvc",
                                                                     SGKey                  = "AntiKt4LCTopoJets",
                                                                     TopoClCollectionSGKey  = "CaloCalTopoClusters",
                                                                     SelectionString        = "AntiKt4LCTopoJets.pt > 50*GeV",
                                                                     ConeSize               = 0)
ToolSvc += EXOT8Ak4CCThinningTool
thinningTools.append(EXOT8Ak4CCThinningTool)

#############################################
# clusters associated with large-R jets (0.2)
#############################################
EXOT8Ak10r2CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT8Ak10r2CCThinningTool",
                                                                        ThinningService         = "EXOT8ThinningSvc",
                                                                        SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                        TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                        SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV",
                                                                        ConeSize                = 0)
ToolSvc += EXOT8Ak10r2CCThinningTool
thinningTools.append(EXOT8Ak10r2CCThinningTool)

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

#========================================================================================================================================
# OPENING ANGLE TOOL
#========================================================================================================================================
## from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
## EXOT8DeltaRTool = DerivationFramework__DeltaRTool( name = "EXOT8DeltaRTool",
##                                                    StoreGateEntryName = "dRkt4kt4",
##                                                    ObjectRequirements = "(AntiKt4LCTopoJets.pt > 50*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.7)",
##                                                    ContainerName = "AntiKt4LCTopoJets",
##                                                  )
## ToolSvc += EXOT8DeltaRTool
   
#========================================================================================================================================
# Triggers (https://indico.cern.ch/event/403233/contribution/4/material/slides/0.pdf)
#
# Not yet known: 25ns menu, L1_HT menu
#========================================================================================================================================
triggers = ["L1_J50",
            "L1_J75",
            "L1_J100",
            "L1_J120",

            "L1_3J40",
            "L1_3J50",
            "L1_3J75",

            "L1_4J15",
            "L1_4J20",
            "L1_3J25.0ETA23",
            "L1_4J15.0ETA25",
            "L1_J75_3J20"
            ]
#trigger = " || ".join(triggers)

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

mv2calo = -10.0 
beamEnergy = jobproperties.Beam.energy()
if (beamEnergy < 4.1e+06):
    resolved_4jetsLCTopo = "count((AntiKt4LCTopoJets.pt > 25*GeV) && (abs(AntiKt4LCTopoJets.eta) < 2.8)) >= 4"
    resolved_4jetsEMTopo = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) >= 4"
else:
    resolved_4jetsLCTopo = "count((AntiKt4LCTopoJets.pt > 25*GeV) && (abs(AntiKt4LCTopoJets.eta) < 2.8)) >= 4 && count(BTagging_AntiKt4LCTopo.MV2c20_discriminant > %s) >= 2" % mv2calo
    resolved_4jetsEMTopo = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) >= 4 && count(BTagging_AntiKt4EMTopo.MV2c20_discriminant > %s) >= 2" % mv2calo
resolved_4jet        = "(%s) || (%s)" % (resolved_4jetsLCTopo, resolved_4jetsEMTopo)

resolved_2jetsLCTopo = "count((AntiKt4LCTopoJets.pt > 25*GeV) && (abs(AntiKt4LCTopoJets.eta) < 2.8)) >= 2"
resolved_2jetsEMTopo = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) >= 2"
resolved_2jet        = "(%s) || (%s)" % (resolved_2jetsLCTopo, resolved_2jetsEMTopo)

singleElectron       = "count((Electrons.Tight)        && (Electrons.pt > 20*GeV) && (abs(Electrons.eta) < 2.5)) >= 1"
singleMuon           = "count((Muons.DFCommonGoodMuon) && (Muons.pt     > 25*GeV) && (abs(Muons.eta)     < 2.5)) >= 1"
singleLepton         = "(%s) || (%s)" % (singleElectron, singleMuon)

boosted_1LargeR_R20  = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"
boosted_1LargeR      = "(%s)" % (boosted_1LargeR_R20)

mv2track = -10.0
boosted_2LargeR_R20  = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 2"
boosted_trackjet_R20 = "count(BTagging_AntiKt2Track.MV2c20_discriminant > %s) >= 2" % mv2track
boosted_trackjet_R30 = "count(BTagging_AntiKt3Track.MV2c20_discriminant > %s) >= 2" % mv2track
if (beamEnergy < 4.1e+06):
    boosted_2LargeR      = "(%s)" % (boosted_2LargeR_R20)
else:
    boosted_2LargeR      = "(%s) && ((%s) || (%s))" % (boosted_2LargeR_R20, boosted_trackjet_R20, boosted_trackjet_R30) 

#eventSkim_zeroLepton   = "(%s) && ((%s) || (%s))" % (trigger, resolved_4jet, boosted_2LargeR)
eventSkim_zeroLepton   = "((%s) || (%s))" % (resolved_4jet, boosted_2LargeR)
eventSkim_singleLepton = "(%s) && ((%s) || (%s))" % (singleLepton, resolved_2jet, boosted_1LargeR)
eventSkim              = "(%s) || (%s)"           % (eventSkim_zeroLepton, eventSkim_singleLepton)

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

addDefaultTrimmedJets(exot8Seq, "EXOT8")
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot8Seq)
#applyJetCalibration_xAODColl("AntiKt4LCTopo", exot8Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot8Seq)

exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel_skim",
                                                         SkimmingTools = [EXOT8SkimmingTool]
                                                        )

exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel", ThinningTools = thinningTools)

#========================================================================================================================================
# Set up Stream
#========================================================================================================================================
streamName  = derivationFlags.WriteDAOD_EXOT8Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_EXOT8Stream )
EXOT8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT8Stream.AcceptAlgs(["EXOT8Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT8ThinningSvc", outStreams=[evtStream] )

#
# Add the containers to the output stream - slimming done here
#
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT8SlimmingHelper = SlimmingHelper("EXOT8SlimmingHelper")

# from DerivationFrameworkExotics.EXOT8ContentList import EXOT8AllVariablesContent, EXOT8SmartContent, EXOT8Content
# EXOT8SlimmingHelper.SmartCollections = EXOT8SmartContent
# EXOT8SlimmingHelper.AllVariables     = EXOT8AllVariablesContent
# EXOT8SlimmingHelper.UserContent      = EXOT8Content

EXOT8SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                        "AntiKt4LCTopoJets",
                                        "BTagging_AntiKt4LCTopo",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKt2Track",
                                        "BTagging_AntiKt3Track",
                                        "BTagging_AntiKt4Track",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "Electrons",
                                        "Muons",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMTopo",
                                        ]
EXOT8SlimmingHelper.ExtraVariables = ["Electrons.charge", "Muons.charge"]
EXOT8SlimmingHelper.AllVariables   = ["TruthParticles",
                                      "TruthEvents",
                                      "TruthVertices",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt2PV0TrackJets",
                                      "AntiKt3PV0TrackJets",
                                      "AntiKt4PV0TrackJets",
                                      #"BTagging_AntiKt4TruthWZ",    #removed in new tags of MC15
                                      "AntiKt4TruthJets",
                                      "AntiKt10TruthJets",
                                      "CombinedMuonTrackParticles",
                                      "ExtrapolatedMuonTrackParticles",
                                      ]

EXOT8SlimmingHelper.StaticContent = [
                                     "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
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

