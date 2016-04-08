#====================================================================
# HIGG2D4.py for 2L2Q filter
# This requires the reductionConf flag HIGG2D4 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
import AthenaCommon.SystemOfUnits as Units

#==================================================================== 
# SET UP STREAM 
#==================================================================== 
streamName = derivationFlags.WriteDAOD_HIGG2D4Stream.StreamName 
fileName = buildFileName(derivationFlags.WriteDAOD_HIGG2D4Stream) 
HIGG2D4Stream = MSMgr.NewPoolRootStream( streamName, fileName) 
HIGG2D4Stream.AcceptAlgs(["HIGG2D4Kernel"]) 

#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
HIGG2D4ThinningHelper = ThinningHelper("HIGG2D4ThinningHelper") 
#trigger navigation content  
HIGG2D4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*' 
HIGG2D4ThinningHelper.AppendToStream(HIGG2D4Stream) 

# MET/Jet tracks
# thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# HIGG2D4TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D4TPThinningTool",
#                                                                    ThinningService        = HIGG2D4ThinningHelper.ThinningSvc(),
#                                                                    SelectionString        = thinning_expression,
#                                                                    InDetTrackParticlesKey = "InDetTrackParticles",
#                                                                    ApplyAnd               = True)
# ToolSvc += HIGG2D4TPThinningTool
# thinningTools.append(HIGG2D4TPThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D4JetTPThinningTool",
                                                                         ThinningService        = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D4JetTPThinningTool
thinningTools.append(HIGG2D4JetTPThinningTool)

HIGG2D4JetTPThinningTool2 = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D4JetTPThinningTool2",
                                                                          ThinningService        = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                          JetKey                 = "AntiKt4EMTopoJets",
                                                                          InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D4JetTPThinningTool2
thinningTools.append(HIGG2D4JetTPThinningTool2)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D4MuonTPThinningTool",
                                                                           ThinningService        = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D4MuonTPThinningTool
thinningTools.append(HIGG2D4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D4ElectronTPThinningTool",
                                                                                 ThinningService        = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D4ElectronTPThinningTool
thinningTools.append(HIGG2D4ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D4PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG2D4PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG2D4PhotonTPThinningTool
thinningTools.append(HIGG2D4PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D4TauTPThinningTool",
                                                                         ThinningService        = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D4TauTPThinningTool
thinningTools.append(HIGG2D4TauTPThinningTool)

# Truth particles
useGenericTruthThinning = True
if useGenericTruthThinning:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top or Bottom quark
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    HIGG2D4TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D4TruthThinningTool", 
                                                                         ThinningService              = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                         ParticleSelectionString      = truth_expression,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = True)
else:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    HIGG2D4TruthThinningTool = DerivationFramework__MenuTruthThinning(name                         = "HIGG2D4TruthThinningTool",
                                                                      ThinningService              = "HIGG2D4ThinningSvc",
                                                                      WritePartons                 = False,
                                                                      WriteHadrons                 = False,
                                                                      WriteBHadrons                = True,
                                                                      WriteGeant                   = False,
                                                                      GeantPhotonPtThresh          = -1.0,
                                                                      WriteTauHad                  = True,
                                                                      PartonPtThresh               = -1.0,
                                                                      WriteBSM                     = True,
                                                                      WriteBosons                  = True,
                                                                      WriteBSMProducts             = True,
                                                                      WriteBosonProducts           = True,
                                                                      WriteTopAndDecays            = True,
                                                                      WriteEverything              = False,
                                                                      WriteAllLeptons              = True,
                                                                      WriteStatus3                 = False,
                                                                      PreserveGeneratorDescendants = True,
                                                                      WriteFirstN                  = -1)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG2D4.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D4TruthThinningTool
    thinningTools.append(HIGG2D4TruthThinningTool)
print "HIGG2D4.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D4.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
# 13 TeV
singleElectronTriggerRequirement=["L1_EM.*"]
diElectronTriggerRequirement=["L1_2EM.*"]
singleMuonTriggerRequirement=["L1_MU.*"]
diMuonTriggerRequirement=["L1_2MU.*"]
electronMuonTriggerRequirement=[]
if jobproperties.Beam.energy()==4000000.0:
    # 8 TeV
    singleElectronTriggerRequirement=["EF_e24vhi_medium1", "EF_e60_medium1"]
    diElectronTriggerRequirement=["EF_2e12Tvh_loose1", "EF_2e12Tvh_loose1_L2StarB"]
    singleMuonTriggerRequirement=["EF_mu24i_tight", "EF_mu36_tight"]
    diMuonTriggerRequirement=["EF_2mu13", "EF_mu18_tight_mu8_EFFS"]
    electronMuonTriggerRequirement=["EF_e12Tvh_medium1_mu8", "EF_e24vhi_loose1_mu8"]
triggerRequirement=singleElectronTriggerRequirement+diElectronTriggerRequirement+singleMuonTriggerRequirement+diMuonTriggerRequirement+electronMuonTriggerRequirement
# 8 TeV MC does not have trigger information
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
print "HIGG2D4.py SkipTriggerRequirement", SkipTriggerRequirement
if SkipTriggerRequirement:
    triggerRequirement=[]
print "HIGG2D4.py triggerRequirement", triggerRequirement

# Pre-selection for di-leptons
electronPtRequirement='(Electrons.pt>6.*GeV)'
electronRequirement='('+electronPtRequirement+')'
#electronQualityRequirement='(Electrons.DFCommonElectronsLHVeryLoose)'
#electronRequirement='('+electronQualityRequirement+'&&'+electronPtRequirement+')'
combinedMuonQualityRequirement='(Muons.muonType==0)'
standaloneMuonQualityRequirement='(Muons.muonType==1 && abs(abs(Muons.eta)-2.6)<0.12)'
segmentTaggedMuonQualityRequirement='(Muons.muonType==2)'
calorimeterTaggedMuonQualityRequirement='(Muons.muonType==3 && abs(Muons.eta)<0.2)'
muonQualityRequirement='('+combinedMuonQualityRequirement+'||'+standaloneMuonQualityRequirement+'||'+segmentTaggedMuonQualityRequirement+'||'+calorimeterTaggedMuonQualityRequirement+')'
muonPtRequirement='(Muons.pt>6.*GeV)'
muonRequirement='('+muonQualityRequirement+'&&'+muonPtRequirement+')'
leptonRequirement='(count('+electronRequirement+')+count('+muonRequirement+')>= 2)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SkimmingToolHIGG2D4_preLep = DerivationFramework__xAODStringSkimmingTool(name = "SkimmingToolHIGG2D4_preLep",
                                                                         expression = leptonRequirement)
ToolSvc += SkimmingToolHIGG2D4_preLep

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D4 = DerivationFramework__SkimmingToolHIGG2(name                    = "SkimmingToolHIGG2D4",
                                                             FilterType              = "2L2Q", 
                                                             NumberOfLeptons         = 2,
                                                             NumberOfElectrons       = 0,
                                                             NumberOfMuons           = 0,
                                                             JetContainerKey         = "AntiKt4EMTopoJets",
                                                             NumberOfJets            = 1,
                                                             JetPtCut                = 15.*Units.GeV,
                                                             JetEtaCut               = 2.6, 
                                                             MergedJetContainerKey0  = "AntiKt4EMTopoJets",
                                                             NumberOfMergedJets0     = 1,
                                                             MergedJetPtCut0         = 100.*Units.GeV,
                                                             MergedJetEtaCut0        = 2.6, 
                                                             MergedJetContainerKey1  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",#"AntiKt10LCTopoJets",
                                                             NumberOfMergedJets1     = 1,
                                                             MergedJetPtCut1         = 100.*Units.GeV,
                                                             MergedJetEtaCut1        = 2.6, 
                                                             MergedJetContainerKey2  = "CamKt12LCTopoBDRSFilteredMU100Y15Jets",#"CamKt12LCTopoJets",
                                                             NumberOfMergedJets2     = 1,
                                                             MergedJetPtCut2         = 100.*Units.GeV,
                                                             MergedJetEtaCut2        = 2.6, 
                                                             NumberOfPhotons         = 0,
                                                             ElectronQuality         = "DFCommonElectronsLHVeryLoose",
                                                             ElectronEtCut           = 6.*Units.GeV,
                                                             MuonQuality             = "DFCommonMuonsLoose",
                                                             MuonPtCut               = 6.*Units.GeV,
                                                             RequireTightLeptons     = False,
                                                             InvariantMassCut        = 5.*Units.GeV,
                                                             DRElectronJetCut        = -1.,
                                                             Trigger2L2Q             = triggerRequirement)
ToolSvc += SkimmingToolHIGG2D4
print SkimmingToolHIGG2D4


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg2d4Seq = CfgMgr.AthSequencer("HIGG2D4Sequence")

# Then apply the TruthWZ fix
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(higg2d4Seq,'HIGG2D4')
    replaceBuggyAntiKt10TruthWZJets(higg2d4Seq,'HIGG2D4')


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg2d4Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG2D4Kernel_preLep",
    SkimmingTools = [SkimmingToolHIGG2D4_preLep]
    )

#====================================================================
# Special jets
#====================================================================
# from JetRec.JetRecStandard import jtm
# from JetRec.JetRecConf import JetAlgorithm

if not "HIGG2D4Jets" in OutputJets:
    OutputJets["HIGG2D4Jets"] = ["AntiKt3PV0TrackJets","AntiKt2PV0TrackJets","AntiKt10LCTopoJets","CamKt12LCTopoJets"]

    if jetFlags.useTruth:
    #     addPrunedJets("CamKt", 1.0, "Truth", rcut=0.5, zcut=0.15, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
    #     addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.15, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
    #     addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
    #     addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.3, ptfrac=0.05, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
        OutputJets["HIGG2D4Jets"].append("AntiKt4TruthJets")
        OutputJets["HIGG2D4Jets"].append("AntiKt4TruthWZJets")
        # OutputJets["HIGG2D4Jets"].append("AntiKt10TruthWZJets")
        # OutputJets["HIGG2D4Jets"].append("AntiKt10TruthJets")
        #OutputJets["HIGG2D4Jets"].append("CamKt12TruthJets")
        addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")

    # CamKtLCTopo 10 and 12
    # addPrunedJets("CamKt", 1.0, "LCTopo", rcut=0.5, zcut=0.15, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
    addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
    # AntiKt10LCTopo trimmed rclus 0.2 and 0.3
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")
    # addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.3, ptfrac=0.05, includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")

# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg2d4Seq)


# Main selection
higg2d4Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG2D4Kernel",
    SkimmingTools = [SkimmingToolHIGG2D4],
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += higg2d4Seq

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D4ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D4SlimmingHelper = SlimmingHelper("HIGG2D4SlimmingHelper")

HIGG2D4SlimmingHelper.SmartCollections = ["Electrons",
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
                                          "PrimaryVertices"]

HIGG2D4SlimmingHelper.ExtraVariables = HIGG2D4ExtraContent
HIGG2D4SlimmingHelper.AllVariables = HIGG2D4ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D4SlimmingHelper.ExtraVariables += HIGG2D4ExtraContentTruth
    HIGG2D4SlimmingHelper.AllVariables += HIGG2D4ExtraContainersTruth

# Add the jet containers to the stream
addJetOutputs(HIGG2D4SlimmingHelper,["HIGG2D4Jets"])
# Add MET_RefFinalFix
addMETOutputs(HIGG2D4SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D4SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D4SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D4SlimmingHelper.AppendContentToStream(HIGG2D4Stream)

