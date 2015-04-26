#********************************************************************
# HIGG4D3.py 
# reductionConf flag HIGG4D3 in Reco_tf.py   
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
HIGG4D3TauMedWrapper = DerivationFramework__TauSelectionWrapper(	name 			= "HIGG4D3TauSelectionWrapper",
                                                                        #IsTauFlag		= IsTauFlag.JetBDTSigMedium,
                                                                        IsTauFlag		= 20,
                                                                        CollectionName		= "TauJets",
                                                                        StoreGateEntryName	= "HIGG4D3JetBDTSigMedium")
ToolSvc += HIGG4D3TauMedWrapper

HIGG4D3TauLooseWrapper = DerivationFramework__TauSelectionWrapper(        name                    = "HIGG4D3TauSelectionWrapper",
                                                                          IsTauFlag               = 19,
                                                                          CollectionName          = "TauJets",
                                                                          StoreGateEntryName      = "HIGG4D3JetBDTSigLoose")
ToolSvc += HIGG4D3TauLooseWrapper


#====================================================================
# DELTA R TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG4D3DeltaRTool = DerivationFramework__DeltaRTool(  name                    = "HIGG4D3DeltaRTool",
                                                       ContainerName           = "Electrons",
                                                       SecondContainerName     = "TauJets",
						       StoreGateEntryName      = "HIGG4D3DeltaR")
ToolSvc += HIGG4D3DeltaRTool

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D3JetTPThinningTool",
                                                                          ThinningService         = "HIGG4D3ThinningSvc",
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          SelectionString         = "AntiKt4EMTopoJets.pt > 15*GeV",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += HIGG4D3JetTPThinningTool
thinningTools.append(HIGG4D3JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D3FatJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D3FatJetTPThinningTool",
                                                                             ThinningService         = "HIGG4D3ThinningSvc",
                                                                             JetKey                  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                             SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ApplyAnd                = True)
ToolSvc += HIGG4D3FatJetTPThinningTool
thinningTools.append(HIGG4D3FatJetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG4D3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG4D3MuonTPThinningTool",
                                                                           ThinningService         = "HIGG4D3ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           SelectionString         = "Muons.pt > 1*GeV",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D3MuonTPThinningTool
thinningTools.append(HIGG4D3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG4D3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "HIGG4D3ElectronTPThinningTool",
                                                                                       ThinningService         = "HIGG4D3ThinningSvc",
                                                                                       SGKey                   = "Electrons",
                                                                                       SelectionString         = "Electrons.pt > 1*GeV",
                                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D3ElectronTPThinningTool
thinningTools.append(HIGG4D3ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG4D3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            	  = "HIGG4D3TauTPThinningTool",
                                                                          ThinningService 	  = "HIGG4D3ThinningSvc",
                                                                          TauKey          	  = "TauJets",
                                                                          SelectionString         = "TauJets.pt > 15*GeV",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D3TauTPThinningTool
thinningTools.append(HIGG4D3TauTPThinningTool)


# Truth tau/nutau and their ancestors and descendants
truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) &&(TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG4D3TruthTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG4D3TruthTool",
                                                             ThinningService              = "HIGG4D3ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)

HIGG4D3TruthTool2 = DerivationFramework__GenericTruthThinning(name                        = "HIGG4D3TruthTool2",
                                                             ThinningService              = "HIGG4D3ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_comb,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = False,
                                                             PreserveAncestors            = True,
                                                             TauHandling                  = False)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG4D3TruthTool
    thinningTools.append(HIGG4D3TruthTool)
    ToolSvc += HIGG4D3TruthTool2
    thinningTools.append(HIGG4D3TruthTool2)


#====================================================================
# SKIMMING TOOL 
#====================================================================

tau1 = '(count(HIGG4D3JetBDTSigLoose && (TauJets.pt > 23.0*GeV)) >= 1)'
tau2 = '(count( TauJets.pt > 33.0*GeV) >= 1)'
tau3 = '(count( TauJets.pt > 23.0*GeV) >= 2)'
hadhad = tau1+' && '+tau2+' && '+tau3

expression = hadhad


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG4D3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG4D3SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG4D3SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG4D3Kernel",
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = [HIGG4D3TauMedWrapper,HIGG4D3TauLooseWrapper,HIGG4D3DeltaRTool],	
                                                                       SkimmingTools = [HIGG4D3SkimmingTool]
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG4D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG4D3Stream )
HIGG4D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG4D3Stream.AcceptAlgs(["HIGG4D3Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG4D3ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGG4D1ExtraContent import *

HIGG4D3SlimmingHelper = SlimmingHelper("HIGG4D3SlimmingHelper")
HIGG4D3SlimmingHelper.SmartCollections = ["Electrons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG4D3SlimmingHelper.ExtraVariables = ExtraContentAll
HIGG4D3SlimmingHelper.AllVariables = ExtraContainersAll 

if globalflags.DataSource()=='geant4':
    HIGG4D3SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    HIGG4D3SlimmingHelper.AllVariables += ExtraContainersTruth

addJetOutputs(HIGG4D3SlimmingHelper,["MyJets"])    
HIGG4D3SlimmingHelper.AppendContentToStream(HIGG4D3Stream)
HIGG4D3Stream.AddItem("xAOD::EventShape#*")
HIGG4D3Stream.AddItem("xAOD::EventShapeAuxInfo#*")


