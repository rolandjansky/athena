#********************************************************************
# HIGG5D1.py (for 0-lepton)
# reductionConf flag HIGG5D1 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
# from DerivationFrameworkJetEtMiss.JetMomentFix import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *


# running on data or MC
from AthenaCommon.GlobalFlags import globalflags
#print "Yoshikazu Nagai test: globalflags.DataSource()"
#print globalflags.DataSource()  # this should be "data" or "geant4"
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC


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

if globalflags.DataSource()=='geant4':
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



#====================================================================
# jet selection 
#====================================================================
#jetSel = '(( count( (AntiKt4EMTopoJets.pt > 0.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 1)'
jetSel = '(( count( (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoJets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (CamKt12LCTopoJets.pt > 100.0*GeV) && (abs(CamKt12LCTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'
# jetSel += '|| (( count( (CamKt10LCTopoPrunedR50Z15Jets.pt > 100.0*GeV) && (abs(CamKt10LCTopoPrunedR50Z15Jets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (CamKt12LCTopoBDRSFilteredMU100Y15Jets.pt > 100.0*GeV) && (abs(CamKt12LCTopoBDRSFilteredMU100Y15Jets.eta) < 2.6) ) ) > 0)'

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


# Then apply the TruthWZ fix
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(higg5d1Seq,'HIGG5D1')
    replaceBuggyAntiKt10TruthWZJets(higg5d1Seq,'HIGG5D1')

#====================================================================
# Special jets
#====================================================================
if not "HIGG5D1Jets" in OutputJets:
    OutputJets["HIGG5D1Jets"] = ["AntiKt3PV0TrackJets","AntiKt2PV0TrackJets","AntiKt10LCTopoJets","CamKt12LCTopoJets"]

    if jetFlags.useTruth:
        OutputJets["HIGG5D1Jets"].append("AntiKt4TruthJets")
        OutputJets["HIGG5D1Jets"].append("AntiKt4TruthWZJets")
        # OutputJets["HIGG5D1Jets"].append("AntiKt10TruthWZJets")
        # OutputJets["HIGG5D1Jets"].append("AntiKt10TruthJets")
        #OutputJets["HIGG5D1Jets"].append("CamKt12TruthJets")
        addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d1Seq,outputGroup="HIGG5D1Jets")

    addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, includePreTools=False, algseq=higg5d1Seq,outputGroup="HIGG5D1Jets")
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d1Seq,outputGroup="HIGG5D1Jets")

# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d1Seq)
applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=higg5d1Seq)


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
addJetOutputs(HIGG5D1SlimmingHelper,["HIGG5D1Jets"])
# Add the MET containers to the stream
addMETOutputs(HIGG5D1SlimmingHelper,["AntiKt4LCTopo","Track"])

# if globalflags.DataSource()!='geant4': # for very early data
# HIGG5D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG5D1SlimmingHelper.IncludeEGammaTriggerContent = True
# HIGG5D1SlimmingHelper.IncludeBJetTriggerContent = True
#HIGG5D1SlimmingHelper.IncludeBPhysTriggerContent = True
#HIGG5D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
HIGG5D1SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG5D1SlimmingHelper.IncludeJetTriggerContent = True

HIGG5D1SlimmingHelper.AppendContentToStream(HIGG5D1Stream)
