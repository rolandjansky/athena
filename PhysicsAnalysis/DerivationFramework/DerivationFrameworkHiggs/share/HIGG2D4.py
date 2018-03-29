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
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkHiggs.TruthCategories import *
import AthenaCommon.SystemOfUnits as Units

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

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
HIGG2D4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*|HLT_xe.*|HLT_j.*|HLT_b.*' 
HIGG2D4ThinningHelper.AppendToStream(HIGG2D4Stream) 

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
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


# calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG2D4TauCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D4TauCCThinningTool",
                                                                    ThinningService       = HIGG2D4ThinningHelper.ThinningSvc(),
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += HIGG2D4TauCCThinningTool
thinningTools.append(HIGG2D4TauCCThinningTool)


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

if DerivationFrameworkIsMonteCarlo:
    ToolSvc += HIGG2D4TruthThinningTool
    thinningTools.append(HIGG2D4TruthThinningTool)
print "HIGG2D4.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
beamEnergy = jobproperties.Beam.energy()
print "HIGG2D4.py jobproperties.Beam.energy()", beamEnergy

# 13 TeV
singleElectronTriggerRequirement=["HLT_e.*"]
diElectronTriggerRequirement=["HLT_2e.*"]
singleMuonTriggerRequirement=["HLT_mu.*"]
diMuonTriggerRequirement=["HLT_2mu.*"]
metTriggerRequirement=[]
# add MET trigger to recover muon trigger efficiency at high pt
if True :
    triglist=[]
    triglist.append("HLT_xe80_tc_lcw_L1XE50") # added on Apr 2016
    triglist.append("HLT_xe90_tc_lcw_L1XE50")
    triglist.append("HLT_xe100_tc_lcw_L1XE50")
    triglist.append("HLT_xe110_tc_lcw_L1XE60")
    triglist.append("HLT_xe80_mht_L1XE50")
    triglist.append("HLT_xe90_mht_L1XE50")
    triglist.append("HLT_xe100_mht_L1XE50")
    triglist.append("HLT_xe100_mht_L1XE60")
    triglist.append("HLT_xe110_mht_L1XE50") # added on Aug 2016
    triglist.append("HLT_xe110_mht_L1XE50_AND_xe70_L1XE50") # added on Sep 2016
    triglist.append("HLT_xe130_mht_L1XE50") # added on Aug 2016
    triglist.append("HLT_xe90_L1XE50")
    triglist.append("HLT_xe100_L1XE50")
    triglist.append("HLT_xe110_L1XE60")
    triglist.append("HLT_xe80_tc_em_L1XE50")
    triglist.append("HLT_xe90_tc_em_L1XE50")
    triglist.append("HLT_xe100_tc_em_L1XE50")
    triglist.append("HLT_xe80_tc_lcw")
    triglist.append("HLT_xe90_tc_lcw")
    triglist.append("HLT_xe100_tc_lcw")
    triglist.append("HLT_xe90_mht")
    triglist.append("HLT_xe100_mht")
    triglist.append("HLT_xe90_tc_lcw_wEFMu_L1XE50")
    triglist.append("HLT_xe90_mht_wEFMu_L1XE50")
    triglist.append("HLT_xe120_pueta")
    triglist.append("HLT_xe120_pufit")
    triglist.append("HLT_xe100_tc_lcw_L1XE60") # added on Jun 2016
    triglist.append("HLT_xe110_tc_em_L1XE50")
    triglist.append("HLT_xe110_tc_em_wEFMu_L1XE50")
    triglist.append("HLT_xe120_pueta_wEFMu")
    triglist.append("HLT_xe120_mht")
    triglist.append("HLT_xe120_tc_lcw")
    triglist.append("HLT_xe120_mht_wEFMu")
    triglist.append("HLT_xe110_L1XE50")
    triglist.append("HLT_xe100_L1XE60")
    triglist.append("HLT_xe120_pufit_wEFMu")
    triglist.append("HLT_xe120_tc_lcw_wEFMu")
    triglist.append("HLT_xe120_tc_em")
    triglist.append("HLT_noalg_L1J400") # added on Nov 2016
    if (beamEnergy > 6.0e+06 and rec.projectName.get_Value() in ['data17_13TeV','data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV']) :  # 13 TeV,   and project 2017:
        triglist.append("HLT_e28_lhtight_nod0_ivarloose")
        triglist.append("HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM")
        triglist.append("HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM")
        triglist.append("HLT_e60_lhmedium_nod0_L1EM24VHI")
        triglist.append("HLT_e140_lhloose_nod0_L1EM24VHI")
        triglist.append("HLT_e300_etcut_L1EM24VHI")
        triglist.append("HLT_mu60")
        triglist.append("HLT_xe110_pufit_L1XE60")
        triglist.append("HLT_xe120_pufit_L1XE60")
        triglist.append("HLT_xe120_mht_xe80_L1XE60")
        triglist.append("HLT_xe110_pufit_L1XE55")
        triglist.append("HLT_xe120_pufit_L1XE55")
        triglist.append("HLT_xe120_mht_xe80_L1XE55")
        triglist.append("HLT_xe110_pufit_L1XE50")
        triglist.append("HLT_xe120_pufit_L1XE50")
        triglist.append("HLT_xe120_mht_xe80_L1XE50")
    metTriggerRequirement+=triglist


electronMuonTriggerRequirement=[]
if beamEnergy==4000000.0:
    # 8 TeV
    singleElectronTriggerRequirement=["EF_e24vhi_medium1", "EF_e60_medium1"]
    diElectronTriggerRequirement=["EF_2e12Tvh_loose1", "EF_2e12Tvh_loose1_L2StarB"]
    singleMuonTriggerRequirement=["EF_mu24i_tight", "EF_mu36_tight"]
    diMuonTriggerRequirement=["EF_2mu13", "EF_mu18_tight_mu8_EFFS"]
    electronMuonTriggerRequirement=["EF_e12Tvh_medium1_mu8", "EF_e24vhi_loose1_mu8"]
triggerRequirement=singleElectronTriggerRequirement+diElectronTriggerRequirement+singleMuonTriggerRequirement+diMuonTriggerRequirement+electronMuonTriggerRequirement+metTriggerRequirement
# 8 TeV MC does not have trigger information
SkipTriggerRequirement=(DerivationFrameworkIsMonteCarlo and (beamEnergy==4000000.0))
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
                                                             MergedJetContainerKey1  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                             NumberOfMergedJets1     = 1,
                                                             MergedJetPtCut1         = 100.*Units.GeV,
                                                             MergedJetEtaCut1        = 2.6, 
                                                             NumberOfPhotons         = 0,
                                                             ElectronQuality         = "DFCommonElectronsLHVeryLoose",
                                                             ElectronEtCut           = 6.*Units.GeV,
                                                             MuonQuality             = "DFCommonMuonsPreselection",
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
# Standard jets
#====================================================================

if not "HIGG2D4Jets" in OutputJets:
    OutputJets["HIGG2D4Jets"] = []

    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
    replaceAODReducedJets(reducedJetList, higg2d4Seq, "HIGG2D4Jets")

#====================================================================
# Special jets
#====================================================================

    if jetFlags.useTruth:
        addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, mods="groomed", includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")

    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, mods="lctopo_groomed", includePreTools=False, algseq=higg2d4Seq,outputGroup="HIGG2D4Jets")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

addVRJets(higg2d4Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet", 
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", 
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000, 
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg2d4Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg2d4Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

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

HIGG2D4SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"                :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"             :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"           :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"        :   "xAOD::BTaggingAuxContainer",
  }

HIGG2D4SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                          "AntiKt4TruthJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt2Track",
#                                           "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
#                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG2D4SlimmingHelper.ExtraVariables = HIGG2D4ExtraContent
HIGG2D4SlimmingHelper.AllVariables = HIGG2D4ExtraContainers
if DerivationFrameworkIsMonteCarlo:
    HIGG2D4SlimmingHelper.ExtraVariables += HIGG2D4ExtraContentTruth
    HIGG2D4SlimmingHelper.AllVariables += HIGG2D4ExtraContainersTruth
HIGG2D4SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["HIGG2D4Jets","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]
addJetOutputs(HIGG2D4SlimmingHelper,["HIGG2D4Jets"],slimmed_content)
# Add MET_RefFinalFix
addMETOutputs(HIGG2D4SlimmingHelper,[],["Track","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"])

HIGG2D4SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D4SlimmingHelper.IncludeEGammaTriggerContent = True
HIGG2D4SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG2D4SlimmingHelper.IncludeJetTriggerContent = True
HIGG2D4SlimmingHelper.IncludeBJetTriggerContent = True

HIGG2D4SlimmingHelper.AppendContentToStream(HIGG2D4Stream)

