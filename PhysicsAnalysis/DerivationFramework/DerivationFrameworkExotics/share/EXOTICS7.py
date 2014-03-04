#********************************************************************
# EXOTICS7.py 
# reductionConf flag EXOTICS7 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

skimExpression = '(jet_AntiKt4LCTopo_n + jet_AntiKt10LCTopo_n) > 2'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
EXOTICS7SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "EXOTICS7SkimmingTool1",
                                                                    expression = skimExpression)
ToolSvc += EXOTICS7SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS7Kernel",
									SkimmingTools = [EXOTICS7SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS7
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS7
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS7Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS7Stream.AddRequireAlgs( "EXOTICS7Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS7BranchList import EXOTICS7Branches
EXOTICS7Stream.AddItem(EXOTICS7Branches)
