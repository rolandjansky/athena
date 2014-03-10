#********************************************************************
# SM4.py 
# reductionConf flag SM4 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsSelection = '((mu_pt > 20*GeV) && (abs(mu_eta) < 2.6) && (mu_medium) && (muonPassesMCP))'
stacoSelection = '((mu_staco_pt > 20*GeV) && (abs(mu_staco_eta) < 2.6) && (mu_staco_isCombinedMuon) && (stacoPassesMCP))' 
electronSelection = '(recommendedEl_pt > 20*GeV) && (abs(recommendedEl_eta) < 2.6) && ( el_loosePP || isLLHElectron )'
electronMuonSelection = 'count('+electronSelection+') + count('+muonsSelection+') >= 1'
electronStacoSelection = 'count('+electronSelection+') + count('+stacoSelection+') >= 1'
expression = electronMuonSelection+' || '+electronStacoSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
SM4SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "SM4SkimmingTool1",
                                                                    expression = expression)
ToolSvc += SM4SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SM4Kernel",
									SkimmingTools = [SM4SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM4
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM4
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
SM4Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
SM4Stream.AddRequireAlgs( "SM4Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkSM.SM4BranchList import SM4Branches
SM4Stream.AddItem(SM4Branches)
