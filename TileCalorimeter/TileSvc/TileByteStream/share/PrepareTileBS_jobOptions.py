###############################################################
#
# Job options file : PrepareTileBS (create TileRawChannels in memory)
#
#==============================================================
# load TileConditions options
include( "TileConditions/TileConditions_jobOptions.py" )

# load options for Detector Description converter
include( "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.py" )

# load options for direct Zebra->Hit conversion
include( "TileHitZebraCnv/TileHitZebraCnv_jobOptions.py" )

#-----------
# Load DLLs 
#-----------
theApp.Dlls += [ 
			 "TileSimAlgs",
			 "TileRecAlgs",
			 "TileRec"]
#----------------
# Add Algorithms
#----------------
# Create Raw Channel from Hit
theApp.TopAlg += [ "TileHitToRawChannel" ]
TileHitToRawChannel = Algorithm( "TileHitToRawChannel" )
TileHitToRawChannel.TileHitContainer="TileHitCnt"
TileHitToRawChannel.TileRawChannelContainer="TileRawChannelCnt"
TileHitToRawChannel.TileInfoName="TileInfo"
# convert RawChannel to Cell
# this is not needed for conversion to ByteStream
# and it is used just for comparison with "read from BS" algorithm
theApp.TopAlg += ["TileRawChannelToCell"]
TileRawChannelToCell = Algorithm( "TileRawChannelToCell" )
TileRawChannelToCell.TileRawChannelContainer = "TileRawChannelCnt"
TileRawChannelToCell.TileCellContainer = "TileCellContainerKey"
TileRawChannelToCell.TileInfoName="TileInfo"
# create ntuple with all TileRawChannels
theApp.TopAlg += ["TileRawChannelToNtuple"]
# Ntuple service output
theApp.HistogramPersistency="HBOOK"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='tilebs_write.ntup' OPT='NEW'" ]
