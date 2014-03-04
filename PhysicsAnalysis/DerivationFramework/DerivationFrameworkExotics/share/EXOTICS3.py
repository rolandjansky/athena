#********************************************************************
# EXOTICS3.py 
# reductionConf flag EXOTICS3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOTICS3Kernel")

#====================================================================
# SET UP STREAM   
#====================================================================
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS3
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_EXOTICS3
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
tree_name=ntupProdFlags.TreeName()
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
EXOTICS3Stream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
EXOTICS3Stream.AddRequireAlgs( "EXOTICS3Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExotics.EXOTICS3BranchList import EXOTICS3Branches
EXOTICS3Stream.AddItem(EXOTICS3Branches)
