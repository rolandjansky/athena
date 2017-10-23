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

from JetRec.JetRecStandard import jtm

exot3Seq = CfgMgr.AthSequencer("EXOT3Sequence")

from AthenaCommon.GlobalFlags import globalflags

isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

#====================================================================
# THINNING TOOL 
#====================================================================

thinningTools = []

# Track thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
EXOT3TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "EXOT3TPThinningTool",
                                                                 ThinningService         = "EXOT3ThinningSvc",
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3TPThinningTool
thinningTools.append(EXOT3TPThinningTool)

# Tracks and CaloClusters associated with TCCs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
EXOT3TCCTPThinningTool = DerivationFramework__TCCTrackParticleThinning(name                    = "EXOT3TCCTPThinningTool",
                                                                       ThinningService         = "EXOT3ThinningSvc",
                                                                       JetKey                  = "AntiKt10TrackCaloClusterJets",
                                                                       TCCKey                  = "TrackCaloClustersCombinedAndNeutral",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       CaloCalTopoClustersKey  = "CaloCalTopoClusters")
ToolSvc += EXOT3TCCTPThinningTool
thinningTools.append(EXOT3TCCTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT3MuonTPThinningTool",
                                                                         ThinningService         = "EXOT3ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3MuonTPThinningTool
thinningTools.append(EXOT3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT3ElectronTPThinningTool",
                                                                               ThinningService         = "EXOT3ThinningSvc",
                                                                               SGKey             	   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3ElectronTPThinningTool
thinningTools.append(EXOT3ElectronTPThinningTool)

# Tracks associated with Photons
EXOT3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT3PhotonTPThinningTool",
                                                                             ThinningService         = "EXOT3ThinningSvc",
                                                                             SGKey             	     = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3PhotonTPThinningTool
thinningTools.append(EXOT3PhotonTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT3AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "EXOT3AKt10JetTPThinningTool",
                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        SelectionString         = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt10JetTPThinningTool
thinningTools.append(EXOT3AKt10JetTPThinningTool)

# Calo-cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT3AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                 = "EXOT3Ak10CCThinningTool",
                                                                      ThinningService       = "EXOT3ThinningSvc",
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                      ConeSize              = 0)
ToolSvc += EXOT3AKt10CCThinningTool
thinningTools.append(EXOT3AKt10CCThinningTool)

#Truth Thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT3MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                = "EXOT3MCThinningTool",
        ThinningService     = "EXOT3ThinningSvc",
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
            "HLT_j480_a10_lcw_subjes_L1J100", # lowest unprescaled, 2017 (2.0e34 menu)
            # Trimmed large-R jet primary triggers (new 2017 nominal)
            "HLT_j460_a10t_lcw_jes_L1J100", # lowest unprescaled, 2017 (1.7e34 menu)
            "HLT_j480_a10t_lcw_jes_L1J100", # lowest unprescaled, 2017 (2.0e34 menu)
            # Trimmed large-R jets, with 1xmass cuts (substructure-enriched triggers)
            "HLT_j420_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_j440_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_j420_a10t_lcw_jes_40smcINF_L1J100", # lowest unprescaled, 2017 (1.7e34 menu)
            "HLT_j440_a10t_lcw_jes_40smcINF_L1J100", # lowest unprescaled, 2017 (2.0e34 menu)
            # Trimmed large-R jets, with 2xmass cuts (substructure-enriched triggers)
            "HLT_2j330_a10t_lcw_jes_L1J100", # prescaled, for mass bootstrap
            "HLT_2j330_a10t_lcw_jes_40smcINF_L1J100", # lowest unprescaled, 2017 (1.7e34 and 2.0e34 menus)
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
            "HLT_j480_a10r_L1J100", # lowest unprescaled, 2017 (2.0e34 menu)
            # HT triggers
            "HLT_ht850_L1J75",
            "HLT_ht850_L1J100",
            "HLT_ht850",
            "HLT_ht1000",
            "HLT_ht1000_L1J75",
            "HLT_ht1000_L1J100",
           ]

topology_selection_1jet = "((count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)  >= 1))"

topology_selection_2jet = "((count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)  >= 2)"

topology_selection_2jet_highpt = "(count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 1000*GeV)  >= 2))"

topology_selection_2jet += " || " + topology_selection_2jet_highpt

EXOT3_trigger1 = "(EF_g120_loose || EF_xe80_tclcw_tight)"
EXOT3_trigger2 = "(HLT_g120_loose || HLT_g140_loose || HLT_xe100 || HLT_xe90_mht_L1XE50 || HLT_xe90_tc_lcw_L1XE50)"
EXOT3_selection = "((count(Photons.pt > 100*GeV) > 0) || (count(Electrons.pt > 100*GeV) > 0))"

expression = '( ' + EXOT3_trigger2 + ' && ' + EXOT3_selection + ' && ' + topology_selection_1jet + ' )'# || ( ' + r2_trigger_selection + ' && ' + topology_selection_2jet + ' ) '
expression2 = topology_selection_2jet

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT3SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool(name = "EXOT3SkimmingTool1", expression = expression)
ToolSvc += EXOT3SkimmingTool1

EXOT3SkimmingTool2 = DerivationFramework__xAODStringSkimmingTool(name = "EXOT3SkimmingTool2", expression = expression2)
ToolSvc += EXOT3SkimmingTool2

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT3TriggerSkimmingTool",
                                                                    TriggerListAND = [],
                                                                    TriggerListOR  = triggers)
ToolSvc += EXOT3TriggerSkimmingTool

# topology_selection_2jet && triggers
EXOT3ANDSkimmingTool = DerivationFramework__FilterCombinationAND(name = "EXOT3ANDSkimmingTool", FilterList = [EXOT3SkimmingTool2, EXOT3TriggerSkimmingTool])
ToolSvc += EXOT3ANDSkimmingTool

# expression || (topology_selection_2jet && triggers)
EXOT3FinalSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT3ORSkimmingTool", FilterList = [EXOT3ANDSkimmingTool, EXOT3SkimmingTool1])
ToolSvc += EXOT3FinalSkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot3Seq

#=======================================
# JETS
#=======================================
# Create TCC objects
from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(exot3Seq, ToolSvc, "CaloCalTopoClusters", "InDetTrackParticles")

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT3"] = []
reducedJetList = [
    "AntiKt2PV0TrackJets", #flavour-tagged automatically
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets",
    "AntiKt10TrackCaloClusterJets"]
replaceAODReducedJets(reducedJetList,exot3Seq,"EXOT3")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets, addTCCTrimmedJets
addDefaultTrimmedJets(exot3Seq,"EXOT3")
addTCCTrimmedJets(exot3Seq,"EXOT3")

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
# A wrapper function which does all the necessary steps
addVRJets(exot3Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")
# Create variable-R trackjets and dress AntiKt10TCC with ghost VR-trkjet
# A wrapper function which does all the necessary steps
# addVRJetsTCC(exot3Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
             # VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
             # ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
             # variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

#b-tagging

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ["AntiKtVR30Rmax4Rmin02TrackJets"], Sequencer = exot3Seq)

#some jets collections are not included in the new jet restoring mechanism and need to be added the old way
#trimmed jets
addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, mods="groomed", algseq=exot3Seq, outputGroup="EXOT3")#AntiKt10PV0TrackTrimmedPtFrac5SmallR20Jets

#jet calibration
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot3Seq)

#=======================================
# SKIMMING, THINNING, AUGMENTATION
#=======================================
exot3Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3Kernel_skim", SkimmingTools = [EXOT3FinalSkimmingTool])
exot3Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3Kernel", ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT3Stream )
EXOT3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT3Stream.AcceptAlgs(["EXOT3Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT3ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT3ContentList import *

EXOT3SlimmingHelper = SlimmingHelper("EXOT3SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
EXOT3SlimmingHelper.SmartCollections = EXOT3SmartContent
EXOT3SlimmingHelper.ExtraVariables = EXOT3ExtraVariables

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
EXOT3SlimmingHelper.AllVariables = EXOT3AllVariablesContent

# Add jet collections created by derivation job
EXOT3SlimmingHelper.StaticContent = EXOT3StaticContent

addJetOutputs(EXOT3SlimmingHelper, ["EXOT3"])

EXOT3SlimmingHelper.AppendToDictionary = {}
listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets', 'AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets', 'AntiKt10PV0TrackTrimmedPtFrac5SmallR20Jets']

# Add VR track-jet collection and its b-tagging container to output stream
EXOT3SlimmingHelper.AppendToDictionary = {
    "AntiKtVR30Rmax4Rmin02TrackJets"            :   "xAOD::JetContainer"        ,
    "AntiKtVR30Rmax4Rmin02TrackJetsAux"         :   "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track"       :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"    :   "xAOD::BTaggingAuxContainer",
}

# Add all variabless for VR track-jets
EXOT3SlimmingHelper.AllVariables  += ["AntiKtVR30Rmax4Rmin02TrackJets"]

# Save certain b-tagging variables for VR track-jet
EXOT3SlimmingHelper.ExtraVariables += [
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c10_discriminant.MV2c100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP"
]

if globalflags.DataSource()=='geant4':
  listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
for i in listJets:
  EXOT3SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'
  EXOT3SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::JetAuxContainer'

# (Don't) Add jet trigger content
#EXOT3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT3SlimmingHelper.IncludeJetTriggerContent = True
EXOT3SlimmingHelper.IncludeEGammaTriggerContent = True

EXOT3SlimmingHelper.AppendContentToStream(EXOT3Stream)
