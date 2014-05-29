#====================================================================
# TOP1.py 
# reductionConf flag TOP1 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
#from DerivationFrameworkMCTruth.MCTruthCommon import *

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TOP1Kernel")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TOP1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOP1Stream )
TOP1Stream = MSMgr.NewPoolStream( streamName, fileName )
TOP1Stream.AcceptAlgs(["TOP1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTop.TOP1ContentList import TOP1Content
TOP1SlimmingHelper = SlimmingHelper("TOP1SlimmingHelper")
TOP1SlimmingHelper.UserContent = TOP1Content
TOP1SlimmingHelper.AppendContentToStream(TOP1Stream)
