#====================================================================
# TCAL2.py
# Test set-up
# It requires the reductionConf flag TCAL2 in Reco_tf.py  
#====================================================================

if not 'TileSaveMBTS' in dir():
    TileSaveMBTS = True

if not 'TileSaveE4prime' in dir():
    TileSaveE4prime = True


# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# CP GROUP TOOLS
#====================================================================

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
TCAL2AugmentationTools = []

if TileSaveMBTS:
    TCAL2MbtsToVectorsTool = CfgMgr.DerivationFramework__MbtsToVectorsTool(name = 'TCAL2MbtsToVectorsTool'
                                                                           , Prefix = 'TCAL2_mbts')
    ToolSvc += TCAL2MbtsToVectorsTool
    TCAL2AugmentationTools.append(TCAL2MbtsToVectorsTool)
    print TCAL2MbtsToVectorsTool

if TileSaveE4prime:
    TCAL2E4prToVectorsTool = CfgMgr.DerivationFramework__MbtsToVectorsTool(name = 'TCAL2E4prToVectorsTool'
                                                                           , SaveEtaPhiInfo = False
                                                                           , Prefix = 'TCAL2_e4pr'
                                                                           , CellContainer = 'E4prContainer')
    ToolSvc += TCAL2E4prToVectorsTool
    TCAL2AugmentationTools.append(TCAL2E4prToVectorsTool)
    print TCAL2E4prToVectorsTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TCAL2Kernel"
                                                                       , AugmentationTools = TCAL2AugmentationTools)


#====================================================================
# SET UP STREAM  
#====================================================================
streamName = derivationFlags.WriteDAOD_TCAL2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TCAL2Stream )
TCAL2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TCAL2Stream.AcceptAlgs(["TCAL2Kernel"])


#====================================================================
# CONTENT DEFINITION
#====================================================================

TCAL2Stream.AddItem("std::vector<float>#TCAL2_*")
TCAL2Stream.AddItem("std::vector<int>#TCAL2_*")

