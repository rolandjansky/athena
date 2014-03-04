#********************************************************************
# EXOTICS0.py 
# reductionConf flag EXOTICS0 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SPECIAL EXOTICS TRUTH THINNING TOOL 
#==================================================================== 

from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__ExoticsTruthThinning
EXOTICS0TruthThinningTool = DerivationFramework__ExoticsTruthThinning( name = "EXOTICS0TruthThinningTool" )
ToolSvc += EXOTICS0TruthThinningTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS0Kernel",
									ThinningTools = [EXOTICS0TruthThinningTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS0
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS0
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS0Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS0Stream.AddRequireAlgs( "EXOTICS0Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS0BranchList import EXOTICS0Branches
EXOTICS0Stream.AddItem(EXOTICS0Branches)
