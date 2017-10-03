#********************************************************************
# HIGG5D2.py (for 1-lepton)
# reductionConf flag HIGG5D2 in Reco_tf.py   
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
  from DerivationFrameworkTau.TauTruthCommon import * 


# running on data or MC
from AthenaCommon.GlobalFlags import globalflags
#print "Yoshikazu Nagai test: globalflags.DataSource()"
#print globalflags.DataSource()  # this should be "data" or "geant4"
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC

if globalflags.DataSource()=='geant4':
  from DerivationFrameworkHiggs.TruthCategories import *


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D2Stream )
HIGG5D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D2Stream.AcceptAlgs(["HIGG5D2Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
HIGG5D2ThinningHelper = ThinningHelper("HIGG5D2ThinningHelper") 
#trigger navigation content
HIGG5D2ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_xe.*|HLT_j.*' 
HIGG5D2ThinningHelper.AppendToStream(HIGG5D2Stream) 


# MC truth thinning (not for data)
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top quark and Bottom quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG5D2MCThinningTool = DerivationFramework__GenericTruthThinning(
    name                         = "HIGG5D2MCThinningTool", 
    ThinningService              = HIGG5D2ThinningHelper.ThinningSvc(),
    ParticleSelectionString      = truth_expression,
    PreserveDescendants          = False,
    PreserveGeneratorDescendants = True,
    PreserveAncestors            = True)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG5D2MCThinningTool
    thinningTools.append(HIGG5D2MCThinningTool)

# MET/Jet tracks
# thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# HIGG5D2TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "HIGG5D2TPThinningTool",
#                                                                 ThinningService         = HIGG5D2ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = thinning_expression,
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
#                                                                 ApplyAnd                = True)
# ToolSvc += HIGG5D2TPThinningTool
# thinningTools.append(HIGG5D2TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG5D2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG5D2JetTPThinningTool",
                                                                ThinningService         = HIGG5D2ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D2JetTPThinningTool
thinningTools.append(HIGG5D2JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG5D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG5D2MuonTPThinningTool",
                                                                            ThinningService         = HIGG5D2ThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D2MuonTPThinningTool
thinningTools.append(HIGG5D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D2ElectronTPThinningTool",
                                                                                        ThinningService         = HIGG5D2ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        BestMatchOnly           = True)
ToolSvc += HIGG5D2ElectronTPThinningTool
thinningTools.append(HIGG5D2ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D2PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG5D2ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG5D2PhotonTPThinningTool
thinningTools.append(HIGG5D2PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG5D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                  = "HIGG5D2TauTPThinningTool",
                                                                          ThinningService         = HIGG5D2ThinningHelper.ThinningSvc(),
                                                                          TauKey                  = "TauJets",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D2TauTPThinningTool
thinningTools.append(HIGG5D2TauTPThinningTool)


# calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG5D2TauCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG5D2TauCCThinningTool",
                                                                    ThinningService       = HIGG5D2ThinningHelper.ThinningSvc(),
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += HIGG5D2TauCCThinningTool
thinningTools.append(HIGG5D2TauCCThinningTool)


#========================================================================
# lepton selection (keep isolation failed leptons for QCD-MJ estimation)
#========================================================================
lepSel = '( count( (Muons.pt > 20.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon) ) + count( (Electrons.pt > 20.0*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose) ) ) > 0'

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
#jetSel = '(( count( (AntiKt4EMTopoJets.pt > 0.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 1) || (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0) || (( count( (AntiKt10LCTopoJets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0) || (( count( (CamKt12LCTopoJets.pt > 100.0*GeV) && (abs(CamKt12LCTopoJets.eta) < 2.6) ) ) > 0)'

#====================================================================
# Trigger selection
#====================================================================
beamEnergy = jobproperties.Beam.energy()
triglist = []
if (beamEnergy < 4.1e+06): # 8 TeV, name should be EF_xxx in Rel19, HLT_xxx in Rel20
    triglist.append("HLT_e24vhi_medium1")
    triglist.append("HLT_e60_medium1")
    triglist.append("HLT_mu24i_tight")
    triglist.append("HLT_mu36_tight")
    triglist.append("HLT_xe100")
    triglist.append("HLT_xe50_j40_dphi1")
    triglist.append("HLT_xe80T_tclcw_loose")
    triglist.append("HLT_xe80_tclcw_loose")
if (beamEnergy > 6.0e+06): # 13 TeV, name should be HLT_xxx
    triglist.append("HLT_e17_lhloose_L1EM15")
    triglist.append("HLT_e24_medium_iloose_L1EM18VH")
    triglist.append("HLT_e24_medium_iloose_L1EM20VH")
    triglist.append("HLT_e24_lhmedium_iloose_L1EM20VH")
    triglist.append("HLT_e24_tight_iloose")
    triglist.append("HLT_e24_lhmedium_L1EM20VHI")
    triglist.append("HLT_e24_lhmedium_L1EM20VH")
    triglist.append("HLT_e24_lhmedium_L1EM18VH")
    triglist.append("HLT_e24_lhmedium_ivarloose")
    triglist.append("HLT_e24_lhmedium_nod0_ivarloose")
    triglist.append("HLT_e24_lhmedium_nod0_L1EM20VH")
    triglist.append("HLT_e24_lhtight_iloose")
    triglist.append("HLT_e24_lhtight_ivarloose")
    triglist.append("HLT_e24_lhtight_nod0_ivarloose")
    triglist.append("HLT_e26_tight_iloose")
    triglist.append("HLT_e26_tight1_iloose")
    triglist.append("HLT_e26_lhtight_iloose")
    triglist.append("HLT_e26_lhtight_iloose")
    triglist.append("HLT_e26_lhtight_ivarloose")
    triglist.append("HLT_e26_lhtight_nod0_ivarloose")
    triglist.append("HLT_e26_lhtight_smooth_ivarloose") # added on Aug 2016
    triglist.append("HLT_e60_medium")
    triglist.append("HLT_e60_medium1")
    triglist.append("HLT_e60_lhmedium")
    triglist.append("HLT_e60_lhmedium_nod0")
    triglist.append("HLT_e120_lhloose")
    triglist.append("HLT_e140_lhloose")
    triglist.append("HLT_e140_lhloose_nod0")
    triglist.append("HLT_e300_etcut")
    triglist.append("HLT_mu14")
    triglist.append("HLT_mu20_iloose_L1MU15")
    triglist.append("HLT_mu20_ivarloose_L1MU15")
    triglist.append("HLT_mu24_iloose_L1MU15")
    triglist.append("HLT_mu24_iloose")
    triglist.append("HLT_mu24_ivarloose")
    triglist.append("HLT_mu24_ivarloose_L1MU15")
    triglist.append("HLT_mu24_imedium")
    triglist.append("HLT_mu24_ivarmedium")
    triglist.append("HLT_mu26_imedium")
    triglist.append("HLT_mu26_ivarmedium")
    triglist.append("HLT_mu40")
    triglist.append("HLT_mu50")
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
    triglist.append("HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20")
    triglist.append("HLT_mu14_ivarloose_3j20_L1MU10_3J20")
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
#print 'DEBUG projectName=%s [%s;%s]' % (rec.projectName.get_Value(),type(rec.projectName),dir(rec.projectName))
print 'DEBUG projectName=%s [%s;%s; cond=%s]' % (rec.projectName.get_Value(),type(rec.projectName),dir(rec.projectName), rec.projectName.get_Value() in ['data17_13TeV','data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV'] )
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

    # triglist.append("L1_EM18VH")
    # triglist.append("L1_EM20VH")
    # triglist.append("L1_EM20VHI")
    # triglist.append("L1_EM22VHI")
    # triglist.append("L1_MU15")
    # triglist.append("L1_MU20")
    # triglist.append("L1_XE50")
    # triglist.append("L1_XE60")
    # triglist.append("L1_XE70")


#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D2JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D2JetSkimmingTool",
                                                                      expression = jetSel)
ToolSvc += HIGG5D2JetSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D2LepSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D2LepSkimmingTool",
                                                                      expression = lepSel)
ToolSvc += HIGG5D2LepSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HIGG5D2TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name          = "HIGG5D2TriggerSkimmingTool",
                                                                         TriggerListOR = triglist )
ToolSvc += HIGG5D2TriggerSkimmingTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg5d2Seq = CfgMgr.AthSequencer("HIGG5D2Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel_trig",
    SkimmingTools = [HIGG5D2TriggerSkimmingTool]
    )

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel_lep",
    SkimmingTools = [HIGG5D2LepSkimmingTool]
    )


#====================================================================
# Standard jets
#====================================================================
if not "HIGG5D2Jets" in OutputJets:
    OutputJets["HIGG5D2Jets"] = []

    #AntiKt2PV0TrackJets
    addStandardJets("AntiKt", 0.2, "PV0Track", 2000, mods="track_ungroomed", algseq=higg5d2Seq, outputGroup="HIGG5D2Jets")
    OutputJets["HIGG5D2Jets"].append("AntiKt2PV0TrackJets")
    #AntiKt4PV0TrackJets
    addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=higg5d2Seq, outputGroup="HIGG5D2Jets")
    OutputJets["HIGG5D2Jets"].append("AntiKt4PV0TrackJets")
    #AntiKt10LCTopoJets
    addStandardJets("AntiKt", 1.0, "LCTopo", mods="lctopo_ungroomed", ptmin=40000, ptminFilter=50000, calibOpt="none", algseq=higg5d2Seq, outputGroup="HIGG5D2Jets")
    OutputJets["HIGG5D2Jets"].append("AntiKt10LCTopoJets")

#====================================================================
# Special jets
#====================================================================
# if not "HIGG5D2Jets" in OutputJets:
    # OutputJets["HIGG5D2Jets"] = ["AntiKt2PV0TrackJets","AntiKt10LCTopoJets","CamKt12LCTopoJets"]

    if jetFlags.useTruth:
      #AntiKt4TruthJets
      addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=higg5d2Seq, outputGroup="HIGG5D2Jets")
      OutputJets["HIGG5D2Jets"].append("AntiKt4TruthJets")
      #AntiKt4TruthWZJets
      addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=higg5d2Seq, outputGroup="HIGG5D2Jets")
      OutputJets["HIGG5D2Jets"].append("AntiKt4TruthWZJets")
      # OutputJets["HIGG5D2Jets"].append("AntiKt10TruthWZJets")
      # OutputJets["HIGG5D2Jets"].append("AntiKt10TruthJets")
      # OutputJets["HIGG5D2Jets"].append("CamKt12TruthJets")
      addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d2Seq,outputGroup="HIGG5D2Jets")

    # addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, includePreTools=False, algseq=higg5d2Seq,outputGroup="HIGG5D2Jets")
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d2Seq,outputGroup="HIGG5D2Jets")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

addVRJets(higg5d2Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet", 
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
FlavorTagInit( JetCollections = ["AntiKt4PV0TrackJets", "AntiKtVR30Rmax4Rmin02TrackJets", "AntiKt2PV0TrackJets"], Sequencer = higg5d2Seq )


# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d2Seq)
# applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=higg5d2Seq)
# applyJetCalibration_OTFJets("AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=higg5d2Seq)



#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg5d2Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel_jet",
    SkimmingTools = [HIGG5D2JetSkimmingTool]
    )

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel",
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += higg5d2Seq



# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="HIGG5D2ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D2ContentList import HIGG5D2Content
from DerivationFrameworkHiggs.HIGG5D2ExtraContent import *
HIGG5D2SlimmingHelper = SlimmingHelper("HIGG5D2SlimmingHelper")

HIGG5D2SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"               :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"            :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"       :   "xAOD::BTaggingAuxContainer",
  }

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
# HIGG5D2SlimmingHelper.AllVariables += ["AntiKtVR30Rmax4Rmin02TrackJets", "BTagging_AntiKtVR30Rmax4Rmin02Track"]
if globalflags.DataSource()=='geant4':
    HIGG5D2SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D2SlimmingHelper.AllVariables += ExtraContainersTruth
HIGG5D2SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
addJetOutputs(HIGG5D2SlimmingHelper,["HIGG5D2Jets"])
# Add the MET containers to the stream
addMETOutputs(HIGG5D2SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG5D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG5D2SlimmingHelper.IncludeEGammaTriggerContent = True
#HIGG5D2SlimmingHelper.IncludeBPhysTriggerContent = True
#HIGG5D2SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
HIGG5D2SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG5D2SlimmingHelper.IncludeJetTriggerContent = True
# if globalflags.DataSource()!='geant4': # for very early data
# HIGG5D2SlimmingHelper.IncludeBJetTriggerContent = True

HIGG5D2SlimmingHelper.AppendContentToStream(HIGG5D2Stream)
