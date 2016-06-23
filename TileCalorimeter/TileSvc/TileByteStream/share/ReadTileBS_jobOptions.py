###############################################################
#
# Job options file : ReadTileBS (read TileRawChannels from ByteStream)
#
#==============================================================
# load TileConditions options
include( "TileConditions/TileConditions_jobOptions.py" )

# load options for Detector Description converter
include( "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.py" )

#-----------
# Load DLLs 
#-----------
theApp.Dlls += [ 
			 "TileRecAlgs",
			 "TileRec",
			 "TileByteStream" ]
#----------------
# Add Algorithms
#----------------
# convert RawChannel to Cell
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
NTupleSvc.Output    = [ "FILE1 DATAFILE='tilebs_read.ntup' OPT='NEW'" ]
