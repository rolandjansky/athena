#********************************************************************
# HIGG6D1.py (for H+ -> tau-jet)
# reductionConf flag HIGG6D1 in Reco_tf.py
# author: sina.bahrasemani@cern.ch, elliot.wesley.parrish@cern.ch
#********************************************************************

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#Include common variables from CP groups
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from DerivationFrameworkHiggs.TruthCategories import *

from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo")

import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

# testing globalflags
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ", is_MC
print "HIGG6D1.py globalflags.DataSource()", globalflags.DataSource()
print "HIGG6D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy()

if is_MC:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
  from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# jet selection - jets with pt>20 and |eta|<2.5
#====================================================================
jetSelEM = ' count ((AntiKt4EMTopoJets.pt > 20.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6)) >= 4'
jetSelEMCalib = ' count ((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20.0*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.6)) >= 4'
jetSel = '( '+ jetSelEM + ' ) || ( '+ jetSelEMCalib + ' )'

#=============================================================================================
# tau selection - tau pt>25GeV, with Ntracks=1,3 and |q_\tau|=1 and |eta|<2.6
#=============================================================================================
tauSel = "(TauJets.pt > 25*GeV && (abs(TauJets.eta)<2.6)"\
         "&& (abs(TauJets.charge)==1)"\
         "&& ((TauJets.nTracks == 1) || (TauJets.nTracks == 3) ) )"

#====================================================================
# Trigger selection 
#====================================================================
## REFERENCE: https://twiki.cern.ch/twiki/bin/view/Atlas/LowestUnprescaled 
TRIGGERS = [
  ## tau+MET 2015/2016/2017
  "HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45",
  "HLT_tau35_medium1_tracktwo_xe70_L1XE45",

  ## tau+MET 2018
  "HLT_tau35_medium1_tracktwoEF_xe70_L1XE45",
  "HLT_tau35_medium1_tracktwoEF_xe70_L1TAU20IM_2J20_XE45",
  "HLT_tau35_mediumRNN_tracktwoMVA_xe70_L1XE45",
  "HLT_tau35_mediumRNN_tracktwoMVA_xe70_L1TAU20IM_2J20_XE45",

  ## MET 2015
  "HLT_xe70",
  "HLT_xe70_tc_lcw",
  "HLT_xe70_mht",

  ## MET 2016
  "HLT_xe80_tc_lcw_L1XE50", 
  "HLT_xe90_mht_L1XE50 ",
  "HLT_xe90_tc_lcw_wEFMu_L1XE50",
  "HLT_xe90_mht_wEFMu_L1XE50", 
  "HLT_xe100_L1XE50", 
  "HLT_xe100_tc_em_L1XE50", 
  "HLT_xe110_pueta_L1XE50",
  "HLT_xe110_mht_L1XE50",
  "HLT_xe120_pueta", 
  "HLT_xe120_pufit",
  "HLT_xe120_tc_lcw_L1XE50", 

  ## MET 2017
  "HLT_xe90_pufit_L1XE50",
  "HLT_xe100_pufit_L1XE55", 
  "HLT_xe100_pufit_L1XE50", 
  "HLT_xe110_pufit_L1XE55",
  "HLT_xe110_pufit_L1XE50",

  ## MET 2018
  "HLT_xe110_pufit_xe65_L1XE50",
  "HLT_xe110_pufit_xe70_L1XE50",
  "HLT_xe120_pufit_L1XE50",

  ## multijt 2015
  "HLT_5j60",

  ## multijet 2016
  "HLT_3j175", 
  "HLT_4j85", 
  "HLT_5j55",
  "HLT_5j70_L14J15",

  ## multijet 2017
  "HLT_noalg_L1J400",
  "HLT_3j225", 
  "HLT_4j100",
  "HLT_4j60_gsc115_boffperf_split",
  "HLT_5j50_gsc70_boffperf_split",
  "HLT_5j50_gsc70_boffperf_split_L14J150ETA25",
  "HLT_5j90",
  "HLT_5j90_L14J150ETA25",
  "HLT_5j60_gsc90_boffperf_split",
  "HLT_5j60_gsc90_boffperf_split_L14J150ETA25",
  "HLT_5j75_0eta240",
  "HLT_5j85_L14J15",
  "HLT_5j70_0eta240_L14J15",
  
  ## multijet 2018
  "HLT_3j200",
  "HLT_4j120",
  "HLT_4j85_gsc115_boffperf_split",
  "HLT_5j50_gsc70_boffperf_split_0eta240_L14J15",
  "HLT_5j60_gsc85_boffperf_split_L14J15",
  "HLT_5j70_0eta240_L14J15",
  "HLT_5j85_L14J15",
  
] #<! end TRIGGERS  

if jobproperties.Beam.energy()==6500000.0:
  # 13 TeV name should be HLT_xxx
  triggerSel = "||".join(TRIGGERS)
  
elif jobproperties.Beam.energy()==4000000.0:
  triggerSel = '(tau29T_medium1_xe40_tight || tau29T_medium1_xe50tclcw_tight || tau27T_medium1_L2loose_xe50_tclcw_tight)'
 
  
#========================================================================
# lepton selection
#========================================================================
electronRequirements = '( (Electrons.pt > 25*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.Loose || Electrons.DFCommonElectronsLHLoose) )'
muonRequirements = '( (Muons.pt > 25.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon) )'

#====================================================================
# SKIMMING TOOL 
#====================================================================
# keep trigger on only for the data for trigger efficiencies
if is_MC:
  expression = '( '+ jetSel + ' ) && (count( ' + tauSel +  ' ) >= 1 )'
else:
  expression = '( '+ jetSel + ' ) && (count( ' + tauSel +  ' ) >= 1) && ( ' + triggerSel +  ' )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG6D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name="HIGG6D1SkimmingTool1", expression = expression)
ToolSvc += HIGG6D1SkimmingTool

#====================================================================
# THINNING TOOLS
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
thinningTools=[]

#====================================================================
# GenericTrackParticleThinning
#====================================================================
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV)"\
                      "&& (InDetTrackParticles.numberOfPixelHits > 0)"\
                      "&& (InDetTrackParticles.numberOfSCTHits > 5)"\
                      "&& (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"

HIGG6D1GenTPThinningTool = DerivationFramework__TrackParticleThinning(
  name="HIGG6D1GenTPThinningTool",
  ThinningService="HIGG6D1ThinningSvc",
  SelectionString=thinning_expression,
  InDetTrackParticlesKey="InDetTrackParticles",
  ApplyAnd=True)
thinningTools.append(HIGG6D1GenTPThinningTool)
ToolSvc += HIGG6D1GenTPThinningTool
                                                                   
#====================================================================
# JetTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG6D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name="HIGG6D1JetTPThinningTool",
  ThinningService="HIGG6D1ThinningSvc",
  JetKey="AntiKt4EMTopoJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  ApplyAnd=True)

thinningTools.append(HIGG6D1JetTPThinningTool)
ToolSvc += HIGG6D1JetTPThinningTool

#====================================================================
# TauTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG6D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
  name="HIGG6D1TauTPThinningTool",
  ThinningService="HIGG6D1ThinningSvc",
  TauKey="TauJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  # SelectionString=tauSel,
  ConeSize=0.6,
  ApplyAnd=False)

thinningTools.append(HIGG6D1TauTPThinningTool)
ToolSvc += HIGG6D1TauTPThinningTool

#====================================================================
# MuonTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG6D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
  name="HIGG6D1MuonTPThinningTool",
  ThinningService="HIGG6D1ThinningSvc",
  MuonKey="Muons",
  InDetTrackParticlesKey="InDetTrackParticles",
  # SelectionString=muonRequirements,
  ConeSize=0.4,
  ApplyAnd=False)

thinningTools.append(HIGG6D1MuonTPThinningTool)
ToolSvc += HIGG6D1MuonTPThinningTool

#====================================================================
# EgammaTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG6D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name="HIGG6D1ElectronTPThinningTool",
  ThinningService= "HIGG6D1ThinningSvc",
  SGKey= "Electrons",
  InDetTrackParticlesKey="InDetTrackParticles",
  # SelectionString=electronRequirements,
  ConeSize=0.4,
  ApplyAnd=False)

thinningTools.append(HIGG6D1ElectronTPThinningTool)
ToolSvc += HIGG6D1ElectronTPThinningTool

#====================================================================
# Tracks themselves
#====================================================================
HIGG6D1TPThinningTool = DerivationFramework__TrackParticleThinning(
  name="HIGG6D1TPThinningTool",
  ThinningService="HIGG6D1ThinningSvc",
  SelectionString="abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
  InDetTrackParticlesKey="InDetTrackParticles")

thinningTools.append(HIGG6D1TPThinningTool)
ToolSvc += HIGG6D1TPThinningTool

#====================================================================
# Truth particles thinning
#====================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
HIGG6D1TruthThinningTool = DerivationFramework__MenuTruthThinning(
  name="HIGG6D1TruthThinningTool",
  ThinningService="HIGG6D1ThinningSvc",
  WritePartons=False,
  WriteHadrons=False,
  WriteBHadrons=True,
  WriteGeant=False,
  GeantPhotonPtThresh=-1.0,
  WriteTauHad=True,
  PartonPtThresh=-1.0,
  WriteBSM=True,
  WriteBosons=True,
  WriteBSMProducts=True,
  WriteTopAndDecays=True,
  WriteEverything=False,
  WriteAllLeptons=False,
  WriteLeptonsNotFromHadrons=True,
  WriteStatus3=True,
  WriteFirstN=-1,
  PreserveGeneratorDescendants=True,
  PreserveDescendants=False)
if is_MC:
    ToolSvc += HIGG6D1TruthThinningTool
    thinningTools.append(HIGG6D1TruthThinningTool)

    
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "HIGG6D1Kernel",
  SkimmingTools=[HIGG6D1SkimmingTool],
  ThinningTools=thinningTools
)

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit 
higg6d1Seq = CfgMgr.AthSequencer("HIGG6D1Sequence")
DerivationFrameworkJob += higg6d1Seq 

FlavorTagInit(JetCollections  = [ 'AntiKt4EMTopoJets' ], Sequencer =higg6d1Seq)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG6D1Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_HIGG6D1Stream )
HIGG6D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG6D1Stream.AcceptAlgs(["HIGG6D1Kernel"])

## Special lines for thinning
## Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG6D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here (smart slimming)
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG6D1SlimmingHelper = SlimmingHelper("HIGG6D1SlimmingHelper")

## Smart Slimming
HIGG6D1SlimmingHelper.SmartCollections = [
  "Electrons",
  "Muons",
  "TauJets",
  "MET_Reference_AntiKt4LCTopo",
  "MET_Reference_AntiKt4EMTopo",
  "AntiKt4LCTopoJets",
  "AntiKt4EMTopoJets",
  "TauMVATESJets",
  "AntiKt4EMPFlowJets_BTagging201810",
  "AntiKt4EMPFlowJets_BTagging201903",
  "AntiKt4EMTopoJets_BTagging201810", 
  "BTagging_AntiKt4EMTopo_201810", 
  "BTagging_AntiKt4EMPFlow_201810",  
  "BTagging_AntiKt4EMPFlow_201903",
  "InDetTrackParticles",
  "PrimaryVertices"
]

if is_MC:
  HIGG6D1SlimmingHelper.SmartCollections += ["AntiKt4TruthJets"]

## Add extra variables
HIGG6D1SlimmingHelper.ExtraVariables += [
  "AntiKt4EMTopoJets.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta"]

HIGG6D1SlimmingHelper.ExtraVariables += [
  "AntiKt4EMPFlowJets_BTagging201903.MV2cl100_discriminant.MV2c10rnn_discriminant.MV2c10_discriminant.MV2c10mu_discriminant"\
  ".DL1_pb.DL1_pc.DL1_pu.DL1mu_pb.DL1mu_pc.DL1mu_pu.DL1rnn_pb.DL1rnn_pc.DL1rnn_pu"\
  ]

HIGG6D1SlimmingHelper.ExtraVariables += [
  "TauJets.BDTEleScore.pantau_CellBasedInput_isPanTauCandidate.pantau_CellBasedInput_DecayMode.ptPanTauCellBased"\
  ".etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased.pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n"\
  ".pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn.pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn",
  "TauNeutralParticleFlowObjects.pt.eta.phi.m.rapidity.bdtPi0Score"]

HIGG6D1SlimmingHelper.ExtraVariables += [
  "Electrons.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium.DFCommonElectronsLHTight"\
  ".DFCommonElectronsML.author.OQ.charge.LHLoose.LHMedium.LHTight.LHValue"]

HIGG6D1SlimmingHelper.ExtraVariables += [
  "Muons.DFCommonGoodMuons",
  "CombinedMuonTrackParticles.d0.z0.vz",
  "InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers"]

HIGG6D1SlimmingHelper.ExtraVariables += [
  "PrimaryVertices.x.y.z.vertexType"]

HIGG6D1SlimmingHelper.AllVariables = [
  "TauChargedParticleFlowObjects"]

if is_MC:
  HIGG6D1SlimmingHelper.StaticContent = [
    "xAOD::TruthParticleContainer#TruthMuons",
    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
    "xAOD::TruthParticleContainer#TruthElectrons",
    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
    "xAOD::TruthParticleContainer#TruthPhotons",
    "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
    "xAOD::TruthParticleContainer#TruthNeutrinos",
    "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux."]
  
  HIGG6D1SlimmingHelper.AllVariables = [
    "TruthParticles",
    "TruthEvents",
    "MET_Truth",
    "METMap_Truth",
    "TruthVertices",
    "TauChargedParticleFlowObjects"
  ]
  
  #HIGG6D1SlimmingHelper.ExtraVariables += [
  #  "AntiKt4LCTopoJets.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T"]
  
  HIGG6D1SlimmingHelper.ExtraVariables += [
    "AntiKt4EMTopoJets.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T"]
  
  HIGG6D1SlimmingHelper.ExtraVariables += [
    "Electrons.truthOrigin.truthType.truthParticleLink","MuonTruthParticles.truthOrigin.truthType"]
  
  HIGG6D1SlimmingHelper.ExtraVariables += [
    "TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]
  
## Add MET_RefFinalFix
addMETOutputs(HIGG6D1SlimmingHelper,["AntiKt4LCTopo","AntiKt4EMTopo","Track"])

## HIGG6D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
## JetTauEtMissTriggerContent: now splitted in 4 slices, use only Tau and MET
HIGG6D1SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG6D1SlimmingHelper.IncludeTauTriggerContent = True
HIGG6D1SlimmingHelper.IncludeJetTriggerContent = True

HIGG6D1SlimmingHelper.AppendContentToStream(HIGG6D1Stream)
