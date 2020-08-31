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
from JetRec.JetRecFlags                                import jetFlags
from JetRec.JetRecConf                                 import JetAlgorithm
from DerivationFrameworkJetEtMiss.DFJetMetFlags        import *
from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__JetDecorAlg
from DerivationFrameworkInDet.InDetCommon              import *
from TriggerMenu.api.TriggerAPI                        import TriggerAPI
from TriggerMenu.api.TriggerEnums                      import TriggerPeriod, TriggerType

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
from DerivationFrameworkCore.DerivationFrameworkCoreConf   import DerivationFramework__DerivationKernel

#========================================================================================================================================
# Set up Stream
#========================================================================================================================================
streamName  = derivationFlags.WriteDAOD_EXOT8Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_EXOT8Stream )
EXOT8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT8Stream.AcceptAlgs(["EXOT8Kernel"])

#========================================================================================================================================
# Triggers (https://indico.cern.ch/event/403233/contribution/4/material/slides/0.pdf)
#
# Not yet known: 25ns menu, L1_HT menu
#========================================================================================================================================

allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34

electronTriggers = TriggerAPI.getUnprescaledAnyPeriod(allperiods, TriggerType.el_single, livefraction=0.95)
electronTriggers.append("HLT_e24_lhmedium_nod0_L1EM20VH")
muonTriggers = TriggerAPI.getUnprescaledAnyPeriod(allperiods, TriggerType.mu_single, livefraction=0.95)
jetTriggers = TriggerAPI.getUnprescaledAnyPeriod(allperiods, TriggerType.j, livefraction=0.95)
jetTriggers += TriggerAPI.getUnprescaledAnyPeriod(allperiods, TriggerType.bj, livefraction=0.95)
jetTriggers += TriggerAPI.getUnprescaledAnyPeriod(allperiods, TriggerType.ht, livefraction=0.95)
jetTriggers += TriggerAPI.getUnprescaledAnyPeriod(allperiods, TriggerType.bj, TriggerType.j, livefraction=0.95)
jetTriggers.append("HLT_j60_gsc100_bmv2c1050_split_xe80_mht_L1XE60")
jetTriggers.append("HLT_2j25_gsc45_bmv2c1070_split_xe80_mht_L12J15_XE55")
jetTriggers.append("HLT_3j15_gsc35_bmv2c1077_split_xe60_mht_L13J15.0ETA25_XE40")
jetTriggers.append("HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21")
jetTriggers.append("HLT_2j35_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21")
jetTriggers.append("HLT_2j35_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21")
jetTriggers.append("HLT_j55_bmv2c2060_split_ht500_L14J15")
jetTriggers.append("HLT_2j55_bmv2c2060_split_ht300_L14J15")
jetTriggers.append("HLT_j80_bmv2c2060_split_xe60_L12J50_XE40")
jetTriggers.append("HLT_j110")
jetTriggers.append("HLT_j260_a10_lcw_L1J75")
jetTriggers.append("HLT_j260_a10t_lcw_jes_L1J75")
jetTriggers.append("HLT_j260_a10t_lcw_jes_L1J75")

triggers = jetTriggers + electronTriggers + muonTriggers

photonTrigger = "HLT_g140_loose"

# We have to turn off the trigger navigation thinning for 2018 data.
# First, figure out whether we're in 2018 data. Thanks to Eirik for this code
isData18 = False
from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary is not None:
    if (inputFileSummary['tag_info']['project_name']=='data18_13TeV'): 
        isData18 = True

#
#  Trigger Nav thinning
#
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT8ThinningHelper = ThinningHelper( "EXOT8ThinningHelper" )
if (globalflags.DataSource() is not "geant4") and (not isData18):
    EXOT8ThinningHelper.TriggerChains = ''

    EXOT8ThinningHelper.TriggerChains += "|".join(electronTriggers)
    EXOT8ThinningHelper.TriggerChains += "|".join(muonTriggers)
    EXOT8ThinningHelper.TriggerChains += "|".join(photonTrigger)
    
    for trigger in jetTriggers:
      if ('bmv2c' or 'bhmv2c' or 'bloose' or 'bmedium' or 'btight') in trigger:
        EXOT8ThinningHelper.TriggerChains += trigger + "|"
    
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
                                                                               SelectionString        = "Electrons.pt > 5*GeV",
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
EXOT8ak4ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak4ThinningTool",
                                                                  ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                  ContainerName   = "AntiKt4EMPFlowJets",
                                                                  ApplyAnd        = False,
                                                                  SelectionString = "AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 20*GeV")
ToolSvc += EXOT8ak4ThinningTool
thinningTools.append(EXOT8ak4ThinningTool)

EXOT8ak4btag201810ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak4btag201810ThinningTool",
                                                                            ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                            ContainerName   = "AntiKt4EMPFlowJets_BTagging201810",
                                                                            ApplyAnd        = False,
                                                                            SelectionString = "AntiKt4EMPFlowJets_BTagging201810.DFCommonJets_Calib_pt > 20*GeV")
ToolSvc += EXOT8ak4btag201810ThinningTool
thinningTools.append(EXOT8ak4btag201810ThinningTool)

EXOT8ak4btag201903ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak4btag201903ThinningTool",
                                                                            ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                            ContainerName   = "AntiKt4EMPFlowJets_BTagging201903",
                                                                            ApplyAnd        = False,
                                                                            SelectionString = "AntiKt4EMPFlowJets_BTagging201903.DFCommonJets_Calib_pt > 20*GeV")
ToolSvc += EXOT8ak4btag201903ThinningTool
thinningTools.append(EXOT8ak4btag201903ThinningTool)

# Thin ak10 jets
EXOT8ak10ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak10ThinningTool",
                                                                   ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                   ContainerName   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                   ApplyAnd        = False,
                                                                   SelectionString = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV")
ToolSvc += EXOT8ak10ThinningTool
thinningTools.append(EXOT8ak10ThinningTool)


# Thin ak10 untrimmed jets
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

# Thin electrons
EXOT8electronThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8electronThinningTool",
                                                                       ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                       ContainerName   = "Electrons",
                                                                       ApplyAnd        = False,
                                                                       SelectionString = "Electrons.pt > 5*GeV")
ToolSvc += EXOT8electronThinningTool
thinningTools.append(EXOT8electronThinningTool)

# Thin muons
EXOT8muonThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8muonThinningTool",
                                                                   ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                   ContainerName   = "Muons",
                                                                   ApplyAnd        = False,
                                                                   SelectionString = "Muons.pt > 5*GeV")
ToolSvc += EXOT8muonThinningTool
thinningTools.append(EXOT8muonThinningTool)

# Thin photons
EXOT8photonThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8photonThinningTool",
                                                                     ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "Photons",
                                                                     ApplyAnd        = False,
                                                                     SelectionString = "Photons.pt > 50*GeV")
ToolSvc += EXOT8photonThinningTool
thinningTools.append(EXOT8photonThinningTool)

#########################################
# truth containers a la TRUTH3
#########################################
from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
from DerivationFrameworkMCTruth.MCTruthCommon import addTopQuarkAndDownstreamParticles
from DerivationFrameworkMCTruth.MCTruthCommon import addHFAndDownstreamParticles
from DerivationFrameworkMCTruth.MCTruthCommon import addTruthCollectionNavigationDecorations

if globalflags.DataSource()=="geant4":
    addStandardTruthContents()
    addTopQuarkAndDownstreamParticles()
    addHFAndDownstreamParticles(addB=True, addC=False, generations=1)
    addTruthCollectionNavigationDecorations(TruthCollections=["TruthTopQuarkWithDecayParticles","TruthBosonsWithDecayParticles"],prefix='Top')

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__BJetRegressionVariables
EXOT8BJetRegressionVariables_EMPFlow = DerivationFramework__BJetRegressionVariables(name = "EXOT8BJetRegressionVariables_EMPFlow",
                                                                                    ContainerName = "AntiKt4EMPFlowJets",
                                                                                    AssociatedTracks = "GhostTrack",
                                                                                    MinTrackPtCuts = [0])

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
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="EXOT8",
                                                 PhotonTriggers=triggers_for_matching)

augmentationTools.append(TrigMatchAug)

#========================================================================================================================================
# Event Skimming
#
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTaggingBenchmarks
#========================================================================================================================================

# Require 2 b-tags from the same algorithm

#Write the taggers out by hand because we don't have a CDI
bfix77_DL1r_2019   ='(count(log(BTagging_AntiKt4EMPFlow_201903.DL1r_pb/(0.018*BTagging_AntiKt4EMPFlow_201903.DL1r_pc+(1-0.018)*BTagging_AntiKt4EMPFlow_201903.DL1r_pu))>2.195))'
bfix77_DL1rmu_2019 ='(count(log(BTagging_AntiKt4EMPFlow_201903.DL1rmu_pb/(0.03*BTagging_AntiKt4EMPFlow_201903.DL1rmu_pc+(1-0.03)*BTagging_AntiKt4EMPFlow_201903.DL1rmu_pu))>2.195))'
bfix77_DL1_2019    ='(count(log(BTagging_AntiKt4EMPFlow_201903.DL1_pb/(0.018*BTagging_AntiKt4EMPFlow_201903.DL1_pc+(1-0.018)*BTagging_AntiKt4EMPFlow_201903.DL1_pu))>2.015))'
bfix77_MV2_2019    ='(count(BTagging_AntiKt4EMPFlow_201903.MV2c10_discriminant>0.691))'
bfix77_DL1r_2018   ='(count(log(BTagging_AntiKt4EMPFlow_201810.DL1r_pb/(0.08*BTagging_AntiKt4EMPFlow_201810.DL1r_pc+(1-0.08)*BTagging_AntiKt4EMPFlow_201810.DL1r_pu))>0.7550000000000002))'
bfix77_DL1rmu_2018 ='(count(log(BTagging_AntiKt4EMPFlow_201810.DL1rmu_pb/(0.03*BTagging_AntiKt4EMPFlow_201810.DL1rmu_pc+(1-0.03)*BTagging_AntiKt4EMPFlow_201810.DL1rmu_pu))>2.1650000000000005))'
bfix77_DL1_2018    ='(count(log(BTagging_AntiKt4EMPFlow_201810.DL1_pb/(0.08*BTagging_AntiKt4EMPFlow_201810.DL1_pc+(1-0.08)*BTagging_AntiKt4EMPFlow_201810.DL1_pu))>1.4150000000000003))'
bfix77_MV2_2018    ='(count(BTagging_AntiKt4EMPFlow_201810.MV2c10_discriminant>0.63))'

#accept if any of the above tags has more than 2
b2tag77_EMPFlow = "(%s >= 2 || %s >= 2 || %s >= 2 || %s >= 2 || %s >= 2 || %s >= 2 || %s >=2 || %s >= 2)" % (bfix77_DL1r_2019, bfix77_DL1rmu_2019, bfix77_DL1_2019, bfix77_MV2_2019, bfix77_DL1r_2018, bfix77_DL1rmu_2018, bfix77_DL1_2018, bfix77_MV2_2018)

# jet skimming
resolved_4jet = "count((AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8)) >= 4 && %s" % b2tag77_EMPFlow
resolved_2jet = "count((AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8)) >= 2 && %s" % b2tag77_EMPFlow

singleElectron       = "count((Electrons.Tight)        && (Electrons.pt > 25*GeV) && (abs(Electrons.eta) < 2.5)) >= 1"
singleMuon           = "count((Muons.DFCommonGoodMuon) && (Muons.pt     > 25*GeV) && (abs(Muons.eta)     < 2.5)) >= 1"
singleLepton         = "(%s) || (%s)" % (singleElectron, singleMuon)

boosted_1LargeR      = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"
boosted_1LargeR_photon = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"

photon               = "count(Photons.pt > 130*GeV) >= 1"

eventSkim_zeroLepton   = "((%s) || (%s))" % (resolved_4jet, boosted_1LargeR)
eventSkim_singleLepton = "((%s) && (%s))" % (singleLepton, resolved_2jet)
eventSkim_photonSel = "((%s) && (%s))" % (photon, boosted_1LargeR_photon)

preSkim_photon = "((%s) && (%s))" % (photonTrigger, photon)

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
EXOT8SkimmingTool_ph_trig = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_ph_trig", expression = photonTrigger)
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
    "AntiKt2TruthJets",
    "AntiKt2LCTopoJets",
    "AntiKt2PV0TrackJets",
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets"]

replaceAODReducedJets(reducedJetList,exot8Seq,"EXOT8")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets,addDefaultUFOSoftDropJets
addDefaultTrimmedJets(exot8Seq,"EXOT8")
addDefaultUFOSoftDropJets(exot8Seq,"EXOT8",dotruth=True)

# Create variable-R trackjets and dress ungroomed large-R jets with ghost VR-trkjet

largeRJetAlgs = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10",
    ]

largeRJetCollections = []
for alg in largeRJetAlgs:
  largeRJetCollections.append(alg+"Jets")

# Add truth labeling to groomed large-R jet collections
if DerivationFrameworkIsMonteCarlo:
  for alg in largeRJetAlgs:
    addJetTruthLabel(jetalg=alg,sequence=exot8Seq,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

# Associate VR track jets
addVRJets(exot8Seq, largeRColls = largeRJetCollections)
addVRJets(exot8Seq, largeRColls = largeRJetCollections, do_ghost=True)
addVRJets(exot8Seq, largeRColls = largeRJetCollections, training='201903') #new trackjet training!

# Also add Hbb Tagger
addHbbTagger(exot8Seq, ToolSvc,nn_config_file="BoostedJetTaggers/HbbTaggerDNN/PreliminaryConfigNovember2017.json")
addHbbTagger(exot8Seq, ToolSvc,nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")

#b-tagging

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903->AntiKt4EMTopo"]
#BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810->AntiKt4EMTopo"]

#tag pFlow jets
FlavorTagInit(scheduleFlipped = False, JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = exot8Seq)

#====================================================================
# Apply jet calibration
#====================================================================

applyJetCalibration_xAODColl("AntiKt4EMPFlow", exot8Seq)
updateJVT_xAODColl("AntiKt4EMPFlow", exot8Seq)
applyBTagging_xAODColl("AntiKt4EMPFlow_BTagging201810", exot8Seq)
applyBTagging_xAODColl("AntiKt4EMPFlow_BTagging201903", exot8Seq)

applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot8Seq)

#====================================================================
# Apply fJVT
#====================================================================

# PFlow fJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getPFlowfJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow', sequence=exot8Seq, algname='JetForwardPFlowJvtToolAlg')

#######################
# Get the jet pull
#######################

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

EXOT8SlimmingHelper.SmartCollections = ["PrimaryVertices",
                                        "Electrons",
                                        "Muons",
                                        "Photons",
                                        "InDetTrackParticles",
                                        "AntiKt2TruthJets",
                                        "AntiKt2LCTopoJets",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag",
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4TruthJets",
                                        "AntiKt4TruthWZJets",
                                        "AntiKt10TruthJets",
                                        "AntiKt10LCTopoJets",
                                        "AntiKt10UFOCSSKJets",
                                        "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TruthSoftDropBeta100Zcut10Jets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                                        "HLT_xAOD__BTaggingContainer_HLTBjetFex",
                                        ]

EXOT8SlimmingHelper.ExtraVariables = ["Electrons.charge",
                                      "Muons.charge",
                                      "InDetTrackParticles.truthMatchProbability",
                                      "AntiKt4EMPFlowJets.DFCommonJets_TrackSumMass",
                                      "AntiKt4EMPFlowJets.DFCommonJets_TrackSumPt",
                                      "AntiKt4EMPFlowJets.TrackSumPt",
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
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.GhostTrackCount",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.Tau1",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.Tau2",
                                      "AntiKt2LCTopoJets.Width",
                                      "AntiKt2LCTopoJets.GhostBQuarksFinal",
                                      "AntiKt2LCTopoJets.GhostTrack",
                                      "AntiKt2LCTopoJets.GhostTrackCount",
                                      "BTagging_AntiKt4EMPFlow_201810.JetVertexCharge_discriminant",
                                      "BTagging_AntiKt4EMPFlow_201903.JetVertexCharge_discriminant",
                                      "Photons."+NewTrigVars["Photons"]
                                      ]

EXOT8SlimmingHelper.AllVariables   = ["TruthEvents",
                                      "CombinedMuonTrackParticles",
                                      "ExtrapolatedMuonTrackParticles",
                                      "HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex",
                                      ]

EXOT8SlimmingHelper.StaticContent = []

# Add VR track-jet collection and its b-tagging container to output stream
EXOT8SlimmingHelper.AppendToDictionary = {
    "BTagging_AntiKt4EMPFlow_201810"                 :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKt4EMPFlow_201810Aux"              :   "xAOD::BTaggingAuxContainer",
    "BTagging_AntiKt4EMPFlow_201903"                 :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKt4EMPFlow_201903Aux"              :   "xAOD::BTaggingAuxContainer",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"                 :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux"              :   "xAOD::BTaggingAuxContainer",
}
                               
# Save certain b-tagging variables for VR track-jet
EXOT8SlimmingHelper.ExtraVariables += [
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.MV2c100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HbbScore",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD",
    "AntiKt10LCTopoJets.NumTrkPt500.NumTrkPt1000.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt500.TrackWidthPt1000",
]

addJetOutputs(EXOT8SlimmingHelper,["AntiKt4EMPFlowJets",
                                   "EXOT8"],
                                   EXOT8SlimmingHelper.SmartCollections,
                                   ["AntiKt2PV0TrackJets",
                                    "AntiKt4PV0TrackJets"])


if globalflags.DataSource()=="geant4":
    for truthc in [
      "TruthElectrons",
      "TruthMuons",
      "TruthPhotons",
      "TruthTaus",
      "TruthNeutrinos",
      "TruthBottom",
      "TruthBSM",
      "HardScatterParticles",
      ]:
      EXOT8SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
      EXOT8SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")
    
    for truthc in [
      "TruthBosons",
      "TruthTopQuark",
      "TruthHF",
      ]:
      EXOT8SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles")
      EXOT8SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.")
      EXOT8SlimmingHelper.StaticContent.append("xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices")
      EXOT8SlimmingHelper.StaticContent.append("xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux.")

EXOT8SlimmingHelper.IncludeJetTriggerContent = True
EXOT8SlimmingHelper.IncludeBJetTriggerContent = True
EXOT8SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT8SlimmingHelper.IncludeMuonTriggerContent = True
EXOT8SlimmingHelper.AppendContentToStream(EXOT8Stream)
