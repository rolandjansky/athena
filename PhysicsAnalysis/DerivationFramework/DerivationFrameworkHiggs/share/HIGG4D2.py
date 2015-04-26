#********************************************************************
# HIGG4D2.py 
# reductionConf flag HIGG4D2 in Reco_tf.py   
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
HIGG4D2TauMedWrapper = DerivationFramework__TauSelectionWrapper(	name 			= "HIGG4D2TauSelectionWrapper",
                                                                        #IsTauFlag		= IsTauFlag.JetBDTSigMedium,
                                                                        IsTauFlag		= 20,
                                                                        CollectionName		= "TauJets",
                                                                        StoreGateEntryName	= "HIGG4D2JetBDTSigMedium")
ToolSvc += HIGG4D2TauMedWrapper

HIGG4D2TauLooseWrapper = DerivationFramework__TauSelectionWrapper(        name                    = "HIGG4D2TauSelectionWrapper",
                                                                          IsTauFlag               = 19,
                                                                          CollectionName          = "TauJets",
                                                                          StoreGateEntryName      = "HIGG4D2JetBDTSigLoose")
ToolSvc += HIGG4D2TauLooseWrapper


#====================================================================
# DELTA R TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG4D2DeltaRTool = DerivationFramework__DeltaRTool(  name                    = "HIGG4D2DeltaRTool",
                                                       ContainerName           = "Electrons",
                                                       SecondContainerName     = "TauJets",
						       StoreGateEntryName      = "HIGG4D2DeltaR")
ToolSvc += HIGG4D2DeltaRTool

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG4D2JetTPThinningTool",
                                                                          ThinningService         = "HIGG4D2ThinningSvc",
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          SelectionString         = "AntiKt4EMTopoJets.pt > 15*GeV",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += HIGG4D2JetTPThinningTool
thinningTools.append(HIGG4D2JetTPThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D2FatJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D2FatJetTPThinningTool",
                                                                             ThinningService         = "HIGG4D2ThinningSvc",
                                                                             JetKey                  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                             SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ApplyAnd                = True)
ToolSvc += HIGG4D2FatJetTPThinningTool
thinningTools.append(HIGG4D2FatJetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG4D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG4D2MuonTPThinningTool",
                                                                           ThinningService         = "HIGG4D2ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D2MuonTPThinningTool
thinningTools.append(HIGG4D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG4D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "HIGG4D2ElectronTPThinningTool",
                                                                                       ThinningService         = "HIGG4D2ThinningSvc",
                                                                                       SGKey                   = "Electrons",
                                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D2ElectronTPThinningTool
thinningTools.append(HIGG4D2ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG4D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            	  = "HIGG4D2TauTPThinningTool",
                                                                          ThinningService 	  = "HIGG4D2ThinningSvc",
                                                                          TauKey          	  = "TauJets",
                                                                          SelectionString         = "TauJets.pt > 15*GeV",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D2TauTPThinningTool
thinningTools.append(HIGG4D2TauTPThinningTool)


# Truth tau/nutau and their ancestors and descendants
truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) &&(TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG4D2TruthTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG4D2TruthTool",
                                                             ThinningService              = "HIGG4D2ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)


HIGG4D2TruthTool2 = DerivationFramework__GenericTruthThinning(name                        = "HIGG4D2TruthTool2",
                                                             ThinningService              = "HIGG4D2ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_comb,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = False,
                                                             PreserveAncestors            = True,
                                                             TauHandling                  = False)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG4D2TruthTool
    thinningTools.append(HIGG4D2TruthTool)
    ToolSvc += HIGG4D2TruthTool2
    thinningTools.append(HIGG4D2TruthTool2)

#====================================================================
# SKIMMING TOOL 
#====================================================================

mu = '(count( (Muons.pt > 24.0*GeV) && Muons.DFCommonGoodMuon ) >= 1)'
e = '(count((Electrons.pt > 24.0*GeV) && ((Electrons.DFCommonElectronsIsEMMedium)||(Electrons.DFCommonElectronsLHMedium))) >= 1)'
tau = '(count( (TauJets.pt > 18.0*GeV) && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3)) ) >= 1)'
etau = '(count(HIGG4D2DeltaR > 0.1) >= 1)'
muhad = mu+' && '+tau
ehad = e+' && '+tau+' && '+etau

expression = '('+muhad+') || ('+ehad+')'


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG4D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG4D2SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG4D2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG4D2Kernel",
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = [HIGG4D2TauMedWrapper,HIGG4D2TauLooseWrapper,HIGG4D2DeltaRTool],	
                                                                       SkimmingTools = [HIGG4D2SkimmingTool]
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG4D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG4D2Stream )
HIGG4D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG4D2Stream.AcceptAlgs(["HIGG4D2Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG4D2ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGG4D1ExtraContent import *

HIGG4D2SlimmingHelper = SlimmingHelper("HIGG4D2SlimmingHelper")
HIGG4D2SlimmingHelper.SmartCollections = ["Electrons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG4D2SlimmingHelper.ExtraVariables = ExtraContentAll
HIGG4D2SlimmingHelper.AllVariables = ExtraContainersAll 

if globalflags.DataSource()=='geant4':
    HIGG4D2SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    HIGG4D2SlimmingHelper.AllVariables += ExtraContainersTruth

addJetOutputs(HIGG4D2SlimmingHelper,["MyJets"])    
HIGG4D2SlimmingHelper.AppendContentToStream(HIGG4D2Stream)
HIGG4D2Stream.AddItem("xAOD::EventShape#*")
HIGG4D2Stream.AddItem("xAOD::EventShapeAuxInfo#*")


