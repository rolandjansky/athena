#********************************************************************
# STDM8.py 
# reductionConf flag STDM8 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkSM import STDMTriggers

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================                                               
# SET UP STREAM 
#====================================================================                                               

streamName = derivationFlags.WriteDAOD_STDM8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM8Stream )
STDM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM8Stream.AcceptAlgs(["STDM8Kernel"])


#====================================================================                                               
# THINNING TOOLS
#====================================================================                                               

thinningTools=[]

#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM8ThinningHelper = ThinningHelper( "STDM8ThinningHelper" )

#trigger navigation content
STDM8ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
STDM8ThinningHelper.AppendToStream( STDM8Stream )


#=====================
# TRACK  THINNING
#=====================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM8MuonTPThinningTool",
                                                                         ThinningService         = STDM8ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM8MuonTPThinningTool
thinningTools.append(STDM8MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "STDM8ElectronTPThinningTool",
                                                                                     ThinningService         = STDM8ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM8ElectronTPThinningTool
thinningTools.append(STDM8ElectronTPThinningTool)


STDM8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "STDM8PhotonTPThinningTool",
                                                                        ThinningService         = STDM8ThinningHelper.ThinningSvc(),
                                                                        SGKey                   = "Photons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM8PhotonTPThinningTool
thinningTools.append(STDM8PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
STDM8TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                 = "STDM8TauTPThinningTool",
                                                                        ThinningService         = STDM8ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        SelectionString         = "TauJets.pt > 15*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM8TauTPThinningTool
thinningTools.append(STDM8TauTPThinningTool)


# Truth leptons and their ancestors and descendants
truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
truth_cond_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning


if globalflags.DataSource()=='geant4':
    from DerivationFrameworkSM.STDMCommonTruthTools import *
    
    STDM8TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "STDM8TruthLepTool",
                                                                  ThinningService              = STDM8ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)

    STDM8TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "STDM8TruthBosTool",
                                                                  ThinningService              = STDM8ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)

    STDM8PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM8PhotonThinning",
                                                                    ThinningService         = STDM8ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "STDMTruthPhotons",
                                                                    ParticleSelectionString = STDMphotonthinningexpr)
    

    ToolSvc += STDM8TruthLepTool
    ToolSvc += STDM8TruthBosTool
    ToolSvc += STDM8PhotonThinning
    thinningTools.append(STDM8TruthLepTool)
    thinningTools.append(STDM8TruthBosTool)
    thinningTools.append(STDM8PhotonThinning)


#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 15*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)'
offlineexpression = '(count('+electronsRequirements+') + count('+muonsRequirements+')) >= 1'

singleElectronTriggerRequirement = STDMTriggers.list_combine_OR(STDMTriggers.single_e_triggers)
singleMuonTriggerRequirement     = STDMTriggers.list_combine_OR(STDMTriggers.single_mu_triggers)
triggerRequirement='('+singleElectronTriggerRequirement+'||'+singleMuonTriggerRequirement+')'

expression = triggerRequirement+' || '+offlineexpression


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM8SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM8SkimmingTool",
                                                               expression = expression)
ToolSvc += STDM8SkimmingTool



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================                    

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM8Sequence = CfgMgr.AthSequencer("STDM8Sequence")

# ADD KERNEL 
STDM8Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM8Kernel",
                                                              SkimmingTools = [STDM8SkimmingTool],
                                                              ThinningTools = thinningTools)

# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM8Sequence



#====================================================================
# SET UP STREAM
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM8Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM8Stream )
#STDM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM8Stream.AcceptAlgs(["STDM8Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="STDM8ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM8SlimmingHelper = SlimmingHelper("STDM8SlimmingHelper")
STDM8SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

STDM8SlimmingHelper.IncludeEGammaTriggerContent = True
STDM8SlimmingHelper.IncludeMuonTriggerContent = True

STDM8SlimmingHelper.ExtraVariables = ExtraContentAll
STDM8SlimmingHelper.AllVariables = ExtraContainersAll
STDM8SlimmingHelper.AllVariables+=["AntiKt2HIJets", "AntiKt3HIJets", "AntiKt4HIJets","AntiKt4HITrackJets"]


if globalflags.DataSource()=='geant4':
    STDM8SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM8SlimmingHelper.AllVariables += ExtraContainersTruth
    STDM8SlimmingHelper.AppendToDictionary = ExtraDictionary


STDM8SlimmingHelper.AppendContentToStream(STDM8Stream)

