#********************************************************************
# HIGG5D1.py (for 0-lepton)
# reductionConf flag HIGG5D1 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
# from DerivationFrameworkJetEtMiss.JetMomentFix import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *

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
streamName = derivationFlags.WriteDAOD_HIGG5D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D1Stream )
HIGG5D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D1Stream.AcceptAlgs(["HIGG5D1Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
HIGG5D1ThinningHelper = ThinningHelper("HIGG5D1ThinningHelper") 
#trigger navigation content
HIGG5D1ThinningHelper.TriggerChains = 'HLT_xe.*|HLT_j.*|HLT_g.*' 
HIGG5D1ThinningHelper.AppendToStream(HIGG5D1Stream) 


# MC truth thinning (not for data)
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top quark and Bottom quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG5D1MCThinningTool = DerivationFramework__GenericTruthThinning(
    name                         = "HIGG5D1MCThinningTool", 
    ThinningService              = HIGG5D1ThinningHelper.ThinningSvc(),
    ParticleSelectionString      = truth_expression,
    PreserveDescendants          = False,
    PreserveGeneratorDescendants = True,
    PreserveAncestors            = True)

if DerivationFrameworkIsMonteCarlo :
    ToolSvc += HIGG5D1MCThinningTool
    thinningTools.append(HIGG5D1MCThinningTool)

# MET/Jet tracks
# thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# HIGG5D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "HIGG5D1TPThinningTool",
#                                                                 ThinningService         = "HIGG5D1ThinningSvc",
#                                                                 SelectionString         = thinning_expression,
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
#                                                                 ApplyAnd                = True)
# ToolSvc += HIGG5D1TPThinningTool
# thinningTools.append(HIGG5D1TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG5D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG5D1JetTPThinningTool",
                                                                ThinningService         = HIGG5D1ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D1JetTPThinningTool
thinningTools.append(HIGG5D1JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG5D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG5D1MuonTPThinningTool",
                                                                            ThinningService         = HIGG5D1ThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D1MuonTPThinningTool
thinningTools.append(HIGG5D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D1ElectronTPThinningTool",
                                                                                        ThinningService         = HIGG5D1ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        BestMatchOnly           = True)
ToolSvc += HIGG5D1ElectronTPThinningTool
thinningTools.append(HIGG5D1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D1PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG5D1ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG5D1PhotonTPThinningTool
thinningTools.append(HIGG5D1PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG5D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                  = "HIGG5D1TauTPThinningTool",
                                                                          ThinningService         = HIGG5D1ThinningHelper.ThinningSvc(),
                                                                          TauKey                  = "TauJets",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D1TauTPThinningTool
thinningTools.append(HIGG5D1TauTPThinningTool)


# calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG5D1TauCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG5D1TauCCThinningTool",
                                                                    ThinningService       = HIGG5D1ThinningHelper.ThinningSvc(),
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += HIGG5D1TauCCThinningTool
thinningTools.append(HIGG5D1TauCCThinningTool)


#====================================================================
# jet selection 
#====================================================================
#jetSel = '(( count( (AntiKt4EMTopoJets.pt > 0.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 1)'
jetSel = '(( count( (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoJets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0)'
# jetSel += '|| (( count( (CamKt12LCTopoJets.pt > 100.0*GeV) && (abs(CamKt12LCTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'
# jetSel += '|| (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.6) ) ) > 0)'
# jetSel += '|| (( count( (CamKt10LCTopoPrunedR50Z15Jets.pt > 100.0*GeV) && (abs(CamKt10LCTopoPrunedR50Z15Jets.eta) < 2.6) ) ) > 0)'
# jetSel += '|| (( count( (CamKt12LCTopoBDRSFilteredMU100Y15Jets.pt > 100.0*GeV) && (abs(CamKt12LCTopoBDRSFilteredMU100Y15Jets.eta) < 2.6) ) ) > 0)'

#====================================================================
# Trigger selection
#====================================================================
beamEnergy = jobproperties.Beam.energy()
triglist = []
if (beamEnergy < 4.1e+06): # 8 TeV, name should be EF_xxx in Rel19, HLT_xxx in Rel20
    #trigSel = '(EventInfo.eventTypeBitmask==1) || EF_xe100 || EF_xe50_j40_dphi1 || EF_xe80T_tclcw_loose || EF_xe80_tclcw_loose'
    triglist.append("HLT_xe100")
    triglist.append("HLT_xe50_j40_dphi1")
    triglist.append("HLT_xe80T_tclcw_loose")
    triglist.append("HLT_xe80_tclcw_loose")
if (beamEnergy > 6.0e+06): # 13 TeV, name should be HLT_xxx
    triglist.append("HLT_xe35")
    triglist.append("HLT_xe50")
    triglist.append("HLT_xe60")
    triglist.append("HLT_xe70")
    triglist.append("HLT_xe80")
    triglist.append("HLT_xe80_L1XE50")
    triglist.append("HLT_xe80_L1XE70")
    triglist.append("HLT_xe100")
    triglist.append("HLT_j80_xe80_dphi1_L1J40_DPHI-J20XE50")
    triglist.append("HLT_j80_xe80_dphi1_L1J40_DPHI-J20s2XE50")
    triglist.append("HLT_j100_xe80_L1J40_DPHI-J20XE50")
    triglist.append("HLT_j100_xe80_L1J40_DPHI-J20s2XE50")
    triglist.append("HLT_g120_loose")
    triglist.append("HLT_g140_loose") # added on Sep 2016
    triglist.append("HLT_g160_loose") # added on Sep 2016
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
# print 'DEBUG projectName=%s [%s;%s]' % (rec.projectName.get_Value(),type(rec.projectName),dir(rec.projectName))
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

    # triglist.append("L1_XE50")
    # triglist.append("L1_XE60")
    # triglist.append("L1_XE70")


#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D1JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D1JetSkimmingTool",
                                                                      expression = jetSel)
ToolSvc += HIGG5D1JetSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HIGG5D1TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name          = "HIGG5D1TriggerSkimmingTool",
                                                                         TriggerListOR = triglist )
ToolSvc += HIGG5D1TriggerSkimmingTool



#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg5d1Seq = CfgMgr.AthSequencer("HIGG5D1Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg5d1Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel_trig",
    SkimmingTools = [HIGG5D1TriggerSkimmingTool]
    )


#====================================================================
# Standard jets
#====================================================================
if not "HIGG5D1Jets" in OutputJets:
    OutputJets["HIGG5D1Jets"] = []

    #AntiKt2PV0TrackJets
    addStandardJets("AntiKt", 0.2, "PV0Track", 2000, mods="track_ungroomed", algseq=higg5d1Seq, outputGroup="HIGG5D1Jets")
    OutputJets["HIGG5D1Jets"].append("AntiKt2PV0TrackJets")
    #AntiKt4PV0TrackJets
    addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=higg5d1Seq, outputGroup="HIGG5D1Jets")
    OutputJets["HIGG5D1Jets"].append("AntiKt4PV0TrackJets")
    #AntiKt10LCTopoJets
    # addStandardJets("AntiKt", 1.0, "LCTopo", mods="lctopo_ungroomed", ptmin=40000, ptminFilter=50000, calibOpt="none", algseq=higg5d1Seq, outputGroup="HIGG5D1Jets")
    # OutputJets["HIGG5D1Jets"].append("AntiKt10LCTopoJets")

#====================================================================
# Special jets
#====================================================================
# if not "HIGG5D1Jets" in OutputJets:
    # OutputJets["HIGG5D1Jets"] = ["AntiKt2PV0TrackJets","AntiKt10LCTopoJets","CamKt12LCTopoJets"]

    if jetFlags.useTruth:
      #AntiKt4TruthJets
      addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=higg5d1Seq, outputGroup="HIGG5D1Jets")
      OutputJets["HIGG5D1Jets"].append("AntiKt4TruthJets")
      #AntiKt4TruthWZJets
      addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=higg5d1Seq, outputGroup="HIGG5D1Jets")
      OutputJets["HIGG5D1Jets"].append("AntiKt4TruthWZJets")

      # OutputJets["HIGG5D1Jets"].append("AntiKt10TruthWZJets")
      # OutputJets["HIGG5D1Jets"].append("AntiKt10TruthJets")
      # OutputJets["HIGG5D1Jets"].append("CamKt12TruthJets")
      addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d1Seq,outputGroup="HIGG5D1Jets")

    # addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, includePreTools=False, algseq=higg5d1Seq,outputGroup="HIGG5D1Jets")
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d1Seq,outputGroup="HIGG5D1Jets")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

addVRJets(higg5d1Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet", 
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", 
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000, 
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
# must re-tag AntiKt4LCTopoJets and AntiKt4PV0TrackJets to make JetFitterNN work with corresponding VR jets (nikola: why?)
# also, re-tag R=0.2 track jets
FlavorTagInit( JetCollections = ["AntiKt4PV0TrackJets", "AntiKtVR30Rmax4Rmin02TrackJets", "AntiKt2PV0TrackJets"], Sequencer = higg5d1Seq )

# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d1Seq)
# applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=higg5d1Seq)
# applyJetCalibration_OTFJets("AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=higg5d1Seq)



#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg5d1Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


higg5d1Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel_jet",
    SkimmingTools = [HIGG5D1JetSkimmingTool]
    )

higg5d1Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel",
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += higg5d1Seq


# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="HIGG5D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D1ContentList import HIGG5D1Content
from DerivationFrameworkHiggs.HIGG5D1ExtraContent import *
HIGG5D1SlimmingHelper = SlimmingHelper("HIGG5D1SlimmingHelper")

HIGG5D1SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"               :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"            :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"       :   "xAOD::BTaggingAuxContainer",
  }

HIGG5D1SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Photons",
                                           "Muons",
                                           "TauJets",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                           "AntiKt4TruthJets",
#                                            "AntiKtVR30Rmax4Rmin02Track",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt2Track",
#                                           "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
#                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                           "InDetTrackParticles",
                                           "PrimaryVertices" ]
if DerivationFrameworkIsMonteCarlo :
    HIGG5D1SlimmingHelper.SmartCollections += [
         "AntiKt4TruthJets",
         "AntiKt4TruthWZJets"
#          ,"AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets"
    ]



HIGG5D1SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D1SlimmingHelper.AllVariables = ExtraContainers
# HIGG5D1SlimmingHelper.AllVariables += ["AntiKtVR30Rmax4Rmin02TrackJets", "BTagging_AntiKtVR30Rmax4Rmin02Track"]
if DerivationFrameworkIsMonteCarlo :
    HIGG5D1SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D1SlimmingHelper.AllVariables += ExtraContainersTruth
HIGG5D1SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["HIGG5D1Jets","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]

addJetOutputs(HIGG5D1SlimmingHelper,["HIGG5D1Jets"],slimmed_content)

# Add the MET containers to the stream
addMETOutputs(HIGG5D1SlimmingHelper,[],["Track","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"])

# if not DerivationFrameworkIsMonteCarlo : # for very early data
# HIGG5D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG5D1SlimmingHelper.IncludeEGammaTriggerContent = True
# HIGG5D1SlimmingHelper.IncludeBJetTriggerContent = True
#HIGG5D1SlimmingHelper.IncludeBPhysTriggerContent = True
#HIGG5D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
HIGG5D1SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG5D1SlimmingHelper.IncludeJetTriggerContent = True

HIGG5D1SlimmingHelper.AppendContentToStream(HIGG5D1Stream)
