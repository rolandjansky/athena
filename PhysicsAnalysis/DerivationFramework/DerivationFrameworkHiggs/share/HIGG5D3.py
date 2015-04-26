#********************************************************************
# HIGG5D3.py (for 0-lepton)
# reductionConf flag HIGG5D3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *


#====================================================================
# Special jets
#====================================================================
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

if not "MyJets" in OutputJets:
    OutputJets["MyJets"] = ["AntiKt3PV0TrackJets","AntiKt2PV0TrackJets"]

    if jetFlags.useTruth:
        addPrunedJets("CamKt", 1.0, "Truth", rcut=0.5, zcut=0.15, algseq=DerivationFrameworkJob)
        OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("CamKt10TruthPrunedR50Z15Jets")])
        addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.15, algseq=DerivationFrameworkJob)
        OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("CamKt12TruthBDRSFilteredMU100Y15Jets")])
        addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.2, ptfrac=0.05, algseq=DerivationFrameworkJob)
        OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")])

    # AntiKt10LCTopo trimmed rclus 0.2
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, algseq=DerivationFrameworkJob)
    OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets")])
    # CamKtLCTopo 10 and 12
    addPrunedJets("CamKt", 1.0, "LCTopo", rcut=0.5, zcut=0.15, algseq=DerivationFrameworkJob)
    OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("CamKt10LCTopoPrunedR50Z15Jets")])
    addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, algseq=DerivationFrameworkJob)
    OutputJets["MyJets"].append(OutputJets["LargeR"][OutputJets["LargeR"].index("CamKt12LCTopoBDRSFilteredMU100Y15Jets")])

    OutputJets["MyJets"].append("AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets")


# running on data or MC
from AthenaCommon.GlobalFlags import globalflags
#print "Yoshikazu Nagai test: globalflags.DataSource()"
#print globalflags.DataSource()  # this should be "data" or "geant4"
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]
# MC truth thinning (not for data)
#if (is_MC):
# from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
# HIGG5D3MCThinningTool = DerivationFramework__MenuTruthThinning(
#     name = "HIGG5D3MCThinningTool",
#     ThinningService = "HIGG5D3ThinningSvc",
#     WritePartons = False,
#     WriteHadrons = False,
#     WriteBHadrons = True,
#     WriteGeant = False,
#     GeantPhotonPtThresh = -1.0,
#     WriteTauHad = True,
#     WriteBSM = False,
#     PartonPtThresh = -1.0,
#     WriteBosons = True,
#     WriteBSMProducts = False,
#     WriteBosonProducts = True,
#     WriteTopAndDecays = True,
#     WriteEverything = False,
#     WriteAllLeptons = True,
#     WriteStatus3 = False,
#     WriteFirstN = -1,
#     PreserveGeneratorDescendants = True
#     )
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top quark and Bottom quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG5D3MCThinningTool = DerivationFramework__GenericTruthThinning(
    name                    = "HIGG5D3MCThinningTool", 
    ThinningService         = "HIGG5D3ThinningSvc",
    ParticleSelectionString = truth_expression,
    PreserveDescendants     = False,
    PreserveGeneratorDescendants = True,
    PreserveAncestors       = True)
if (is_MC) :
    ToolSvc += HIGG5D3MCThinningTool
    thinningTools.append(HIGG5D3MCThinningTool)

#====================================================================
# Skimming Tool
#====================================================================
beamEnergy = jobproperties.Beam.energy()
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG5VBF
if (beamEnergy < 4.1e+06): # 8 TeV - Run1
    if (is_MC): 
        HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                        JetContainerKey         = "AntiKt4EMTopoJets",
                                                                        # jet multiplicity requirement 2b + 2j
                                                                        ReqNAllJets             = True,
                                                                        NumberOfAllJets         = 4,
                                                                        AllJetPtCut             = 40.*Units.GeV,
                                                                        AllJetEtaCut            = 4.9,
                                                                        # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                        ReqNCentralJets         = True,
                                                                        NumberOfCentralJets     = 2,
                                                                        CentralJetPtCut         = 40.*Units.GeV,
                                                                        CentralJetEtaCut        = 2.6,
                                                                        # trigger requirement
                                                                        ReqTrigger              = False,
                                                                        Triggers                = [],
                                                                        # Mjj requirement
                                                                        ReqVBFMjj               = True,
                                                                        MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                        DoDebug                 = False,
                                                                        #photon requirement (p. rose)
                                                                        PhotonContainerKey      = "Photons",
                                                                        ReqPhoton               = True,
                                                                        PhotonPtCut             = 20.*Units.GeV,
                                                                        CentralPhotonEtaCut     = 2.6)

    else :
        HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                        JetContainerKey         = "AntiKt4EMTopoJets",
                                                                        # jet multiplicity requirement 2b + 2j
                                                                        ReqNAllJets             = True,
                                                                        NumberOfAllJets         = 4,
                                                                        AllJetPtCut             = 40.*Units.GeV,
                                                                        AllJetEtaCut            = 4.9,
                                                                        # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                        ReqNCentralJets         = True,
                                                                        NumberOfCentralJets     = 2,
                                                                        CentralJetPtCut         = 40.*Units.GeV,
                                                                        CentralJetEtaCut        = 2.6,
                                                                        # trigger requirement
                                                                        ReqTrigger              = True,
                                                                        #Triggers                = ["EF_2b35_loose_4j35_a4tchad","EF_b35_medium_j35_a4tchad_vbf_3L1J15_FJ15","EF_b35_medium_j35_a4tchad_vbf_2L1FJ15", "EF_g20_loose"],
                                                                        Triggers                = ["HLT_2b35_loose_4j35_a4tchad","HLT_b35_medium_j35_a4tchad_vbf_3L1J15_FJ15","HLT_b35_medium_j35_a4tchad_vbf_2L1FJ15", "HLT_g20_loose"],
                                                                        # Mjj requirement
                                                                        ReqVBFMjj               = True,
                                                                        MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                        DoDebug                 = False,
                                                                        
                                                                        #photon requirement (p. rose)
                                                                        PhotonContainerKey      = "Photons",
                                                                        ReqPhoton               = True,
                                                                        PhotonPtCut             = 20.*Units.GeV,
                                                                        CentralPhotonEtaCut     = 2.6)

if (beamEnergy > 6.0e+06): # 13 TeV - Run2
    HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                    JetContainerKey         = "AntiKt4EMTopoJets",
                                                                    # jet multiplicity requirement 2b + 2j
                                                                    ReqNAllJets             = True,
                                                                    NumberOfAllJets         = 4,
                                                                    AllJetPtCut             = 40.*Units.GeV,
                                                                    AllJetEtaCut            = 4.9,
                                                                    # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                    ReqNCentralJets         = True,
                                                                    NumberOfCentralJets     = 2,
                                                                    CentralJetPtCut         = 40.*Units.GeV,  # to be adjusted according to b-jet trigger pT threshold
                                                                    CentralJetEtaCut        = 2.6,
                                                                    # trigger requirement
                                                                    ReqTrigger              = False,
                                                                    Triggers                = [], # to be added
                                                                    # Mjj requirement
                                                                    ReqVBFMjj               = True,
                                                                    MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                    DoDebug                 = False,
                                                                    #photon requirement (p. rose)
                                                                    PhotonContainerKey      = "Photons",
                                                                    ReqPhoton               = True,
                                                                    PhotonPtCut             = 20.*Units.GeV,
                                                                    CentralPhotonEtaCut     = 2.6)


ToolSvc += HIGG5D3SkimmingTool

#print HIGG5D3SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
#if (is_MC):
#    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#        "HIGG5D3Kernel",
#        ThinningTools = [HIGG5D3MCThinningTool],
#        SkimmingTools = [HIGG5D3SkimmingTool]
#        )
#else:
#    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#        "HIGG5D3Kernel",
#        SkimmingTools = [HIGG5D3SkimmingTool]
#        )
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D3Kernel",
    ThinningTools = thinningTools,
    SkimmingTools = [HIGG5D3SkimmingTool]
    )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D3Stream )
HIGG5D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D3Stream.AcceptAlgs(["HIGG5D3Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG5D3ThinningSvc", outStreams=[evtStream] )

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
                                           "TauJets",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "MET_Reference_AntiKt4LCTopo",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4LCTopoJets",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt4LCTopo",
                                           "InDetTrackParticles",
                                           "PrimaryVertices" ]

HIGG5D3SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D3SlimmingHelper.AllVariables = ExtraContainers
if (is_MC) :
    HIGG5D3SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D3SlimmingHelper.AllVariables += ExtraContainersTruth

# Add the jet containers to the stream
addJetOutputs(HIGG5D3SlimmingHelper,["MyJets"])
# Add the MET containers to the stream
addMETOutputs(HIGG5D3SlimmingHelper,["AntiKt4LCTopo"])

HIGG5D3SlimmingHelper.AppendContentToStream(HIGG5D3Stream)
