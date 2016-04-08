#********************************************************************
# STDM5.py 
# reductionConf flag STDM5 in Reco_tf.py   
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

streamName = derivationFlags.WriteDAOD_STDM5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM5Stream )
STDM5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM5Stream.AcceptAlgs(["STDM5Kernel"])


#====================================================================                                               
# THINNING TOOLS
#====================================================================                                               

thinningTools=[]

#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM5ThinningHelper = ThinningHelper( "STDM5ThinningHelper" )

#trigger navigation content
STDM5ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
STDM5ThinningHelper.AppendToStream( STDM5Stream )


#=====================
# TRACK  THINNING 
#=====================


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "STDM5JetTPThinningTool",
                                                                        ThinningService         = STDM5ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        SelectionString         = "AntiKt4EMTopoJets.pt > 15*GeV",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = True)
ToolSvc += STDM5JetTPThinningTool


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM5MuonTPThinningTool",
                                                                         ThinningService         = STDM5ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM5MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(      name                    = "STDM5ElectronTPThinningTool",
                                                                                     ThinningService         = STDM5ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM5ElectronTPThinningTool

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
STDM5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "STDM5TauTPThinningTool",
                                                                        ThinningService         = STDM5ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        SelectionString         = "TauJets.pt > 15*GeV",
                                                                        ConeSize                = 0.6,
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM5TauTPThinningTool



thinningTools= [STDM5JetTPThinningTool,STDM5MuonTPThinningTool,STDM5ElectronTPThinningTool,STDM5TauTPThinningTool]

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkSM.STDMCommonTruthTools import *

    #TruthThinning
    # Truth leptons and their ancestors and descendants

    truth_cond_boson = "((abs(TruthParticles.pdgId) == 23) || (abs(TruthParticles.pdgId) == 24))"
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 14) &&(TruthParticles.pt > 1*GeV) && (TruthParticles.status ==1) && (TruthParticles.barcode<200000))"
    photonthinningexpr = "(TruthPhotons.classifierParticleOrigin != 42) && !(TruthPhotons.classifierParticleOrigin >= 23 && TruthPhotons.classifierParticleOrigin <= 35)"

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    STDM5TruthLepTool = DerivationFramework__GenericTruthThinning(name                         = "STDM5TruthLepTool", 
                                                                  ThinningService              = STDM5ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_Lepton,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = False,
                                                                  PreserveAncestors            = True)
    
    STDM5PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "STDM5PhotonThinning",
                                                                    ThinningService         = STDM5ThinningHelper.ThinningSvc(),
                                                                    ParticlesKey            = "TruthPhotons", 
                                                                    ParticleSelectionString = photonthinningexpr)
    
    STDM5TruthBosTool = DerivationFramework__GenericTruthThinning(name                         = "STDM5TruthBosTool",
                                                                  ThinningService              = STDM5ThinningHelper.ThinningSvc(),
                                                                  ParticleSelectionString      = truth_cond_boson,
                                                                  PreserveDescendants          = False,
                                                                  PreserveGeneratorDescendants = True,
                                                                  PreserveAncestors            = False)


    ToolSvc += STDM5TruthLepTool
    ToolSvc += STDM5TruthBosTool
    ToolSvc += STDM5PhotonThinning
    thinningTools.append(STDM5TruthLepTool)
    thinningTools.append(STDM5TruthBosTool)
    thinningTools.append(STDM5PhotonThinning)




#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 6*GeV && Muons.DFCommonGoodMuon && Muons.DFCommonMuonsLoose)'
electronsRequirements = '((Electrons.pt > 6*GeV) && ((Electrons.DFCommonElectronsIsEMLoose) || (Electrons.DFCommonElectronsLHLoose) || (Electrons.DFCommonElectronsML)))'
muonOnlySelection = '(count('+muonsRequirements+') >= 3)'
electronOnlySelection = '(count('+electronsRequirements+') >= 3)'
twoMuonsOneElectron = '((count('+muonsRequirements+') >= 2) && (count('+electronsRequirements+')>=1))'
oneMuonTwoElectrons = '((count('+muonsRequirements+') >= 1) && (count('+electronsRequirements+')>=2))'
expression = muonOnlySelection+' || '+electronOnlySelection+' || '+twoMuonsOneElectron+' || '+oneMuonTwoElectrons
print "STDM5 skimming expression: ",expression
###OLD
#stacoSelection = '(mu_staco_loose && mu_staco_pt > 10*GeV)'
#muonsSelection = '(mu_loose && mu_pt > 10*GeV)'
#electronSelection = '(el_loosePP || isLLHElectron || isMLElectron) && (recommendedEl_pt > 10*GeV)'
#muonsOnly = '(count('+stacoSelection+') >= 3) || (count('+muonsSelection+') >= 3)'
#electronsOnly = '(count('+electronSelection+') >= 3)'
#twoMuonsOneElectron = '((count('+stacoSelection+') >= 2) || (count('+muonsSelection+') >= 2) ) && (count('+electronSelection+') > 1)'
#oneMuonTwoElectrons = '((count('+stacoSelection+') >= 1) || (count('+muonsSelection+') >= 1) ) && (count('+electronSelection+') > 2)' 
#expression = muonsOnly+' || '+electronsOnly+' || '+twoMuonsOneElectron+' || '+oneMuonTwoElectrons

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM5SkimmingTool",
                                                               expression = expression)
ToolSvc += STDM5SkimmingTool

STDM5AugmentationTools = []



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM5Sequence = CfgMgr.AthSequencer("STDM5Sequence")

# ADD KERNEL 
STDM5Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM5Kernel",
                                                                 SkimmingTools = [STDM5SkimmingTool],
                                                                 ThinningTools = thinningTools)
# FIX TRUTH JETS
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(STDM5Sequence,"STDM5")

# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM5Sequence


#====================================================================
# SET UP STREAM
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM5Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM5Stream )
#STDM5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM5Stream.AcceptAlgs(["STDM5Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="STDM5ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM5SlimmingHelper = SlimmingHelper("STDM5SlimmingHelper")
STDM5SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4LCTopoJets",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4LCTopo",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"  ]

STDM5SlimmingHelper.IncludeEGammaTriggerContent = True
STDM5SlimmingHelper.IncludeMuonTriggerContent = True


STDM5SlimmingHelper.ExtraVariables = ExtraContentAll
STDM5SlimmingHelper.AllVariables = ExtraContainersAll
if globalflags.DataSource()=='geant4':
    STDM5SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM5SlimmingHelper.AllVariables += ExtraContainersTruth
    
STDM5SlimmingHelper.AppendContentToStream(STDM5Stream)
STDM5Stream.AddItem("xAOD::EventShape#*")
STDM5Stream.AddItem("xAOD::EventShapeAuxInfo#*")
#STDM5Stream.AddItem("xAOD::TrigNavigation#*")
#STDM5Stream.AddItem("xAOD::TrigNavigationAuxInfo#*")

if globalflags.DataSource()=='geant4':
    STDM5Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons" )
    STDM5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuonsAux." )
    STDM5Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons" )
    STDM5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectronsAux." )
    STDM5Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons" )
    STDM5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotonsAux." )
    STDM5Stream.AddItem( "xAOD::TruthParticleContainer#TruthNeutrinos" )
    STDM5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." )
    STDM5Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
    STDM5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux." )


