###############################################################
#
# Job options file : WriteTileBS (write TileRawChannels to ByteStream)
#
#==============================================================
# prepare TileRawChannels in memory
include( "TileByteStream/PrepareTileBS_jobOptions.py" )

# and now write them to ByteStream
#-----------
# Load DLLs 
#-----------
theApp.Dlls += [ 
			 "TileByteStream" ]
# ByteStream conversion
# add TileRawChannelContainer key to BS Item List
StreamBS = Algorithm( "StreamBS" )
#StreamBS.ItemList +=["2925#*"]
StreamBS.ItemList +=["2927#*"]
