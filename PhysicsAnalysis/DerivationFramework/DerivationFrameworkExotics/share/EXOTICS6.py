#********************************************************************
# EXOTICS6.py 
# reductionConf flag EXOTICS6 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# THINNING TOOL 
#====================================================================

thinExpression = '(trk_d0_wrtPV < 1.5) && ((trk_z0_wrtPV * sin( trk_theta_wrtPV )) <= 1.5)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPThinningTool
EXOTICS6ThinningTool = DerivationFramework__NTUPThinningTool(   name = "EXOTICS6ThinningTool",
                                                                suffix = "_thin",
                                                                branchesForThinning = [ "trk_pt",
                                                                                        "trk_eta",
                                                                                        "trk_d0_wrtPV",
                                                                                        "trk_z0_wrtPV",
                                                                                        "trk_phi_wrtPV",
                                                                                        "trk_theta_wrtPV",
                                                                                        "trk_qoverp_wrtPV",
                                                                                        "trk_cov_d0_wrtPV",
                                                                                        "trk_cov_z0_wrtPV",
                                                                                        "trk_cov_phi_wrtPV",
                                                                                        "trk_cov_theta_wrtPV",
                                                                                        "trk_cov_qoverp_wrtPV",
                                                                                        "trk_d0_wrtBL",
                                                                                        "trk_d0_err_wrtBL",
                                                                                        "trk_eta_atCalo",
                                                                                        "trk_phi_atCalo",
                                                                                        "trk_eta_atCalo2ndLayer",
                                                                                        "trk_phi_atCalo2ndLayer",
                                                                                        "trk_chi2",
                                                                                        "trk_ndof",
                                                                                        "trk_nBLHits",
                                                                                        "trk_nPixHits",
                                                                                        "trk_nSCTHits",
                                                                                        "trk_nTRTHits",
                                                                                        "trk_nTRTHighTHits",
                                                                                        "trk_nPixHoles",
                                                                                        "trk_nSCTHoles",
                                                                                        "trk_nTRTHoles",
                                                                                        "trk_nPixelDeadSensors",
                                                                                        "trk_nSCTDeadSensors",
                                                                                        "trk_expectBLayerHit"],
                                                                expression = thinExpression)
ToolSvc += EXOTICS6ThinningTool

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = '(EF_g120_loose || EF_xe80_tclcw_tight) && ((count(ph_cl_pt) > 0) && (count(ph_cl_pt > 100*GeV) > 0)) || ((count(el_pt) > 0) && (count(el_pt > 100*GeV) > 0))'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
EXOTICS6SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "EXOTICS6SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOTICS6SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS6Kernel",
									SkimmingTools = [EXOTICS6SkimmingTool],
									ThinningTools = [EXOTICS6ThinningTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS6
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS6
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS6Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS6Stream.AddRequireAlgs( "EXOTICS6Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS6BranchList import EXOTICS6Branches
EXOTICS6Stream.AddItem(EXOTICS6Branches)
