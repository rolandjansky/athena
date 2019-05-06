#====================================================================
# PassThroughExample.py
# This an example job options script showing how to set up a 
# derivation that neither skims, thins or slims: all events are written
# in full to the output. 
# If an AODFix is scheduled in the release, this will be run,
# so it will not be an exact copy of the input   
# It requires the reductionConf flag PASSTHR in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PASSTHRKernel")

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_PASSTHR here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_PASSTHRStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PASSTHRStream )
PASSTHRStream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
PASSTHRStream.AcceptAlgs(["PASSTHRKernel"])

#====================================================================
# CONTENT LIST  
#====================================================================
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )
