#********************************************************************
# HIGG5D3.py (for 0-lepton)
# reductionConf flag HIGG5D3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

# running on data or MC
from AthenaCommon.GlobalFlags import globalflags

if DerivationFrameworkIsMonteCarlo :
  from DerivationFrameworkHiggs.TruthCategories import *


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D3Stream )
HIGG5D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D3Stream.AcceptAlgs(["HIGG5D3Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
HIGG5D3ThinningHelper = ThinningHelper("HIGG5D3ThinningHelper") 
#trigger navigation content
HIGG5D3ThinningHelper.TriggerChains = 'HLT_g.*|HLT_2g.*|HLT_mu.*|HLT_j.*|HLT_b.*|HLT_2b.*' 
HIGG5D3ThinningHelper.AppendToStream(HIGG5D3Stream) 

import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'HIGG5D3',HIGG5D3ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'HIGG5D3',HIGG5D3ThinningHelper) )
thinningTools.append( HIGG5Common.getPhotonTrackParticleThinning(       'HIGG5D3',HIGG5D3ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('HIGG5D3',HIGG5D3ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('HIGG5D3',HIGG5D3ThinningHelper) )

# Truth particles
if DerivationFrameworkIsMonteCarlo:
    thinningTools.append(HIGG5Common.getTruthThinningTool('HIGG5D3', HIGG5D3ThinningHelper))


#====================================================================
# Skimming Tool
#====================================================================
Run2DataTriggers=['HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23',
                  'HLT_2j35_bmv2c2070_split_2j35_L13J25.0ETA23',
                  'HLT_2j35_btight_2j35_L13J25.0ETA23',
                  'HLT_2j35_btight_2j35_L14J15.0ETA25',
                  'HLT_2j35_btight_split_2j35_L13J25.0ETA23',
                  'HLT_2j35_btight_split_2j35_L14J15.0ETA25',
                  'HLT_2j45_bmedium_2j45_L13J25.0ETA23',
                  'HLT_2j45_bmedium_2j45_L14J15.0ETA25',
                  'HLT_2j45_bmedium_split_2j45_L13J25.0ETA23',
                  'HLT_2j45_bmedium_split_2j45_L14J15.0ETA25',
                  'HLT_2j45_bmv2c2070_split_2j45_L14J15', # added on Nov 2016
                  'HLT_2j45_bmv2c2070_split_2j45', # added on Nov 2016
                  'HLT_2j45_bmv2c2077_2j45_L13J25.0ETA23',
                  'HLT_2j45_bmv2c2077_split_2j45_L13J25.0ETA23',
                  'HLT_2j45_btight_2j45',
                  'HLT_2j45_btight_2j45_L13J25.ETA23',
                  'HLT_2j45_btight_split_2j45',
                  'HLT_2j45_btight_split_2j45_L13J25.ETA23',
                  'HLT_2j55_bloose_L14J20.0ETA49',
                  'HLT_2j55_bmedium_2j55',
                  'HLT_2j55_bmedium_2j55_L13J25.ETA23',
                  'HLT_2j55_bmedium_L14J20.0ETA49',
                  'HLT_2j55_bmedium_split_2j55',
                  'HLT_2j55_bmedium_split_2j55_L13J25.ETA23',
                  'HLT_2j55_bperf_L14J20.0ETA49',
                  'HLT_2j65_btight_j65',
                  'HLT_2j65_btight_split_j65',
                  'HLT_2j70_bmedium_j70',
                  'HLT_2j70_bmedium_split_j70',
                  'HLT_2j70_btight_j70',
                  'HLT_2j70_btight_split_j70',
                  'HLT_2j75_bmedium_j75',
                  'HLT_2j75_bmedium_split_j75',
                  'HLT_j0_perf_bperf_L1MU10',

                  'HLT_mu4_4j40_dr05_L1MU4_3J15',
                  'HLT_mu4_4j40_dr05_L1MU4_3J20',
                  'HLT_mu4_j40_dr05_3j40_L14J20',
                  'HLT_mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu26_imedium_2j35_boffperf_split', # added on May 2016
                  'HLT_mu26_imedium_2j35_bperf',

                  'HLT_e26_tight_iloose_2j35_bperf',
                  'HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490',
                  'HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700',
                  'HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700',
                  'HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700',
                  'HLT_g20_loose',
                  'HLT_g25_loose', # updated with TriggerMenu-00-12-40 (July 2015)
                  'HLT_g25_medium_L1EM22VHI', # added on Aug 2016
                  'HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm1000', # added on Aug 2016
                  'HLT_g20_tight_icaloloose_j35_bmv2c1077_split_3j35_0eta490_invm500',
                  'HLT_g20_tight_icaloloose_j15_gsc35_bmv2c1077_split_3j35_0eta490_invm500',
                  # 'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j25_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700', # modified on Jan 2016
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490',
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_split_2j35_0eta490',
                  'HLT_g25_loose_L1EM20VH_4j35_0eta490', # added on Oct 2015
                  'HLT_g10_loose',
                  'HLT_g15_loose_L1EM7',
                  'HLT_g20_loose_L1EM12',
                  'HLT_g25_loose_L1EM15',
                  'HLT_g35_loose_L1EM15',
                  'HLT_g40_loose_L1EM15',
                  'HLT_g45_loose_L1EM15',
                  'HLT_g50_loose_L1EM15',
                  'HLT_g60_loose',
                  'HLT_g70_loose',
                  'HLT_g80_loose',
                  'HLT_g100_loose',
                  'HLT_g120_loose', # added on Jan 2016
                  'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490', # added on Apr 2016
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_split_2j35_0eta490',
                  'HLT_2g20_tight',
                  'HLT_2g22_tight',
                  'HLT_g35_loose_g25_loose',
                  'HLT_g35_medium_g25_medium', # added on Aug 2016

                  'HLT_j150_bmv2c2060_split_j50_bmv2c2060_split',
                  'HLT_j100_2j55_bmv2c2060_split',
                  'HLT_j55_bmv2c2060_ht500_L14J15',
                  'HLT_j55_bmv2c2060_split_ht500_L14J15',
                  'HLT_j175_gsc225_bmv2c1040_split',#Single b-jet
                  'HLT_j225_gsc275_bmv2c1060_split',
                  'HLT_j225_gsc275_bmv2c1070_split',
                  'HLT_j225_gsc300_bmv2c1070_split',
                  'HLT_j225_gsc300_bmv2c1077_split',
                  'HLT_j225_gsc360_bmv2c1077_split',
                  'HLT_j225_gsc360_bmv2c1085_split',
                  'HLT_j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split',
                  'HLT_j175_bmv2c2040_split',
                  'HLT_j225_bmv2c2060_split',
                  'HLT_j275_bmv2c2070_split',
                  'HLT_j300_bmv2c2077_split',
                  'HLT_j360_bmv2c2085_split',
                  'HLT_j225_bloose',
                  'HLT_j175_bmedium',
                  'HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490',#b+j,2017-2018
                  'HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49',
                  'HLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49',
                  'HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split',
                  'HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split',
                  'HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF',
                  'HLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split',
                  'HLT_g35_medium_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF',
                  'HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25',
                  'HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25',
                  'HLT_2j15_gsc35_bmv2c1050_split_2j15_gsc35_boffperf_split_L14J15.0ETA25',
                  'HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25',
                  'HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25', 
                  'HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25',
                  'HLT_2j35_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21',
                  'HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
                  'HLT_2j35_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21',
                  'HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
                  'HLT_2j35_gsc55_bmv2c2060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
                  'HLT_2j35_gsc55_bmv2c2070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
                  'HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25',
                  'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30',
                  'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1077_split_L1J85_3J30',
                  'HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30',
                  'HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split',
                  'HLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split',

                  'HLT_j100_2j55_bmedium',
                  'HLT_j150_bmedium_j50_bmedium', # added on May 2016
                  'HLT_j80_bmv2c2085_split_2j60_320eta490',
                  'HLT_j80_0eta240_2j60_320eta490',
                  'HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490',
                  'HLT_j80_0eta240_j60_j45_320eta490', # added on May 2016
                  'HLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split', # added on Nov 2016
                  'HLT_2j35_bmv2c2060_split_2j35_L14J15'] # added on Dec 2016



Run2MCTriggers=["L1_3J20_4J20.0ETA49_MJJ-400",
                "L1_3J25.ETA23",
                "L1_4J20",
                "L1_3J15_BTAG-MU4J15",
                "L1_MJJ-700",
                "L1_MJJ-400",
                "L1_EM15VH",
                "L1_EM13VH", # based on TriggerMenu-00-09-41-12 for MC15 production
                "L1_2EM15VH",
                "L1_J100",
                "L1_J75_3J20",
                "L1_4J15.0ETA25",
                "L1_J40.0ETA25_2J25_J20.31ETA49",
                "L1_J25.0ETA23_2J15.31ETA49",
] # added on May 2016


from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG5VBF
if DerivationFrameworkIsMonteCarlo :
    HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                    JetContainerKey         = "AntiKt4EMTopoJets",
                                                                    # jet multiplicity requirement 2b + 2j
                                                                    ReqNAllJets             = False,
                                                                    NumberOfAllJets         = 4,
                                                                    AllJetPtCut             = 40.*Units.GeV,
                                                                    AllJetEtaCut            = 4.9,
                                                                    # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                    ReqNCentralJets         = True,
                                                                    NumberOfCentralJets     = 2,
                                                                    CentralJetPtCut         = 15.*Units.GeV,  # to be adjusted according to b-jet trigger pT threshold
                                                                    CentralJetEtaCut        = 2.6,
                                                                    # trigger requirement
                                                                    ReqTrigger              = True,
                                                                    Triggers                = Run2MCTriggers,
                                                                    # Mjj requirement
                                                                    ReqVBFMjj               = False,
                                                                    MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                    DoDebug                 = False,
                                                                    #photon requirement (p. rose)
                                                                    PhotonContainerKey      = "Photons",
                                                                    ReqPhoton               = False,
                                                                    PhotonPtCut             = 20.*Units.GeV,
                                                                    CentralPhotonEtaCut     = 2.6)
else:
    HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                    JetContainerKey         = "AntiKt4EMTopoJets",
                                                                    # jet multiplicity requirement 2b + 2j
                                                                    ReqNAllJets             = False,
                                                                    NumberOfAllJets         = 4,
                                                                    AllJetPtCut             = 40.*Units.GeV,
                                                                    AllJetEtaCut            = 4.9,
                                                                    # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                    ReqNCentralJets         = True,
                                                                    NumberOfCentralJets     = 2,
                                                                    CentralJetPtCut         = 15.*Units.GeV,  # to be adjusted according to b-jet trigger pT threshold
                                                                    CentralJetEtaCut        = 2.6,
                                                                    # trigger requirement
                                                                    ReqTrigger              = True,
                                                                    Triggers                = Run2DataTriggers,
                                                                    # Mjj requirement
                                                                    ReqVBFMjj               = False,
                                                                    MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                    DoDebug                 = False,
                                                                    #photon requirement (p. rose)
                                                                    PhotonContainerKey      = "Photons",
                                                                    ReqPhoton               = False,
                                                                    PhotonPtCut             = 20.*Units.GeV,
                                                                    CentralPhotonEtaCut     = 2.6)        

ToolSvc += HIGG5D3SkimmingTool

#print HIGG5D3SkimmingTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg5d3Seq = CfgMgr.AthSequencer("HIGG5D3Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg5d3Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D3Kernel_obj",
    SkimmingTools = [HIGG5D3SkimmingTool]
    )

#====================================================================
# Standard jets
#====================================================================
if not "HIGG5D3Jets" in OutputJets:
    OutputJets["HIGG5D3Jets"] = []

    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets']
    replaceAODReducedJets(reducedJetList, higg5d3Seq, "HIGG5D3Jets")
    from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
    # Set up geometry and BField
    import AthenaCommon.AtlasUnixStandardJob

    include("RecExCond/AllDet_detDescr.py")
    runTCCReconstruction(higg5d3Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles")
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
    addTCCTrimmedJets(higg5d3Seq, "HIGG5D3Jets")

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg5d3Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()
addDefaultTrimmedJets(higg5d3Seq,"HIGG5D3Jets");
# Also add Hbb Tagger
addVRJets(higg5d3Seq)
addHbbTagger(higg5d3Seq, ToolSvc)
# QGTaggerTool ###
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=higg5d3Seq, algname="QGTaggerToolAlg")

FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg5d3Seq)

higg5d3Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D3Kernel",
    ThinningTools = thinningTools
    )


#===================================================================
# Run b-tagging
#===================================================================

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d3Seq)

DerivationFrameworkJob += higg5d3Seq

#====================================================================
# Add the containers to the output stream - slimming done here
#===================o=================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGG5D3ExtraContent import *
HIGG5D3SlimmingHelper = SlimmingHelper("HIGG5D3SlimmingHelper")
HIGG5D3SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"                :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"             :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"           :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"        :   "xAOD::BTaggingAuxContainer",
  }

HIGG5D3SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Photons",
                                           "Muons",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4EMPFlowJets",
                                           "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                           "AntiKt4TruthJets",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt4EMPFlow",
                                           "BTagging_AntiKt2Track",
                                           "InDetTrackParticles",
                                           "PrimaryVertices" ]
if DerivationFrameworkIsMonteCarlo :
    HIGG5D3SlimmingHelper.SmartCollections += [
         "AntiKt4TruthJets"
#          ,"AntiKt4TruthWZJets"
#          ,"AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets"
    ]



HIGG5D3SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D3SlimmingHelper.AllVariables = ExtraContainers
if DerivationFrameworkIsMonteCarlo :
    HIGG5D3SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D3SlimmingHelper.AllVariables += ExtraContainersTruth
HIGG5D3SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()



# Add the jet containers to the stream
slimmed_content=["HIGG5D3Jets",
                "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
                ]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]
# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(HIGG5D3SlimmingHelper,["HIGG5D3Jets"],slimmed_content,['AntiKt4PV0TrackJets'])

HIGG5D3SlimmingHelper.IncludeEGammaTriggerContent = True
HIGG5D3SlimmingHelper.IncludeJetTriggerContent = True
HIGG5D3SlimmingHelper.IncludeBJetTriggerContent = True

HIGG5D3SlimmingHelper.AppendContentToStream(HIGG5D3Stream)
