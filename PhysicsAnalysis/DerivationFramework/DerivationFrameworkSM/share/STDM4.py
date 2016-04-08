#********************************************************************
# STDM4.py 
# reductionConf flag STDM4 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *


#====================================================================                                               
# SET UP STREAM 
#====================================================================                                               

streamName = derivationFlags.WriteDAOD_STDM4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM4Stream )
STDM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM4Stream.AcceptAlgs(["STDM4Kernel"])


#====================================================================                                               
# THINNING TOOLS
#====================================================================                                               

thinningTools=[]

#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM4ThinningHelper = ThinningHelper( "STDM4ThinningHelper" )

#trigger navigation content
STDM4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
STDM4ThinningHelper.AppendToStream( STDM4Stream )


#=====================
# TRACK  THINNING
#=====================


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "STDM4JetTPThinningTool",
                                                                        ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        SelectionString         = "AntiKt4EMTopoJets.pt > 10*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = True) 
ToolSvc += STDM4JetTPThinningTool
thinningTools.append(STDM4JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM4MuonTPThinningTool",
                                                                         ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4MuonTPThinningTool
thinningTools.append(STDM4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "STDM4ElectronTPThinningTool",
                                                                                     ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4ElectronTPThinningTool
thinningTools.append(STDM4ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
STDM4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                 = "STDM4TauTPThinningTool",
                                                                        ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        SelectionString         = "TauJets.pt > 15*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM4TauTPThinningTool
thinningTools.append(STDM4TauTPThinningTool)


# Truth leptons and their ancestors and descendants
truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
truth_cond_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"
photonthinningexpr = "(TruthPhotons.classifierParticleOrigin != 42) && !(TruthPhotons.classifierParticleOrigin >= 23 && TruthPhotons.classifierParticleOrigin <= 35)"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning


if globalflags.DataSource()=='geant4':
    from DerivationFrameworkSM.STDMCommonTruthTools import *
    
    STDM4TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "STDM4TruthLepTool",
                                                                  ThinningService              = STDM4ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)

    STDM4TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "STDM4TruthBosTool",
                                                                  ThinningService              = STDM4ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)

    STDM4PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM4PhotonThinning",
                                                                    ThinningService         = STDM4ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "TruthPhotons",
                                                                    ParticleSelectionString = photonthinningexpr)
    

    ToolSvc += STDM4TruthLepTool
    ToolSvc += STDM4TruthBosTool
    ToolSvc += STDM4PhotonThinning
    thinningTools.append(STDM4TruthLepTool)
    thinningTools.append(STDM4TruthBosTool)
    thinningTools.append(STDM4PhotonThinning)


#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 15*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 20*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.DFCommonElectronsIsEMLoose) || (Electrons.DFCommonElectronsLHLoose))'
offlineexpression = '(count('+electronsRequirements+') + count('+muonsRequirements+')) >= 1'

singleElectronTriggerRequirement = '( HLT_e24_medium_iloose_EM18VH  || HLT_e24_medium_iloose_EM20VH || HLT_e24_tight_iloose_EM20VH || HLT_e24_tight_iloose || HLT_e26_tight_iloose || HLT_e60_medium || HLT_e120_loose || HLT_e140_loose || HLT_e24_lhmedium_iloose_EM18VH  || HLT_e24_lhmedium_iloose_EM20VH || HLT_e24_lhtight_iloose_EM20VH || HLT_e24_lhtight_iloose || HLT_e26_lhtight_iloose || HLT_e60_lhmedium || HLT_e120_lhloose || HLT_e140_lhloose )'
singleMuonTriggerRequirement='(HLT_mu20_iloose_L1MU15 || HLT_mu24_imedium || HLT_mu26_imedium || HLT_mu50)'
triggerRequirement='('+singleElectronTriggerRequirement+'||'+singleMuonTriggerRequirement+')'

expression = triggerRequirement+' || '+offlineexpression


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM4SkimmingTool",
                                                               expression = expression)
ToolSvc += STDM4SkimmingTool



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================                    

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM4Sequence = CfgMgr.AthSequencer("STDM4Sequence")

# ADD KERNEL 
STDM4Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM4Kernel",
                                                              SkimmingTools = [STDM4SkimmingTool],
                                                              ThinningTools = thinningTools)
# FIX TRUTH JETS
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(STDM4Sequence,"STDM4")

# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM4Sequence



#====================================================================
# SET UP STREAM
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM4Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM4Stream )
#STDM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM4Stream.AcceptAlgs(["STDM4Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="STDM4ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM4SlimmingHelper = SlimmingHelper("STDM4SlimmingHelper")
STDM4SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

STDM4SlimmingHelper.IncludeEGammaTriggerContent = True
STDM4SlimmingHelper.IncludeMuonTriggerContent = True

STDM4SlimmingHelper.ExtraVariables = ExtraContentAll
STDM4SlimmingHelper.AllVariables = ExtraContainersAll

if globalflags.DataSource()=='geant4':
    STDM4SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM4SlimmingHelper.AllVariables += ExtraContainersTruth
    
STDM4SlimmingHelper.AppendContentToStream(STDM4Stream)
STDM4Stream.AddItem("xAOD::EventShape#*")
STDM4Stream.AddItem("xAOD::EventShapeAuxInfo#*")

if globalflags.DataSource()=='geant4':
    STDM4Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons" )
    STDM4Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuonsAux." )
    STDM4Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons" )
    STDM4Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectronsAux." )
    STDM4Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons" )
    STDM4Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotonsAux." )
    STDM4Stream.AddItem( "xAOD::TruthParticleContainer#TruthNeutrinos" )
    STDM4Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." )
    STDM4Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
    STDM4Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux." )
