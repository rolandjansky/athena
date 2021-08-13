#====================================================================
# TRIG1.py
# For use in menuXtreme 
#====================================================================
# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
#====================================================================
# FIND THE RUN NUMBER (NEEDED TO LOCATE XMLs)
#====================================================================
import PyUtils.AthFile as athFile
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
af = athFile.fopen(svcMgr.EventSelector.InputCollections[0])
run_number = af.run_number[0]
#====================================================================
# SETUP BUNCH CROSSING TOOL - WE WILL ALWAYS RUN OVER EB DATA
#====================================================================
from AthenaCommon.GlobalFlags import globalflags
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
#globalflags.DataSource = 'data' 
globalflags.DatabaseInstance = 'CONDBR2'

from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()

#====================================================================
# SKIMMING TOOLS 
#====================================================================
# Set up your skimming tools (you can have as many as you need). 
# The tools must use the DerivationFrameworkInterfaces as in this example.
# Currently no skimming - keep all events
#====================================================================
# AUGMENTATION TOOLS 
# Used to decorate enhanced bias weighting quantites for rate estimations
#====================================================================
#
# TODO this doesn't work - component not yet exposed
from EnhancedBiasWeighter.EnhancedBiasWeighterConf import EnhancedBiasWeighter
TRIG1AugmentationTool = EnhancedBiasWeighter(name = "TRIG1AugmentationTool")
TRIG1AugmentationTool.RunNumber = run_number
TRIG1AugmentationTool.UseBunchCrossingData = True
# These are needed for MC rates
#TRIG1AugmentationTool.MCCrossSection = xxx
#TRIG1AugmentationTool.MCFilterEfficiency = xxx
#TRIG1AugmentationTool.MCKFactor = xxx
ToolSvc += TRIG1AugmentationTool
print TRIG1AugmentationTool
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# The name of the kernel (LooseSkimKernel in this case) must be unique to this 
# derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRIG1Kernel",
SkimmingTools = [],
AugmentationTools = [TRIG1AugmentationTool])
#====================================================================
# SET UP STREAM   
#====================================================================
# SKIMMING
# The base name (DAOD_TRIG1 here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_TRIG1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TRIG1Stream )
TRIG1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRIG1Stream.AcceptAlgs(["TRIG1Kernel"])
#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRIG1SlimmingHelper = SlimmingHelper("TRIG1SlimmingHelper")
##Smart Slimming
TRIG1SlimmingHelper.SmartCollections = [
]
TRIG1SlimmingHelper.AllVariables = [
      "LVL1EmTauRoIs",
      "LVL1EnergySumRoIs",
      "LVL1JetEtRoIs",
      "LVL1JetRoIs",
      "LVL1MuonRoIs",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
      "HLT_xAOD__JetContainer_a4tclcwsubjesFS",
]
TRIG1SlimmingHelper.IncludeMuonTriggerContent   = True
TRIG1SlimmingHelper.IncludeEGammaTriggerContent = True
TRIG1SlimmingHelper.IncludeJetTriggerContent    = True
TRIG1SlimmingHelper.IncludeTauTriggerContent    = True
TRIG1SlimmingHelper.IncludeEtMissTriggerContent = True
TRIG1SlimmingHelper.IncludeBJetTriggerContent = True
TRIG1SlimmingHelper.IncludeBPhysTriggerContent = True
TRIG1SlimmingHelper.IncludeMinBiasTriggerContent = True
TRIG1SlimmingHelper.AppendContentToStream(TRIG1Stream)
