#====================================================================
# JETM1.py 
# reductionConf flag JETM1 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
triggers = [
'L1_RD0_FILLED',
'HLT_j15',       'HLT_j15_320eta490',
'HLT_j25',       'HLT_j25_320eta490',
'HLT_j60',       'HLT_j60_320eta490',
'HLT_j35',       'HLT_j35_320eta490',
'HLT_j45_L1RDO', 
'HLT_j45',       'HLT_j45_320eta490',
'HLT_j55_L1RDO', 
'HLT_j55',       'HLT_j55_320eta490',
'HLT_j85_L1RDO',
'HLT_j85',       'HLT_j85_320eta490',
'HLT_j110',      'HLT_j110_320eta490',
'HLT_j35_j35_320eta490',
'HLT_j45_j45_320eta490',
'HLT_j55_j55_320eta490',
'HLT_j60_j60_320eta490',
'HLT_j85_j85_320eta490',
'HLT_j15_j15_320eta490',
'HLT_j25_j25_320eta490',
'HLT_j175',
'HLT_j260',
'HLT_j360',
'HLT_j175_320eta490',
'HLT_j260_320eta490',
'HLT_j360_320eta490'
]

# NOTE: need to be able to OR isSimulated as an OR with the trigger
orstr =' || '
trigger = '('+orstr.join(triggers)+')'
expression = trigger+' || (EventInfo.eventTypeBitmask==1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM1SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "JETM1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM1SkimmingTool

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"           # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"           # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))" # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                # Photon
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM1TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM1TruthThinningTool",
                                                                        ThinningService = "JETM1ThinningSvc",
                                                                        ParticlesKey = "TruthParticles",
                                                                        VerticesKey = "TruthVertices",
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM1TruthThinningTool
    thinningTools.append(JETM1TruthThinningTool)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm1Seq = CfgMgr.AthSequencer("JETM1Sequence")
DerivationFrameworkJob += jetm1Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm1Seq += CfgMgr.DerivationFramework__DerivationKernel("JETM1Kernel" ,
                                                         SkimmingTools = [JETM1SkimmingTool],
                                                         ThinningTools = thinningTools)


#====================================================================
# Special jets
#====================================================================

OutputJets["JETM1"] = []

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm1Seq,"JETM1")

if jetFlags.useTruth:
    # CamKt R=1.2 jets
    addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.15, algseq=jetm1Seq, outputGroup="JETM1")
    addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.04, algseq=jetm1Seq, outputGroup="JETM1")

# CamKt R=1.2 jets
addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, algseq=jetm1Seq, outputGroup="JETM1")
addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.04, algseq=jetm1Seq, outputGroup="JETM1")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM1Stream )
JETM1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM1Stream.AcceptAlgs(["JETM1Kernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM1SlimmingHelper = SlimmingHelper("JETM1SlimmingHelper")
JETM1SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "PrimaryVertices"]
JETM1SlimmingHelper.AllVariables = ["BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments"
                                    ]
#JETM1SlimmingHelper.ExtraVariables = []

# Trigger content
JETM1SlimmingHelper.IncludeJetTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM1SlimmingHelper,["SmallR","LargeR","JETM1"],[], # smart list
              ["AntiKt3PV0TrackJets",
               "AntiKt4PV0TrackJets",
               "AntiKt4TruthWZJets",
               "AntiKt10LCTopoJets",
               "AntiKt10TruthJets",
               "AntiKt10TruthWZJets",
               "CamKt12LCTopoJets",
               "CamKt12TruthJets",
               "CamKt12TruthWZJets"]# veto list
              )

JETM1SlimmingHelper.AppendContentToStream(JETM1Stream)
JETM1Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM1Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
