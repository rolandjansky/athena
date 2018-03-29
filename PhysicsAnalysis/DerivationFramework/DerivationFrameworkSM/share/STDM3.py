#********************************************************************
# STDM3.py 
# reductionConf flag STDM3 in Reco_tf.py   
# 
# multi-purpose derivation
# skimming on two leptons
# 
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

streamName = derivationFlags.WriteDAOD_STDM3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM3Stream )
STDM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM3Stream.AcceptAlgs(["STDM3Kernel"])

isMC = globalflags.DataSource()=='geant4'

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]


#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM3ThinningHelper = ThinningHelper( "STDM3ThinningHelper" )

#trigger navigation content
STDM3ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*|HLT_2g10_loose_mu20'
STDM3ThinningHelper.AppendToStream( STDM3Stream )

#===================== 
# TRACK  THINNING
#=====================  

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM3MuonTPThinningTool",
                                                                         ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM3MuonTPThinningTool
thinningTools.append(STDM3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "STDM3ElectronTPThinningTool",
                                                                                     ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM3ElectronTPThinningTool
thinningTools.append(STDM3ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
STDM3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                 = "STDM3TauTPThinningTool",
                                                                        ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        SelectionString         = "TauJets.pt > 15*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM3TauTPThinningTool
thinningTools.append(STDM3TauTPThinningTool)


STDM3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "STDM3PhotonTPThinningTool",
                                                                        ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                        SGKey                   = "Photons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM3PhotonTPThinningTool
thinningTools.append(STDM3PhotonTPThinningTool)

# Truth leptons and their ancestors and descendants
truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
truth_cond_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning

isMC = globalflags.DataSource()=='geant4'

if isMC:
    from DerivationFrameworkSM.STDMCommonTruthTools import *
    
    STDM3TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "STDM3TruthLepTool",
                                                                  ThinningService              = STDM3ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)
    
    STDM3TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "STDM3TruthBosTool",
                                                                  ThinningService              = STDM3ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    STDM3TruthThinning = DerivationFramework__MenuTruthThinning(name                       = "STDM3TruthThinning",
                                                                ThinningService            = STDM3ThinningHelper.ThinningSvc(),
                                                                WritePartons               = False,
                                                                WriteHadrons               = False,
                                                                WriteBHadrons              = True,
                                                                WriteCHadrons              = True,
                                                                WritettHFHadrons           = True,
                                                                WriteGeant                 = False,
                                                                GeantPhotonPtThresh        = -1.0,
                                                                WriteTauHad                = True,
                                                                PartonPtThresh             = -1.0,
                                                                WriteBSM                   = True,
                                                                WriteBosons                = True,
                                                                WriteBSMProducts           = True,
                                                                WriteBosonProducts         = True,
                                                                WriteTopAndDecays          = True,
                                                                WriteEverything            = False,
                                                                WriteAllLeptons            = True,
                                                                WriteStatus3               = True,
                                                                PreserveDescendants        = False, 
                                                                PreserveGeneratorDescendants = False,
                                                                PreserveAncestors          = True,
                                                                WriteFirstN                = 10)

    STDM3PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM3PhotonThinning",
                                                                    ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "STDMTruthPhotons",
                                                                    ParticleSelectionString = STDMphotonthinningexpr)

    ToolSvc += STDM3TruthLepTool
    ToolSvc += STDM3TruthBosTool
    ToolSvc += STDM3TruthThinning
    ToolSvc += STDM3PhotonThinning
    thinningTools.append(STDM3TruthLepTool)
    thinningTools.append(STDM3TruthBosTool)
    thinningTools.append(STDM3TruthThinning)
    thinningTools.append(STDM3PhotonThinning)
    
#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 15*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)'
loose_electronsRequirements='(Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHVeryLoose)'
loose_muonsRequirements = '(Muons.pt >= 20*GeV) && (Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection)'

muonOnlySelection = 'count('+muonsRequirements+') >=2'
loose_muonOnlySelection = 'count('+loose_muonsRequirements+') >=2'

electronOnlySelection = 'count('+electronsRequirements+') >= 2'
loose_electronOnlySelection = 'count('+loose_electronsRequirements+') >= 2'

electronMuonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+')) >= 2'
loose_electronMuonSelection = '(count('+loose_electronsRequirements+') + count('+loose_muonsRequirements+')) >= 2'

offlineexpression = " || ".join([muonOnlySelection,electronOnlySelection,electronMuonSelection,
                               loose_muonOnlySelection,loose_electronOnlySelection,loose_electronMuonSelection])

diElectronTriggerRequirement = STDMTriggers.list_combine_OR(STDMTriggers.multi_e_triggers) 
diMuonTriggerRequirement     = STDMTriggers.list_combine_OR(STDMTriggers.multi_mu_triggers) 
lggTriggerRequirement = '( HLT_e20_lhmedium_nod0_2g10_loose || HLT_e20_lhmedium_nod0_2g10_loose_L1EM15VH_3EM8VH || HLT_e24_lhmedium_nod0_2g12_loose || HLT_e24_lhmedium_nod0_2g12_medium || HLT_2g10_loose_mu20)'
triggerRequirement='('+diElectronTriggerRequirement+'||'+diMuonTriggerRequirement+"||"+ lggTriggerRequirement+')'

expression = triggerRequirement+' || '+offlineexpression


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM3SkimmingTool",
                                                                 expression = expression)
ToolSvc += STDM3SkimmingTool


#=====================================================
# CREATE AND SCHEDULE THE DERIVATION KERNEL ALGORITHM   
#=====================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE
STDM3Sequence = CfgMgr.AthSequencer("STDM3Sequence")


# ADD KERNEL
STDM3Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM3Kernel",
                                                              SkimmingTools = [STDM3SkimmingTool],
                                                              ThinningTools = thinningTools)

# JET REBUILDING
reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, STDM3Sequence, "STDM3Jets")

# FAKE LEPTON TAGGER
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
STDM3Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()

# ADD SEQUENCE TO JOB
DerivationFrameworkJob += STDM3Sequence

#====================================================================
# SET UP STREAM   
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM3Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM3Stream )
#STDM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM3Stream.AcceptAlgs(["STDM3Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="STDM3ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM3SlimmingHelper = SlimmingHelper("STDM3SlimmingHelper")
STDM3SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

STDM3SlimmingHelper.IncludeEGammaTriggerContent = True
STDM3SlimmingHelper.IncludeMuonTriggerContent = True


STDM3SlimmingHelper.ExtraVariables = ExtraContentAll
STDM3SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m"]
STDM3SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

STDM3SlimmingHelper.AllVariables = ExtraContainersAll

# # btagging variables
from  DerivationFrameworkFlavourTag.BTaggingContent import *

STDM3SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMTopoJets")
STDM3SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt2PV0TrackJets")

ExtraDictionary["BTagging_AntiKt4EMTopo"]    = "xAOD::BTaggingContainer"
ExtraDictionary["BTagging_AntiKt4EMTopoAux"] = "xAOD::BTaggingAuxContainer"
ExtraDictionary["BTagging_AntiKt2Track"]     = "xAOD::BTaggingContainer"
ExtraDictionary["BTagging_AntiKt2TrackAux"]  = "xAOD::BTaggingAuxContainer"

if isMC:
    STDM3SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM3SlimmingHelper.AllVariables += ExtraContainersTruth
    STDM3SlimmingHelper.AppendToDictionary.update(ExtraDictionary)

addJetOutputs(STDM3SlimmingHelper,["STDM3","STDM3Jets"])

STDM3SlimmingHelper.AppendContentToStream(STDM3Stream)
