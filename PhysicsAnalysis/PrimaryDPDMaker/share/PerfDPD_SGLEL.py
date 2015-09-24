##-----------------------------------------------------------------------------
## Name: PerfDPD_SGLEL.py
##
## Author: Paul Laycock
## Email:  paul.james.laycock@cern.ch
##
## Description: This defines the content of the high pt electron DPD output stream.
##
##-----------------------------------------------------------------------------

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DESDM_SGLEL_Seq = CfgMgr.AthSequencer("DESDM_SGLEL_Seq")


##============================================================================
## Define the skimming (event selection) for the DESD_SGLEL output stream
##============================================================================
# Object selection strings
sel_sglel  = 'Electrons.pt > 100*GeV && ( Electrons.Medium || Electrons.LHMedium )'

# Event selection string
desdm_sglel = '(count('+sel_sglel+')>=1)'

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DESDM_SGLEL_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_SGLEL_SkimmingTool",
                                                                        expression = desdm_sglel)

ToolSvc += DESDM_SGLEL_SkimmingTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DESDM_SGLEL_Seq += CfgMgr.DerivationFramework__DerivationKernel("DESDM_SGLELKernel",
                                                               SkimmingTools = [DESDM_SGLEL_SkimmingTool]
                                                               )
topSequence += DESDM_SGLEL_Seq


##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteDESDM_SGLELStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_SGLELStream )

if primDPD.WriteDESDM_SGLELStream.isVirtual or primDPD.isVirtual() :
    StreamDESDM_SGLEL=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    StreamDESDM_SGLEL=MSMgr.NewPoolStream( streamName, fileName )
    pass

StreamDESDM_SGLEL.AddRequireAlgs(["DESDM_SGLELKernel"])


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList,
# list should be updated once egamma thinning is functional

ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)



