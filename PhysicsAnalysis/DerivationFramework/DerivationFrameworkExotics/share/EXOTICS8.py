#********************************************************************
# EXOTICS8.py 
# reductionConf flag EXOTICS8 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# OPENING ANGLE TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
EXOTICS8DeltaRTool = DerivationFramework__DeltaRTool( name = "EXOTICS8DeltaRTool",
                                                      StoreGateEntryName = "dRkt4kt10",
                                                      Phi1BranchName = "jet_AntiKt10LCTopo_phi",
                                                      Phi2BranchName = "jet_AntiKt4LCTopo_phi",
                                                      Eta1BranchName = "jet_AntiKt10LCTopo_eta",
						      Eta2BranchName = "jet_AntiKt4LCTopo_eta")
ToolSvc += EXOTICS8DeltaRTool

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = '(jet_AntiKt4LCTopo_n > 1) && (jet_AntiKt10LCTopo_n > 1) && (count(dRkt4kt10 > 1.0) > 1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
EXOTICS8SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "EXOTICS8SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOTICS8SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS8Kernel",
									AugmentationTools = [EXOTICS8DeltaRTool],
									SkimmingTools = [EXOTICS8SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS8
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS8
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS8Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS8Stream.AddRequireAlgs( "EXOTICS8Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS8BranchList import EXOTICS8ExcludedBranches
EXOTICS8Stream.RemoveItem(EXOTICS8ExcludedBranches)
