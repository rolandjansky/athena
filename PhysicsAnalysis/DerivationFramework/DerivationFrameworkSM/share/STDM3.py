#********************************************************************
# STDM3.py 
# reductionConf flag STDM3 in Reco_tf.py   
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

streamName = derivationFlags.WriteDAOD_STDM3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM3Stream )
STDM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM3Stream.AcceptAlgs(["STDM3Kernel"])


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
STDM3ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
STDM3ThinningHelper.AppendToStream( STDM3Stream )


#===================== 
# TRACK  THINNING
#=====================  


# Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "STDM3JetTPThinningTool",
                                                                        ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4EMTopoJets",
#                                                                        SelectionString         = "AntiKt4EMTopoJets.pt > 10*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = True)
ToolSvc += STDM3JetTPThinningTool
thinningTools.append(STDM3JetTPThinningTool)

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


# Truth leptons and their ancestors and descendants
truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
truth_cond_lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"
photonthinningexpr = "(TruthPhotons.classifierParticleOrigin != 42) && !(TruthPhotons.classifierParticleOrigin >= 23 && TruthPhotons.classifierParticleOrigin <= 35)"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning


if globalflags.DataSource()=='geant4':
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
    
    STDM3PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM3PhotonThinning",
                                                                    ThinningService         = STDM3ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "TruthPhotons",
                                                                    ParticleSelectionString = photonthinningexpr)

    
    ToolSvc += STDM3TruthLepTool
    ToolSvc += STDM3TruthBosTool
    ToolSvc += STDM3PhotonThinning
    thinningTools.append(STDM3TruthLepTool)
    thinningTools.append(STDM3TruthBosTool)
    thinningTools.append(STDM3PhotonThinning)
    
#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 15*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.DFCommonElectronsIsEMLoose) || (Electrons.DFCommonElectronsLHLoose))'
muonOnlySelection = 'count('+muonsRequirements+') >=2'
electronOnlySelection = 'count('+electronsRequirements+') >= 2'
electronMuonSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+')) >= 2'
offlineexpression = muonOnlySelection+' || '+electronOnlySelection+' || '+electronMuonSelection

diElectronTriggerRequirement = '( HLT_2e12_loose_L12EM10VH || HLT_2e15_loose_L12EM13VH || HLT_2e17_loose || HLT_2e17_loose_L12EM15 || HLT_2e12_lhloose_L12EM10VH || HLT_2e15_lhloose_L12EM13VH || HLT_2e17_lhloose || HLT_2e17_lhloose_L12EM15 )'
diMuonTriggerRequirement='(HLT_2mu10 || HLT_2mu14 || HLT_mu24_mu8noL1)'
triggerRequirement='('+diElectronTriggerRequirement+'||'+diMuonTriggerRequirement+')'

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
# FIX TRUTH JETS
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(STDM3Sequence,"STDM3")  

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


STDM3SlimmingHelper.AllVariables = ExtraContainersAll


if globalflags.DataSource()=='geant4':
    STDM3SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM3SlimmingHelper.AllVariables += ExtraContainersTruth

    
STDM3SlimmingHelper.AppendContentToStream(STDM3Stream)
STDM3Stream.AddItem("xAOD::EventShape#*")
STDM3Stream.AddItem("xAOD::EventShapeAuxInfo#*")

if globalflags.DataSource()=='geant4':
    STDM3Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons" )
    STDM3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuonsAux." )
    STDM3Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons" )
    STDM3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectronsAux." )
    STDM3Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons" )
    STDM3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotonsAux." )
    STDM3Stream.AddItem( "xAOD::TruthParticleContainer#TruthNeutrinos" )
    STDM3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." )
    STDM3Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
    STDM3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux." )
