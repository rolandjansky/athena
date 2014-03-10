#********************************************************************
# SM5.py 
# reductionConf flag SM5 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

stacoSelection = '(mu_staco_loose && mu_staco_pt > 10*GeV)'
muonsSelection = '(mu_loose && mu_pt > 10*GeV)'
electronSelection = '(el_loosePP || isLLHElectron || isMLElectron) && (recommendedEl_pt > 10*GeV)'
electronSelection = '(el_loosePP) && (recommendedEl_pt > 10*GeV)'
muonsOnly = '(count('+stacoSelection+') >= 3) || (count('+muonsSelection+') >= 3)'
electronsOnly = '(count('+electronSelection+') >= 3)'
twoMuonsOneElectron = '((count('+stacoSelection+') >= 2) || (count('+muonsSelection+') >= 2) ) && (count('+electronSelection+') > 1)'
oneMuonTwoElectrons = '((count('+stacoSelection+') >= 1) || (count('+muonsSelection+') >= 1) ) && (count('+electronSelection+') > 2)' 
expression = muonsOnly+' || '+electronsOnly+' || '+twoMuonsOneElectron+' || '+oneMuonTwoElectrons

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
SM5SkimmingTool = DerivationFramework__NTUPStringSkimmingTool( name = "SM5SkimmingTool1",
                                                                    expression = expression)
ToolSvc += SM5SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SM5Kernel",
									SkimmingTools = [SM5SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM5
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM5
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
SM5Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
SM5Stream.AddRequireAlgs( "SM5Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkSM.SM5BranchList import SM5Branches
SM5Stream.AddItem(SM5Branches)
