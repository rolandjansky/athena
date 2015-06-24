##-----------------------------------------------------------------------------
## Name: PrimaryDPD_PhotonJetStream.py
##
## Author: Chris Young
## Email:  christopher.young@cern.ch
##
## Description: This defines the content of the high pt Photon DPD output stream.
##
##-----------------------------------------------------------------------------

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DESDM_PHOJET_Seq = CfgMgr.AthSequencer("DESDM_PHOJET_Seq")


##============================================================================
## Define the skimming (event selection) for the DESD_PHOJET output stream
##============================================================================
# Object selection strings
sel_phojet  = 'Photons.pt > 190*GeV && Photons.Tight'

# Event selection string
desd_phojet = '(count('+sel_phojet+')>=1)'

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DESDM_PHOJET_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_PHOJET_SkimmingTool",
                                                                        expression = desd_phojet)

ToolSvc += DESDM_PHOJET_SkimmingTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DESDM_PHOJET_Seq += CfgMgr.DerivationFramework__DerivationKernel("DESDM_PHOJETKernel",
                                                               SkimmingTools = [DESDM_PHOJET_SkimmingTool]
                                                               )
topSequence += DESDM_PHOJET_Seq


##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteDESDM_PHOJETStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_PHOJETStream )

if primDPD.WriteDESDM_PHOJETStream.isVirtual or primDPD.isVirtual() :
    StreamDESDM_PHOJET=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    StreamDESDM_PHOJET=MSMgr.NewPoolStream( streamName, fileName )
    pass

StreamDESDM_PHOJET.AddRequireAlgs(["DESDM_PHOJETKernel"])


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList,
# list should be copied from AllCells but add back the thinned tracking info for egamma

ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)



