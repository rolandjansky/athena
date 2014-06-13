#********************************************************************
# HIGGS5D1.py 
# reductionConf flag HIGGS5D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGGS5D1Kernel")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGGS5D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGGS5D1Stream )
HIGGS5D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGGS5D1Stream.AcceptAlgs(["HIGGS5D1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGGS5D1ContentList import HIGGS5D1Content
HIGGS5D1SlimmingHelper = SlimmingHelper("HIGGS5D1SlimmingHelper")
HIGGS5D1SlimmingHelper.UserContent = HIGGS5D1Content
HIGGS5D1SlimmingHelper.AppendContentToStream(HIGGS5D1Stream)
