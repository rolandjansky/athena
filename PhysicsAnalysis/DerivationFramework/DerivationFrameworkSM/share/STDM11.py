#********************************************************************
# STDM11.py 
# reductionConf flag STDM11 in Reco_tf.py   
# Format for inclusive jets and leading jets measurements
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import (
    derivationFlags, buildFileName, MSMgr, DerivationFrameworkJob)
from DerivationFrameworkJetEtMiss.JetCommon import addJetOutputs
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkCore.LHE3WeightMetadata import addLHE3Weights

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_STDM11Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM11Stream )
STDM11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM11Stream.AcceptAlgs(["STDM11Kernel"])

isMC = globalflags.DataSource()=='geant4'

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

#=====================
# TRIGGER NAV THINNING
#=====================
# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM11ThinningHelper = ThinningHelper( "STDM11ThinningHelper" )

#trigger navigation content
STDM11ThinningHelper.TriggerChains = 'HLT_j.*|HLT_ht.*'
STDM11ThinningHelper.AppendToStream( STDM11Stream )

#===================== 
# TRUTH  THINNING
#=====================  
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
STDM11MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                = "STDM11MCThinningTool",
        ThinningService     = STDM11ThinningHelper.ThinningSvc(),
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
  ToolSvc += STDM11MCThinningTool
  thinningTools.append(STDM11MCThinningTool)


#====================================================================
# SKIMMING TOOL 
#====================================================================
triggers = ["HLT_noalg_mb_L1J12","HLT_mb_sptrk","HLT_j0_perf_L1RD0_FILLED","HLT_j15","HLT_j25","HLT_j35","HLT_j55","HLT_j60","HLT_j85","HLT_j100","HLT_j110","HLT_j150","HLT_j175","HLT_j200","HLT_j260","HLT_j300","HLT_j320","HLT_j360","HLT_j380","HLT_j400","HLT_j420","HLT_j440","HLT_j450","HLT_j460","HLT_j420_320eta490","HLT_j400_320eta490","HLT_j380_320eta490","HLT_j260_320eta490","HLT_j200_320eta490","HLT_j175_320eta490","HLT_j150_320eta490","HLT_j110_320eta490","HLT_j85_320eta490","HLT_j60_320eta490","HLT_j55_320eta490","HLT_j45_320eta490","HLT_j35_320eta490","HLT_j25_320eta490","HLT_j15_320eta490"]

topology_selection_jet = "( (count  (AntiKt4EMTopoJets.pt > 60*GeV) >= 1)  || (count ((AntiKt4EMPFlowJets.pt > 60*GeV) ) >= 1) || (count ((AntiKt4EMTopoJets.pt > 15*GeV) && (EventInfo.averageInteractionsPerCrossing < 5)) >= 1) || (count ((AntiKt4EMPFlowJets.pt > 15*GeV) && (EventInfo.averageInteractionsPerCrossing < 5)) >= 1) )"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM11SkimmingTool2 = DerivationFramework__xAODStringSkimmingTool( name = "STDM11SkimmingTool2",
                                                                    expression = topology_selection_jet)
ToolSvc += STDM11SkimmingTool2

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
STDM11TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "STDM11TriggerSkimmingTool",
                                                                     TriggerListAND = [],
                                                                     TriggerListOR  = triggers)
ToolSvc += STDM11TriggerSkimmingTool

STDM11ANDSkimmingTool = DerivationFramework__FilterCombinationAND(name = "STDM11ANDSkimmingTool", FilterList = [STDM11SkimmingTool2, STDM11TriggerSkimmingTool])
ToolSvc += STDM11ANDSkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM11Sequence = CfgMgr.AthSequencer("STDM11Sequence")

# ADD KERNEL 
STDM11Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM11Kernel",
                                                                 SkimmingTools = [STDM11ANDSkimmingTool],
                                                                 ThinningTools = thinningTools)

# JET REBUILDING
reducedJetList = ["AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, STDM11Sequence, "STDM11Jets")

# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM11Sequence

#====================================================================
# Jet reconstruction/retagging
#====================================================================s
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = STDM11Sequence)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM11SlimmingHelper = SlimmingHelper("STDM11SlimmingHelper")
STDM11SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                         "AntiKt4EMPFlowJets", 					
                                         "PrimaryVertices"]

STDM11SlimmingHelper.ExtraVariables = ExtraContentJets

addJetOutputs(STDM11SlimmingHelper,["STDM11","STDM11Jets"])

STDM11SlimmingHelper.IncludeJetTriggerContent = True

STDM11SlimmingHelper.AppendContentToStream(STDM11Stream)
