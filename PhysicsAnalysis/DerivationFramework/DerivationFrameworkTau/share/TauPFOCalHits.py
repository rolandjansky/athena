#********************************************************************
# DerivationFrameworkTau/TauPFOCalHits.py
# Decorates neutral PFOs with summary calibration hit information
#********************************************************************

## NOTE: For some reason using DerivationFrameworkJob from DerivationFrameworkMaster
##       causes a segfault when running the truth matching, so we have to create 
##       our own AthSequencer
#from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob = CfgMgr.AthSequencer("TauPFOCalHitSequencer")


#===============================
# SETUP TAU TRUTH MATCHING TOOLS 
#===============================
### NOTE: Since we created our own AthSequencer, we can't just import TauTruthCommon, as it
##        adds tools to the derivations AthSequencer :-(. So we need to duplicate the TauTruthCommon.py
##        code here. Maybe could refactor a little to avoid this in the future. 
#from DerivationFrameworkTau.TauTruthCommon import *
##======================== COPY OF TauTruthCommon.py ======================##
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool

DFCommonTauTruthWrapperTools = []

# Tau Truth making and matching
# Set up the MCTruthClassifier
from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
DFCommonTauTruthClassifier = MCTruthClassifier(name = "DFCommonTauTruthClassifier",
                                    ParticleCaloExtensionTool="")
ToolSvc += DFCommonTauTruthClassifier

# Build the truth taus
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau
DFCommonTruthTauTool = DerivationFramework__TruthCollectionMakerTau(name             = "DFCommonTruthTauTool",
                                                             NewCollectionName       = "TruthTaus",
                                                             MCTruthClassifier       = DFCommonTauTruthClassifier)
ToolSvc += DFCommonTruthTauTool
DFCommonTauTruthWrapperTools.append(DFCommonTruthTauTool)

# Matching
# Only do if working with AOD
from RecExConfig.ObjKeyStore import objKeyStore
if objKeyStore.isInInput( "xAOD::TauJetContainer", "TauJets" ):    
    DFCommonTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="DFCommonTauTruthMatchingTool")
    ToolSvc += DFCommonTauTruthMatchingTool
    DFCommonTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "DFCommonTauTruthMatchingWrapper",
                                                                                    TauTruthMatchingTool = DFCommonTauTruthMatchingTool,
                                                                                    TauContainerName     = "TauJets")
    ToolSvc += DFCommonTauTruthMatchingWrapper
    print DFCommonTauTruthMatchingWrapper
    DFCommonTauTruthWrapperTools.append(DFCommonTauTruthMatchingWrapper)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TauTruthCommonKernel",
                                                                         AugmentationTools = DFCommonTauTruthWrapperTools)

##======================== END COPY OF TauTruthCommon.py =============================##

#=======================
# SETUP CALHIT DECORATOR 
#=======================
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauPFOCalHitDecorator
TauPFOCalHitDecorator = DerivationFramework__TauPFOCalHitDecorator(
    name              = "TauPFOCalHitDecorator",
    TauContainerName  = "TauJets",
    )
ToolSvc += TauPFOCalHitDecorator

# Kernel algorithm
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TauPFOCalHitsKernel",
                                AugmentationTools = [TauPFOCalHitDecorator])
topSequence += DerivationFrameworkJob

# EOF
