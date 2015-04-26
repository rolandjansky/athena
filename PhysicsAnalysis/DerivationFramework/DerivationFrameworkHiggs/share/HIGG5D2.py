#********************************************************************
# HIGG5D2.py (for 1-lepton)
# reductionConf flag HIGG5D2 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *


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
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
# HIGG5D2MCThinningTool = DerivationFramework__MenuTruthThinning(
#     name = "HIGG5D2MCThinningTool",
#     ThinningService = "HIGG5D2ThinningSvc",
#     WritePartons = False,
#     WriteHadrons = False,
#     WriteBHadrons = True,
#     WriteGeant = False,
#     GeantPhotonPtThresh = -1.0,
#     WriteTauHad = True,
#     PartonPtThresh = -1.0,
#     WriteBSM = False,
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
HIGG5D2MCThinningTool = DerivationFramework__GenericTruthThinning(
    name                    = "HIGG5D2MCThinningTool", 
    ThinningService         = "HIGG5D2ThinningSvc",
    ParticleSelectionString = truth_expression,
    PreserveDescendants     = False,
    PreserveGeneratorDescendants = True,
    PreserveAncestors       = True)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG5D2MCThinningTool
    thinningTools.append(HIGG5D2MCThinningTool)

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG5D2TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "HIGG5D2TPThinningTool",
                                                                ThinningService         = "HIGG5D2ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG5D2TPThinningTool
thinningTools.append(HIGG5D2TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG5D2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG5D2JetTPThinningTool",
                                                                ThinningService         = "HIGG5D2ThinningSvc",
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG5D2JetTPThinningTool
thinningTools.append(HIGG5D2JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG5D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG5D2MuonTPThinningTool",
                                                                            ThinningService         = "HIGG5D2ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D2MuonTPThinningTool
thinningTools.append(HIGG5D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D2ElectronTPThinningTool",
                                                                                        ThinningService         = "HIGG5D2ThinningSvc",
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        BestMatchOnly           = True)
ToolSvc += HIGG5D2ElectronTPThinningTool
thinningTools.append(HIGG5D2ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D2PhotonTPThinningTool",
                                                                                      ThinningService         = "HIGG5D2ThinningSvc",
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG5D2PhotonTPThinningTool
thinningTools.append(HIGG5D2PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG5D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                  = "HIGG5D2TauTPThinningTool",
                                                                          ThinningService         = "HIGG5D2ThinningSvc",
                                                                          TauKey                  = "TauJets",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D2TauTPThinningTool
thinningTools.append(HIGG5D2TauTPThinningTool)


#========================================================================
# lepton selection (keep isolation failed leptons for QCD-MJ estimation)
#========================================================================
lepSel = '( count( (Muons.pt > 15.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon) ) + count( (DFCommonElectrons_pt > 15.0*GeV) && (abs(DFCommonElectrons_eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose) ) ) > 0'

#====================================================================
# jet selection 
#====================================================================
#jetSel = '(( count( (AntiKt4EMTopoJets.pt > 0.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 1) || (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0) || (( count( (AntiKt10LCTopoJets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0) || (( count( (CamKt12LCTopoJets.pt > 100.0*GeV) && (abs(CamKt12LCTopoJets.eta) < 2.6) ) ) > 0) || (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0) || (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets.eta) < 2.6) ) ) > 0) || (( count( (CamKt10LCTopoPrunedR50Z15Jets.pt > 100.0*GeV) && (abs(CamKt10LCTopoPrunedR50Z15Jets.eta) < 2.6) ) ) > 0) || (( count( (CamKt12LCTopoBDRSFilteredMU100Y15Jets.pt > 100.0*GeV) && (abs(CamKt12LCTopoBDRSFilteredMU100Y15Jets.eta) < 2.6) ) ) > 0)'
jetSel = '(( count( (AntiKt4EMTopoJets.pt > 0.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 1) || (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0) || (( count( (AntiKt10LCTopoJets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0) || (( count( (CamKt12LCTopoJets.pt > 100.0*GeV) && (abs(CamKt12LCTopoJets.eta) < 2.6) ) ) > 0)'

#====================================================================
# Trigger selection
#====================================================================
beamEnergy = jobproperties.Beam.energy()
trigSel = '' # This is the string that will eventually go into the parser
if (beamEnergy < 4.1e+06): # 8 TeV, name should be EF_xxx in Rel19, HLT_xxx in Rel20
    #trigSel = '(EventInfo.eventTypeBitmask==1) || EF_e24vhi_medium1 || EF_e60_medium1 || EF_mu24i_tight || EF_mu36_tight || EF_xe100 || EF_xe50_j40_dphi1 || EF_xe80T_tclcw_loose || EF_xe80_tclcw_loose'
    trigSel = '(EventInfo.eventTypeBitmask==1) || HLT_e24vhi_medium1 || HLT_e60_medium1 || HLT_mu24i_tight || HLT_mu36_tight || HLT_xe100 || HLT_xe50_j40_dphi1 || HLT_xe80T_tclcw_loose || HLT_xe80_tclcw_loose'
if (beamEnergy > 6.0e+06): # 13 TeV, name should be HLT_xxx
    #trigSel = 'HLT_e24_medium1_iloose || HLT_e24_lhmedium_iloose || HLT_e24_medium_iloose || HLT_e24_loose1 || HLT_e28_tight1_iloose || HLT_mu26 || HLT_mu24 || HLT_xe100'
    trigSel = 'L1_EM24VHI || L1_EM50 || L1_MU20' # use L1 till HLT thresholds are fixed

#====================================================================
# SKIMMING TOOL 
#====================================================================
expression = '( '+ jetSel + ' ) && ( ' +lepSel + ' ) && ( ' + trigSel + ' )'

#print "Yoshikazu Nagai test: beamEnergy & selection expression"
#print beamEnergy
#print expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "HIGG5D2SkimmingTool",
                                                                  expression = expression)
ToolSvc += HIGG5D2SkimmingTool
#print HIGG5D2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
#if (is_MC):
#    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#        "HIGG5D2Kernel",
#        ThinningTools = [HIGG5D2MCThinningTool,HIGG5D2TPThinningTool,HIGG5D2JetTPThinningTool,HIGG5D2MuonTPThinningTool,HIGG5D2ElectronTPThinningTool],
#        SkimmingTools = [HIGG5D2SkimmingTool]
#        )
#else:
#    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#        "HIGG5D2Kernel",
#        ThinningTools = [HIGG5D2TPThinningTool,HIGG5D2JetTPThinningTool,HIGG5D2MuonTPThinningTool,HIGG5D2ElectronTPThinningTool],
#        SkimmingTools = [HIGG5D2SkimmingTool]
#        )
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel",
    ThinningTools = thinningTools,
    SkimmingTools = [HIGG5D2SkimmingTool]
    )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D2Stream )
HIGG5D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D2Stream.AcceptAlgs(["HIGG5D2Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG5D2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D2ContentList import HIGG5D2Content
from DerivationFrameworkHiggs.HIGG5D2ExtraContent import *
HIGG5D2SlimmingHelper = SlimmingHelper("HIGG5D2SlimmingHelper")

HIGG5D2SlimmingHelper.SmartCollections = [ "Electrons",
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

HIGG5D2SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D2SlimmingHelper.AllVariables = ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG5D2SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D2SlimmingHelper.AllVariables += ExtraContainersTruth

# Add the jet containers to the stream
addJetOutputs(HIGG5D2SlimmingHelper,["MyJets"])
# Add the MET containers to the stream
addMETOutputs(HIGG5D2SlimmingHelper,["AntiKt4LCTopo"])

HIGG5D2SlimmingHelper.AppendContentToStream(HIGG5D2Stream)
