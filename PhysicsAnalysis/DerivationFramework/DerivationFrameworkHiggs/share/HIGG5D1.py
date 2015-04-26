#********************************************************************
# HIGG5D1.py (for 0-lepton)
# reductionConf flag HIGG5D1 in Reco_tf.py   
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
# from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
# HIGG5D1MCThinningTool = DerivationFramework__MenuTruthThinning(
#     name = "HIGG5D1MCThinningTool",
#     ThinningService = "HIGG5D1ThinningSvc",
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
HIGG5D1MCThinningTool = DerivationFramework__GenericTruthThinning(
    name                    = "HIGG5D1MCThinningTool", 
    ThinningService         = "HIGG5D1ThinningSvc",
    ParticleSelectionString = truth_expression,
    PreserveDescendants     = False,
    PreserveGeneratorDescendants = True,
    PreserveAncestors       = True)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG5D1MCThinningTool
    thinningTools.append(HIGG5D1MCThinningTool)

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG5D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "HIGG5D1TPThinningTool",
                                                                ThinningService         = "HIGG5D1ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG5D1TPThinningTool
thinningTools.append(HIGG5D1TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG5D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG5D1JetTPThinningTool",
                                                                ThinningService         = "HIGG5D1ThinningSvc",
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG5D1JetTPThinningTool
thinningTools.append(HIGG5D1JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG5D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG5D1MuonTPThinningTool",
                                                                            ThinningService         = "HIGG5D1ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D1MuonTPThinningTool
thinningTools.append(HIGG5D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D1ElectronTPThinningTool",
                                                                                        ThinningService         = "HIGG5D1ThinningSvc",
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        BestMatchOnly           = True)
ToolSvc += HIGG5D1ElectronTPThinningTool
thinningTools.append(HIGG5D1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D1PhotonTPThinningTool",
                                                                                      ThinningService         = "HIGG5D1ThinningSvc",
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG5D1PhotonTPThinningTool
thinningTools.append(HIGG5D1PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG5D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                  = "HIGG5D1TauTPThinningTool",
                                                                          ThinningService         = "HIGG5D1ThinningSvc",
                                                                          TauKey                  = "TauJets",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D1TauTPThinningTool
thinningTools.append(HIGG5D1TauTPThinningTool)

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
    #trigSel = '(EventInfo.eventTypeBitmask==1) || EF_xe100 || EF_xe50_j40_dphi1 || EF_xe80T_tclcw_loose || EF_xe80_tclcw_loose'
    trigSel = '(EventInfo.eventTypeBitmask==1) || HLT_xe100 || HLT_xe50_j40_dphi1 || HLT_xe80T_tclcw_loose || HLT_xe80_tclcw_loose'
if (beamEnergy > 6.0e+06): # 13 TeV, name should be HLT_xxx
    #trigSel = 'HLT_xe100 || HLT_xe70'
    trigSel = 'L1_XE70 || L1_XE50' ## use L1 till HLT thresholds are fixed, L1_XE50 will be default XE trigger

#====================================================================
# SKIMMING TOOL 
#====================================================================
expression = '( '+ jetSel + ' ) && ( ' + trigSel + ' )'

#print "Yoshikazu Nagai test: beamEnergy & selection expression"
#print beamEnergy
#print expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D1SkimmingTool",
                                                                    expression = expression)
ToolSvc += HIGG5D1SkimmingTool
#print HIGG5D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
#if (is_MC):
#    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#        "HIGG5D1Kernel",
#        ThinningTools = [HIGG5D1MCThinningTool],
#        SkimmingTools = [HIGG5D1SkimmingTool]
#        )
#else:
#    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
#        "HIGG5D1Kernel",
#        SkimmingTools = [HIGG5D1SkimmingTool]
#        )
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel",
    ThinningTools = thinningTools,
    SkimmingTools = [HIGG5D1SkimmingTool]
    )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D1Stream )
HIGG5D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D1Stream.AcceptAlgs(["HIGG5D1Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG5D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D1ContentList import HIGG5D1Content
from DerivationFrameworkHiggs.HIGG5D1ExtraContent import *
HIGG5D1SlimmingHelper = SlimmingHelper("HIGG5D1SlimmingHelper")

HIGG5D1SlimmingHelper.SmartCollections = [ "Electrons",
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

HIGG5D1SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D1SlimmingHelper.AllVariables = ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG5D1SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D1SlimmingHelper.AllVariables += ExtraContainersTruth

# Add the jet containers to the stream
addJetOutputs(HIGG5D1SlimmingHelper,["MyJets"])
# Add the MET containers to the stream
addMETOutputs(HIGG5D1SlimmingHelper,["AntiKt4LCTopo"])

HIGG5D1SlimmingHelper.AppendContentToStream(HIGG5D1Stream)
