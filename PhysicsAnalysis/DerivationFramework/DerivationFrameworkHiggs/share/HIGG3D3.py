#********************************************************************
# HIGG3D3.py
# reductionConf flag HIGG3D3 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from AthenaCommon.GlobalFlags import globalflags

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG3D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG3D3Stream )
HIGG3D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG3D3Stream.AcceptAlgs(["HIGG3D3Kernel_skimming"])

## Prepare thinning service and add trigger chains for TrigNavigation thinning
TriggerChains = ["HLT_mu24_L1MU15",
                 "HLT_g10_etcut",
                 "HLT_g10_loose",
                 "HLT_g15_loose_L1EM7",
                 "HLT_g20_loose_L1EM12",
                 "HLT_g20_loose_L1EM15",
                 "HLT_g20_etcut_L1EM12",
                 "HLT_e17_lhloose_L1EM15",
                 "HLT_e17_loose_L1EM15",
                 "HLT_g25_loose_L1EM15",
                 "HLT_g35_loose_L1EM15",
                 "HLT_g40_loose_L1EM15",
                 "HLT_g45_loose_L1EM15",
                 "HLT_g50_loose_L1EM15",
                 "HLT_e60_medium",
                 "HLT_e60_lhmedium",
                 "HLT_g35_loose_L1EM15_g25_loose_L1EM15",
                 "HLT_e24_lhmedium_L1EM18VH",
                 "HLT_e24_lhmedium_L1EM20VH",
                 "HLT_e20_lhmedium_g35_loose",
                 "HLT_2e12_loose_L12EM10VH",
                 "HLT_2e12_lhloose_L12EM10VH",
                 "HLT_2e15_vloose_L12EM13VH",
                 "HLT_2g20_loose_g15_loose",
                 "HLT_mu4",
                 "HLT_mu6",
                 "HLT_mu10",
                 "HLT_mu14",
                 "HLT_mu18",
                 "HLT_mu20_L1MU15",
                 "HLT_mu24",
                 "HLT_mu40",
                 "HLT_mu20_iloose_L1MU15",
                 "HLT_mu50",
                 "HLT_e5_etcut",
                 "HLT_e10_etcut_L1EM7",
                 "HLT_e13_etcut_trkcut_L1EM10",
                 "HLT_e15_etcut_L1EM7",
                 "HLT_e18_etcut_trkcut_L1EM15",
                 "HLT_e20_etcut_L1EM12",
                 "HLT_e25_etcut_L1EM15",
                 "HLT_e30_etcut_L1EM15",
                 "HLT_e40_etcut_L1EM15",
                 "HLT_e50_etcut_L1EM15",
                 "HLT_e60_etcut",
                 "HLT_e5_lhvloose",
                 "HLT_e5_lhvloose_nod0",
                 "HLT_e10_lhvloose_L1EM7",
                 "HLT_e12_lhvloose_L1EM10VH",
                 "HLT_e12_lhvloose_nod0_L1EM10VH",
                 "HLT_e15_lhvloose_L1EM7",
                 "HLT_e15_lhvloose_L1EM13VH",
                 "HLT_e15_lhvloose_nod0_L1EM13VH",
                 "HLT_e17_lhvloose",
                 "HLT_e17_lhvloose_nod0",
                 "HLT_e20_lhvloose_L1EM12",
                 "HLT_e20_lhvloose",
                 "HLT_e20_lhvloose_nod0",
                 "HLT_e24_lhvloose_nod0_L1EM18VH",
                 "HLT_e24_lhvloose_nod0_L1EM20VH",
                 ## Additional 2016 lowest un-prescaled triggers
                 "HLT_mu20_ivarloose_L1MU15",
                 "HLT_e24_lhmedium_L1EM20VHI",
                 "HLT_e24_lhmedium_nod0_ivarloose",
                 "HLT_e60_lhmedium_nod0",
                 "HLT_e120_lhloose_nod0",
                 "HLT_e250_etcut",
                 "HLT_2mu10",
                 "HLT_2e12_lhvloose_nod0_L12EM10VH",
                 ## More triggers
                 "HLT_2e12_lhloose_mu10",
                 "HLT_2e12_lhloose_nod0_mu10",
                 "HLT_2e12_lhmedium_mu10",
                 "HLT_2e12_lhmedium_nod0_mu10",
                 "HLT_2e12_lhvloose_L12EM10VH",
                 "HLT_2e12_loose_mu10",
                 "HLT_2e12_medium_mu10",
                 "HLT_2e15_lhvloose_L12EM13VH",
                 "HLT_2e15_lhvloose_nod0_L12EM13VH",
                 "HLT_2e17_lhloose",
                 "HLT_2e17_lhloose_L12EM15",
                 "HLT_2e17_lhloose_nod0_L12EM15",
                 "HLT_2e17_lhvloose",
                 "HLT_2e17_lhvloose_nod0",
                 "HLT_2e17_loose_L12EM15",
                 "HLT_2mu14",
                 "HLT_2mu15",
                 "HLT_2mu4",
                 "HLT_2mu6",
                 "HLT_e100_etcut",
                 "HLT_e100_lhvloose",
                 "HLT_e100_lhvloose_nod0",
                 "HLT_e10_lhvloose_nod0_L1EM7",
                 "HLT_e120_etcut",
                 "HLT_e120_lhloose",
                 "HLT_e120_lhvloose",
                 "HLT_e120_lhvloose_nod0",
                 "HLT_e12_lhloose",
                 "HLT_e12_lhloose_2mu10",
                 "HLT_e12_lhloose_cutd0dphideta_L1EM10VH",
                 "HLT_e12_lhloose_L1EM10VH",
                 "HLT_e12_lhloose_nod0",
                 "HLT_e12_lhloose_nod0_2mu10",
                 "HLT_e12_lhloose_nod0_L1EM10VH",
                 "HLT_e12_lhloose_nodeta_L1EM10VH",
                 "HLT_e12_lhloose_nodphires_L1EM10VH",
                 "HLT_e12_lhmedium",
                 "HLT_e12_lhmedium_2mu10",
                 "HLT_e12_lhmedium_nod0",
                 "HLT_e12_lhmedium_nod0_2mu10",
                 "HLT_e12_loose",
                 "HLT_e12_loose_2mu10",
                 "HLT_e12_loose_L1EM10VH",
                 "HLT_e12_medium",
                 "HLT_e12_medium_2mu10",
                 "HLT_e12_vloose_L1EM10VH",
                 "HLT_e13_etcut_trkcut_L1EM12",
                 "HLT_e140_etcut",
                 "HLT_e140_lhloose",
                 "HLT_e140_lhloose_L1EM24VHI",
                 "HLT_e140_lhloose_nod0",
                 "HLT_e140_lhloose_nod0_L1EM24VHI",
                 "HLT_e140_lhvloose",
                 "HLT_e140_lhvloose_nod0",
                 "HLT_e14_etcut",
                 "HLT_e14_lhtight",
                 "HLT_e14_lhtight_nod0",
                 "HLT_e14_tight",
                 "HLT_e15_lhloose_cutd0dphideta_L1EM13VH",
                 "HLT_e15_lhloose_L1EM13VH",
                 "HLT_e15_lhloose_nod0_L1EM13VH",
                 "HLT_e15_lhtight_iloose_L1EM13VH",
                 "HLT_e15_lhvloose_nod0_L1EM7",
                 "HLT_e15_loose_L1EM13VH",
                 "HLT_e15_vloose_L1EM13VH",
                 "HLT_e17_lhloose",
                 "HLT_e17_lhloose_2e9_lhloose",
                 "HLT_e17_lhloose_cutd0dphideta",
                 "HLT_e17_lhloose_cutd0dphideta_L1EM15",
                 "HLT_e17_lhloose_mu14",
                 "HLT_e17_lhloose_nod0",
                 "HLT_e17_lhloose_nod0_2e9_lhloose_nod0",
                 "HLT_e17_lhloose_nod0_L1EM15",
                 "HLT_e17_lhloose_nod0_mu14",
                 "HLT_e17_lhloose_nodeta_L1EM15",
                 "HLT_e17_lhloose_nodphires_L1EM15",
                 "HLT_e17_lhmedium",
                 "HLT_e17_lhmedium_2e9_lhmedium",
                 "HLT_e17_lhmedium_iloose",
                 "HLT_e17_lhmedium_iloose_L1EM15HI",
                 "HLT_e17_lhmedium_ivarloose_L1EM15HI",
                 "HLT_e17_lhmedium_L1EM15HI",
                 "HLT_e17_lhmedium_nod0",
                 "HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0",
                 "HLT_e17_lhmedium_nod0_iloose",
                 "HLT_e17_lhmedium_nod0_iloose_L1EM15HI",
                 "HLT_e17_lhmedium_nod0_ivarloose_L1EM15HI",
                 "HLT_e17_lhmedium_nod0_L1EM15HI",
                 "HLT_e17_loose",
                 "HLT_e17_loose_mu14",
                 "HLT_e17_medium",
                 "HLT_e17_medium_iloose",
                 "HLT_e17_medium_iloose_L1EM15HI",
                 "HLT_e17_medium_ivarloose_L1EM15HI",
                 "HLT_e17_medium_L1EM15HI",
                 "HLT_e17_vloose",
                 "HLT_e200_etcut",
                 "HLT_e20_lhmedium",
                 "HLT_e20_lhmedium_nod0",
                 "HLT_e20_lhmedium_nod0_g35_loose",
                 "HLT_e20_lhvloose_nod0_L1EM12",
                 "HLT_e20_medium",
                 "HLT_e20_medium_g35_loose",
                 "HLT_e20_vloose",
                 "HLT_e24_lhmedium_iloose",
                 "HLT_e24_lhmedium_ivarloose",
                 "HLT_e24_lhmedium_L1EM15VH",
                 "HLT_e24_lhmedium_L1EM15VH_g25_medium",
                 "HLT_e24_lhmedium_L1EM20VHI_mu8noL1",
                 "HLT_e24_lhmedium_nod0_iloose",
                 "HLT_e24_lhmedium_nod0_L1EM15VH",
                 "HLT_e24_lhmedium_nod0_L1EM15VH_g25_medium",
                 "HLT_e24_lhmedium_nod0_L1EM18VH",
                 "HLT_e24_lhmedium_nod0_L1EM20VH",
                 "HLT_e24_lhmedium_nod0_L1EM20VHI",
                 "HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1",
                 "HLT_e24_lhtight_iloose",
                 "HLT_e24_lhtight_ivarloose",
                 "HLT_e24_lhtight_L1EM20VH",
                 "HLT_e24_lhtight_nod0_iloose",
                 "HLT_e24_lhtight_nod0_ivarloose",
                 "HLT_e24_lhtight_nod0_L1EM20VH",
                 "HLT_e24_lhvloose_L1EM18VH",
                 "HLT_e24_lhvloose_L1EM20VH",
                 "HLT_e24_loose_L1EM20VHI",
                 "HLT_e24_medium_iloose_L1EM20VH",
                 "HLT_e24_medium_L1EM15VH",
                 "HLT_e24_medium_L1EM15VH_g25_medium",
                 "HLT_e24_medium_L1EM20VHI",
                 "HLT_e24_medium_L1EM20VHI_mu8noL1",
                 "HLT_e24_tight_L1EM20VHI",
                 "HLT_e24_vloose_L1EM18VH",
                 "HLT_e24_vloose_L1EM20VH",
                 "HLT_e25_lhvloose_L1EM15",
                 "HLT_e25_lhvloose_nod0_L1EM15",
                 "HLT_e26_lhmedium_L1EM22VHI",
                 "HLT_e26_lhmedium_L1EM22VHI_mu8noL1",
                 "HLT_e26_lhmedium_nod0_L1EM22VHI",
                 "HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1",
                 "HLT_e26_lhtight_cutd0dphideta_ivarloose",
                 "HLT_e26_lhtight_iloose",
                 "HLT_e26_lhtight_ivarloose",
                 "HLT_e26_lhtight_nod0",
                 "HLT_e26_lhtight_nod0_iloose",
                 "HLT_e26_lhtight_nod0_ivarloose",
                 "HLT_e26_lhtight_smooth_iloose",
                 "HLT_e26_lhtight_smooth_ivarloose",
                 "HLT_e26_lhvloose_L1EM20VH",
                 "HLT_e26_lhvloose_nod0_L1EM20VH",
                 "HLT_e26_medium_L1EM22VHI",
                 "HLT_e26_medium_L1EM22VHI_mu8noL1",
                 "HLT_e26_vloose_L1EM20VH",
                 "HLT_e28_lhtight_iloose",
                 "HLT_e28_lhtight_ivarloose",
                 "HLT_e28_lhtight_nod0_iloose",
                 "HLT_e28_lhtight_nod0_ivarloose",
                 "HLT_e28_lhtight_nod0_ivarloose_L1EM22VHI",
                 "HLT_e28_lhtight_nod0_L1EM22VHI",
                 "HLT_e28_lhtight_smooth_iloose",
                 "HLT_e28_lhtight_smooth_ivarloose",
                 "HLT_e28_tight_iloose",
                 "HLT_e300_etcut",
                 "HLT_e30_lhvloose_L1EM15",
                 "HLT_e30_lhvloose_nod0_L1EM15",
                 "HLT_e40_lhvloose",
                 "HLT_e40_lhvloose_L1EM15",
                 "HLT_e40_lhvloose_nod0",
                 "HLT_e40_lhvloose_nod0_L1EM15",
                 "HLT_e40_vloose",
                 "HLT_e4_etcut",
                 "HLT_e50_lhvloose_L1EM15",
                 "HLT_e50_lhvloose_nod0_L1EM15",
                 "HLT_e5_lhloose",
                 "HLT_e5_lhloose_nod0",
                 "HLT_e5_lhtight",
                 "HLT_e5_lhtight_e4_etcut",
                 "HLT_e5_lhtight_nod0",
                 "HLT_e5_lhtight_nod0_e4_etcut",
                 "HLT_e5_loose",
                 "HLT_e5_tight",
                 "HLT_e5_tight_e4_etcut",
                 "HLT_e60_lhloose",
                 "HLT_e60_lhloose_nod0",
                 "HLT_e60_lhmedium_L1EM24VHI",
                 "HLT_e60_lhmedium_nod0_L1EM24VHI",
                 "HLT_e60_lhvloose",
                 "HLT_e60_lhvloose_nod0",
                 "HLT_e60_loose",
                 "HLT_e60_medium_L1EM24VHI",
                 "HLT_e60_vloose",
                 "HLT_e70_etcut",
                 "HLT_e70_lhloose",
                 "HLT_e70_lhloose_nod0",
                 "HLT_e70_lhvloose",
                 "HLT_e70_lhvloose_nod0",
                 "HLT_e70_loose",
                 "HLT_e7_lhmedium",
                 "HLT_e7_lhmedium_mu24",
                 "HLT_e7_lhmedium_nod0",
                 "HLT_e7_lhmedium_nod0_mu24",
                 "HLT_e7_medium",
                 "HLT_e7_medium_mu24",
                 "HLT_e80_etcut",
                 "HLT_e80_lhvloose",
                 "HLT_e80_lhvloose_nod0",
                 "HLT_e9_etcut",
                 "HLT_e9_lhloose",
                 "HLT_e9_lhloose_nod0",
                 "HLT_e9_lhmedium",
                 "HLT_e9_lhmedium_nod0",
                 "HLT_e9_lhtight",
                 "HLT_e9_lhtight_nod0",
                 "HLT_e9_loose",
                 "HLT_e9_medium",
                 "HLT_e9_tight",
                 "HLT_mu0_perf",
                 "HLT_mu14_iloose",
                 "HLT_mu14_ivarloose",
                 "HLT_mu20",
                 "HLT_mu20_2mu4noL1",
                 "HLT_mu20_imedium_L1MU10",
                 "HLT_mu20_ivarmedium_L1MU10",
                 "HLT_mu20_mu8noL1",
                 "HLT_mu22",
                 "HLT_mu22_2mu4noL1",
                 "HLT_mu22_mu8noL1",
                 "HLT_mu24_2mu4noL1",
                 "HLT_mu24_iloose",
                 "HLT_mu24_iloose_L1MU15",
                 "HLT_mu24_imedium",
                 "HLT_mu24_ivarloose",
                 "HLT_mu24_ivarloose_L1MU15",
                 "HLT_mu24_ivarmedium",
                 "HLT_mu24_mu8noL1",
                 "HLT_mu26",
                 "HLT_mu26_imedium",
                 "HLT_mu26_ivarmedium",
                 "HLT_mu26_mu8noL1",
                 "HLT_mu28_imedium",
                 "HLT_mu28_ivarmedium"
                 ]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HIGG3D3TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name                = "HIGG3D3TrigSkimmingTool",
                                                                    TriggerListOR       = TriggerChains )
ToolSvc += HIGG3D3TrigSkimmingTool

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG3D3ThinningHelper = ThinningHelper( "HIGG3D3ThinningHelper" )
HIGG3D3ThinningHelper.TriggerChains = " | ".join(TriggerChains)
HIGG3D3ThinningHelper.AppendToStream( HIGG3D3Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]

# InDetTrackParticle Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#MET Track Thinning
MET_track_thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
HIGG3D3MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG3D3MetTPThinningTool",
                                                                      ThinningService        = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                      SelectionString        = MET_track_thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG3D3MetTPThinningTool
thinningTools.append(HIGG3D3MetTPThinningTool)

#Jet Track Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG3D3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG3D3JetTPThinningTool",
                                                                          ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += HIGG3D3JetTPThinningTool
thinningTools.append(HIGG3D3JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG3D3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG3D3MuonTPThinningTool",
                                                                           ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D3MuonTPThinningTool
thinningTools.append(HIGG3D3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG3D3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG3D3ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D3ElectronTPThinningTool
thinningTools.append(HIGG3D3ElectronTPThinningTool)

# Tracks themselves
inclusive_track_thinning_expression = "(InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV) < 1.5*mm && InDetTrackParticles.pt > 10.*GeV)"
HIGG3D3TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG3D3TPThinningTool",
                                                                   ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                   SelectionString         = inclusive_track_thinning_expression,
                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D3TPThinningTool
thinningTools.append(HIGG3D3TPThinningTool)

# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond3 = "(abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 6)"  # b and Top quark
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG3D3TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG3D3TruthThinningTool",
                                                                     ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                     ParticleSelectionString = truth_expression,
                                                                     PreserveDescendants     = False,
                                                                     PreserveAncestors       = True,
                                                                     SimBarcodeOffset        = DerivationFrameworkSimBarcodeOffset )

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG3D3TruthThinningTool
    thinningTools.append(HIGG3D3TruthThinningTool)

#====================================================================
# SKIMMING TOOLS
#====================================================================
electronIDRequirements = '(Electrons.DFCommonElectronsLHVeryLoose)'
electronRequirements = '(Electrons.pt > 7*GeV) && (abs(Electrons.eta) < 2.6) && '+electronIDRequirements
muonsRequirements = '(Muons.pt > 7*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonGoodMuon)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG3D3ElJDeltaRTool = DerivationFramework__DeltaRTool( name = "HIGG3D3ElJDeltaRTool",
                                                        StoreGateEntryName = "dRElecJet",
                                                        ObjectRequirements = electronRequirements,
                                                        SecondObjectRequirements = "(AntiKt4EMTopoJets.pt > 15*GeV)",
                                                        ContainerName = "Electrons",
                                                        SecondContainerName = "AntiKt4EMTopoJets" )
ToolSvc += HIGG3D3ElJDeltaRTool
HIGG3D3MuJDeltaRTool = DerivationFramework__DeltaRTool( name = "HIGG3D3MuJDeltaRTool",
                                                        StoreGateEntryName = "dRMuJet",
                                                        ObjectRequirements = muonsRequirements,
                                                        SecondObjectRequirements = "(AntiKt4EMTopoJets.pt > 15*GeV)",
                                                        ContainerName = "Muons",
                                                        SecondContainerName = "AntiKt4EMTopoJets" )
ToolSvc += HIGG3D3MuJDeltaRTool

eSelection = '(count('+electronRequirements+')>=1)'
mSelection = '(count('+muonsRequirements+')>=1)'
expression = '(' + eSelection+' || '+mSelection+')' + '&& ( (count(dRMuJet > 2.5) >= 1) || (count(dRElecJet > 2.5) >= 1))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG3D3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG3D3SkimmingTool1",
                                                                   expression = expression)
ToolSvc += HIGG3D3SkimmingTool

skimmingTools = []

# Keep only ~5% of MC to reduce the total derivation size, adding this to the skimming tool list FIRST
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
    mcPrescaleTool = DerivationFramework__PrescaleTool(name = "HIGG3D3MCPrescaleTool",
                                                       Prescale = 9)
    ToolSvc += mcPrescaleTool
    skimmingTools.append(mcPrescaleTool)

skimmingTools += [HIGG3D3TrigSkimmingTool,HIGG3D3SkimmingTool]

higg3d3Seq = CfgMgr.AthSequencer("HIGG3d3Sequence")
higg3d3PreSeq = CfgMgr.AthSequencer("HIGG3d3PreSelectionSequence")

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
# needed for non-prompt lepton tagging below
reducedJetList = ["AntiKt4PV0TrackJets"]
replaceAODReducedJets(reducedJetList, higg3d3Seq,"HIGG3D3")

#===================================================================
# B-TAGGING
#===================================================================
# Add flavor tagging to the PFlow Jet collections
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg3d3Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg3d3Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

#========================================
# CREATE THE DERIVATION KERNEL ALGORITHMS
#========================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
higg3d3PreSeq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D3Kernel_skimming",
                                                              SkimmingTools     = skimmingTools,
                                                              AugmentationTools = [HIGG3D3ElJDeltaRTool,HIGG3D3MuJDeltaRTool] # needed by skimming
                                                              )

DerivationFrameworkJob += higg3d3PreSeq
higg3d3PreSeq += higg3d3Seq

higg3d3Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D3Kernel_thinning",
                                                           ThinningTools = thinningTools
                                                           )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D3SlimmingHelper = SlimmingHelper("HIGG3D3SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D3ExtraContent import *
#HIGG3D3SlimmingHelper.UserContent = HIGG3D3Content
HIGG3D3SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Muons",
                                           "Photons",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4EMPFlowJets",
                                           "InDetTrackParticles",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt4EMPFlow",
                                           "PrimaryVertices",
                                           "MET_Reference_AntiKt4EMTopo" ]

HIGG3D3SlimmingHelper.ExtraVariables = list(HIGG3D3ExtraVariables)
HIGG3D3SlimmingHelper.AllVariables = list(HIGG3D3ExtraContainers)

HIGG3D3SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
HIGG3D3SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer',
                                            'BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer'
                                           }

if globalflags.DataSource()=='geant4':
    HIGG3D3SlimmingHelper.SmartCollections += ["AntiKt4TruthJets",
                                               "AntiKt4TruthWZJets"]

    # Add special truth containers
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addBSMAndDownstreamParticles
    addStandardTruthContents()
    addBSMAndDownstreamParticles()

    HIGG3D3SlimmingHelper.AppendToDictionary = {
                                                'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                               }
    HIGG3D3SlimmingHelper.AllVariables += list(HIGG3D3ExtraTruthContainers)
    HIGG3D3SlimmingHelper.ExtraVariables += list(HIGG3D3ExtraTruthVariables)

# Add Trigger content
HIGG3D3SlimmingHelper.IncludeMuonTriggerContent = True
HIGG3D3SlimmingHelper.IncludeEGammaTriggerContent = True

# Add MET to output stream
addMETOutputs(HIGG3D3SlimmingHelper, ["AntiKt4EMPFlow", "Track"],
                                     ["AntiKt4EMPFlow",
                                      "AntiKt4EMTopo"] # smart collections list
                                     )

HIGG3D3SlimmingHelper.AppendContentToStream(HIGG3D3Stream)
