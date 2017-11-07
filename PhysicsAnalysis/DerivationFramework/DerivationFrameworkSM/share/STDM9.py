#********************************************************************
# STDM9.py 
# reductionConf flag STDM9 in Reco_tf.py   
#
# This derivation is modeled after EXOT3.
# It is designed for jet-substructure-based precision measurements.
#
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkSM import STDMTriggers

from JetRec.JetRecStandard import jtm

from DerivationFrameworkCore.WeightMetadata import *

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *



stdm9Seq = CfgMgr.AthSequencer("STDM9Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_STDM9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM9Stream )
STDM9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM9Stream.AcceptAlgs(["STDM9Kernel"])

from AthenaCommon.GlobalFlags import globalflags
isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True





#====================================================================
# THINNING TOOL 
#====================================================================

thinningTools = []


#=====================
# TRIGGER NAV THINNING
#=====================

# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM9ThinningHelper = ThinningHelper( "STDM9ThinningHelper" )

#trigger navigation content
STDM9ThinningHelper.TriggerChains = 'HLT_j.*|HLT_ht.*'
STDM9ThinningHelper.AppendToStream( STDM9Stream )
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#===================== 
# TRACK  THINNING
#=====================  

thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
STDM9TPThinningTool = DerivationFramework__TrackParticleThinning(name = "STDM9TPThinningTool",
                                                                 ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9TPThinningTool
thinningTools.append(STDM9TPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM9MuonTPThinningTool",
                                                                            ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9MuonTPThinningTool
thinningTools.append(STDM9MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(     name                    = "STDM9ElectronTPThinningTool",
                                                                                        ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                                        SGKey               = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9ElectronTPThinningTool
thinningTools.append(STDM9ElectronTPThinningTool)

STDM9PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(     name                    = "STDM9PhotonTPThinningTool",
                                                                                        ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                                        SGKey               = "Photons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9PhotonTPThinningTool
thinningTools.append(STDM9PhotonTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM9AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                = "STDM9AKt4JetTPThinningTool",
                                                                        ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4LCTopoJets",
                                                                        SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9AKt4JetTPThinningTool
thinningTools.append(STDM9AKt4JetTPThinningTool)

STDM9AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "STDM9AKt10JetTPThinningTool",
                                                                        ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        SelectionString         = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9AKt10JetTPThinningTool
thinningTools.append(STDM9AKt10JetTPThinningTool)

# Calo-cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

STDM9AKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "STDM9Ak4CCThinningTool",
                                                                      ThinningService       = STDM9ThinningHelper.ThinningSvc(),
                                                                      SGKey                 = "AntiKt4LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt4LCTopoJets.pt > 150*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                      ConeSize              = 0)
ToolSvc += STDM9AKt4CCThinningTool
thinningTools.append(STDM9AKt4CCThinningTool)


STDM9AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "STDM9Ak10CCThinningTool",
                                                                      ThinningService       = STDM9ThinningHelper.ThinningSvc(),
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                      ConeSize              = 0)
ToolSvc += STDM9AKt10CCThinningTool
thinningTools.append(STDM9AKt10CCThinningTool)

#Truth Thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
STDM9MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                = "STDM9MCThinningTool",
        ThinningService     = STDM9ThinningHelper.ThinningSvc(),
        WriteEverything     = False,
        WritePartons        = True,
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
        WriteFirstN         = -1)

if isMC:
  ToolSvc += STDM9MCThinningTool
  thinningTools.append(STDM9MCThinningTool)

#====================================================================
# SKIMMING TOOL 
#====================================================================

triggers = [
            "HLT_j260_a10_lcw_L1J75",
            "HLT_j260_a10_lcw_nojcalib_L1J75",
            "HLT_j260_a10_nojcalib_L1J75",
            "HLT_j260_a10_sub_L1J75",
            "HLT_j260_a10_lcw_sub_L1J75",
            "HLT_j300_a10_sub_L1J75",
            "HLT_j300_a10_lcw_L1J75",
            "HLT_j360_a10_lcw_L1J100",
            "HLT_j380_a10_lcw_L1J100",
            "HLT_j400_a10_lcw_L1J100",
            "HLT_j420_a10_lcw_L1J100",
            "HLT_j440_a10_lcw_L1J100",
            "HLT_j460_a10_sub_L1J100",
            "HLT_j460_a10_lcw_L1J100",
            "HLT_j460_a10_nojcalib_L1J100",
            "HLT_j460_a10_lcw_nojcalib_L1J100",
            "HLT_j460_a10_lcw_sub_L1J100",
            "HLT_j300_a10_lcw_sub_L1SC85",
            "HLT_j420_a10_lcw_subjes_L1J100",
            "HLT_j440_a10_lcw_subjes_L1J100",
            "HLT_j460_a10_lcw_subjes_L1J100",
            "HLT_j480_a10_lcw_subjes_L1J100",
            "HLT_j300_a10_lcw_L1SC85",
            "HLT_j260_a10r_L1J75",
            "HLT_j300_a10r_L1J75",
            "HLT_j360_a10r_L1J100",
            "HLT_j380_a10r_L1J100",
            "HLT_j400_a10r_L1J100",
            "HLT_j420_a10r_L1J100",
            "HLT_j440_a10r_L1J100",
            "HLT_j460_a10r_L1J100",
            "HLT_j480_a10r_L1J100",
            "HLT_ht850_L1J75",
            "HLT_ht850_L1J100",
            "HLT_ht850",
            "HLT_ht1000",
            "HLT_ht1000_L1J75",
            "HLT_ht1000_L1J100",
           ]

#From STDM1
triggers+=["HLT_j15","HLT_j25","HLT_j35","HLT_j55","HLT_j60","HLT_j85","HLT_j100","HLT_j110","HLT_j150","HLT_j175","HLT_j200","HLT_j260","HLT_j300","HLT_j320","HLT_j360","HLT_j380","HLT_j400","HLT_j420","HLT_j440","HLT_j450","HLT_j460"]

topology_selection_2jet = "((count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV)  >= 2)"
topology_selection_2jet += " || (count (abs(AntiKt4LCTopoJets.eta) < 2.8 && AntiKt4LCTopoJets.pt > 100*GeV)  >= 2))"

expression2 = topology_selection_2jet

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM9SkimmingTool2 = DerivationFramework__xAODStringSkimmingTool( name = "STDM9SkimmingTool2",
                                                                    expression = topology_selection_2jet)
ToolSvc += STDM9SkimmingTool2

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
STDM9TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "STDM9TriggerSkimmingTool",
                                                                     TriggerListAND = [],
                                                                     TriggerListOR  = triggers)
ToolSvc += STDM9TriggerSkimmingTool

STDM9ANDSkimmingTool = DerivationFramework__FilterCombinationAND(name = "STDM9ANDSkimmingTool", FilterList = [STDM9SkimmingTool2, STDM9TriggerSkimmingTool])
ToolSvc += STDM9ANDSkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# JET REBUILDING
reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, stdm9Seq, "STDM9Jets")

DerivationFrameworkJob += stdm9Seq

#=======================================
# Augmentation: Jet Collections
#=======================================

from JetRec.JetRecConf import JetAlgorithm

addDefaultTrimmedJets(stdm9Seq, "STDM9")
#addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, algseq=stdm9Seq, outputGroup="STDM9")
applyJetCalibration_xAODColl("AntiKt4EMTopo",stdm9Seq) # adds this to DerivationFrameworkJob by default
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", stdm9Seq)

stdm9Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM9Kernel_skim", SkimmingTools = [STDM9ANDSkimmingTool])
stdm9Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM9Kernel", ThinningTools = thinningTools)
# Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="STDM9ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDM9ContentList import *

STDM9SlimmingHelper = SlimmingHelper("STDM9SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
STDM9SlimmingHelper.SmartCollections = STDM9SmartContent
STDM9SlimmingHelper.ExtraVariables = STDM9ExtraVariables
if isMC:
  STDM9SlimmingHelper.ExtraVariables = STDM9ExtraVariablesTruth

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
STDM9SlimmingHelper.AllVariables = STDM9AllVariablesContent

# Add jet collections created by derivation job
STDM9SlimmingHelper.StaticContent = STDM9StaticContent

addJetOutputs(STDM9SlimmingHelper, ["STDM9","STDM9Jets"])

STDM9SlimmingHelper.AppendToDictionary = {}
listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets']
if globalflags.DataSource()=='geant4':
  listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
for i in listJets:
  STDM9SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'

# (Dont) Add jet triger content
#STDM9SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
STDM9SlimmingHelper.IncludeJetTriggerContent = True
STDM9SlimmingHelper.IncludeEGammaTriggerContent = True

STDM9SlimmingHelper.AppendContentToStream(STDM9Stream)
