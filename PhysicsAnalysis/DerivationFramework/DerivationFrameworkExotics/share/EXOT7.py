#********************************************************************
# EXOT7.py 
# reductionConf flag EXOT7 in Reco_tf.py
#********************************************************************

exot7Seq = CfgMgr.AthSequencer("EXOT7Sequence")

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *

from DerivationFrameworkCore.WeightMetadata import *

from JetRec.JetRecStandard import jtm

# old subjet maker
#from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetMakerTool
#jtm += SubjetMakerTool("subjetmaker", R=0.2, type="Kt", PtCut=10000.)
#jtm.modifiersMap["calib"] += [jtm.subjetmaker]

# to be changed to the new one later *after import of ExtendedJetCommon*:
#from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetFinderTool
#ToolSvc += SubjetFinderTool("subjetfinder")
#subjetfinder = ToolSvc.subjetfinder
#from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetRecorderTool
#ToolSvc += SubjetRecorderTool("subjetrecorder")
#subjetrecorder = ToolSvc.subjetrecorder
#subjetfinder.JetAlgorithm = "Kt"
#subjetfinder.JetRadius = 0.2
#subjetrecorder.SubjetLabel = "Kt2Subjets"
#subjetrecorder.SubjetContainerName = "AntiKt10TrimmedPtFrac5SmallR20Jets_Kt2Subjets"
#subjetfinder.SubjetRecorder = subjetrecorder
#jtm.modifiersMap["calib"] += [subjetfinder]
#exot7Seq.jetalgAntiKt10LCTopoTrimmedPtFrac5SmallR20.Tools[0].JetModifiers += [subjetfinder] 

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
# changed to a local version so we can update the eta range to 2.8
from DerivationFrameworkMuons.MuonsCommon import *
#from DerivationFrameworkExotics.EXOTMuonsCommon import *


#from JetRec.JetRecConf import JetDumper
#ToolSvc += JetDumper("jdmp")
#jdmp = ToolSvc.jdmp
#jdmp.ContainerName = "AntiKt10TrimmedPtFrac5SmallR20Jets"
#jdmp.AssociatedParticleVectors += ["Kt2Subjets"]
#jdmp.MaxObject = 500
#exot7Seq.jetalgAntiKt10TrimmedPtFrac5SmallR20Jets.Tools += [jdmp]
#ToolSvc += JetDumper("jdmp3")
#jdmp3 = ToolSvc.jdmp3
#jdmp3.ContainerName = "AntiKt10TrimmedPtFrac5SmallR20Jets_Kt2Subjets"
#jdmp3.Detail = 2
#jdmp3.ElementLinkMoments += ["Parent"]
#jetalg.Tools += [jdmp3]



# add C/A jets for HTT

from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

from DerivationFrameworkExotics.JetDefinitions import *
#addDefaultTrimmedJets(exot7Seq, "EXOT7")
if globalflags.DataSource()=='geant4':
    addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, algseq=exot7Seq, outputGroup="EXOT7", mods="exotJetMods")
addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, algseq=exot7Seq, outputGroup="EXOT7", mods="exotJetMods")

if isMC:
  addStandardJets("CamKt", 1.5, "Truth", mods = "truth", algseq = exot7Seq, outputGroup = "EXOT7")
  addStandardJets("CamKt", 0.8, "Truth", mods = "truth", algseq = exot7Seq, outputGroup = "EXOT7")

addStandardJets("CamKt", 1.5, "PV0Track", mods = "track_ungroomed", algseq = exot7Seq, outputGroup = "EXOT7")
addStandardJets("CamKt", 0.8, "PV0Track", mods = "track_ungroomed", algseq = exot7Seq, outputGroup = "EXOT7")

addStandardJets("CamKt", 1.5, "LCTopo", mods = "calib", algseq = exot7Seq, outputGroup = "EXOT7")
addStandardJets("CamKt", 0.8, "LCTopo", mods = "calib", algseq = exot7Seq, outputGroup = "EXOT7")

applyJetCalibration_xAODColl("AntiKt4EMTopo", exot7Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot7Seq)

#=======================================
# SKIMMING   
#=======================================

#====================================================================
# OPENING ANGLE TOOL
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
EXOT7DeltaRTool = DerivationFramework__DeltaRTool( name = "EXOT7DeltaRTool",
                                                   StoreGateEntryName = "dRkt4kt10",
                                                   ObjectRequirements = "(AntiKt4LCTopoJets.pt > 300*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.7)",
                                                   SecondObjectRequirements = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 300*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)",
                                                   ContainerName = "AntiKt4LCTopoJets",
                                                   SecondContainerName = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                 )
ToolSvc += EXOT7DeltaRTool

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression  = '((((count((AntiKt4LCTopoJets.pt > 300*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.7)) >= 1)) && (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>300*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)) >0 ) && (count(dRkt4kt10 > 1.0) >= 1))'
    expression += ' || ( ( ( (count((CamKt15LCTopoJets.pt > 400*GeV) && (abs(CamKt15LCTopoJets.eta) < 2.7)) >= 1) && (count((CamKt15LCTopoJets.pt > 150*GeV) && (abs(CamKt15LCTopoJets.eta) < 2.7)) >= 2) )'
    expression += ' || ( (count((CamKt8LCTopoJets.pt > 400*GeV) && (abs(CamKt8LCTopoJets.eta) < 2.7)) >= 1) && (count((CamKt8LCTopoJets.pt > 150*GeV) && (abs(CamKt8LCTopoJets.eta) < 2.7)) >= 2) ) )'
    # AND of lepton veto for tt res. all had (note the lepton p_T cuts are higher, to make the veto softer and allow for calibrations)
    expression += ' && !(((count(Electrons.Tight && Electrons.pt > 30*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( count(Muons.DFCommonGoodMuon && Muons.pt > 30*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0))) ))'
if (beamEnergy > 6.0e+06):
    expression  = '((((count((AntiKt4LCTopoJets.pt > 300*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.7)) >= 1)) && (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>300*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)) >0 ) && (count(dRkt4kt10 > 1.0) >= 1))'
    # for ZinvThad/monotop (begin)
    expression += ' || ( (count(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>250*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)) >= 1 ) && (HLT_xe70 || HLT_j80_xe80) )' 
    expression += ' || ( (count(AntiKt4LCTopoJets.pt>45*GeV && (abs(AntiKt4LCTopoJets.eta)<2.7)) >= 2 ) && (HLT_xe70 || HLT_j80_xe80) )'
    # for ZinvThad/monotop (end)
    expression += ' || ( ( ( (count((CamKt15LCTopoJets.pt > 400*GeV) && (abs(CamKt15LCTopoJets.eta) < 2.7)) >= 1) && (count((CamKt15LCTopoJets.pt > 150*GeV) && (abs(CamKt15LCTopoJets.eta) < 2.7)) >= 2) )'
    expression += ' || ( (count((CamKt8LCTopoJets.pt > 400*GeV) && (abs(CamKt8LCTopoJets.eta) < 2.7)) >= 1) && (count((CamKt8LCTopoJets.pt > 150*GeV) && (abs(CamKt8LCTopoJets.eta) < 2.7)) >= 2) ) )'
    # AND of lepton veto for tt res. all had (note the lepton p_T cuts are higher, to make the veto softer and allow for calibrations)
    expression += ' && !(((count(Electrons.Tight && Electrons.pt > 30*GeV && Electrons.eta > -2.4 && Electrons.eta < 2.4)>0)) || (( count(Muons.DFCommonGoodMuon && Muons.pt > 30*GeV && Muons.eta > -2.4 && Muons.eta < 2.4)>0))) ))'
    #expression += ' && !(((HLT_e28_tight_iloose || HLT_e60_medium) && (count(Electrons.Tight && Electrons.pt > 30*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( (HLT_mu26_imedium || HLT_mu50) && count(Muons.DFCommonGoodMuon && Muons.pt > 30*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0))) ))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT7StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT7StringSkimmingTool",
                                                                         expression = expression)

ToolSvc += EXOT7StringSkimmingTool
print EXOT7StringSkimmingTool

# define thinning tool 
thinningTools=[]
# Tracks associated with jets (for e-in-jet OR)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT7JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT7JetTPThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       JetKey                  = "AntiKt4LCTopoJets",
                                                                       SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && AntiKt4LCTopoJets.eta > -2.8 && AntiKt4LCTopoJets.eta < 2.8",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT7JetTPThinningTool
thinningTools.append(EXOT7JetTPThinningTool)

# for HTT
EXOT7CA8JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT7CA8JetTPThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       JetKey                  = "CamKt8LCTopoJets",
                                                                       SelectionString         = "CamKt8LCTopoJets.pt > 150*GeV && CamKt8LCTopoJets.eta > -2.7 && CamKt8LCTopoJets.eta < 2.7",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT7CA8JetTPThinningTool
thinningTools.append(EXOT7CA8JetTPThinningTool)

EXOT7CA15JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT7CA15JetTPThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       JetKey                  = "CamKt15LCTopoJets",
                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV && CamKt15LCTopoJets.eta > -2.7 && CamKt15LCTopoJets.eta < 2.7",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT7CA15JetTPThinningTool
thinningTools.append(EXOT7CA15JetTPThinningTool)

EXOT7Ak10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT7Ak10JetTPThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       JetKey                  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                       SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta > -2.7 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta < 2.7",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT7CA15JetTPThinningTool
thinningTools.append(EXOT7CA15JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT7MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT7MuonTPThinningTool",
                                                                         ThinningService         = "EXOT7ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                         SelectionString = "Muons.pt > 7*GeV",
                                                                         ConeSize=0.4)
ToolSvc += EXOT7MuonTPThinningTool
thinningTools.append(EXOT7MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT7ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT7ElectronTPThinningTool",
                                                                               ThinningService         = "EXOT7ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                               SelectionString         = "Electrons.pt > 7*GeV",
                                                                               ConeSize = 0.4
                                                                              )
ToolSvc += EXOT7ElectronTPThinningTool
thinningTools.append(EXOT7ElectronTPThinningTool)

# calo cluster thinning for electrons only (only affects egClusterCollection)
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT7ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name                    = "EXOT7ElectronCCThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       SGKey                   = "Electrons",
                                                                       CaloClCollectionSGKey   = "egammaClusters",
                                                                       #TopoClCollectionSGKey   = "CaloCalTopoCluster",
                                                                       SelectionString         = "Electrons.pt > 7*GeV",
                                                                       #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                       ConeSize                = 0.4)
ToolSvc += EXOT7ElectronCCThinningTool
thinningTools.append(EXOT7ElectronCCThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT7CA15CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT7CA15CCThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       SGKey                   = "CamKt15LCTopoJets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT7CA15CCThinningTool
thinningTools.append(EXOT7CA15CCThinningTool)

EXOT7CA8CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT7CA8CCThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       SGKey                   = "CamKt8LCTopoJets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "CamKt8LCTopoJets.pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT7CA8CCThinningTool
thinningTools.append(EXOT7CA8CCThinningTool)

EXOT7A10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT7A10CCThinningTool",
                                                                       ThinningService         = "EXOT7ThinningSvc",
                                                                       SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT7A10CCThinningTool
thinningTools.append(EXOT7A10CCThinningTool)

# Truth particles
#pythia_status = "(TruthParticle.status ==1 || TruthParticle.status == 2)"
#truth_eta = "abs(TruthParticle.eta) < 2.8"
#truth_quarks  = "(abs(TruthParticle.pdgId) == 1 || abs(TruthParticle.pdgId) == 2 || abs(TruthParticle.pdgId) == 3 || abs(TruthParticle.pdgId) == 4 || abs(TruthParticle.pdgId) == 5 || abs(TruthParticle.pdgId) == 6)"
#truth_leptons = "(abs(TruthParticle.pdgId) == 11 || abs(TruthParticle.pdgId) == 12 || abs(TruthParticle.pdgId) == 13 || abs(TruthParticle.pdgId) == 14 ||abs(TruthParticle.pdgId) == 15 ||abs(TruthParticle.pdgId) ==16)"
#truth_bhadrons = "(abs(TruthParticle.pdgId)==51 ||abs(TruthParticle.pdgId)==52 ||abs(TruthParticle.pdgId)==53 || abs(TruthParticle.pdgId)==54 || abs(TruthParticle.pdgId)==55 || abs(TruthParticle.pdgId)==56 || abs(TruthParticle.pdgId)==511 ||abs(TruthParticle.pdgId)==521|| abs(TruthParticle.pdgId)==531 ||abs(TruthParticle.pdgId)==541 ||abs(TruthParticle.pdgId)==551 || abs(TruthParticle.pdgId)==513 || abs(TruthParticle.pdgId)==523 || abs(TruthParticle.pdgId)==533 ||abs(TruthParticle.pdgId)==543 || abs(TruthParticle.pdgId)==553 || abs(TruthParticle.pdgId)==5101 || abs(TruthParticle.pdgId)==5201 || abs(TruthParticle.pdgId)==5301 ||abs(TruthParticle.pdgId)==5401 ||  abs(TruthParticle.pdgId)==5103 || abs(TruthParticle.pdgId)==5203 || abs(TruthParticle.pdgId)==5303 || abs(TruthParticle.pdgId)==5403 || abs(TruthParticle.pdgId)==5503 || abs(TruthParticle.pdgId)==515 || abs(TruthParticle.pdgId)==525 || abs(TruthParticle.pdgId)==535 || abs(TruthParticle.pdgId)==545 || abs(TruthParticle.pdgId)==555 || abs(TruthParticle.pdgId)==5122 || abs(TruthParticle.pdgId)==5212 || abs(TruthParticle.pdgId)==5222 || abs(TruthParticle.pdgId)==5312 || abs(TruthParticle.pdgId)==5322 || abs(TruthParticle.pdgId)==5332 || abs(TruthParticle.pdgId)==5412 || abs(TruthParticle.pdgId)==5422  || abs(TruthParticle.pdgId)==5432 || abs(TruthParticle.pdgId)==5442 || abs(TruthParticle.pdgId)==5512 || abs(TruthParticle.pdgId)==5522 || abs(TruthParticle.pdgId)==5532 || abs(TruthParticle.pdgId)==5542 || abs(TruthParticle.pdgId)==5114 || abs(TruthParticle.pdgId)==5214 || abs(TruthParticle.pdgId)==5224 || abs(TruthParticle.pdgId)==5314 || abs(TruthParticle.pdgId)==5324 || abs(TruthParticle.pdgId)==5334 || abs(TruthParticle.pdgId)==5414 || abs(TruthParticle.pdgId)==5424 || abs(TruthParticle.pdgId)==5434 || abs(TruthParticle.pdgId)==5444 || abs(TruthParticle.pdgId)==5514 || abs(TruthParticle.pdgId)==5524 || abs(TruthParticle.pdgId)==5534 || abs(TruthParticle.pdgId)==5544 || abs(TruthParticle.pdgId)==5554  || abs(TruthParticle.pdgId)==5132 || abs(TruthParticle.pdgId)==5232 || abs(TruthParticle.pdgId)==5142 || abs(TruthParticle.pdgId)==5242 || abs(TruthParticle.pdgId)==5342 || abs(TruthParticle.pdgId)==5112)"
#truth_signal = "abs(TruthParticle.pdgId) ==25 || abs(TruthParticle.pdgId)==39 || abs(TruthParticle.pdgId)==32 || abs(TruthParticle.pdgId)==5100021"
#truth_WZ = "abs(TruthParticle.pdgId) == 24 || abs(TruthParticle.pdgId) ==23"
#truth_expression = '('+pythia_status+' && '+truth_eta+')'
#truth_expression2 = '('+truth_quarks+' || '+truth_leptons+' || '+truth_bhadrons+' || '+truth_signal+' || '+truth_WZ+')'
#final_expression = '('+truth_expression+' && '+truth_expression2+')'
 

# MC thinning ( status && Eta cut && pdgids)
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#EXOT7MCThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT7MCThinningTool",
#                                                                ThinningService = "EXOT7ThinningSvc",
#                                                                ParticleSelectionString = final_expression, 
#                                                                PreserveDescendants = False)
#
#
#from AthenaCommon.GlobalFlags import globalflags
#if globalflags.DataSource()=='geant4':
#    ToolSvc += EXOT7MCThinningTool
#    thinningTools.append(EXOT7MCThinningTool)


from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT7MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT7MCThinningTool",
                                                             ThinningService = "EXOT7ThinningSvc",
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = True,
	                                                     WriteBHadrons              = True,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = True,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = False,
	                                                     WriteBosons                = False,
	                                                     WriteBSMProducts           = False,
	                                                     WriteTopAndDecays          = False,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = -1,
                                                             PreserveDescendants        = False)

EXOT7TMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT7TMCThinningTool",
                                                             ThinningService = "EXOT7ThinningSvc",
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = False,
	                                                     WriteBHadrons              = False,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = False,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = True,
	                                                     WriteBosons                = True,
	                                                     WriteBSMProducts           = True,
	                                                     WriteTopAndDecays          = True,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = -1,
                                                             PreserveDescendants        = True)

EXOT7TAMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT7TAMCThinningTool",
                                                             ThinningService = "EXOT7ThinningSvc",
                                                             WritePartons               = False,
                                                             WriteHadrons               = False,
                                                             WriteBHadrons              = False,
                                                             WriteGeant                 = False,
                                                             GeantPhotonPtThresh        = -1.0,
                                                             WriteTauHad                = False,
                                                             PartonPtThresh             = -1.0,
                                                             WriteBSM                   = False,
                                                             WriteBosons                = False,
                                                             WriteBSMProducts           = False,
                                                             WriteTopAndDecays          = True,
                                                             WriteEverything            = False,
                                                             WriteAllLeptons            = False,
                                                             WriteLeptonsNotFromHadrons         = False,
                                                             WriteStatus3               = False,
                                                             WriteFirstN                = 10,
                                                             PreserveDescendants        = False,
                                                             PreserveAncestors          = True)

#
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT7MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT7MCGenThinningTool",
                                                                ThinningService = "EXOT7ThinningSvc",
                                                                ParticleSelectionString = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021", 
                                                                PreserveDescendants = False)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
    ToolSvc += EXOT7MCThinningTool
    thinningTools.append(EXOT7MCThinningTool)
    ToolSvc += EXOT7TMCThinningTool
    thinningTools.append(EXOT7TMCThinningTool)
    ToolSvc += EXOT7TAMCThinningTool
    thinningTools.append(EXOT7TAMCThinningTool)
    ToolSvc += EXOT7MCGenThinningTool
    thinningTools.append(EXOT7MCGenThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot7Seq
exot7Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT7Kernel_skim",
                                                         AugmentationTools = [EXOT7DeltaRTool],
                                                         SkimmingTools = [EXOT7StringSkimmingTool])
exot7Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT7Kernel",
                                                         ThinningTools = thinningTools)


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT7Stream )
EXOT7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT7Stream.AcceptAlgs(["EXOT7Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT7ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT7ContentList import *
EXOT7SlimmingHelper = SlimmingHelper("EXOT7SlimmingHelper")
EXOT7SlimmingHelper.StaticContent = EXOT7Content

EXOT7SlimmingHelper.SmartCollections = EXOT7SmartCollections
EXOT7SlimmingHelper.ExtraVariables = EXOT7ExtraVariables
EXOT7SlimmingHelper.AllVariables = EXOT7AllVariables
#EXOT7SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT7SlimmingHelper.IncludeJetTriggerContent = True
EXOT7SlimmingHelper.IncludeBJetTriggerContent = True

EXOT7SlimmingHelper.AppendContentToStream(EXOT7Stream)
