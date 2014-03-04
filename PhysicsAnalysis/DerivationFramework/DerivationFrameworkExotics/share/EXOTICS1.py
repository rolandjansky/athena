#********************************************************************
# EXOTICS1.py 
# reductionConf flag EXOTICS1 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = '(EF_j170_a4tchad_ht700 || EF_j15_a4tchad || EF_j25_a4tchad || EF_j35_a4tchad || EF_j45_a4tchad || EF_j55_a4tchad || EF_j80_a4tchad || EF_j110_a4tchad || EF_j145_a4tchad || EF_j180_a4tchad || EF_j220_a4tchad || EF_j280_a4tchad || EF_j360_a4tchad || EF_j460_a4tchad)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
EXOTICS1SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "EXOTICS1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOTICS1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS1Kernel",
									SkimmingTools = [EXOTICS1SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS1
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS1
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS1Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS1Stream.AddRequireAlgs( "EXOTICS1Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS1BranchList import EXOTICS1Branches
EXOTICS1Stream.AddItem(EXOTICS1Branches)
