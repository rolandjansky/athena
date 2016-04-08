#********************************************************************
# EXOT16.py 
# reductionConf flag EXOT16 in Reco_tf.py
#********************************************************************


 
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

# changed to a local version so we can update the eta range to 2.8
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#from DerivationFrameworkExotics.EXOTMuonsCommon import *

from DerivationFrameworkExotics.JetDefinitions import *

from JetRec.JetRecStandard import jtm

from DerivationFrameworkCore.WeightMetadata import *

exot16Seq = CfgMgr.AthSequencer("EXOT16Sequence")

isMC=(globalflags.DataSource()=='geant4')

#=======================================
# Augmentation: VariableR
#=======================================

jtm.modifiersMap["smallvr_lctopo_modifiers"] = [
  #"applyCalibrationTool:offset",
  "jetfilter", #for topo jets
  jtm.jetens, #for topo jets
  jtm.caloqual_cluster, #for ungroomed topo jets, if jetFlags.useCaloQualityTool()
  #jtm.jvf,
  #jtm.trkmoms, "trackassoc", #for ungroomed topo jets if jetFlags.useTracks()
]

jtm.modifiersMap["largevr_lctopo_modifiers"] = [
  #"applyCalibrationTool:offset",
  "jetfilter", #for topo jets
  jtm.jetens, #for topo jets
  jtm.caloqual_cluster, #for ungroomed topo jets, if jetFlags.useCaloQualityTool()
  #jtm.jvf,
  #jtm.trkmoms, "trackassoc", #for ungroomed topo jets if jetFlags.useTracks()
  #"truthassoc", #for ungroomed topo jets if jetFlags.useTruth()
  jtm.ktsplitter
]

jtm.modifiersMap["smallvr_track_modifiers"] = jtm.modifiersMap["pv0track"]  ## ZJH: for R&D
if isMC:
   jtm.modifiersMap["smallvr_truth_modifiers"] = jtm.modifiersMap["truth"]
   jtm.modifiersMap["largevr_truth_modifiers"] = jtm.modifiersMap["truth"]

jtm.modifiersMap["largevr_track_modifiers"] = [
  jtm.ktsplitter,
]

# Small VR lctopo jets
# WARNING: Set ghostArea to 0 (should be 0.01)
# this ssaves a lot of memory in Grid job
# also requires commenting out jet calibration
# Large VR lctopo jets
jfind_largevr_lctopo = jtm.addJetFinder("AntiKtVR600Rmax10Rmin2LCTopoJets", "AntiKt", 1.0, "lctopo", "largevr_lctopo_modifiers",
                ghostArea=0 , ptmin=50000, ptminFilter=50000,
                variableRMinRadius=0.2, variableRMassScale=600000, calibOpt="none")

jfind_smallvr_lctopo = jtm.addJetFinder("AntiKtVR60Rmax4Rmin2LCTopoJets", "AntiKt", 0.4, "lctopo", "smallvr_lctopo_modifiers",
#                ghostArea=0.01 , ptmin=7000, ptminFilter=7000,  # ~45s for each ttbar MC event... 
                ghostArea=0 , ptmin=7000, ptminFilter=7000,
                variableRMinRadius=0.2, variableRMassScale=60000, calibOpt="none")

# VR track jets
jfind_smallvr_track = jtm.addJetFinder("AntiKtVR20Rmax4Rmin0TrackJets", "AntiKt", 0.4, "pv0track", "smallvr_track_modifiers",
                ghostArea=0 , ptmin=2000, ptminFilter=7000,
                variableRMinRadius=0, variableRMassScale=20000, calibOpt="none")

jfind_mediumvr_track= jtm.addJetFinder("AntiKtVR40Rmax4Rmin0TrackJets", "AntiKt", 0.4, "pv0track", "smallvr_track_modifiers",
                ghostArea=0 , ptmin=2000, ptminFilter=7000,
                variableRMinRadius=0, variableRMassScale=40000, calibOpt="none")

jfind_largevr_track = jtm.addJetFinder("AntiKtVR60Rmax4Rmin0TrackJets", "AntiKt", 0.4, "pv0track", "smallvr_track_modifiers",
                ghostArea=0 , ptmin=2000, ptminFilter=7000,
                variableRMinRadius=0, variableRMassScale=60000, calibOpt="none")


if isMC:
    ## Small VR truth jets
    jfind_smallvr_truth = jtm.addJetFinder("AntiKtVR60Rmax4Rmin2TruthJets", "AntiKt", 0.4, "truth", "smallvr_truth_modifiers",
                                       ghostArea=0 , ptmin=5000, ptminFilter=7000,
                                       variableRMinRadius=0.2, variableRMassScale=60000, calibOpt="")

    ## Large VR truth jets
    jfind_largevr_truth = jtm.addJetFinder("AntiKtVR600Rmax10Rmin2TruthJets", "AntiKt", 1.0, "truth", "largevr_truth_modifiers",
                                       ghostArea=0 , ptmin=40000, ptminFilter=40000,
                                       variableRMinRadius=0.2, variableRMassScale=600000, calibOpt="")
                
from JetRec.JetRecConf import JetAlgorithm
jetalg_smallvr_lctopo = JetAlgorithm("jfind_smallvr_lctopo", Tools = [ jfind_smallvr_lctopo] )
jetalg_largevr_lctopo = JetAlgorithm("jfind_largevr_lctopo", Tools = [ jfind_largevr_lctopo] )
jetalg_smallvr_track = JetAlgorithm("jfind_smallvr_track" , Tools = [ jfind_smallvr_track ] )
jetalg_mediumvr_track= JetAlgorithm("jfind_mediumvr_track", Tools = [ jfind_mediumvr_track] )
jetalg_largevr_track = JetAlgorithm("jfind_largevr_track" , Tools = [ jfind_largevr_track ] )

if isMC:
    jetalg_smallvr_truth = JetAlgorithm("jfind_smallvr_truth", Tools = [ jfind_smallvr_truth] )
    jetalg_largevr_truth = JetAlgorithm("jfind_largevr_truth", Tools = [ jfind_largevr_truth] )

exot16Seq += jetalg_smallvr_lctopo
exot16Seq += jetalg_largevr_lctopo
exot16Seq += jetalg_smallvr_track 
exot16Seq += jetalg_mediumvr_track
exot16Seq += jetalg_largevr_track 
if isMC:
    exot16Seq += jetalg_smallvr_truth
    exot16Seq += jetalg_largevr_truth

addDefaultTrimmedJets(exot16Seq, "EXOT16")
#addStandardJets("CamKt", 1.5, "LCTopo", mods = "calib_notruth", algseq = exot16Seq, outputGroup = "EXOT16")

#=======================================
# VRJET BTAGGING
#=======================================

# set up alieas for histograms
from BTagging.BTaggingFlags import BTaggingFlags
#BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR20Rmax4Rmin0Track->AntiKt3Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo"]
#BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR40Rmax4Rmin0Track->AntiKt3Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo"]
#BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR60Rmax4Rmin0Track->AntiKt3Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR20Rmax4Rmin0Track->AntiKt4EMTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR40Rmax4Rmin0Track->AntiKt4EMTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR60Rmax4Rmin0Track->AntiKt4EMTopo"]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR60Rmax4Rmin2LCTopo->AntiKt4EMTopo"]


# run retagging
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ["AntiKt4PV0TrackJets", "AntiKt4LCTopoJets", "AntiKtVR60Rmax4Rmin2LCTopoJets", "AntiKtVR20Rmax4Rmin0TrackJets", "AntiKtVR40Rmax4Rmin0TrackJets", "AntiKtVR60Rmax4Rmin0TrackJets", ], 
              Sequencer=exot16Seq ) ## Must re-tag Akt4LCTopo and Akt4Trk to make JetFitterNN work with corresponding VR jets (JZ)

#=======================================
# SKIMMING   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = '((count((AntiKtVR60Rmax4Rmin2LCTopoJets.pt > 5*GeV) && (abs(AntiKtVR60Rmax4Rmin2LCTopoJets.eta)<3.5)) >= 0)  || (count((AntiKtVR600Rmax10Rmin2LCTopoJets.pt > 5*GeV) && (abs(AntiKtVR600Rmax10Rmin2LCTopoJets.eta)<3.5)) >= 0))'   ## Dummy filter: keep everything for calibration R&D
##if (beamEnergy < 4.1e+06):
##    SkipTriggerRequirement=(globalflags.DataSource()=='geant4')
##    if  SkipTriggerRequirement:
##      expression = '(((count((AntiKt4LCTopoJets.pt > 15*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.4)) >0 )) && (((count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || ((count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
##    else:
##      expression = '(((count((AntiKt4LCTopoJets.pt > 15*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.4)) >0 )) && ((((EventInfo.eventTypeBitmask==1) || EF_e24vhi_medium1 || EF_e60_medium1) && (count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( ((EventInfo.eventTypeBitmask==1) || EF_mu24i_tight || EF_mu36_tight) && count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
##if (beamEnergy > 6.0e+06):
##    SkipTriggerRequirement=(globalflags.DataSource()=='geant4')
##    if  SkipTriggerRequirement:
##        expression = '(((count((AntiKt4LCTopoJets.pt > 15*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.4)) >0 )) && (((count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
##    else:
##        expression = '(((count((AntiKt4LCTopoJets.pt > 15*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.4)) >0 )) && (((HLT_e28_tight_iloose || HLT_e60_medium) && (count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( (HLT_mu26_imedium || HLT_mu50) && count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT16StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT16StringSkimmingTool",
                                                                         expression = expression)

ToolSvc += EXOT16StringSkimmingTool
print EXOT16StringSkimmingTool


# Tracks thinning 
#thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV)"
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
#EXOT16D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    =  "EXOT16D1TPThinningTool",
#                                                                    ThinningService         =  "EXOT16ThinningSvc",
#                                                                    SelectionString         =  thinning_expression,
#                                                                    InDetTrackParticlesKey  =  "InDetTrackParticles",
#                                                                    ApplyAnd                =  False)
#ToolSvc += EXOT16D1TPThinningTool


# define thinning tool 
thinningTools=[]
# Tracks associated with jets (for e-in-jet OR)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT16JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(    	name                    = "EXOT16JetTPThinningTool",
                                                                                        ThinningService         = "EXOT16ThinningSvc",
                                                                                        JetKey                  = "AntiKt4LCTopoJets",
                                                                                        SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && AntiKt4LCTopoJets.eta > -2.8 && AntiKt4LCTopoJets.eta < 2.8",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT16JetTPThinningTool
thinningTools.append(EXOT16JetTPThinningTool)

##EXOT16CA15JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT16CA15JetTPThinningTool",
##                                                                       ThinningService         = "EXOT16ThinningSvc",
##                                                                       JetKey                  = "CamKt15LCTopoJets",
##                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV && CamKt15LCTopoJets.eta > -2.7 && CamKt15LCTopoJets.eta < 2.7",
##                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
##ToolSvc += EXOT16CA15JetTPThinningTool
##thinningTools.append(EXOT16CA15JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT16MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT16MuonTPThinningTool",
                                                                            ThinningService         = "EXOT16ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            SelectionString = "Muons.pt > 7*GeV",
                                                                            ConeSize=0.4)
ToolSvc += EXOT16MuonTPThinningTool
thinningTools.append(EXOT16MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT16ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT16ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT16ThinningSvc",
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 7*GeV",
                                                                                        ConeSize = 0.4
                                                                              )
ToolSvc += EXOT16ElectronTPThinningTool
thinningTools.append(EXOT16ElectronTPThinningTool)

# calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT16ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT16ElectronCCThinningTool",
                                                                                     ThinningService         = "EXOT16ThinningSvc",
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.4)
ToolSvc += EXOT16ElectronCCThinningTool
thinningTools.append(EXOT16ElectronCCThinningTool)

##from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
##EXOT16CA15CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT16CA15CCThinningTool",
##                                                                       ThinningService         = "EXOT16ThinningSvc",
##                                                                       SGKey                   = "CamKt15LCTopoJets",
##                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
##                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV",
##                                                                       ConeSize                = 0)
##ToolSvc += EXOT16CA15CCThinningTool
##thinningTools.append(EXOT16CA15CCThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT16Ak10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT16Ak10CCThinningTool",
                                                                       ThinningService         = "EXOT16ThinningSvc",
                                                                       SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT16Ak10CCThinningTool
thinningTools.append(EXOT16Ak10CCThinningTool)

# Truth particles (this is not working for ghost-associated b-hadrons)
#pythia_status = "(TruthParticles.status == 1 || TruthParticles.status == 2)"
#truth_eta = "abs(TruthParticles.eta) < 2.8"
#truth_quarks  = "(abs(TruthParticles.pdgId) == 1 || abs(TruthParticles.pdgId) == 2 || abs(TruthParticles.pdgId) == 3 || abs(TruthParticles.pdgId) == 4 || abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 6)"
#truth_leptons = "(abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 14 ||abs(TruthParticles.pdgId) == 15 ||abs(TruthParticles.pdgId) ==16)"
#truth_bhadrons = "(abs(TruthParticles.pdgId)==51 ||abs(TruthParticles.pdgId)==52 ||abs(TruthParticles.pdgId)==53 || abs(TruthParticles.pdgId)==54 || abs(TruthParticles.pdgId)==55 || abs(TruthParticles.pdgId)==56 || abs(TruthParticles.pdgId)==511 ||abs(TruthParticles.pdgId)==521|| abs(TruthParticles.pdgId)==531 ||abs(TruthParticles.pdgId)==541 ||abs(TruthParticles.pdgId)==551 || abs(TruthParticles.pdgId)==513 || abs(TruthParticles.pdgId)==523 || abs(TruthParticles.pdgId)==533 ||abs(TruthParticles.pdgId)==543 || abs(TruthParticles.pdgId)==553 || abs(TruthParticles.pdgId)==5101 || abs(TruthParticles.pdgId)==5201 || abs(TruthParticles.pdgId)==5301 ||abs(TruthParticles.pdgId)==5401 ||  abs(TruthParticles.pdgId)==5103 || abs(TruthParticles.pdgId)==5203 || abs(TruthParticles.pdgId)==5303 || abs(TruthParticles.pdgId)==5403 || abs(TruthParticles.pdgId)==5503 || abs(TruthParticles.pdgId)==515 || abs(TruthParticles.pdgId)==525 || abs(TruthParticles.pdgId)==535 || abs(TruthParticles.pdgId)==545 || abs(TruthParticles.pdgId)==555 || abs(TruthParticles.pdgId)==5122 || abs(TruthParticles.pdgId)==5212 || abs(TruthParticles.pdgId)==5222 || abs(TruthParticles.pdgId)==5312 || abs(TruthParticles.pdgId)==5322 || abs(TruthParticles.pdgId)==5332 || abs(TruthParticles.pdgId)==5412 || abs(TruthParticles.pdgId)==5422  || abs(TruthParticles.pdgId)==5432 || abs(TruthParticles.pdgId)==5442 || abs(TruthParticles.pdgId)==5512 || abs(TruthParticles.pdgId)==5522 || abs(TruthParticles.pdgId)==5532 || abs(TruthParticles.pdgId)==5542 || abs(TruthParticles.pdgId)==5114 || abs(TruthParticles.pdgId)==5214 || abs(TruthParticles.pdgId)==5224 || abs(TruthParticles.pdgId)==5314 || abs(TruthParticles.pdgId)==5324 || abs(TruthParticles.pdgId)==5334 || abs(TruthParticles.pdgId)==5414 || abs(TruthParticles.pdgId)==5424 || abs(TruthParticles.pdgId)==5434 || abs(TruthParticles.pdgId)==5444 || abs(TruthParticles.pdgId)==5514 || abs(TruthParticles.pdgId)==5524 || abs(TruthParticles.pdgId)==5534 || abs(TruthParticles.pdgId)==5544 || abs(TruthParticles.pdgId)==5554  || abs(TruthParticles.pdgId)==5132 || abs(TruthParticles.pdgId)==5232 || abs(TruthParticles.pdgId)==5142 || abs(TruthParticles.pdgId)==5242 || abs(TruthParticles.pdgId)==5342 || abs(TruthParticles.pdgId)==5112)"
#truth_signal = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021"
#truth_WZ = "abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) ==23"
#truth_expression = '('+pythia_status+' && '+truth_eta+')'
#truth_expression2 = '('+truth_quarks+' || '+truth_leptons+' || '+truth_bhadrons+' || '+truth_signal+' || '+truth_WZ+')'
#final_expression = '('+truth_expression+' && '+truth_expression2+')'
 

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT16MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT16MCThinningTool",
                                                             ThinningService = "EXOT16ThinningSvc",
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = True,
	                                                     WriteBHadrons              = True,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = True,
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
                                                             PreserveDescendants        = False)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT16MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT16MCGenThinningTool",
                                                                ThinningService = "EXOT16ThinningSvc",
                                                                ParticleSelectionString = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021", 
                                                                PreserveDescendants = False)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
    ToolSvc += EXOT16MCThinningTool
    thinningTools.append(EXOT16MCThinningTool)
    ToolSvc += EXOT16MCGenThinningTool
    thinningTools.append(EXOT16MCGenThinningTool)



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot16Seq
exot16Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT16Kernel_skim", SkimmingTools = [EXOT16StringSkimmingTool])
exot16Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT16Kernel", ThinningTools = thinningTools)


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT16Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT16Stream )
EXOT16Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT16Stream.AcceptAlgs(["EXOT16Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT16ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT16
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT16ContentList import *
EXOT16SlimmingHelper = SlimmingHelper("EXOT16SlimmingHelper")

EXOT16SlimmingHelper.AppendToDictionary=EXOT16Content

EXOT16SlimmingHelper.SmartCollections = EXOT16SmartCollections

EXOT16SlimmingHelper.AllVariables = EXOT16AllVariables

EXOT16SlimmingHelper.ExtraVariables = EXOT16ExtraVariables

addMETOutputs(EXOT16SlimmingHelper, ["MET_Track"], ["AntiKt4EMTopo"])
EXOT16SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT16SlimmingHelper.IncludeMuonTriggerContent = True

EXOT16SlimmingHelper.AppendContentToStream(EXOT16Stream)

