#********************************************************************
# HIGG5D3.py (for 0-lepton)
# reductionConf flag HIGG5D3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
# from DerivationFrameworkJetEtMiss.JetMomentFix import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

if DerivationFrameworkIsMonteCarlo: 
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

# running on data or MC
from AthenaCommon.GlobalFlags import globalflags
# print "DEBUG is MC ? ",DerivationFrameworkIsMonteCarlo

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


# # MC truth thinning (not for data)
# truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
# truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
# truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top quark and Bottom quark
# truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
# truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'
# from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
# HIGG5D3MCThinningTool = DerivationFramework__GenericTruthThinning(
#     name                    = "HIGG5D3MCThinningTool", 
#     ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
#     ParticleSelectionString = truth_expression,
#     PreserveDescendants     = False,
#     PreserveGeneratorDescendants = True,
#     PreserveAncestors       = True)
# if DerivationFrameworkIsMonteCarlo :
#     ToolSvc += HIGG5D3MCThinningTool
#     thinningTools.append(HIGG5D3MCThinningTool)


# MET/Jet tracks
thinning_expression = "( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG5D3TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "HIGG5D3TPThinningTool",
                                                                  ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = thinning_expression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D3TPThinningTool
thinningTools.append(HIGG5D3TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG5D3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG5D3JetTPThinningTool",
                                                                ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                #SelectionString         = "AntiKt4EMTopoJets.pt > 20*GeV",
                                                                ApplyAnd                = True)
ToolSvc += HIGG5D3JetTPThinningTool
thinningTools.append(HIGG5D3JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG5D3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG5D3MuonTPThinningTool",
                                                                            ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D3MuonTPThinningTool
thinningTools.append(HIGG5D3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D3ElectronTPThinningTool",
                                                                                        ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        BestMatchOnly           = True)
ToolSvc += HIGG5D3ElectronTPThinningTool
thinningTools.append(HIGG5D3ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D3PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG5D3PhotonTPThinningTool
thinningTools.append(HIGG5D3PhotonTPThinningTool)

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
                  # 'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j25_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700', # modified on Jan 2016
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490',
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
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490', # added on Apr 2016
                  'HLT_2g20_tight',
                  'HLT_2g22_tight',
                  'HLT_g35_loose_g25_loose',
                  'HLT_g35_medium_g25_medium', # added on Aug 2016
                  'HLT_j225_bmv2c2060_split',
                  'HLT_j150_bmv2c2060_split_j50_bmv2c2060_split',
                  'HLT_j100_2j55_bmv2c2060_split',
                  'HLT_j55_bmv2c2060_ht500_L14J15',
                  'HLT_j55_bmv2c2060_split_ht500_L14J15',
                  'HLT_j225_bloose',
                  'HLT_j175_bmedium',
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
                "L1_J75_3J20"] # added on May 2016


# Run2MCTriggers=["HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400",
#                 "HLT_2j55_bmedium_2j55_L13J25.ETA23",
#                 "HLT_2j45_btight_2j45_L13J25.ETA23",
#                 "HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400",
#                 "HLT_mu4_3j35_dr05_j35_bloose_L13J15_BTAG-MU4J15",
#                 "HLT_mu4_3j35_dr05_j35_bloose_L14J20",
#                 "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700",
#                 "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700",
#                 "HLT_g20_loose_2j40_0eta490_3j25_0eta490"]

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

    #AntiKt2PV0TrackJets
    addStandardJets("AntiKt", 0.2, "PV0Track", 2000, mods="track_ungroomed", algseq=higg5d3Seq, outputGroup="HIGG5D3Jets")
    OutputJets["HIGG5D3Jets"].append("AntiKt2PV0TrackJets")
    #AntiKt4PV0TrackJets
    addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=higg5d3Seq, outputGroup="HIGG5D3Jets")
    OutputJets["HIGG5D3Jets"].append("AntiKt4PV0TrackJets")

#====================================================================
# Special jets
#====================================================================
# if not "HIGG5D3Jets" in OutputJets:
    if jetFlags.useTruth:
      #AntiKt4TruthJets
      addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=higg5d3Seq, outputGroup="HIGG5D3Jets")
      OutputJets["HIGG5D3Jets"].append("AntiKt4TruthJets")

higg5d3Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D3Kernel",
    ThinningTools = thinningTools
    )


#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit( JetCollections = ["AntiKt4PV0TrackJets", "AntiKt2PV0TrackJets"], Sequencer = higg5d3Seq )

# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d3Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg5d3Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


DerivationFrameworkJob += higg5d3Seq


# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName=HIGG5D3ThinningHelper.ThinningSvc(), outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#===================o=================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D3ContentList import HIGG5D3Content
from DerivationFrameworkHiggs.HIGG5D3ExtraContent import *
HIGG5D3SlimmingHelper = SlimmingHelper("HIGG5D3SlimmingHelper")

HIGG5D3SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Photons",
                                           "Muons",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "AntiKt4EMTopoJets",
#                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                           "AntiKt4TruthJets",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt2Track",
#                                           "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
#                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
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
#                 ,"AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
                ]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
#              "AntiKt4TruthWZJets"
             ]
addJetOutputs(HIGG5D3SlimmingHelper,["HIGG5D3Jets"],slimmed_content)
# Add the MET containers to the stream
addMETOutputs(HIGG5D3SlimmingHelper,[],["AntiKt4EMTopo"])

HIGG5D3SlimmingHelper.IncludeEGammaTriggerContent = True
HIGG5D3SlimmingHelper.IncludeJetTriggerContent = True
HIGG5D3SlimmingHelper.IncludeBJetTriggerContent = True

HIGG5D3SlimmingHelper.AppendContentToStream(HIGG5D3Stream)
