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
from DerivationFrameworkSM import STDMTriggers
from JetRec.JetRecStandard import jtm
from DerivationFrameworkCore.WeightMetadata import *

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

# Add Truth MetaData
if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import *

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

thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 3.0)'
STDM9TPThinningTool = DerivationFramework__TrackParticleThinning(name = "STDM9TPThinningTool",
                                                                 ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9TPThinningTool
thinningTools.append(STDM9TPThinningTool)


#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM9AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "STDM9AKt10JetTPThinningTool",
                                                                        ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        SelectionString         = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9AKt10JetTPThinningTool
thinningTools.append(STDM9AKt10JetTPThinningTool)

STDM9AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                = "STDM9AKt4JetTPThinningTool",
                                                                        ThinningService         = STDM9ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        SelectionString         = "AntiKt4EMTopoJets.pt > 15*GeV && abs(AntiKt4EMTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM9AKt4JetTPThinningTool
thinningTools.append(STDM9AKt4JetTPThinningTool)


# Calo-cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

STDM9AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "STDM9Ak10CCThinningTool",
                                                                      ThinningService       = STDM9ThinningHelper.ThinningSvc(),
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                      AdditionalClustersKey = ["LCOriginTopoClusters"])
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

triggers = ["HLT_j360","HLT_j340","HLT_j380","HLT_j400","HLT_j420","HLT_j450","HLT_j420"]

topology_selection_2jet = "((count (abs(AntiKt4EMTopoJets.eta) < 2.5 && AntiKt4EMTopoJets.pt > 450*GeV) >= 2))"

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

applyJetCalibration_xAODColl("AntiKt4EMTopo",stdm9Seq) # adds this to DerivationFrameworkJob by default

stdm9Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM9Kernel_skim", SkimmingTools = [STDM9ANDSkimmingTool])
stdm9Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM9Kernel", ThinningTools = thinningTools)

addRscanJets("AntiKt",0.6,"LCTopo",stdm9Seq,"STDM9")

# QGTaggerTool ###
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=stdm9Seq, algname="QGTaggerToolAlg")

#improved fJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation,getPFlowfJVT
# MVfJvt #
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=stdm9Seq, algname='JetForwardJvtToolBDTAlg')
# PFlow fJvt #
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=stdm9Seq, algname='JetForwardPFlowJvtToolAlg')

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

STDM9SlimmingHelper.AppendToDictionary = {}

addJetOutputs(STDM9SlimmingHelper, ["STDM9","STDM9Jets"])

# (Dont) Add jet triger content
STDM9SlimmingHelper.IncludeJetTriggerContent = True

addOriginCorrectedClusters(STDM9SlimmingHelper, writeLC=True, writeEM=True)

STDM9SlimmingHelper.AppendContentToStream(STDM9Stream)
