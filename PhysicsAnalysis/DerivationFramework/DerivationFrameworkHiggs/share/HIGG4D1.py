#********************************************************************
# HIGG4D1.py 
# reductionConf flag HIGG4D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *



#====================================================================
# Special jets
#====================================================================
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

if not "MyJets" in OutputJets:
    OutputJets["MyJets"] = []
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, algseq=DerivationFrameworkJob)
    OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets")])
    


#====================================================================
# TAU SELECTOR TOOL 
#====================================================================
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
HIGG4D1TauMedWrapper = DerivationFramework__TauSelectionWrapper(	name 			= "HIGG4D1TauSelectionWrapper",
                                                                        #IsTauFlag		= IsTauFlag.JetBDTSigMedium,
                                                                        IsTauFlag		= 20,
                                                                        CollectionName		= "TauJets",
                                                                        StoreGateEntryName	= "HIGG4D1JetBDTSigMedium")
ToolSvc += HIGG4D1TauMedWrapper

HIGG4D1TauLooseWrapper = DerivationFramework__TauSelectionWrapper(        name                    = "HIGG4D1TauSelectionWrapper",
                                                                          IsTauFlag               = 19,
                                                                          CollectionName          = "TauJets",
                                                                          StoreGateEntryName      = "HIGG4D1JetBDTSigLoose")
ToolSvc += HIGG4D1TauLooseWrapper


#====================================================================
# DELTA R TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG4D1DeltaRTool = DerivationFramework__DeltaRTool(  name                    = "HIGG4D1DeltaRTool",
                                                       ContainerName           = "Electrons",
                                                       SecondContainerName     = "TauJets",
						       StoreGateEntryName      = "HIGG4D1DeltaR")
ToolSvc += HIGG4D1DeltaRTool

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D1JetTPThinningTool",
                                                                          ThinningService         = "HIGG4D1ThinningSvc",
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          SelectionString         = "AntiKt4EMTopoJets.pt > 15*GeV",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += HIGG4D1JetTPThinningTool
thinningTools.append(HIGG4D1JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D1FatJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D1FatJetTPThinningTool",
                                                                             ThinningService         = "HIGG4D1ThinningSvc",
                                                                             JetKey                  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                             SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ApplyAnd                = True)
ToolSvc += HIGG4D1FatJetTPThinningTool
thinningTools.append(HIGG4D1FatJetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG4D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG4D1MuonTPThinningTool",
                                                                           ThinningService         = "HIGG4D1ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D1MuonTPThinningTool
thinningTools.append(HIGG4D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG4D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "HIGG4D1ElectronTPThinningTool",
                                                                                       ThinningService         = "HIGG4D1ThinningSvc",
                                                                                       SGKey                   = "Electrons",
                                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D1ElectronTPThinningTool
thinningTools.append(HIGG4D1ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG4D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            	  = "HIGG4D1TauTPThinningTool",
                                                                          ThinningService 	  = "HIGG4D1ThinningSvc",
                                                                          TauKey          	  = "TauJets",
                                                                          SelectionString         = "TauJets.pt > 15*GeV",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D1TauTPThinningTool
thinningTools.append(HIGG4D1TauTPThinningTool)


#Truth tau/nutau and their ancestors and descendants
truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG4D1TruthTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG4D1TruthTool",
                                                             ThinningService              = "HIGG4D1ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)

HIGG4D1TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "HIGG4D1TruthTool2",
                                                              ThinningService              = "HIGG4D1ThinningSvc",
                                                              ParticleSelectionString      = truth_cond_comb,
                                                              PreserveDescendants          = False,
                                                              PreserveGeneratorDescendants = False,
                                                              PreserveAncestors            = True,
                                                              TauHandling                  = False)


#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning 
#HIGG4D1TruthTool = DerivationFramework__MenuTruthThinning(  name = "HIGG4D1TruthTool",
#                                                             ThinningService = "HIGG4D1ThinningSvc", 
#                                                            WritePartons = False,
#                                                            WriteHadrons = False, 
#                                                            WriteBHadrons = False, 
#                                                            WriteGeant = False, 
#                                                            GeantPhotonPtThresh = -1.0
#                                                            WriteTauHad = True,
#                                                            WriteBSM = False,
#                                                            PartonPtThresh = -1.0,
#                                                            WriteBosons = True,
#                                                            WriteBSMProducts = False,
#                                                            WriteBosonProducts = False, 
#                                                            WriteTopAndDecays = False,
#                                                            WriteEverything = False,
#                                                            WriteAllLeptons = False,
#                                                            WriteLeptonsNotFromHadrons = True,
#                                                            WriteStatus3 = False, 
#                                                            WriteFirstN = -1,
#                                                            PreserveGeneratorDescendants = True,
#                                                            PreserveAncestors = True 
#                                                            )


if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG4D1TruthTool
    thinningTools.append(HIGG4D1TruthTool)
    ToolSvc += HIGG4D1TruthTool2
    thinningTools.append(HIGG4D1TruthTool2)


#====================================================================
# SKIMMING TOOL 
#====================================================================

leplep = '(count( (Muons.pt > 10.0*GeV) && (Muons.DFCommonGoodMuon) ) + count((Electrons.pt > 13.0*GeV) && ((Electrons.DFCommonElectronsIsEMMedium)||(Electrons.DFCommonElectronsLHMedium)))) >=2 && (count( (Muons.pt > 13.0*GeV) && (Muons.DFCommonGoodMuon) ) + count((Electrons.pt > 13.0*GeV) && ((Electrons.DFCommonElectronsIsEMMedium)||(Electrons.DFCommonElectronsLHMedium)))) >= 1'

expression = leplep



from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG4D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG4D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG4D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG4D1Kernel",
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = [HIGG4D1TauMedWrapper,HIGG4D1TauLooseWrapper,HIGG4D1DeltaRTool],	
                                                                       SkimmingTools = [HIGG4D1SkimmingTool]
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG4D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG4D1Stream )
HIGG4D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG4D1Stream.AcceptAlgs(["HIGG4D1Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG4D1ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGG4D1ExtraContent import *

HIGG4D1SlimmingHelper = SlimmingHelper("HIGG4D1SlimmingHelper")
HIGG4D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG4D1SlimmingHelper.ExtraVariables = ExtraContentAll
HIGG4D1SlimmingHelper.AllVariables = ExtraContainersAll 

if globalflags.DataSource()=='geant4':
    HIGG4D1SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    HIGG4D1SlimmingHelper.AllVariables += ExtraContainersTruth
    
addJetOutputs(HIGG4D1SlimmingHelper,["MyJets"])
HIGG4D1SlimmingHelper.AppendContentToStream(HIGG4D1Stream)
HIGG4D1Stream.AddItem("xAOD::EventShape#*")
HIGG4D1Stream.AddItem("xAOD::EventShapeAuxInfo#*")

