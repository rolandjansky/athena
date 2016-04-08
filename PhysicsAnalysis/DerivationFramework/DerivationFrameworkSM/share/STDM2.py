#********************************************************************
# STDM2.py 
# reductionConf flag STDM2 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from AthenaCommon.GlobalFlags import globalflags

#====================================================================
# SKIMMING TOOL 
#====================================================================
photonRequirements = '(DFCommonPhotons_et >= 15*GeV) && (abs(DFCommonPhotons_eta) < 2.5)'# && (Photons.Loose)'
electronRequirements = '(Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.5) && ( (Electrons.DFCommonElectronsIsEMLoose) || (Electrons.DFCommonElectronsLHLoose) )'

photonSelection = '(count(' + photonRequirements + ') >= 1)'
electronSelection = '(count(' + electronRequirements + ') >= 1)'

singleElectronTriggerRequirements = '( HLT_e24_medium_iloose_EM18VH  || HLT_e24_medium_iloose_EM20VH || HLT_e24_tight_iloose_EM20VH || HLT_e24_tight_iloose || HLT_e26_tight_iloose || HLT_e60_medium || HLT_e120_loose || HLT_e140_loose || HLT_e24_lhmedium_iloose_EM18VH  || HLT_e24_lhmedium_iloose_EM20VH || HLT_e24_lhtight_iloose_EM20VH || HLT_e24_lhtight_iloose || HLT_e26_lhtight_iloose || HLT_e60_lhmedium || HLT_e120_lhloose || HLT_e140_lhloose || HLT_e26_lhtight_nod0_iloose || HLT_e60_lhmedium_nod0 || HLT_e24_lhtight_nod0_iloose || HLT_e24_lhmedium_nod0_iloose_L1EM20VH || HLT_e24_lhmedium_nod0_iloose_L1EM18VH || HLT_e120_lhloose_nod0 )'

diElectronTriggerRequirements = '( HLT_2e12_loose_L12EM10VH || HLT_2e15_loose_L12EM13VH || HLT_2e17_loose || HLT_2e17_loose_L12EM15 || HLT_2e12_lhloose_L12EM10VH || HLT_2e15_lhloose_L12EM13VH || HLT_2e17_lhloose || HLT_2e17_lhloose_L12EM15 )'

triElectronTriggerRequirements = '( HLT_e17_loose_2e9_loose || HLT_e17_medium_2e9_medium || HLT_e17_medium_iloose_2e9_medium || HLT_e17_lhloose_2e9_lhloose || HLT_e17_lhmedium_2e9_lhmedium || HLT_e17_lhmedium_iloose_2e9_lhmedium || HLT_e17_lhloose_nod0_2e9_lhloose_nod0 || HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0 || HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0 )'

singlePhotonTriggerRequirements = '( HLT_g10_loose || HLT_g15_loose_L1EM7 || HLT_g20_loose_L1EM12 || HLT_g25_loose_L1EM15  || HLT_g35_loose_L1EM15 || HLT_g25_loose || HLT_g25_medium || HLT_g35_loose || HLT_g35_medium || HLT_g40_loose_L1EM15 || HLT_g45_loose_L1EM15 || HLT_g50_loose_L1EM15 || HLT_g50_loose || HLT_g60_loose || HLT_g70_loose || HLT_g80_loose || HLT_g100_loose || HLT_g120_loose || HLT_g140_loose || HLT_g140_loose_HLTCalo || HLT_g200_etcut )'

diPhotonTriggerRequirements = '( HLT_2g20_loose_L12EM15 || HLT_2g20_loose || HLT_2g20_tight || HLT_2g22_tight || HLT_2g25_tight || HLT_g35_loose_g25_loose || HLT_g35_medium_HLTCalo_g25_medium_HLTCalo || HLT_g35_loose_L1EM15_g25_loose_L1EM15 || HLT_g35_medium_g25_medium || HLT_2g50_loose || HLT_2g60_loose_L12EM15VH )'

triPhotonTriggerRequirements = '( HLT_3g15_loose || HLT_g20_loose_2g15_loose_L12EM13VH || HLT_2g20_loose_g15_loose || HLT_3g20_loose)'

electronPhotonTriggerRequirements = '( HLT_e24_medium_L1EM15VH_g25_medium || HLT_e20_medium_g35_loose || HLT_e24_lhmedium_L1EM15VH_g25_medium || HLT_e20_lhmedium_g35_loose || HLT_e20_lhmedium_2g10_loose )'

firstDataTriggerRequirements = '( HLT_2g10_loose || HLT_g20_loose_L1EM15 || HLT_e17_loose_L1EM15 || HLT_e17_lhloose_L1EM15 || HLT_e17_lhloose_cutd0dphideta_L1EM15 || HLT_e17_lhloose_nod0_L1EM15 || HLT_e17_lhloose_nodeta_L1EM15 || HLT_e17_lhloose_nodphires_L1EM15 || HLT_noalg_L1EM12 || HLT_noalg_L12EM7 )'

expression = ' ( ' + photonSelection + ' || ' + electronSelection + ' ) && ( ' + singleElectronTriggerRequirements + ' || ' + diElectronTriggerRequirements + ' || ' + triElectronTriggerRequirements + ' || ' + singlePhotonTriggerRequirements + ' || ' + diPhotonTriggerRequirements + ' || ' + triPhotonTriggerRequirements + ' || ' + electronPhotonTriggerRequirements + ' || ' + firstDataTriggerRequirements + ' ) '

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM2SkimmingTool1",
                                                                 expression = expression)
ToolSvc += STDM2SkimmingTool

#====================================================================
# THINNING TOOLS
#====================================================================
# Generic track thinning
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
STDM2TPThinningTool = DerivationFramework__TrackParticleThinning( name                  = "STDM2TPThinningTool",
                                                                ThinningService         = "STDM2ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += STDM2TPThinningTool

# Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name            = "STDM2JetTPThinningTool",
                                                                ThinningService         = "STDM2ThinningSvc",
                                                                JetKey                  = "AntiKt4LCTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                SelectionString = "AntiKt4LCTopoJets.pt > 15*GeV",
                                                                ApplyAnd                = False)
ToolSvc += STDM2JetTPThinningTool

# All Track within a cone DeltaR=0.6 around Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "STDM2ElectronTPThinningTool",
                                                             ThinningService        = "STDM2ThinningSvc",
                                                             SGKey                  = "Electrons",
                                                             GSFTrackParticlesKey   = "GSFTrackParticles",
                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                             SelectionString        = electronRequirements,
                                                             BestMatchOnly          = True,
                                                             ConeSize               = 0.6,
                                                             ApplyAnd               = False)
ToolSvc += STDM2ElectronTPThinningTool

# Track associated to all Electrons for ambiguity resolver tool
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM2ElectronTPThinningToolAR = DerivationFramework__EgammaTrackParticleThinning(name = "STDM2ElectronTPThinningToolAR",
                                                             ThinningService        = "STDM2ThinningSvc",
                                                             SGKey                  = "Electrons",
                                                             GSFTrackParticlesKey   = "GSFTrackParticles",
                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                             BestMatchOnly          = True,
                                                             ApplyAnd               = False)
ToolSvc += STDM2ElectronTPThinningToolAR

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "STDM2PhotonTPThinningTool",
                                                           ThinningService        = "STDM2ThinningSvc",
                                                           SGKey                  = "Photons",
                                                           GSFTrackParticlesKey   = "GSFTrackParticles",
                                                           InDetTrackParticlesKey = "InDetTrackParticles",
                                                           SelectionString        = photonRequirements,
                                                           BestMatchOnly          = True,
                                                           ConeSize               = 0.6,
                                                           ApplyAnd               = False)
ToolSvc += STDM2PhotonTPThinningTool

#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
STDM2_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "STDM2_MaxCellDecoratorTool",
                                                                    SGKey_electrons = "Electrons",
                                                                    SGKey_photons   = "Photons"
                                                                  )
ToolSvc += STDM2_MaxCellDecoratorTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE
STDM2Sequence = CfgMgr.AthSequencer("STDM2Sequence")

# ADD KERNEL
STDM2Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM2Kernel",
                                                              SkimmingTools = [STDM2SkimmingTool],
                                                              AugmentationTools = [STDM2_MaxCellDecoratorTool],
                                                              ThinningTools = [STDM2TPThinningTool,STDM2JetTPThinningTool,STDM2ElectronTPThinningTool,STDM2ElectronTPThinningToolAR,STDM2PhotonTPThinningTool])

# FIX TRUTH JETS 
if globalflags.DataSource()=='geant4':
  replaceBuggyAntiKt4TruthWZJets(STDM2Sequence,"STDM2")
  
# ADD SEQUENCE TO JOB
DerivationFrameworkJob += STDM2Sequence


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_STDM2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM2Stream )
STDM2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM2Stream.AcceptAlgs(["STDM2Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="STDM2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM2SlimmingHelper = SlimmingHelper("STDM2SlimmingHelper")
STDM2SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4LCTopoJets",
                                        "BTagging_AntiKt4LCTopo",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

STDM2SlimmingHelper.IncludeEGammaTriggerContent = True

STDM2SlimmingHelper.ExtraVariables = ExtraContentElectrons+ExtraContentPhotons+ExtraContentBtag+ExtraContentVtx+ExtraContentTrk+ExtraContentJets+["Electrons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]+["Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]
STDM2SlimmingHelper.AllVariables = ExtraContainersJets#+ExtraContainers6Jets #do not exist for now
if globalflags.DataSource()=='geant4':
    STDM2SlimmingHelper.ExtraVariables += ExtraElectronsTruth+ExtraPhotonsTruth
    STDM2SlimmingHelper.AllVariables   += ExtraContainersTruth+ExtraContainersTruthPhotons #["AntiKt6TruthJets", "BTagging_AntiKt6TruthWZ","AntiKt6TruthWZJets"] do not exist for now

STDM2SlimmingHelper.AppendContentToStream(STDM2Stream)
STDM2Stream.AddItem("xAOD::EventShape#*")
STDM2Stream.AddItem("xAOD::EventShapeAuxInfo#*")
