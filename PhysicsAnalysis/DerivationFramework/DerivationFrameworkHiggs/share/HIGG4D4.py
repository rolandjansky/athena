#********************************************************************
# HIGG4D4.py 
# reductionConf flag HIGG4D4 in Reco_tf.py   
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
HIGG4D4TauMedWrapper = DerivationFramework__TauSelectionWrapper(	name 			= "HIGG4D4TauSelectionWrapper",
                                                                        #IsTauFlag		= IsTauFlag.JetBDTSigMedium,
                                                                        IsTauFlag		= 20,
                                                                        CollectionName		= "TauJets",
                                                                        StoreGateEntryName	= "HIGG4D4JetBDTSigMedium")
ToolSvc += HIGG4D4TauMedWrapper

HIGG4D4TauLooseWrapper = DerivationFramework__TauSelectionWrapper(        name                    = "HIGG4D4TauSelectionWrapper",
                                                                          IsTauFlag               = 19,
                                                                          CollectionName          = "TauJets",
                                                                          StoreGateEntryName      = "HIGG4D4JetBDTSigLoose")
ToolSvc += HIGG4D4TauLooseWrapper


#====================================================================
# DELTA R TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG4D4DeltaRTool = DerivationFramework__DeltaRTool(  name                    = "HIGG4D4DeltaRTool",
                                                       ContainerName           = "Electrons",
                                                       SecondContainerName     = "TauJets",
						       StoreGateEntryName      = "HIGG4D4DeltaR")
ToolSvc += HIGG4D4DeltaRTool

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D4JetTPThinningTool",
                                                                          ThinningService         = "HIGG4D4ThinningSvc",
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          SelectionString         = "AntiKt4EMTopoJets.pt > 15*GeV",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += HIGG4D4JetTPThinningTool
thinningTools.append(HIGG4D4JetTPThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG4D4FatJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG4D4FatJetTPThinningTool",
                                                                             ThinningService         = "HIGG4D4ThinningSvc",
                                                                             JetKey                  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                             SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ApplyAnd                = True)
ToolSvc += HIGG4D4FatJetTPThinningTool
thinningTools.append(HIGG4D4FatJetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG4D4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG4D4MuonTPThinningTool",
                                                                           ThinningService         = "HIGG4D4ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           SelectionString         = "Muons.pt > 1*GeV",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D4MuonTPThinningTool
thinningTools.append(HIGG4D4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG4D4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "HIGG4D4ElectronTPThinningTool",
                                                                                       ThinningService         = "HIGG4D4ThinningSvc",
                                                                                       SGKey                   = "Electrons",
                                                                                       SelectionString         = "Electrons.pt > 1*GeV",
                                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D4ElectronTPThinningTool
thinningTools.append(HIGG4D4ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG4D4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            	  = "HIGG4D4TauTPThinningTool",
                                                                          ThinningService 	  = "HIGG4D4ThinningSvc",
                                                                          TauKey          	  = "TauJets",
                                                                          SelectionString         = "TauJets.pt > 15*GeV",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG4D4TauTPThinningTool
thinningTools.append(HIGG4D4TauTPThinningTool)


# Truth tau/nutau and their ancestors and descendants
truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) &&(TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG4D4TruthTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG4D4TruthTool",
                                                             ThinningService              = "HIGG4D4ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)

HIGG4D4TruthTool2 = DerivationFramework__GenericTruthThinning(name                        = "HIGG4D4TruthTool2",
                                                             ThinningService              = "HIGG4D4ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_comb,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = False,
                                                             PreserveAncestors            = True,
                                                             TauHandling                  = False)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG4D4TruthTool
    thinningTools.append(HIGG4D4TruthTool)
    ToolSvc += HIGG4D4TruthTool2
    thinningTools.append(HIGG4D4TruthTool2)


#====================================================================
# SKIMMING TOOL 
#====================================================================

tau1 = '(count(((TauJets.nTracks >= 1) || (TauJets.nTracks <= 3)) && (TauJets.pt > 120.0*GeV)) >= 1)'
tau2 = '(count(((TauJets.nTracks >= 1) || (TauJets.nTracks <= 3)) && (TauJets.pt > 45.0*GeV)) >= 2)'
hadhad = tau1+' && '+tau2

expression = hadhad


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG4D4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG4D4SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG4D4SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG4D4Kernel",
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = [HIGG4D4TauMedWrapper,HIGG4D4TauLooseWrapper,HIGG4D4DeltaRTool],	
                                                                       SkimmingTools = [HIGG4D4SkimmingTool]
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG4D4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG4D4Stream )
HIGG4D4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG4D4Stream.AcceptAlgs(["HIGG4D4Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG4D4ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGG4D1ExtraContent import *

HIGG4D4SlimmingHelper = SlimmingHelper("HIGG4D4SlimmingHelper")
HIGG4D4SlimmingHelper.SmartCollections = ["Electrons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG4D4SlimmingHelper.ExtraVariables = ExtraContentAll
HIGG4D4SlimmingHelper.AllVariables = ExtraContainersAll 

if globalflags.DataSource()=='geant4':
    HIGG4D4SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    HIGG4D4SlimmingHelper.AllVariables += ExtraContainersTruth

addJetOutputs(HIGG4D4SlimmingHelper,["MyJets"])    
HIGG4D4SlimmingHelper.AppendContentToStream(HIGG4D4Stream)
HIGG4D4Stream.AddItem("xAOD::EventShape#*")
HIGG4D4Stream.AddItem("xAOD::EventShapeAuxInfo#*")


