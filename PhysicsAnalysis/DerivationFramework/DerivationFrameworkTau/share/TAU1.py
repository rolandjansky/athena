#********************************************************************
# TAU1.py 
# reductionConf flag TAU1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = 'count((EF_mu24i_tight == 1) || (EF_mu36_tight == 1) || (EF_e24vh_medium1 == 1) || (EF_e24vhi_medium1 == 1) || (EF_e45_medium1 == 1) || (EF_e60_medium1 == 1)) >= 1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
TAU1SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "TAU1SkimmingTool",
                                                                expression = expression)

ToolSvc += TAU1SkimmingTool



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TAU1Kernel",
									SkimmingTools = [TAU1SkimmingTool]
                                                                      )




#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_TAU1
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_TAU1
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
TAU1Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
TAU1Stream.AddRequireAlgs( "TAU1Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkTau.TAU1BranchList import TAU1Branches
TAU1Stream.AddItem(TAU1Branches)
