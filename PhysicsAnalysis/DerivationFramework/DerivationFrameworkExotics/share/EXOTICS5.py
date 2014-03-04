#********************************************************************
# EXOTICS5.py 
# reductionConf flag EXOTICS5 in Reco_tf.py   
#********************************************************************
# NEED TO ADD TRUTH THINNING
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = '(EF_xe80_tclcw || EF_xe80_tclcw_loose || EF_e24vhi_medium1 || EF_e60_medium1 || EF_mu24i_tight || EF_mu36_tight) && ( (count(jet_AntiKt4TopoEM_pt > 100*GeV) >= 1 ) || (count(jet_AntiKt4LCTopo_pt > 100*GeV) >= 1) )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
EXOTICS5SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "EXOTICS5SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOTICS5SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS5Kernel",
									SkimmingTools = [EXOTICS5SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS5
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS5
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS5Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS5Stream.AddRequireAlgs( "EXOTICS5Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS5BranchList import EXOTICS5Branches
EXOTICS5Stream.AddItem(EXOTICS5Branches)
