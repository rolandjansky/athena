#********************************************************************
# EXOT3.py 
# reductionConf flag EXOT3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import ElectronsCPDetailedContent

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel


from JetRec.JetRecStandard import jtm

from AthenaCommon.GlobalFlags import globalflags
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND


isMC = False
if DerivationFrameworkHasTruth:
  isMC = True
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT3Stream )
EXOT3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT3Stream.AcceptAlgs(["EXOT3Kernel"])

#=====================
# TRIGGER NAV THINNING
#=====================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT3ThinningHelper = ThinningHelper("EXOT3ThinningHelper")
EXOT3ThinningHelper.TriggerChains = 'HLT_j.*|HLT_g.*'
EXOT3ThinningHelper.AppendToStream( EXOT3Stream )

#====================================================================
# THINNING TOOL 
#====================================================================

thinningTools = []

# Track thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
EXOT3TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "EXOT3TPThinningTool",
                                                                 ThinningService         = EXOT3ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3TPThinningTool
thinningTools.append(EXOT3TPThinningTool)

# Tracks and CaloClusters associated with TCCs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
EXOT3TCCTPThinningTool = DerivationFramework__TCCTrackParticleThinning(name                         = "EXOT3TCCTPThinningTool",
                                                                       ThinningService              = EXOT3ThinningHelper.ThinningSvc(),
                                                                       JetKey                       = "AntiKt10TrackCaloClusterJets",
                                                                       TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
                                                                       InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                       CaloCalTopoClustersKey       = "CaloCalTopoClusters",
                                                                       ThinOriginCorrectedClusters  = True,
                                                                       OriginCaloCalTopoClustersKey = "LCOriginTopoClusters")
ToolSvc += EXOT3TCCTPThinningTool
thinningTools.append(EXOT3TCCTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT3MuonTPThinningTool",
                                                                         ThinningService         = EXOT3ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3MuonTPThinningTool
thinningTools.append(EXOT3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT3ElectronTPThinningTool",
                                                                               ThinningService         = EXOT3ThinningHelper.ThinningSvc(),
                                                                               SGKey             	   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3ElectronTPThinningTool
thinningTools.append(EXOT3ElectronTPThinningTool)

# Tracks associated with Photons
EXOT3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT3PhotonTPThinningTool",
                                                                             ThinningService         = EXOT3ThinningHelper.ThinningSvc(),
                                                                             SGKey             	     = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3PhotonTPThinningTool
thinningTools.append(EXOT3PhotonTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT3AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "EXOT3AKt10JetTPThinningTool",
                                                                        ThinningService         = EXOT3ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        SelectionString         = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt10JetTPThinningTool
thinningTools.append(EXOT3AKt10JetTPThinningTool)

# Calo-cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT3AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                 = "EXOT3Ak10CCThinningTool",
                                                                      ThinningService       = EXOT3ThinningHelper.ThinningSvc(),
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                      AdditionalClustersKey = ["LCOriginTopoClusters"])
ToolSvc += EXOT3AKt10CCThinningTool
thinningTools.append(EXOT3AKt10CCThinningTool)

#Truth Thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT3MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                = "EXOT3MCThinningTool",
        ThinningService     = EXOT3ThinningHelper.ThinningSvc(),
        WriteEverything     = False,
        WritePartons        = False,
        WriteHadrons        = True,
        WriteBHadrons       = True,
        WriteGeant          = False,
        WriteTauHad         = False,
        WriteBSM            = True,
        WriteBosons         = True,
        WriteBosonProducts  = True,
        WriteBSMProducts    = True,
        WriteTopAndDecays   = True,
        WriteAllLeptons     = True,
        WriteStatus3        = False,
        WriteFirstN         = -1,
        PreserveDescendants = True)

if isMC:
  ToolSvc += EXOT3MCThinningTool
  thinningTools.append(EXOT3MCThinningTool)

#====================================================================
# SKIMMING TOOL 
#====================================================================

triggers = [
            # Standard large-R jet support triggers (bootstrap, etc)
            "HLT_j110_a10_lcw_subjes_L1J30", # new to 2018
            "HLT_j175_a10_lcw_subjes_L1J50", # new to 2018
            "HLT_j260_a10_lcw_L1J75",
            "HLT_j260_a10_lcw_nojcalib_L1J75",
            "HLT_j260_a10_nojcalib_L1J75",
            "HLT_j260_a10_sub_L1J75",
            "HLT_j260_a10_lcw_sub_L1J75",
            "HLT_j300_a10_sub_L1J75",
            "HLT_j300_a10_lcw_L1J75",
            "HLT_j300_a10_lcw_sub_L1SC85",
            "HLT_j300_a10_lcw_L1SC85",
            # Standard large-R jet primary triggers
            "HLT_j360_a10_lcw_sub_L1J100", # lowest unprescaled, 2015
            "HLT_j360_a10_lcw_L1J100",
            "HLT_j380_a10_lcw_L1J100",
            "HLT_j400_a10_lcw_L1J100",
            "HLT_j420_a10_lcw_L1J100", # lowest unprescaled, 2016
            "HLT_j440_a10_lcw_L1J100",
            "HLT_j460_a10_sub_L1J100",
            "HLT_j460_a10_lcw_L1J100",
            "HLT_j460_a10_lcw_subjes_L1J100", # lowest unprescaled, 2017 (1.7e34 menu)
            "HLT_j460_a10_nojcalib_L1J100",
            "HLT_j460_a10_lcw_nojcalib_L1J100",
            "HLT_j460_a10_lcw_sub_L1J100",
            "HLT_j480_a10_lcw_subjes_L1J100", # lowest unprescaled, 2017 (2.0e34 menu) and 2018
            "HLT_j500_a10_lcw_subjes_L1J100", # backup, 2018
            "HLT_j520_a10_lcw_subjes_L1J100", # backup, 2018
            "HLT_j540_a10_lcw_subjes_L1J100", # backup, 2018
            # Trimmed large-R jet primary triggers (new 2017 nominal)
            "HLT_j460_a10t_lcw_jes_L1J100", # lowest unprescaled, 2017 (1.7e34 menu)
            "HLT_j480_a10t_lcw_jes_L1J100", # lowest unprescaled, 2017 (2.0e34 menu) and 2018
            "HLT_j500_a10t_lcw_jes_L1J100", # backup, 2018
            "HLT_j520_a10t_lcw_jes_L1J100", # backup, 2018
            "HLT_j540_a10t_lcw_jes_L1J100", # backup, 2018
            # Trimmed large-R jets, with 1xmass cuts (substructure-enriched triggers)
            "HLT_j390_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_j420_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_j440_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_j390_a10t_lcw_jes_30smcINF_L1J100", # unprescaled for 99% of 2017
            "HLT_j420_a10t_lcw_jes_35smcINF_L1J100", # lowest unprescaled, 2018
            "HLT_j420_a10t_lcw_jes_40smcINF_L1J100", # lowest unprescaled, 2017 (1.7e34 menu)
            "HLT_j440_a10t_lcw_jes_35smcINF_L1J100", # backup, 2018
            "HLT_j440_a10t_lcw_jes_40smcINF_L1J100", # lowest unprescaled, 2017 (2.0e34 menu)
            # Trimmed large-R jets, with 2xmass cuts (substructure-enriched triggers)
            "HLT_2j330_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_2j330_a10t_lcw_jes_L1SC111", # prescaled, for mass bootstrap, 2018, L1Topo
            "HLT_2j330_a10t_lcw_jes_30smcINF_L1J100", # unprescaled for 99% of 2017
            "HLT_2j330_a10t_lcw_jes_35smcINF_L1J100", # lowest unprescaled, 2018
            "HLT_2j330_a10t_lcw_jes_35smcINF_L1SC111", # lowest unprescaled, 2018, L1Topo
            "HLT_2j330_a10t_lcw_jes_40smcINF_L1J100", # lowest unprescaled, 2017 (1.7e34 and 2.0e34 menus)
            "HLT_2j340_a10t_lcw_jes_35smcINF_L1J100", # backup, 2018
            "HLT_2j340_a10t_lcw_jes_35smcINF_L1SC111", # backup, 2018, L1Topo
            "HLT_2j340_a10t_lcw_jes_40smcINF_L1J100", # backup, 2018
            "HLT_2j350_a10t_lcw_jes_35smcINF_L1J100", # backup, 2018
            "HLT_2j350_a10t_lcw_jes_35smcINF_L1SC111", # backup, 2018, L1Topo
            "HLT_2j350_a10t_lcw_jes_40smcINF_L1J100", # backup, 2018
            # Trimmed large-R jets, with two ET cuts and one mass cut (1x substructure + ISR triggers)
            "HLT_j310_a10t_lcw_jes_60smcINF_j310_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j325_a10t_lcw_jes_60smcINF_j325_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j340_a10t_lcw_jes_60smcINF_j340_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j370_a10t_lcw_jes_40smcINF_j370_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j370_a10t_lcw_jes_50smcINF_j370_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j370_a10t_lcw_jes_60smcINF_j370_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j380_a10t_lcw_jes_40smcINF_j380_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            "HLT_j380_a10t_lcw_jes_50smcINF_j380_a10t_lcw_jes_L1SC111", # Potential primary, 2018, L1Topo
            # Reclustered large-R jet support triggers
            "HLT_j260_a10r_L1J75",
            "HLT_j300_a10r_L1J75",
            # Reclustered large-R jet primary triggers
            "HLT_j360_a10r_L1J100", # lowest unprescaled, 2015 - TRIGGER HAD A BUG IN 2015
            "HLT_j380_a10r_L1J100",
            "HLT_j400_a10r_L1J100",
            "HLT_j420_a10r_L1J100", # lowest unprescaled, 2016
            "HLT_j440_a10r_L1J100",
            "HLT_j460_a10r_L1J100", # lowest unprescaled, 2017 (1.7e34 menu)
            "HLT_j480_a10r_L1J100", # lowest unprescaled, 2017 (2.0e34 menu) and 2018
            "HLT_j500_a10r_L1J100", # backup, 2018
            "HLT_j520_a10r_L1J100", # backup, 2018
            "HLT_j540_a10r_L1J100", # backup, 2018
            # HT triggers
            "HLT_ht850_L1J75",
            "HLT_ht850_L1J100",
            "HLT_ht850",
            "HLT_ht1000",
            "HLT_ht1000_L1J75",
            "HLT_ht1000_L1J100",
           ]

topology_selection_1jet = "((count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)  >= 1))"

topology_selection_2jet_lowpt =  "(count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)  >= 2)"

topology_selection_2jet_highpt = "(count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 1000*GeV)  >= 2)"

topology_selection_2jet_rcpflow_lowpt =  "(count (abs(AntiKt10RCEMPFlowJets.eta) < 2.8 && AntiKt10RCEMPFlowJets.pt > 100*GeV && AntiKt10RCEMPFlowJets.m > 30*GeV)  >= 2)"

topology_selection_2jet_rcpflow_highpt = "(count (abs(AntiKt10RCEMPFlowJets.eta) < 2.8 && AntiKt10RCEMPFlowJets.pt > 1000*GeV)  >= 2)"

topology_selection_2jet =  "(" + topology_selection_2jet_lowpt + " || " + topology_selection_2jet_highpt + " || " + topology_selection_2jet_rcpflow_lowpt + " || " + topology_selection_2jet_rcpflow_highpt + ")"

if isMC:
  topology_selection_2jet_rctruth_lowpt =  "(count (abs(AntiKt10RCTruthJets.eta) < 2.8 && AntiKt10RCTruthJets.pt > 100*GeV && AntiKt10RCTruthJets.m > 30*GeV)  >= 2)"

  topology_selection_2jet_rctruth_highpt = "(count (abs(AntiKt10RCTruthJets.eta) < 2.8 && AntiKt10RCTruthJets.pt > 1000*GeV)  >= 2)"

  topology_selection_2jet = "(" + topology_selection_2jet_lowpt + " || " + topology_selection_2jet_highpt + " || " + topology_selection_2jet_rcpflow_lowpt + " || " + topology_selection_2jet_rcpflow_highpt + " || " + topology_selection_2jet_rctruth_lowpt + " || " + topology_selection_2jet_rctruth_highpt + ")"

EXOT3_trigger2 = "(HLT_g120_loose || HLT_g140_loose || HLT_xe100 || HLT_xe90_mht_L1XE50 || HLT_xe90_tc_lcw_L1XE50)"
EXOT3_selection = "((count(Photons.pt > 100*GeV) > 0) || (count(Electrons.pt > 100*GeV) > 0))"


#------------------------------------------
#pre-skimming tools

#photons and electrons
#NOTE this does *NOT* include 1 jet selection
EXOT3PreSkimmingTool1 = DerivationFramework__xAODStringSkimmingTool(name = "EXOT3PreSkimmingTool1",
                                                                    expression = '( ' + EXOT3_trigger2 + ' && ' + EXOT3_selection + ' )')#photons and electrons
ToolSvc += EXOT3PreSkimmingTool1

#trigger
EXOT3TriggerPreSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT3TriggerPreSkimmingTool",
                                                                       TriggerListAND = [],
                                                                       TriggerListOR  = triggers)#triggers
ToolSvc += EXOT3TriggerPreSkimmingTool

#------------------------------------------
#skimming tools
#1 jet, photons and electrons
EXOT3SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool(name = "EXOT3SkimmingTool1",
                                                                 expression = '( ' + EXOT3_trigger2 + ' && ' + EXOT3_selection + ' && ' + topology_selection_1jet + ' )')#1 jet, photons and electrons
ToolSvc += EXOT3SkimmingTool1

#2 jets
EXOT3SkimmingTool2 = DerivationFramework__xAODStringSkimmingTool(name = "EXOT3SkimmingTool2",
                                                                 expression = topology_selection_2jet)#2 jets
ToolSvc += EXOT3SkimmingTool2

EXOT3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT3TriggerSkimmingTool",
                                                                    TriggerListAND = [],
                                                                    TriggerListOR  = triggers)#triggers
ToolSvc += EXOT3TriggerSkimmingTool

#------------------------------------------
#pre-skimming tools combinations

#combination: (1 jet, photons and electrons) || (triggers)
EXOT3CombinedPreSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT3ORPreSkimmingTool",
                                                                        FilterList = [EXOT3TriggerPreSkimmingTool, EXOT3PreSkimmingTool1])
ToolSvc += EXOT3CombinedPreSkimmingTool

#------------------------------------------
#skimming tools combinations

#2 jets && triggers
EXOT3ANDSkimmingTool = DerivationFramework__FilterCombinationAND(name = "EXOT3ANDSkimmingTool",
                                                                 FilterList = [EXOT3SkimmingTool2, EXOT3TriggerSkimmingTool])
ToolSvc += EXOT3ANDSkimmingTool

#combination: (1 jet, photons and electrons) || (2 jets && triggers)
EXOT3CombinedSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT3ORSkimmingTool",
                                                                     FilterList = [EXOT3ANDSkimmingTool, EXOT3SkimmingTool1])
ToolSvc += EXOT3CombinedSkimmingTool

#=======================================
# CREATE PRIVATE SEQUENCES
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE SKIMMING, THINNING AND AUGMENTATION TOOLS  
#=======================================
#this pre-sequence skims events based on what is available from the input xAOD, thus reducing the use of CPU resources
exot3PreSeq = CfgMgr.AthSequencer("EXOT3PreSequence")
exot3PreSeq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3PreKernel_skim", SkimmingTools = [EXOT3CombinedPreSkimmingTool])
DerivationFrameworkJob += exot3PreSeq

#the main sequence tools will be passed after jets have been reconstructed
exot3Seq = CfgMgr.AthSequencer("EXOT3Sequence")
exot3PreSeq += exot3Seq

#=======================================
# JETS
#=======================================
#b-tagging
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit 
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = exot3Seq)

# Create TCC objects
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction, runUFOReconstruction

# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(exot3Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")

# running UFO reconstruction
from JetRecTools.ConstModHelpers import getConstModSeq, xAOD
## Add PFlow constituents
pflowCSSKSeq = getConstModSeq(["CS","SK"], "EMPFlow")

# add the pflow cssk sequence to the main jetalg if not already there :
if pflowCSSKSeq.getFullName() not in [t.getFullName() for t in DerivationFrameworkJob.jetalg.Tools]:
  DerivationFrameworkJob.jetalg.Tools += [pflowCSSKSeq]

# Add UFO constituents
runUFOReconstruction(exot3Seq, ToolSvc, PFOPrefix="CSSK",caloClusterName="LCOriginTopoClusters")


#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT3"] = []
reducedJetList = [
    "AntiKt2PV0TrackJets",
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets",
    "AntiKt10TrackCaloClusterJets",
    "AntiKt10UFOCSSKJets"]
replaceAODReducedJets(reducedJetList,exot3Seq,"EXOT3")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
if isMC:
  from JetSimTools.JetSimToolsConf import JetChargedParticlesTool
  jtm += JetChargedParticlesTool("ChargedParticles", )
  jtm.modifiersMap['truth_groomed'] += [ jtm.ChargedParticles ]
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets, addTCCTrimmedJets, addDefaultUFOSoftDropJets
addDefaultTrimmedJets(exot3Seq,"EXOT3")
addTCCTrimmedJets(exot3Seq,"EXOT3")
addDefaultUFOSoftDropJets(exot3Seq,"EXOT3",dotruth=True)

#
# Adding ExCoM sub-jets for each trimmed large-R jet
#
ExKtJetCollection__FatJet = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
#  doTrackJet = False
ExCoMJetCollection__SubJet = addExKtCoM(exot3Seq, ToolSvc, ExKtJetCollection__FatJet, nSubjets=2, doTrackSubJet=False, ExGhostLabels=["GhostBHadronsFinal","GhostCHadronsFinal"], min_subjet_pt_mev=0, subjetAlgName="CoM")


BTaggingFlags.CalibrationChannelAliases += [
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo"]

largeRJetAlgs = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20",
    "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10",
    ]

largeRJetCollections = []
for alg in largeRJetAlgs:
  largeRJetCollections.append(alg+"Jets")

# Add truth labeling to groomed large-R jet collections
if isMC:
  for alg in largeRJetAlgs:
    addJetTruthLabel(jetalg=alg,sequence=exot3Seq,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
# A wrapper function which does all the necessary steps
addVRJets(exot3Seq, largeRColls = largeRJetCollections)
addVRJets(exot3Seq, largeRColls = largeRJetCollections, training='201903') #new trackjet training!

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMPFlow"]

#jet calibration
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot3Seq)

#================================================================
# Add Hbb tagger
#================================================================

addHbbTagger(exot3Seq, ToolSvc)
addHbbTagger(
    exot3Seq, ToolSvc,
    nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",
    nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")

#================================================================
# ADD RECLUSTERED JETS TO DERIVATION
#================================================================

ToolSvc += CfgMgr.JetReclusteringTool("MyJetReclusteringToolPFlow", InputJetContainer = "AntiKt4EMPFlowJets", OutputJetContainer = "AntiKt10RCEMPFlowJets", InputJetPtMin = 20,  RCJetPtMin = 50, ReclusterRadius = 1.0, TrimPtFrac = 0.0)
exot3Seq += CfgMgr.AthJetReclusteringAlgo("JetRecAlgoPFlow", JetReclusteringTool = ToolSvc.MyJetReclusteringToolPFlow)

if isMC:
  ToolSvc += CfgMgr.JetReclusteringTool("MyJetReclusteringToolTruth", InputJetContainer = "AntiKt4TruthJets", OutputJetContainer = "AntiKt10RCTruthJets", InputJetPtMin = 20, RCJetPtMin = 50, ReclusterRadius = 1.0, TrimPtFrac = 0.0)
  exot3Seq += CfgMgr.AthJetReclusteringAlgo("JetRecAlgoTruth", JetReclusteringTool = ToolSvc.MyJetReclusteringToolTruth)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE SKIMMING, THINNING AND AUGMENTATION TOOLS
#=======================================
exot3Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3Kernel_skim", SkimmingTools = [EXOT3CombinedSkimmingTool])
exot3Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3Kernel",      ThinningTools = thinningTools)

#=======================================
# GHOST ASSOCIATION
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addJetPtAssociation
if DerivationFrameworkHasTruth:
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=exot3Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow",  truthjetalg="AntiKt4TruthJets", sequence=exot3Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",  truthjetalg="AntiKt10TruthJets", sequence=exot3Seq, algname="JetPtAssociationAlg")
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT3ContentList import *

EXOT3SlimmingHelper = SlimmingHelper("EXOT3SlimmingHelper")

TruthAssociationVars = '.GhostTruth.GhostTruthAssociationLink.GhostPartons.GhostPartonsPt.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruthCount'

EXOT3SlimmingHelper.ExtraVariables = ["AntiKt4EMTopoJets"+TruthAssociationVars,"AntiKt4EMPFlowJets"+TruthAssociationVars,"AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"+TruthAssociationVars
                                      ]


#=====================
# Variable definitions
#=====================

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
EXOT3SlimmingHelper.SmartCollections = EXOT3SmartContent
EXOT3SlimmingHelper.SmartCollections += ["BTagging_AntiKtVR30Rmax4Rmin02Track_201903", "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                         "BTagging_AntiKtVR30Rmax4Rmin02Track_201810", "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                         "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"]
EXOT3SlimmingHelper.ExtraVariables = EXOT3ExtraVariables
EXOT3SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
EXOT3SlimmingHelper.AllVariables = EXOT3AllVariablesContent

# Add jet collections created by derivation job
EXOT3SlimmingHelper.StaticContent = EXOT3StaticContent

# addJetOutputs(EXOT3SlimmingHelper, ["EXOT3"], ["AntiKt4TruthJets", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"])

EXOT3SlimmingHelper.AppendToDictionary = {}
listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets', 'AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets', 'AntiKt10RCEMPFlowJets','AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets']

# Add VR track-jet collection and its b-tagging container to output stream
EXOT3SlimmingHelper.AppendToDictionary = {
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"            :   "xAOD::JetContainer"        ,
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810Aux"         :   "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"       :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux"    :   "xAOD::BTaggingAuxContainer",
    "LCOriginTopoClusters"                      :   "xAOD::CaloClusterContainer",
    "LCOriginTopoClustersAux"                   :   "xAOD::ShallowAuxContainer" ,
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"                 :   "xAOD::JetContainer"        ,
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJetsAux"              :   "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub"            :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubAux"         :   "xAOD::BTaggingAuxContainer",

}

# Add all variabless for VR track-jets
EXOT3SlimmingHelper.AllVariables  += ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"
        ]

# Save certain b-tagging variables for VR track-jet
EXOT3SlimmingHelper.ExtraVariables += [
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.DL1_pb.DL1_pu.DL1_pc.DL1rmu_pb.DL1rmu_pu.DL1rmu_pc.DL1r_pb.DL1r_pu.DL1r_pc",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.MV2c10_discriminant.MV2c100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP",
    "LCOriginTopoClusters.calEta.calPhi",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HbbScore",
    "Muons.EnergyLoss.energyLossType",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.ExCoM2SubJets",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub.DL1_pb.DL1_pu.DL1_pc.DL1rmu_pb.DL1rmu_pu.DL1rmu_pc.DL1r_pb.DL1r_pu.DL1r_pc",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub.MV2c10_discriminant.MV2c100_discriminant",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP"
    "AntiKt10LCTopoJets.GhostAntiKt2TrackJet",
    "AntiKt10TrackCaloClusterJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810",
    "AntiKt10TrackCaloClusterJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag",
    "AntiKt10TrackCaloClusterJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903",
    "AntiKt10TrackCaloClusterJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag",
    "AntiKt10UFOCSSKJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810",
    "AntiKt10UFOCSSKJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag",
    "AntiKt10UFOCSSKJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903",
    "AntiKt10UFOCSSKJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag",
]

if isMC:
  listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
  listJets.extend(['AntiKt10RCTruthJets'])
for i in listJets:
  EXOT3SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'
  EXOT3SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::JetAuxContainer'

# (Don't) Add jet trigger content
#EXOT3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT3SlimmingHelper.IncludeJetTriggerContent = True
EXOT3SlimmingHelper.IncludeEGammaTriggerContent = True

EXOT3SlimmingHelper.AppendContentToStream(EXOT3Stream)

