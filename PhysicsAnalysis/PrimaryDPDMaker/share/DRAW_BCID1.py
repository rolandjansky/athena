####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DRAW_BCID1_Seq = CfgMgr.AthSequencer("DRAW_BCID1_Seq")

from PrimaryDPDMaker.PrimaryDPDMakerConf import BCIDFilterTool
bcidFilterTool = BCIDFilterTool( name        = "BCID1Filter",
                                 AcceptBCIDs = jobproperties.PrimaryDPDFlags.WriteDRAW_BCID1.AcceptBCIDs,
                                 RejectBCIDs = jobproperties.PrimaryDPDFlags.WriteDRAW_BCID1.RejectBCIDs
                                 )

bcidFilterTool.OutputLevel = 3 # INFO
ToolSvc += bcidFilterTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DRAW_BCID1_Seq += CfgMgr.DerivationFramework__DerivationKernel("DRAW_BCID1Kernel",
                                                                  SkimmingTools = [bcidFilterTool]
                                                                )
topSequence += DRAW_BCID1_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDRAW_BCID1.StreamName
fileName   = buildFileName( primDPD.WriteDRAW_BCID1 )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
StreamDRAW_BCID1 = MSMgr.NewByteStream( streamName, fileName )
StreamDRAW_BCID1.AddRequireAlgs(["DRAW_BCID1Kernel"])

# Don't write an output RAW file if it is empty
StreamDRAW_BCID1.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
