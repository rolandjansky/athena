####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_BCID2_Seq = CfgMgr.AthSequencer("DRAW_BCID2_Seq")

from PrimaryDPDMaker.PrimaryDPDMakerConf import BCIDFilterTool
bcidFilterTool = BCIDFilterTool( name        = "BCID2Filter",
                                 AcceptBCIDs = jobproperties.PrimaryDPDFlags.WriteDRAW_BCID2.AcceptBCIDs,
                                 RejectBCIDs = jobproperties.PrimaryDPDFlags.WriteDRAW_BCID2.RejectBCIDs
                                 )

bcidFilterTool.OutputLevel = 3 # INFO
ToolSvc += bcidFilterTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_BCID2_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_BCID2Kernel",
                                                                  SkimmingTools = [bcidFilterTool]
                                                                )
topSequence += DRAW_BCID2_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_BCID2.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_BCID2 )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_BCID2 = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_BCID2.AddRequireAlgs(["DRAW_BCID2Kernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_BCID2.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
