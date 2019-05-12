#********************************************************************
# EXOT8.py
# reductionConf flag EXOT8 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon            import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon    import *
from DerivationFrameworkJetEtMiss.METCommon            import *
from DerivationFrameworkEGamma.EGammaCommon            import *
from DerivationFrameworkMuons.MuonsCommon              import *
from DerivationFrameworkFlavourTag.HbbCommon           import *
from DerivationFrameworkCore.WeightMetadata            import *
from DerivationFrameworkFlavourTag.FlavourTagCommon    import FlavorTagInit
from DerivationFrameworkFlavourTag.FlavourTagCommon    import applyBTagging_xAODColl
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecConf import JetAlgorithm
from DerivationFrameworkJetEtMiss.DFJetMetFlags        import *
from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__JetDecorAlg
from DerivationFrameworkInDet.InDetCommon              import *

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkCore.DerivationFrameworkCoreConf   import DerivationFramework__DerivationKernel

#========================================================================================================================================
# Set up Stream
#========================================================================================================================================
streamName  = derivationFlags.WriteDAOD_EXOT8Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_EXOT8Stream )
EXOT8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT8Stream.AcceptAlgs(["EXOT8Kernel"])

#
#  Trigger Nav thinning
#
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT8ThinningHelper = ThinningHelper( "EXOT8ThinningHelper" )
if globalflags.DataSource() is not "geant4":
    # single small-R jet
    EXOT8ThinningHelper.TriggerChains = 'HLT_j0_perf_L1RD0_FILLED|HLT_j15|HLT_j25|HLT_j35|HLT_j45|HLT_j55|HLT_j60|HLT_j85|HLT_j110|HLT_j175|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j260|HLT_j340|HLT_j360|HLT_j380|HLT_j400|HLT_j420|HLT_j450|HLT_j225_gsc380_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j225_gsc400_boffperf_split|HLT_j225_gsc420_boffperf_split|HLT_j225_gsc440_boffperf_split|'

    # multi small-R jet
    EXOT8ThinningHelper.TriggerChains += 'HLT_3j175|HLT_3j200|HLT_3j225|HLT_4j85|HLT_4j100|HLT_4j120|HLT_5j55|HLT_5j60|HLT_5j70_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j85_L14J15|HLT_5j90|HLT_5j90_L14J150ETA25|HLT_6j45|HLT_6j60|HLT_6j60_L14J15|HLT_6j70|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j70_L14J15|HLT_7j45|HLT_7j45_L14J15|HLT_7j45_L14J150ETA25|HLT_7j45_L14J20|HLT_10j40_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_10j40_L14J20|HLT_10j40_L14J150ETA25|HLT_10j40_L16J15|HLT_4j60_gsc100_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_4j60_gsc115_boffperf_split|HLT_4j85_gsc115_boffperf_split|HLT_5j50_gsc70_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j50_gsc70_boffperf_split_L14J150ETA25|HLT_5j60_gsc85_boffperf_split|HLT_5j60_gsc85_boffperf_split_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j60_gsc85_boffperf_split_L14J150ETA25|HLT_5j60_gsc90_boffperf_split|HLT_5j60_gsc90_boffperf_split_L14J150ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j45_gsc60_boffperf_split|HLT_6j45_gsc60_boffperf_split_L14J150ETA25|HLT_6j50_gsc70_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j50_gsc70_boffperf_split_L14J15|HLT_6j50_gsc70_boffperf_split_L14J150ETA25|HLT_7j25_gsc45_boffperf_split_L14J20|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_7j25_gsc45_boffperf_split_L14J150ETA25|HLT_7j35_gsc45_boffperf_split_L14J15|HLT_5j65_0eta240_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j70_0eta240_L14J15|HLT_5j75_0eta240|HLT_5j75_0eta240_L14J150ETA25|HLT_6j45_0eta240|HLT_6j45_0eta240_L14J20|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j50_0eta240_L14J15|HLT_6j55_0eta240_L14J15|HLT_6j60_0eta240_L14J20|HLT_6j60_0eta240_L14J150ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j50_gsc65_boffperf_split_0eta240|HLT_5j50_gsc65_boffperf_split_0eta240_L14J150ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j50_gsc70_boffperf_split_0eta240|HLT_5j50_gsc70_boffperf_split_0eta240_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j50_gsc70_boffperf_split_0eta240_L14J150ETA25|HLT_5j55_gsc75_boffperf_split_0eta240|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_5j55_gsc75_boffperf_split_0eta240_L14J150ETA25|HLT_6j25_gsc45_boffperf_split_0eta240_L14J20|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j25_gsc45_boffperf_split_0eta240_L14J150ETA25|HLT_6j25_gsc50_boffperf_split_0eta240_L14J20|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j25_gsc50_boffperf_split_0eta240_L14J150ETA25|HLT_6j35_gsc55_boffperf_split_0eta240_L14J150ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j35_gsc55_boffperf_split_0eta240_L14J20|HLT_6j35_gsc55_boffperf_split_0eta240_L15J150ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_6j45_gsc55_boffperf_split_0eta240_L14J15|HLT_j260_320eta490|'

    # jet+MET
    EXOT8ThinningHelper.TriggerChains += 'HLT_j80_xe80|'

    # large-R jets
    EXOT8ThinningHelper.TriggerChains += 'HLT_j.*a10.*|'

    # HT
    EXOT8ThinningHelper.TriggerChains += 'HLT_ht700_L1J75|HLT_ht1000|HLT_ht1000_L1J100|'

    # b-jet
    EXOT8ThinningHelper.TriggerChains += 'HLT_j225_bloose|HLT_j175_bmedium|HLT_j175_bmv2c2040_split|HLT_j225_bmv2c2060_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j275_bmv2c2070_split|HLT_j300_bmv2c2077_split|HLT_j360_bmv2c2085_split|HLT_j175_gsc225_bmv2c1040_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j225_gsc275_bmv2c1070_split|HLT_j225_gsc275_bmv2c1060_split|HLT_j225_gsc275_bhmv2c1060_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j225_gsc300_bmv2c1077_split|HLT_j225_gsc300_bmv2c1070_split|HLT_j225_gsc300_bhmv2c1070_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j225_gsc360_bmv2c1085_split|HLT_j225_gsc360_bmv2c1077_split|HLT_j225_gsc360_bhmv2c1077_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j65_btight_3j65_L13J25.0ETA23|HLT_j65_bmv2c2070_split_3j65_L14J15|HLT_j70_bmedium_3j70_L13J25.0ETA23|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j75_bmv2c2070_split_3j75_L14J15.0ETA25|HLT_j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split|HLT_j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j100_2j55_bmedium|HLT_j150_bmedium_j50_bmedium|HLT_2j35_btight_2j35_L13J25.0ETA23|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j45_bmedium_2j45_L13J25.0ETA23|HLT_2j65_btight_j65|HLT_2j70_bmedium_j70|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j150_bmv2c2077_split_j50_bmv2c2077_split|HLT_j150_bmv2c2060_split_j50_bmv2c2060_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j175_bmv2c2060_split_j50_bmv2c2050_split|HLT_j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split|HLT_j100_2j55_bmv2c2077_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j100_2j55_bmv2c2060_split|HLT_2j65_bmv2c2070_split_j65|HLT_2j70_bmv2c2070_split_j70|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j70_bmv2c2060_split_j70|HLT_2j75_bmv2c2070_split_j75|HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1077_split_L1J85_3J30|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30|HLT_2j35_bmv2c2070_split_2j35_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25|HLT_2j35_bmv2c1040_split_2j35_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j35_bmv2c2060_split_2j35_L14J15.0ETA25|HLT_2j35_bmv2c2050_split_2j35_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j15_gsc35_bmv2c1050_split_2j15_gsc35_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25|HLT_2j35_bmv2c1060_split_3j35|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j45_bmv2c2077_split_3j45|HLT_2j45_bmv2c2077_split_3j45_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j15_gsc35_bmv2c1060_split_3j15_gsc35_boffperf_split|HLT_2j15_gsc35_bmv2c1050_split_3j15_gsc35_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j25_gsc45_bmv2c1070_split_3j25_gsc45_boffperf_split|HLT_2j25_gsc45_bmv2c1060_split_3j25_gsc45_boffperf_split|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split|HLT_3j50_gsc65_bmv2c1077_split_L13J35.0ETA23|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_3j35_bmv2c1070_split_j35_L14J15.0ETA25|HLT_3j15_gsc35_bmv2c1070_split_j15_gsc35_boffperf_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_4j35_bmv2c1077_split_L14J15.0ETA25|HLT_2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_4j15_gsc35_bmv2c1077_split_L14J15.0ETA25|HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_j55_bmv2c2060_split_ht500_L14J15|HLT_j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j55_bmv2c2070_split_ht300_L14J15|HLT_2j55_bmv2c2060_split_ht300_L14J15|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21|HLT_mu4_j40_dr05_3j40_L14J20|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_mu4_4j40_dr05_L1MU4_3J15|HLT_mu4_3j45_dr05_L1MU4_3J15|HLT_j80_bmv2c2060_split_xe60_L12J50_XE40|'

    # single electron
    EXOT8ThinningHelper.TriggerChains += 'HLT_e24_lhmedium_L1EM20VH|HLT_e60_lhmedium|HLT_e120_lhloose|HLT_e24_lhtight_nod0_ivarloose|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_e24_lhmedium_nod0_L1EM20VH|HLT_e60_lhmedium_nod0|HLT_e60_medium|HLT_e140_lhloose_nod0|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_e300_etcut|HLT_e26_lhtight_nod0_ivarloose|HLT_e26_lhtight_nod0|'

    # single muon
    EXOT8ThinningHelper.TriggerChains += 'HLT_mu20_iloose_L1MU15|HLT_mu40|HLT_mu60_0eta105_msonly|HLT_mu24_iloose|HLT_mu24_iloose_L1MU15|HLT_mu24_ivarloose|'
    EXOT8ThinningHelper.TriggerChains += 'HLT_mu24_ivarloose_L1MU15|HLT_mu50|HLT_mu24_ivarmedium|HLT_mu24_imedium|HLT_mu26_ivarmedium|HLT_mu26_imedium'
EXOT8ThinningHelper.AppendToStream( EXOT8Stream )

#========================================================================================================================================
# Thinning Tools
#========================================================================================================================================
thinningTools=[]

#########################################
# Tracks associated with akt2 jets
#########################################
# It is necessary to apply the akt2-based track thinning before other track
# thinning tools due to the faulty logic of the thinning tools
from ThinningUtils.ThinningUtilsConf import DeltaRThinningTool
# Applying only DeltaR thinning and not EleLink thinning is fine as long as ConeSize
# is sufficiently large compared to the jet size.  If it is reduced to something close
# to the size of the jet, there is no guarantee that all ghost-associated tracks will
# be picked up

EXOT8BaselineTrack = "(InDetTrackParticles.EXOT8DFLoose) && (InDetTrackParticles.pt > 0.5*GeV) && (abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm) && (InDetTrackParticles.d0 < 2.0*mm)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT8TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name            = "EXOT8TrackParticleThinningTool",
                                                                            ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                            SelectionString = EXOT8BaselineTrack,
                                                                            InDetTrackParticlesKey = "InDetTrackParticles",
                                                                            ApplyAnd        = True)

ToolSvc += EXOT8TrackParticleThinningTool
thinningTools.append(EXOT8TrackParticleThinningTool)

EXOT8ak2DeltaRTrackThinningTool = DeltaRThinningTool(name            = "EXOT8ak2DeltaRTrackThinningTool",
                                                     ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                     SGKey           = "InDetTrackParticles",
                                                     ConeSize        = 0.33,
                                                     ApplyAnd        = True)

ToolSvc += EXOT8ak2DeltaRTrackThinningTool

from ThinningUtils.ThinningUtilsConf import ThinAssociatedObjectsTool
EXOT8ak2TrackThinningTool = ThinAssociatedObjectsTool(name               = "EXOT8ak2TrackThinningTool",
                                                      ThinningService    = EXOT8ThinningHelper.ThinningSvc(),
                                                      SGKey              = "AntiKt2LCTopoJets",
                                                      ChildThinningTools = [EXOT8ak2DeltaRTrackThinningTool])

ToolSvc += EXOT8ak2TrackThinningTool
thinningTools.append(EXOT8ak2TrackThinningTool)

#########################################
# Tracks associated with Muons
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "EXOT8MuonTPThinningTool",
                                                                         ThinningService        = EXOT8ThinningHelper.ThinningSvc(),
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         SelectionString        = "Muons.pt > 5*GeV",
                                                                         ConeSize               = 0)
ToolSvc += EXOT8MuonTPThinningTool
thinningTools.append(EXOT8MuonTPThinningTool)

#########################################
# Tracks associated with Electrons
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "EXOT8ElectronTPThinningTool",
                                                                                ThinningService        = EXOT8ThinningHelper.ThinningSvc(),
                                                                                SGKey                  = "Electrons",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                SelectionString        = "Electrons.pt > 25*GeV", # Remove tracks with high pt cut
                                                                                ConeSize               = 0)
ToolSvc += EXOT8ElectronTPThinningTool
thinningTools.append(EXOT8ElectronTPThinningTool)

##########################################
#Tracks associated with Photons
##########################################
EXOT8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT8PhotonTPThinningTool",
                                                                             ThinningService         = EXOT8ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += EXOT8PhotonTPThinningTool
thinningTools.append(EXOT8PhotonTPThinningTool)

# Thin ak4 jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT8ak4ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak4ThinningTool",
                                                                  ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                  ContainerName   = "AntiKt4EMTopoJets",
                                                                  ApplyAnd        = False,
                                                                  SelectionString = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20*GeV")
ToolSvc += EXOT8ak4ThinningTool
thinningTools.append(EXOT8ak4ThinningTool)

# Thin ak10 jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT8ak10ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak10ThinningTool",
                                                                   ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                   ContainerName   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                   ApplyAnd        = False,
                                                                   SelectionString = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV")
ToolSvc += EXOT8ak10ThinningTool
thinningTools.append(EXOT8ak10ThinningTool)


# Thin ak10 untrimmed jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT8ak10UntrimmedThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak10UntrimmedThinningTool",
                                                                             ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                             ContainerName   = "AntiKt10LCTopoJets",
                                                                             ApplyAnd        = False,
                                                                             SelectionString = "AntiKt10LCTopoJets.pt > 150*GeV")
ToolSvc += EXOT8ak10UntrimmedThinningTool
thinningTools.append(EXOT8ak10UntrimmedThinningTool)

# Thin akt2 jets
EXOT8ak2Jet = "(AntiKt2LCTopoJets.pt > 10*GeV) && (abs(AntiKt2LCTopoJets.eta) < 3.1)"

from ThinningUtils.ThinningUtilsConf import DeltaRThinningTool
EXOT8ak10DeltaRak2ThinningTool = DeltaRThinningTool(name            = "EXOT8ak10DeltaRak2ThinningTool",
                                                    ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                    SGKey           = "AntiKt2LCTopoJets",
                                                    ConeSize        = 1.4,
                                                    ApplyAnd        = True)

ToolSvc += EXOT8ak10DeltaRak2ThinningTool

EXOT8ak2ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak2ThinningTool",
                                                                  ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                  SelectionString = EXOT8ak2Jet,
                                                                  ContainerName   = "AntiKt2LCTopoJets",
                                                                  ApplyAnd        = True)

ToolSvc += EXOT8ak2ThinningTool
thinningTools.append(EXOT8ak2ThinningTool)

from ThinningUtils.ThinningUtilsConf import ThinAssociatedObjectsTool
EXOT8ak10ak2ThinningTool = ThinAssociatedObjectsTool(name               = "EXOT8ak10ak2ThinningTool",
                                                     ThinningService    = EXOT8ThinningHelper.ThinningSvc(),
                                                     SGKey              = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                     SelectionString    = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV)",
                                                     ChildThinningTools = [EXOT8ak10DeltaRak2ThinningTool])

ToolSvc += EXOT8ak10ak2ThinningTool
thinningTools.append(EXOT8ak10ak2ThinningTool)

#########################################
# truth containers a la TRUTH3
#########################################
from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents

if globalflags.DataSource()=="geant4":
    addStandardTruthContents()

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__BJetRegressionVariables
EXOT8BJetRegressionVariables_EMTopo = DerivationFramework__BJetRegressionVariables(name = "EXOT8BJetRegressionVariables_EMTopo",
                                                                                   ContainerName = "AntiKt4EMTopoJets",
                                                                                   AssociatedTracks = "GhostTrack",
                                                                                   MinTrackPtCuts = [0])
EXOT8BJetRegressionVariables_EMPFlow = DerivationFramework__BJetRegressionVariables(name = "EXOT8BJetRegressionVariables_EMPFlow",
                                                                                    ContainerName = "AntiKt4EMPFlowJets",
                                                                                    AssociatedTracks = "GhostTrack",
                                                                                    MinTrackPtCuts = [0])

ToolSvc += EXOT8BJetRegressionVariables_EMTopo
augmentationTools.append(EXOT8BJetRegressionVariables_EMTopo)
ToolSvc += EXOT8BJetRegressionVariables_EMPFlow
augmentationTools.append(EXOT8BJetRegressionVariables_EMPFlow)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
EXOT8TrackSelectionTool = DerivationFramework__InDetTrackSelectionToolWrapper(name = "EXOT8TrackSelectionTool",
                                                                              ContainerName = "InDetTrackParticles",
                                                                              DecorationName = "EXOT8DFLoose" )
EXOT8TrackSelectionTool.TrackSelectionTool.CutLevel = "Loose"
ToolSvc += EXOT8TrackSelectionTool
augmentationTools.append(EXOT8TrackSelectionTool)

triggers_for_matching = ['HLT_g60_loose',
                         'HLT_g140_loose',
                         'HLT_g160_loose'
                         ]

from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="EXOT6",
                                                 PhotonTriggers=triggers_for_matching)

augmentationTools.append(TrigMatchAug)

#========================================================================================================================================
# Triggers (https://indico.cern.ch/event/403233/contribution/4/material/slides/0.pdf)
#
# Not yet known: 25ns menu, L1_HT menu
#========================================================================================================================================
triggers = [# single small-R jet
            "HLT_j0_perf_L1RD0_FILLED",
            "HLT_j15",
            "HLT_j25",
            "HLT_j35",
            "HLT_j45",
            "HLT_j55",
            "HLT_j60",
            "HLT_j85",
            "HLT_j110",
            "HLT_j175",
            "HLT_j260",
            "HLT_j340",
            "HLT_j360",
            "HLT_j380",
            "HLT_j400",
            "HLT_j420",
            "HLT_j450",
            "HLT_j225_gsc380_boffperf_split",
            "HLT_j225_gsc400_boffperf_split",
            "HLT_j225_gsc420_boffperf_split",
            "HLT_j225_gsc440_boffperf_split",

            # multi small-R jet
            "HLT_3j175",
            "HLT_3j200",
            "HLT_3j225",
            "HLT_4j85",
            "HLT_4j100",
            "HLT_4j120",
            "HLT_5j55",
            "HLT_5j60",
            "HLT_5j70_L14J15",
            "HLT_5j85_L14J15",
            "HLT_5j90",
            "HLT_5j90_L14J150ETA25",
            "HLT_6j45",
            "HLT_6j60",
            "HLT_6j60_L14J15",
            "HLT_6j70",
            "HLT_6j70_L14J15",
            "HLT_7j45",
            "HLT_7j45_L14J15",
            "HLT_7j45_L14J150ETA25",
            "HLT_7j45_L14J20",
            "HLT_10j40_L14J15",
            "HLT_10j40_L14J20",
            "HLT_10j40_L14J150ETA25",
            "HLT_10j40_L16J15",
            "HLT_4j60_gsc100_boffperf_split",
            "HLT_4j60_gsc115_boffperf_split",
            "HLT_4j85_gsc115_boffperf_split",
            "HLT_5j50_gsc70_boffperf_split",
            "HLT_5j50_gsc70_boffperf_split_L14J150ETA25",
            "HLT_5j60_gsc85_boffperf_split",
            "HLT_5j60_gsc85_boffperf_split_L14J15",
            "HLT_5j60_gsc85_boffperf_split_L14J150ETA25",
            "HLT_5j60_gsc90_boffperf_split",
            "HLT_5j60_gsc90_boffperf_split_L14J150ETA25",
            "HLT_6j45_gsc60_boffperf_split",
            "HLT_6j45_gsc60_boffperf_split_L14J150ETA25",
            "HLT_6j50_gsc70_boffperf_split",
            "HLT_6j50_gsc70_boffperf_split_L14J15",
            "HLT_6j50_gsc70_boffperf_split_L14J150ETA25",
            "HLT_7j25_gsc45_boffperf_split_L14J20",
            "HLT_7j25_gsc45_boffperf_split_L14J150ETA25",
            "HLT_7j35_gsc45_boffperf_split_L14J15",

            # multi small-R jet: central
            "HLT_5j65_0eta240_L14J15",
            "HLT_5j70_0eta240_L14J15",
            "HLT_5j75_0eta240",
            "HLT_5j75_0eta240_L14J150ETA25",
            "HLT_6j45_0eta240",
            "HLT_6j45_0eta240_L14J20",
            "HLT_6j50_0eta240_L14J15",
            "HLT_6j55_0eta240_L14J15",
            "HLT_6j60_0eta240_L14J20",
            "HLT_6j60_0eta240_L14J150ETA25",

            "HLT_5j50_gsc65_boffperf_split_0eta240",
            "HLT_5j50_gsc65_boffperf_split_0eta240_L14J150ETA25",
            "HLT_5j50_gsc70_boffperf_split_0eta240",
            "HLT_5j50_gsc70_boffperf_split_0eta240_L14J15",
            "HLT_5j50_gsc70_boffperf_split_0eta240_L14J150ETA25",
            "HLT_5j55_gsc75_boffperf_split_0eta240",
            "HLT_5j55_gsc75_boffperf_split_0eta240_L14J150ETA25",
            "HLT_6j25_gsc45_boffperf_split_0eta240_L14J20",
            "HLT_6j25_gsc45_boffperf_split_0eta240_L14J150ETA25",
            "HLT_6j25_gsc50_boffperf_split_0eta240_L14J20",
            "HLT_6j25_gsc50_boffperf_split_0eta240_L14J150ETA25",
            "HLT_6j35_gsc55_boffperf_split_0eta240_L14J150ETA25",
            "HLT_6j35_gsc55_boffperf_split_0eta240_L14J20",
            "HLT_6j35_gsc55_boffperf_split_0eta240_L15J150ETA25",
            "HLT_6j45_gsc55_boffperf_split_0eta240_L14J15",

            # small-R jet: forward
            "HLT_j260_320eta490",

            # jet+MET
            "HLT_j80_xe80",

            # large-R jet triggers
            "HLT_j.*a10.*",

            # HT
            "HLT_ht700_L1J75",
            "HLT_ht1000",
            "HLT_ht1000_L1J100",

            # 1b
            "HLT_j225_bloose",
            "HLT_j175_bmedium",
            "HLT_j175_bmv2c2040_split",
            "HLT_j225_bmv2c2060_split",
            "HLT_j275_bmv2c2070_split",
            "HLT_j300_bmv2c2077_split",
            "HLT_j360_bmv2c2085_split",
            "HLT_j175_gsc225_bmv2c1040_split",
            "HLT_j225_gsc275_bmv2c1070_split",
            "HLT_j225_gsc275_bmv2c1060_split",
            "HLT_j225_gsc275_bhmv2c1060_split",
            "HLT_j225_gsc300_bmv2c1077_split",
            "HLT_j225_gsc300_bmv2c1070_split",
            "HLT_j225_gsc300_bhmv2c1070_split",
            "HLT_j225_gsc360_bmv2c1085_split",
            "HLT_j225_gsc360_bmv2c1077_split",
            "HLT_j225_gsc360_bhmv2c1077_split",

            # 1b+3j
            "HLT_j65_btight_3j65_L13J25.0ETA23",
            "HLT_j65_bmv2c2070_split_3j65_L14J15",
            "HLT_j70_bmedium_3j70_L13J25.0ETA23",
            "HLT_j75_bmv2c2070_split_3j75_L14J15.0ETA25",
            "HLT_j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split",
            "HLT_j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split",
            "HLT_j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split",

            # 2b
            "HLT_j100_2j55_bmedium",
            "HLT_j150_bmedium_j50_bmedium",
            "HLT_2j35_btight_2j35_L13J25.0ETA23",
            "HLT_2j45_bmedium_2j45_L13J25.0ETA23",
            "HLT_2j65_btight_j65",
            "HLT_2j70_bmedium_j70",
            "HLT_j150_bmv2c2077_split_j50_bmv2c2077_split",
            "HLT_j150_bmv2c2060_split_j50_bmv2c2060_split",
            "HLT_j175_bmv2c2060_split_j50_bmv2c2050_split",
            "HLT_j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split",
            "HLT_j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split",

            # 2b+1j
            "HLT_j100_2j55_bmv2c2077_split",
            "HLT_j100_2j55_bmv2c2060_split",
            "HLT_2j65_bmv2c2070_split_j65",
            "HLT_2j70_bmv2c2070_split_j70",
            "HLT_2j70_bmv2c2060_split_j70",
            "HLT_2j75_bmv2c2070_split_j75",

            "HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1077_split_L1J85_3J30",
            "HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30",
            "HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30",

            # 2b+2j
            "HLT_2j35_bmv2c2070_split_2j35_L14J15",
            "HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25",
            "HLT_2j35_bmv2c1040_split_2j35_L14J15.0ETA25",
            "HLT_2j35_bmv2c2060_split_2j35_L14J15.0ETA25",
            "HLT_2j35_bmv2c2050_split_2j35_L14J15",

            "HLT_2j15_gsc35_bmv2c1050_split_2j15_gsc35_boffperf_split_L14J15.0ETA25",
            "HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25",
            "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25",
            "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25",
            "HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25",
            "HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25",
            "HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25",
            "HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25",

            # 2b+3j
            "HLT_2j35_bmv2c1060_split_3j35",
            "HLT_2j45_bmv2c2077_split_3j45",
            "HLT_2j45_bmv2c2077_split_3j45_L14J15.0ETA25",
            "HLT_2j15_gsc35_bmv2c1060_split_3j15_gsc35_boffperf_split",
            "HLT_2j15_gsc35_bmv2c1050_split_3j15_gsc35_boffperf_split",
            "HLT_2j25_gsc45_bmv2c1070_split_3j25_gsc45_boffperf_split",
            "HLT_2j25_gsc45_bmv2c1060_split_3j25_gsc45_boffperf_split",
            "HLT_2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split",

            # 3b
            "HLT_3j50_gsc65_bmv2c1077_split_L13J35.0ETA23",

            # 3b+1j
            "HLT_3j35_bmv2c1070_split_j35_L14J15.0ETA25",
            "HLT_3j15_gsc35_bmv2c1070_split_j15_gsc35_boffperf_split_L14J15.0ETA25",

            # 4b
            "HLT_4j35_bmv2c1077_split_L14J15.0ETA25",
            "HLT_2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25",
            "HLT_4j15_gsc35_bmv2c1077_split_L14J15.0ETA25",
            "HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25",

            # 1b+HT
            "HLT_j55_bmv2c2060_split_ht500_L14J15",
            "HLT_j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21",

            # 2b+HT
            "HLT_2j55_bmv2c2070_split_ht300_L14J15",
            "HLT_2j55_bmv2c2060_split_ht300_L14J15",
            "HLT_2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21",

            # b-jet+mu
            "HLT_mu4_j40_dr05_3j40_L14J20",
            "HLT_mu4_4j40_dr05_L1MU4_3J15",
            "HLT_mu4_3j45_dr05_L1MU4_3J15",

            # b-jet+MET
            "HLT_j80_bmv2c2060_split_xe60_L12J50_XE40",

            # single electron
            "HLT_e24_lhmedium_L1EM20VH",
            "HLT_e60_lhmedium",
            "HLT_e120_lhloose",
            "HLT_e24_lhtight_nod0_ivarloose",
            "HLT_e24_lhmedium_nod0_L1EM20VH",
            "HLT_e60_lhmedium_nod0",
            "HLT_e60_medium",
            "HLT_e140_lhloose_nod0",
            "HLT_e300_etcut",
            "HLT_e26_lhtight_nod0_ivarloose",
            "HLT_e26_lhtight_nod0",

            # single muon
            "HLT_mu20_iloose_L1MU15",
            "HLT_mu40",
            "HLT_mu60_0eta105_msonly",
            "HLT_mu24_iloose",
            "HLT_mu24_iloose_L1MU15",
            "HLT_mu24_ivarloose",
            "HLT_mu24_ivarloose_L1MU15",
            "HLT_mu50",
            "HLT_mu24_ivarmedium",
            "HLT_mu24_imedium",
            "HLT_mu26_ivarmedium",
            "HLT_mu26_imedium",
            ]

trigger_photon = "HLT_g140_loose"

#========================================================================================================================================
# Event Skimming
#
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTaggingBenchmarks
#========================================================================================================================================

# $equire 2 b-tags from the same algorithm
b2tag_EMTopo_MV2c10_Fixed    = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2c10) >= 2"
b2tag_EMTopo_MV2r_Fixed   = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2r)   >= 2"
b2tag_EMTopo_MV2rmu_Fixed = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2rmu) >= 2"
b2tag_EMTopo_MV2          = "(%s || %s || %s)" % (b2tag_EMTopo_MV2c10_Fixed,b2tag_EMTopo_MV2r_Fixed,b2tag_EMTopo_MV2rmu_Fixed)

b2tag_EMTopo_DL1_Fixed       = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_DL1)    >= 2"
b2tag_EMTopo_DL1r_Fixed      = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_DL1r)   >= 2"
b2tag_EMTopo_DL1rmu_Fixed    = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_DL1rmu) >= 2"
b2tag_EMTopo_DL1             = "(%s || %s || %s)" % (b2tag_EMTopo_DL1_Fixed,b2tag_EMTopo_DL1r_Fixed,b2tag_EMTopo_DL1rmu_Fixed)

b2tag77_EMTopo         = "(%s || %s)" % (b2tag_EMTopo_MV2, b2tag_EMTopo_DL1)


b2tag_EMPFlow_MV2c10_Fixed = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_MV2c10)    >= 2"
b2tag_EMPFlow_MV2r_Fixed   = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_MV2r)   >= 2"
b2tag_EMPFlow_MV2rmu_Fixed = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_MV2rmu) >= 2"
b2tag_EMPFlow_MV2          = "(%s || %s || %s)" % (b2tag_EMPFlow_MV2c10_Fixed,b2tag_EMPFlow_MV2r_Fixed,b2tag_EMPFlow_MV2rmu_Fixed)

b2tag_EMPFlow_DL1_Fixed       = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_DL1)    >= 2"
b2tag_EMPFlow_DL1r_Fixed      = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_DL1r)   >= 2"
b2tag_EMPFlow_DL1rmu_Fixed    = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_DL1rmu) >= 2"
b2tag_EMPFlow_DL1             = "(%s || %s || %s)" % (b2tag_EMPFlow_DL1_Fixed,b2tag_EMPFlow_DL1r_Fixed,b2tag_EMPFlow_DL1rmu_Fixed)

b2tag77_EMPFlow         = "(%s || %s)" % (b2tag_EMPFlow_MV2, b2tag_EMPFlow_DL1)

# jet skimming
resolved_4jetsEMTopo  = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt  > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)  < 2.8)) >= 4 && %s" % b2tag77_EMTopo
resolved_4jetsEMPFlow = "count((AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8)) >= 4 && %s" % b2tag77_EMPFlow
resolved_4jet         = "(%s || %s)" % (resolved_4jetsEMTopo, resolved_4jetsEMPFlow)

resolved_2jetsEMTopo  = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt  > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)  < 2.8)) >= 2 && %s" % b2tag77_EMTopo
resolved_2jetsEMPFlow = "count((AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8)) >= 2 && %s" % b2tag77_EMPFlow
resolved_2jet         = "(%s || %s)" % (resolved_2jetsEMTopo, resolved_2jetsEMPFlow)

singleElectron       = "count((Electrons.Tight)        && (Electrons.pt > 25*GeV) && (abs(Electrons.eta) < 2.5)) >= 1"
singleMuon           = "count((Muons.DFCommonGoodMuon) && (Muons.pt     > 25*GeV) && (abs(Muons.eta)     < 2.5)) >= 1"
singleLepton         = "(%s) || (%s)" % (singleElectron, singleMuon)

boosted_1LargeR      = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"
boosted_1LargeR_photon = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"

photon               = "count(Photons.pt > 130*GeV) >= 1"

eventSkim_zeroLepton   = "((%s) || (%s))" % (resolved_4jet, boosted_1LargeR)
eventSkim_singleLepton = "((%s) && (%s))" % (singleLepton, resolved_2jet)
eventSkim_photonSel = "((%s) && (%s))" % (photon, boosted_1LargeR_photon)

preSkim_photon = "((%s) && (%s))" % (trigger_photon, photon)

#------------------------------------------
#pre-skimming tools

#trigger
EXOT8TriggerPreSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT8TriggerPreSkimmingTool",
                                                                       TriggerListAND = [],
                                                                       TriggerListOR  = triggers)
ToolSvc += EXOT8TriggerPreSkimmingTool

#1 lepton
EXOT8PreSkimmingTool_sl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8PreSkimmingTool_sl", expression = singleLepton)
ToolSvc += EXOT8PreSkimmingTool_sl

#photon trigger + photon
EXOT8PreSkimmingTool_photon = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8PreSkimmingTool_photon", expression = preSkim_photon)
ToolSvc += EXOT8PreSkimmingTool_photon

#------------------------------------------
#skimming tools

#trigger
EXOT8TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT8TriggerSkimmingTool",
                                                                    TriggerListAND = [],
                                                                    TriggerListOR  = triggers)
ToolSvc += EXOT8TriggerSkimmingTool

#1 lepton
EXOT8SkimmingTool_sl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_sl", expression = eventSkim_singleLepton)
ToolSvc += EXOT8SkimmingTool_sl

#0 lepton
EXOT8SkimmingTool_zl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_zl", expression = eventSkim_zeroLepton)
ToolSvc += EXOT8SkimmingTool_zl

#photon trigger
EXOT8SkimmingTool_ph_trig = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_ph_trig", expression = trigger_photon)
ToolSvc += EXOT8SkimmingTool_ph_trig

#photon + large-R
EXOT8SkimmingTool_ph =  DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_ph", expression = eventSkim_photonSel)
ToolSvc += EXOT8SkimmingTool_ph

#------------------------------------------
#pre-skimming tools combinations

#trigger || 1 lepton || (photon trigger && photon)
EXOT8PreSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT8PreSkimmingTool", FilterList = [EXOT8TriggerPreSkimmingTool, EXOT8PreSkimmingTool_sl, EXOT8PreSkimmingTool_photon])
ToolSvc += EXOT8PreSkimmingTool

#------------------------------------------
#skimming tools combinations

#trigger && 0 lepton
EXOT8ANDSkimmingTool_zl = DerivationFramework__FilterCombinationAND(name = "EXOT8ANDSkimmingTool_zl", FilterList = [EXOT8TriggerSkimmingTool, EXOT8SkimmingTool_zl])
ToolSvc += EXOT8ANDSkimmingTool_zl

#photon trigger && 1 photon
EXOT8ANDSkimmingTool_photon = DerivationFramework__FilterCombinationAND(name = "EXOT8ANDSkimmingTool_photon", FilterList = [EXOT8SkimmingTool_ph_trig, EXOT8SkimmingTool_ph])
ToolSvc += EXOT8ANDSkimmingTool_photon

#(trigger && 0 lepton) || 1 lepton || (trigger photon + photon + large-R)
EXOT8SkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT8SkimmingTool", FilterList = [EXOT8ANDSkimmingTool_zl, EXOT8SkimmingTool_sl, EXOT8ANDSkimmingTool_photon])
ToolSvc += EXOT8SkimmingTool


#=======================================
# Create private sequences
# Create the derivation kernel algorithm and pass the above skimming, thinning and augmentation tools
#=======================================
#this pre-sequence skims events based on what is available from the input xAOD, thus reducing the use of CPU resources
exot8PreSeq = CfgMgr.AthSequencer("EXOT8PreSequence")
exot8PreSeq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8PreKernel_skim", SkimmingTools = [EXOT8PreSkimmingTool])
DerivationFrameworkJob += exot8PreSeq


#the main sequence tools will be passed after jets have been reconstructed
exot8Seq = CfgMgr.AthSequencer("EXOT8Sequence")
exot8PreSeq += exot8Seq

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT8"] = []
reducedJetList = [
    "AntiKt2PV0TrackJets", #flavour-tagged automatically
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot8Seq,"EXOT8")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot8Seq,"EXOT8")

# Adds a new jet collection for SoftDrop with constituent modifiers CS and SK applied
if globalflags.DataSource()=="geant4":
    addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=exot8Seq, outputGroup="EXOT8", writeUngroomed=True)

addCSSKSoftDropJets(exot8Seq, "EXOT8")

# Schedule AntiKt2 jets
from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets
addStandardJets("AntiKt",0.2,"LCTopo", mods="lctopo_ungroomed", calibOpt="none", ghostArea=0.01, ptmin=2000, ptminFilter=7000, algseq=exot8Seq, outputGroup="EXOT8")

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(exot8Seq)
# Also add Hbb Tagger
addHbbTagger(exot8Seq, ToolSvc)
addHbbTagger(exot8Seq, ToolSvc,nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")


#b-tagging

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#tag pFlow jets
FlavorTagInit(scheduleFlipped = False, JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = exot8Seq)

#====================================================================
# Apply jet calibration
#====================================================================
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot8Seq)
updateJVT_xAODColl("AntiKt4EMTopo", exot8Seq)
applyBTagging_xAODColl("AntiKt4EMTopo",exot8Seq)

applyJetCalibration_xAODColl("AntiKt4EMPFlow", exot8Seq)
updateJVT_xAODColl("AntiKt4EMPFlow", exot8Seq)
applyBTagging_xAODColl("AntiKt4EMPFlow", exot8Seq)

applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot8Seq)


#######################
# Get the jet pull
#######################
jtm.modifiersMap["jetmomcopy"] = [jtm.pull] # Add jet pull to the jtm modifiers map.
jetmomcopy_EMTopo = jtm.addJetCopier("DFJetMomentsCopy_AntiKt4EMTopoJets", "AntiKt4EMTopoJets", "jetmomcopy") # Output, Input, modifier map index. Makes jetRecTool.g
exot8Seq += JetAlgorithm("jetalgDFJetMomentsCopy_AntiKt4EMTopoJets", Tools = [jetmomcopy_EMTopo] ) # Run this tool

## Schedule the JetDecorAlg which copies jet moments back to the original collection
exot8Seq += DerivationFramework__JetDecorAlg("DecorJet_EMTopo")
exot8Seq.DecorJet_EMTopo.InputCollection="DFJetMomentsCopy_AntiKt4EMTopoJets" # Which collection to copy moments FROM
exot8Seq.DecorJet_EMTopo.MomentsToCopy=[
    "float#PullMag@PullMag",   "float#PullPhi@PullPhi",   "float#Pull_C00@Pull_C00",
    "float#Pull_C01@Pull_C01", "float#Pull_C10@Pull_C10", "float#Pull_C11@Pull_C11"]
jtm.modifiersMap["jetmomcopy"] = [jtm.pull] # Add jet pull to the jtm modifiers map.
jetmomcopy_EMPFlow = jtm.addJetCopier("DFJetMomentsCopy_AntiKt4EMPFlowJets", "AntiKt4EMPFlowJets", "jetmomcopy") # Output, Input, modifier map index. Makes jetRecTool.g
exot8Seq += JetAlgorithm("jetalgDFJetMomentsCopy_AntiKt4EMPFlowJets", Tools = [jetmomcopy_EMPFlow] ) # Run this tool

### Schedule the JetDecorAlg which copies jet moments back to the original collection
exot8Seq += DerivationFramework__JetDecorAlg("DecorJet_EMPFlow")
exot8Seq.DecorJet_EMPFlow.InputCollection="DFJetMomentsCopy_AntiKt4EMPFlowJets" # Which collection to copy moments FROM
exot8Seq.DecorJet_EMPFlow.MomentsToCopy=[
    "float#PullMag@PullMag",   "float#PullPhi@PullPhi",   "float#Pull_C00@Pull_C00",
    "float#Pull_C01@Pull_C01", "float#Pull_C10@Pull_C10", "float#Pull_C11@Pull_C11"]



exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel_skim",SkimmingTools = [EXOT8SkimmingTool])
exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel", ThinningTools = thinningTools,
                                                                        AugmentationTools = augmentationTools)




# Add the containers to the output stream - slimming done here
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT8SlimmingHelper = SlimmingHelper("EXOT8SlimmingHelper")

EXOT8SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKt2Track",
                                        "BTagging_AntiKtVR30Rmax4Rmin02TrackGhostTag",
                                        "PrimaryVertices",
                                        "Electrons",
                                        "Muons",
                                        "Photons",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4TruthJets",
                                        "AntiKt4TruthWZJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                                        "HLT_xAOD__BTaggingContainer_HLTBjetFex",
                                        "InDetTrackParticles",
                                        "AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt2LCTopoJets",
                                        ]

EXOT8SlimmingHelper.ExtraVariables = ["Electrons.charge", 
                                      "Muons.charge", 
                                      "AntiKt4EMTopoJets.DFCommonJets_TrackSumMass",
                                      "AntiKt4EMTopoJets.DFCommonJets_TrackSumPt",
                                      "AntiKt4EMTopoJets.TrackSumPt",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkPt0",
                                      "AntiKt4EMTopoJets.VecSumPtTrkPt0",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkCleanPt0",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMTopoJets.VecSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMTopoJets.PullMag",
                                      "AntiKt4EMTopoJets.PullPhi",
                                      "AntiKt4EMTopoJets.Pull_C00",
                                      "AntiKt4EMTopoJets.Pull_C01",
                                      "AntiKt4EMTopoJets.Pull_C10",
                                      "AntiKt4EMTopoJets.Pull_C11",
                                      "AntiKt4EMPFlowJets.ScalSumPtTrkPt0",
                                      "AntiKt4EMPFlowJets.VecSumPtTrkPt0",
                                      "AntiKt4EMPFlowJets.ScalSumPtTrkCleanPt0",
                                      "AntiKt4EMPFlowJets.ScalSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMPFlowJets.VecSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMPFlowJets.PullMag",
                                      "AntiKt4EMPFlowJets.PullPhi",
                                      "AntiKt4EMPFlowJets.Pull_C00",
                                      "AntiKt4EMPFlowJets.Pull_C01",
                                      "AntiKt4EMPFlowJets.Pull_C10",
                                      "AntiKt4EMPFlowJets.Pull_C11",
                                      "BTagging_AntiKt4EMTopo.JetVertexCharge_discriminant",
                                      "BTagging_AntiKt4EMTopo.SV1_normdist",
                                      "BTagging_AntiKt4EMTopo.SV1_masssvx",
                                      "BTagging_AntiKt2Track.JetVertexCharge_discriminant",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.GhostTrackCount",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.Tau1",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.Tau2",
                                      "AntiKt2LCTopoJets.Width",
                                      "AntiKt2LCTopoJets.GhostBQuarksFinal",
                                      "AntiKt2LCTopoJets.GhostTrack",
                                      "AntiKt2LCTopoJets.GhostTrackCount",
                                      "Photons."+NewTrigVars["Photons"]
                                      ]

EXOT8SlimmingHelper.AllVariables   = ["TruthEvents",
                                      "TruthElectrons",
                                      "TruthMuons",
                                      "TruthPhotons",
                                      "TruthTaus",
                                      "TruthNeutrinos",
                                      "TruthBSM",
                                      "TruthTop",
                                      "TruthBoson",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt2PV0TrackJets",
                                      "AntiKt10TruthJets",
                                      "CombinedMuonTrackParticles",
                                      "ExtrapolatedMuonTrackParticles",
                                      ]
                                      
EXOT8SlimmingHelper.StaticContent = [
                                     "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
                                     "xAOD::JetContainer#AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux.",
                                     "xAOD::JetContainer#AntiKt10LCTopoCSSKJets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoCSSKJetsAux.",
                                    ]

# Add VR track-jet collection and its b-tagging container to output stream
EXOT8SlimmingHelper.AppendToDictionary = {
    "AntiKtVR30Rmax4Rmin02TrackJets"                 :   "xAOD::JetContainer"        ,
    "AntiKtVR30Rmax4Rmin02TrackJetsAux"              :   "xAOD::JetAuxContainer"     ,
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"    :   "xAOD::JetContainer"        ,
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux" :   "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track"            :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"         :   "xAOD::BTaggingAuxContainer",
    "BTagging_AntiKt4EMPFlow"                        :   "xAOD::BTaggingContainer"   , 
    "BTagging_AntiKt4EMPFlowAux"                     :   "xAOD::BTaggingAuxContainer",
}
                               
# Add all variabless for VR track-jets
EXOT8SlimmingHelper.AllVariables  += ["AntiKtVR30Rmax4Rmin02TrackJets"]
EXOT8SlimmingHelper.SmartCollections  += ["BTagging_AntiKtVR30Rmax4Rmin02Track"]

# Save certain b-tagging variables for VR track-jet
EXOT8SlimmingHelper.ExtraVariables += [
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP",
]
EXOT8SlimmingHelper.ExtraVariables += [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HbbScore",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD"
]


if globalflags.DataSource()=="geant4":
    EXOT8SlimmingHelper.StaticContent += [
                                     "xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.",
                                     ]
    EXOT8SlimmingHelper.AppendToDictionary = {
      "AntiKt10TruthSoftDropBeta100Zcut10Jets"   :   "xAOD::JetContainer"        ,
      "AntiKt10TruthSoftDropBeta100Zcut10JetsAux":   "xAOD::JetAuxContainer"        ,
    }
    EXOT8SlimmingHelper.AllVariables  += ["AntiKt10TruthSoftDropBeta100Zcut10Jets"]

EXOT8SlimmingHelper.IncludeJetTriggerContent = True
EXOT8SlimmingHelper.IncludeBJetTriggerContent = True
EXOT8SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT8SlimmingHelper.IncludeMuonTriggerContent = True
EXOT8SlimmingHelper.AppendContentToStream(EXOT8Stream)
