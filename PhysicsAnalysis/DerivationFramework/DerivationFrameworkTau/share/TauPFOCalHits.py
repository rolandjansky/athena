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
# Note: seems that we can now use TauTruthCommon directly
from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
scheduleTauTruthTools()

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
