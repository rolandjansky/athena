#********************************************************************
# HIGG6D2.py (for H+ -> tau-lep)
# reductionConf flag HIGG6D2 in Reco_tf.py
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
print "is_MC = ",is_MC
print "HIGG6D2.py globalflags.DataSource()", globalflags.DataSource()
print "HIGG6D2.py jobproperties.Beam.energy()", jobproperties.Beam.energy()

if is_MC:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
  from DerivationFrameworkCore.LHE3WeightMetadata import *


 
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG6D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG6D2Stream )
HIGG6D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG6D2Stream.AcceptAlgs(["HIGG6D2Kernel"])


#======================================================================
# SKIMMING 
#=======================================================================

# tau selection - tau pt>25GeV, with Ntracks=1,3 and |q_\tau|=1 and |eta|<2.6
#====================================================================
tauSel = "(TauJets.pt > 25*GeV"\
         "&& (abs(TauJets.eta)<2.6)"\
         "&& (abs(TauJets.charge)==1)"\
         "&& ((TauJets.nTracks == 1) || (TauJets.nTracks == 3) ) )"
         
# Trigger selection 
#====================================================================
## REFERENCE: https://twiki.cern.ch/twiki/bin/view/Atlas/LowestUnprescaled
if jobproperties.Beam.energy()==6500000.0:
  # 13 TeV name should be HLT_xxx
  singleElectronTriggerRequirement = [
    "HLT_e24_lhmedium_L1EM20VH",
    "HLT_e24_lhtight_nod0_ivarloose",
    "HLT_e26_lhtight_nod0_ivarloose",
    "HLT_e26_lhtight_nod0"
    "HLT_e60_lhmedium_nod0",
    "HLT_e60_medium",
    "HLT_e60_lhmedium",
    "HLT_e120_lhloose",
    "HLT_e140_lhloose_nod0",
    "HLT_e300_etcut",
  ]
  singleMuonTriggerRequirement = [
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu60_0eta105_msonly",
    "HLT_mu24_iloose",
    "HLT_mu24_ivarloose",
    "HLT_mu40",
    "HLT_mu50",
    "HLT_mu24_ivarmedium",
    "HLT_mu24_imedium",
    "HLT_mu26_ivarmedium",
    "HLT_mu26_imedium",
  ]
  
  singleLepTrigger = " ({0})".format("||".join(singleElectronTriggerRequirement+singleMuonTriggerRequirement))
  
# lepton selection
#========================================================================
electronRequirements = "( (Electrons.pt > 25*GeV)"\
                       "&& (abs(Electrons.eta) < 2.6)"\
                       "&& (Electrons.DFCommonElectronsLHLoose))"

muonRequirements = "( (Muons.pt > 25*GeV)"\
                   "&& (abs(Muons.eta) < 2.6)"\
                   "&& (Muons.DFCommonMuonsPreselection) )"

lepSel = "( ( (count({0}) >=1)  || (count({1}) >=1)))".format(electronRequirements, muonRequirements)
lepSel2 = "(( count({0}) + count({1}) ) >=2 )".format(electronRequirements, muonRequirements)


# jet selection: jets with pt>20 and |eta|<2.6, nJet>=1 + nEl + nTau
#====================================================================

jetSelEM = "count(({0}.pt > 20.0*GeV) && (abs({0}.eta) < 2.6))"\
           " >= (1 + count({1}) + count({2}))".format(
             "AntiKt4EMTopoJets", electronRequirements, tauSel)

jetSelEMCalib = "count(({0}.DFCommonJets_Calib_pt > 20.0*GeV)&& (abs({0}.DFCommonJets_Calib_eta) < 2.6))"\
                " >= (1 + count({1}) + count({2}) )".format(
                  "AntiKt4EMTopoJets", electronRequirements, tauSel)

jetSel = "({0}) || ({1})".format(jetSelEM, jetSelEMCalib)


# - - - - Skimming Tool 
expression = "({0}) &&  ( ( ({1}) && (count({2}) >= 1) ) || ({3}) )  && ({4})".format(
  jetSel, lepSel, tauSel, lepSel2, singleLepTrigger)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG6D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(
  name="HIGG6D2SkimmingTool1",
  expression=expression)

ToolSvc += HIGG6D2SkimmingTool
print "HIGG6D2 SKIMMING TOOL: ", HIGG6D2SkimmingTool
print "HIGG6D2 SKIMMING TOOL EXPRESSION:", expression


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG6D2ThinningHelper = ThinningHelper( "HIGG6D2ThinningHelper" )

# - - - - trigger Thinning Tool
HIGG6D2ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_tau.*|HLT_xe.*'
HIGG6D2ThinningHelper.AppendToStream(HIGG6D2Stream)


# GenericTrackParticleThinning
#====================================================================
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV)"\
                      "&& (InDetTrackParticles.numberOfPixelHits > 0)"\
                      "&& (InDetTrackParticles.numberOfSCTHits > 5)"\
                      "&& (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG6D2GenTPThinningTool = DerivationFramework__TrackParticleThinning(
  name="HIGG6D2GenTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SelectionString=thinning_expression,
  InDetTrackParticlesKey ="InDetTrackParticles",
  ApplyAnd=True
)

thinningTools.append(HIGG6D2GenTPThinningTool)
ToolSvc += HIGG6D2GenTPThinningTool

# Tracks themselves
#====================================================================
HIGG6D2TPThinningTool = DerivationFramework__TrackParticleThinning(
  name="HIGG6D2TPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SelectionString="abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
  InDetTrackParticlesKey ="InDetTrackParticles"
)

thinningTools.append(HIGG6D2TPThinningTool)
ToolSvc += HIGG6D2TPThinningTool


# EMTopoJetTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG6D2EMTopoJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name="HIGG6D2EMTopoJetTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  JetKey="AntiKt4EMTopoJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  ApplyAnd=True)

thinningTools.append(HIGG6D2EMTopoJetTPThinningTool)
ToolSvc += HIGG6D2EMTopoJetTPThinningTool


# LCTopoJetTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG6D2LCTopoJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name="HIGG6D2LCTopoJetTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  JetKey="AntiKt4LCTopoJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  ApplyAnd=True)

thinningTools.append(HIGG6D2LCTopoJetTPThinningTool)
ToolSvc += HIGG6D2LCTopoJetTPThinningTool


# TauTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
tauThinningRequirements = "(TauJets.pt > 10*GeV"\
                          "&& (abs(TauJets.eta)<2.6)"\
                          "&& (abs(TauJets.charge)==1 || abs(TauJets.charge)==3)"\
                          "&& ((TauJets.nTracks == 1) || (TauJets.nTracks == 3)) )"
HIGG6D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
  name= "HIGG6D2TauTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  TauKey="TauJets",
  InDetTrackParticlesKey="InDetTrackParticles",
  SelectionString="(TauJets.pt > 20*GeV)",
  ConeSize = 0.6,
  ApplyAnd = False)

thinningTools.append(HIGG6D2TauTPThinningTool)
ToolSvc += HIGG6D2TauTPThinningTool


# MuonTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
muonThinningRequirements = "( (Muons.pt > 5*GeV)"\
                           "&& (abs(Muons.eta) < 2.6)"\
                           "&& (Muons.DFCommonGoodMuon) )"
HIGG6D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
  name="HIGG6D2MuonTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  MuonKey= "Muons",
  InDetTrackParticlesKey="InDetTrackParticles",
  SelectionString="(Muons.pt > 5*GeV)",
  ConeSize=0.4,
  ApplyAnd=False)

thinningTools.append(HIGG6D2MuonTPThinningTool)
ToolSvc += HIGG6D2MuonTPThinningTool

# EgammaTrackParticleThinning
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
electronThinningRequirements = "( (Electrons.pt > 10*GeV)"\
                               "&& (abs(Electrons.eta) < 2.6)"\
                               "&& (Electrons.DFCommonElectronsLHLoose))"
HIGG6D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name= "HIGG6D2ElectronTPThinningTool",
  ThinningService=HIGG6D2ThinningHelper.ThinningSvc(),
  SGKey= "Electrons",
  InDetTrackParticlesKey="InDetTrackParticles",
  SelectionString="(Electrons.pt > 10*GeV)",
  ConeSize=0.4,
  ApplyAnd=False)

thinningTools.append(HIGG6D2ElectronTPThinningTool)
ToolSvc += HIGG6D2ElectronTPThinningTool


# Truth particles thinning
#====================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
HIGG6D2TruthThinningTool = DerivationFramework__MenuTruthThinning(
  name="HIGG6D2TruthThinningTool",
  ThinningService="HIGG6D2ThinningSvc",
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
    ToolSvc += HIGG6D2TruthThinningTool
    thinningTools.append(HIGG6D2TruthThinningTool)


#===========================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#===========================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "HIGG6D2Kernel",
  SkimmingTools=[HIGG6D2SkimmingTool],
  ThinningTools=thinningTools
)

#====================================================================
# Add the containers to the output stream - slimming done here (smart slimming)
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG6D2SlimmingHelper = SlimmingHelper("HIGG6D2SlimmingHelper")

## Smart Slimming
HIGG6D2SlimmingHelper.SmartCollections = [
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
  "InDetTrackParticles",
  "PrimaryVertices",
]

if is_MC:
  HIGG6D2SlimmingHelper.SmartCollections += ["AntiKt4TruthJets"]
  
## Add extra variables
HIGG6D2SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta"]

HIGG6D2SlimmingHelper.ExtraVariables += [
  "AntiKt4EMPFlowJets_BTagging201903.MV2cl100_discriminant.MV2c10rnn_discriminant.MV2c10_discriminant.MV2c10mu_discriminant"\
  ".DL1_pb.DL1_pc.DL1_pu.DL1mu_pb.DL1mu_pc.DL1mu_pu.DL1rnn_pb.DL1rnn_pc.DL1rnn_pu"\
  ]

HIGG6D2SlimmingHelper.ExtraVariables += [
  "TauJets.BDTEleScore.pantau_CellBasedInput_isPanTauCandidate.pantau_CellBasedInput_DecayMode"\
  ".ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased."\
  "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n.pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn.pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn",
  "TauNeutralParticleFlowObjects.pt.eta.phi.m.rapidity.bdtPi0Score"]

HIGG6D2SlimmingHelper.ExtraVariables += [
  "Electrons.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium."\
  "DFCommonElectronsLHTight.DFCommonElectronsML.author.OQ.charge.LHLoose.LHMedium.LHTight.LHValue"
]

HIGG6D2SlimmingHelper.ExtraVariables += [
  "Muons.DFCommonGoodMuons",
  "CombinedMuonTrackParticles.d0.z0.vz",
  "InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers"
]

HIGG6D2SlimmingHelper.ExtraVariables += ["PrimaryVertices.x.y.z.vertexType"]

HIGG6D2SlimmingHelper.AllVariables = [
  "TauChargedParticleFlowObjects"
]

if is_MC:
  HIGG6D2SlimmingHelper.StaticContent = [
    "xAOD::TruthParticleContainer#TruthMuons",
    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
    "xAOD::TruthParticleContainer#TruthElectrons",
    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
    "xAOD::TruthParticleContainer#TruthPhotons",
    "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
    "xAOD::TruthParticleContainer#TruthNeutrinos",
    "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux."
  ]  
  HIGG6D2SlimmingHelper.AllVariables = [
    "TruthParticles",
    "TruthEvents",
    "MET_Truth",
    "METMap_Truth",
    "MET_Track",
    "TruthVertices",
    "TauChargedParticleFlowObjects",
  ]
  HIGG6D2SlimmingHelper.ExtraVariables += ["AntiKt4LCTopoJets.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T"]
  HIGG6D2SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T"]
  HIGG6D2SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink","MuonTruthParticles.truthOrigin.truthType"]
  HIGG6D2SlimmingHelper.ExtraVariables += ["TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]

HIGG6D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG6D2SlimmingHelper.IncludeEGammaTriggerContent = True
HIGG6D2SlimmingHelper.IncludeTauTriggerContent = True
HIGG6D2SlimmingHelper.IncludeEtMissTriggerContent = True

HIGG6D2SlimmingHelper.AppendContentToStream(HIGG6D2Stream)
