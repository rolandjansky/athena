#********************************************************************
# TAU3.py 
# reductionConf flag TAU3 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

muRequirement = 'count( (mu_staco_pt > 22000) && (mu_staco_isCombinedMuon == 1) && (mu_staco_nBLHits > 0) && (mu_staco_nPixHits > 1) && (mu_staco_nSCTHits > 5) && (mu_staco_nPixHoles+mu_staco_nSCTHoles < 3) ) >= 1'
tauRequirement = 'count( (tau_pt > 18000.0) && (abs(tau_charge) ==1) && ( (tau_numTrack == 1) || (tau_numTrack == 3) )  ) >= 1'
pvRequirement = 'count( (vxp_type == 1) && (vxp_nTracks > 3) ) >= 1'
expression = muRequirement+' && '+tauRequirement+' && '+pvRequirement

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
TAU3SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "TAU3SkimmingTool",
                                                                expression = expression)

ToolSvc += TAU3SkimmingTool



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TAU3Kernel",
									SkimmingTools = [TAU3SkimmingTool]
                                                                      )




#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_TAU3
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_TAU3
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
TAU3Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
TAU3Stream.AddRequireAlgs( "TAU3Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkTau.TAUGeneralBranchList import TAUGeneralBranches
TAU3Stream.AddItem(TAUGeneralBranches)
