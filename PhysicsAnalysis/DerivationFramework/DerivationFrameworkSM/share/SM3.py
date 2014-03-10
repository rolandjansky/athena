#********************************************************************
# SM3.py 
# reductionConf flag SM3 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsSelection = '((mu_pt > 15*GeV) && (abs(mu_eta) < 2.6) && (mu_medium) && (muonPassesMCP))'
stacoSelection = '((mu_staco_pt > 15*GeV) && (abs(mu_staco_eta) < 2.6) && (mu_staco_isCombinedMuon) && (stacoPassesMCP))' 
muonOnlySelection = '(count('+muonsSelection+') >= 2) || (count('+stacoSelection+') >= 2)'
electronSelection = '(recommendedEl_pt > 15*GeV) && (abs(recommendedEl_eta) < 2.6) && ( el_loosePP || isLLHElectron )'
electronOnlySelection = 'count('+electronSelection+') >= 2'
electronMuonSelection = 'count('+electronSelection+') + count('+muonsSelection+') >= 2'
electronStacoSelection = 'count('+electronSelection+') + count('+stacoSelection+') >= 2'
expression = muonOnlySelection+' || '+electronOnlySelection+' || '+electronMuonSelection+' || '+electronStacoSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
SM3SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "SM3SkimmingTool1",
                                                                    expression = expression)
ToolSvc += SM3SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SM3Kernel",
									SkimmingTools = [SM3SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM3
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM3
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
SM3Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
SM3Stream.AddRequireAlgs( "SM3Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkSM.SM3BranchList import SM3Branches
SM3Stream.AddItem(SM3Branches)
