###############################################################
#
# Job options file : ReadTileBS (read TileCells from ByteStream)
#
#==============================================================
# load TileConditions options
# loaded already?
# #include "TileConditions/TileConditions_jobOptions.txt"
# load options for Detector Description converter
# loaded already ?
# #include "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.txt"
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
# Threshold on Cell energy (in MeV)
ToolSvc = Service( "ToolSvc" )
ToolSvc.TileROD_Decoder.TileCellEthresholdMeV = -1000000.0
# take TileCell from TileCellIDC and store in CaloContainer
theApp.TopAlg += ["TileCellIDCToCell"]
TileCellIDCToCell = Algorithm( "TileCellIDCToCell" )
TileCellIDCToCell.TileCellIDC = "TileCellIDC"
TileCellIDCToCell.TileCellContainer = "TileCellContainerKey"
TileCellIDCToCell.TileInfoName="TileInfo"
# if this algorithm was included before
# change name for output container to avoid clash
TileRawChannelToCell = Algorithm( "TileRawChannelToCell" )
TileRawChannelToCell.TileCellContainer = "TileCellFromRC"
# create ntuple with all TileCells
theApp.TopAlg += ["TileCellIDCToNtuple"]
# Ntuple service output
theApp.HistogramPersistency="HBOOK"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output    = [ "FILE1 DATAFILE='tilecellbs_read.ntup' OPT='NEW'" ]
