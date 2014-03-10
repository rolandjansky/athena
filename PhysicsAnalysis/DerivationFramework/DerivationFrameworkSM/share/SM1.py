#********************************************************************
# SM1.py 
# reductionConf flag SM1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SM1Kernel")

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM1
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_SM1
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
SM1Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
SM1Stream.AddRequireAlgs( "SM1Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkSM.SM1BranchList import SM1Branches
SM1Stream.AddItem(SM1Branches)
