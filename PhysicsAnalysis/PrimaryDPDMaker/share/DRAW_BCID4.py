####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_BCID4_Seq = CfgMgr.AthSequencer("DRAW_BCID4_Seq")

from PrimaryDPDMaker.PrimaryDPDMakerConf import BCIDFilterTool
bcidFilterTool = BCIDFilterTool( name        = "BCID4Filter",
                                 AcceptBCIDs = jobproperties.PrimaryDPDFlags.WriteDRAW_BCID4.AcceptBCIDs,
                                 RejectBCIDs = jobproperties.PrimaryDPDFlags.WriteDRAW_BCID4.RejectBCIDs
                                 )

bcidFilterTool.OutputLevel = 3 # INFO
ToolSvc += bcidFilterTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_BCID4_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_BCID4Kernel",
                                                                  SkimmingTools = [bcidFilterTool]
                                                                )
topSequence += DRAW_BCID4_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_BCID4.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_BCID4 )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_BCID4 = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_BCID4.AddRequireAlgs(["DRAW_BCID4Kernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_BCID4.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
