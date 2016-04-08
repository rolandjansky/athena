##-----------------------------------------------------------------------------
## Name: PhysDPD_HIPs.py
##
## Description: Separating out the HIPs filters from the DESDM_RPVLL to their own stream.
##
##-----------------------------------------------------------------------------

# ##########################################################################################
# Imports and initial setup
# ##########################################################################################

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

from LongLivedParticleDPDMaker.HipsFlags import primHIPsDESD

streamName = primDPD.WriteDESD_HIPsStream.StreamName
fileName   = buildFileName( primDPD.WriteDESD_HIPsStream )
HIPsStream      = MSMgr.NewPoolStream( streamName,fileName )


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

HIPsFilterNames = [] # Filters should append their final selection algo names to this list
filtersToBookkeep = [] # Not sure what this does?


HipTriggerFilterTool = skimtool( name = "HipTriggerFilterTool",
                                 expression = primHIPsDESD.HipsTriggerFilterExpression())

ToolSvc += HipTriggerFilterTool

topSequence += kernel( "HIPsTriggerFilterKernel",
                       SkimmingTools = [HipTriggerFilterTool] )

HIPsFilterNames.extend(["HIPsTriggerFilterKernel"])

################# OR together all the filters into one, if at least one analysis selection is active

HIPsStream.AcceptAlgs(HIPsFilterNames)



# ##########################################################################################
# Output stream definition
# ##########################################################################################

# Add everything from the input file
from AthenaCommon.KeyStore import CfgKeyStore,CfgItemList
theCKS = CfgKeyStore('KeyStore')
outList = CfgItemList( 'HIPsStream' )
outList += theCKS['inputFile']

# Remove items here, as needed
outList.removeAllItems( 'RpcPadContainer#RPCPAD' ) # Avoids crash in 20.1.0.2

# Configure the stream
HIPsStream.AddItem(outList())

# make sure bookkeeping info is kept for additional filters
HIPsStream.AddOtherAlgsToBookkeep(filtersToBookkeep)
