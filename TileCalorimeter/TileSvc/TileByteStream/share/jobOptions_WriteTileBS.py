#**************************************************************
#
# jopOptions file for writing TileCal ByteStream
#
#==============================================================
#---------------------------------------------------------------
# input = Zebra 
#---------------------------------------------------------------
theApp.setup( ZEBRA )

include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE 
EventSelector.indet = FALSE
EventSelector.calos = TRUE
EventSelector.muons = FALSE
EventSelector.tile       = TRUE
EventSelector.emBarrel   = FALSE;                        
EventSelector.emEndcap   = FALSE;                        
EventSelector.hec        = FALSE;                        
EventSelector.fcal       = FALSE;                        
theApp.EvtSel = "EventSelector"
EventSelector.dataSetNames = ["Y23406.1", "Y23406.2"]
EventSelector.runs = [1, 30000]
#---------------------------------------------------------------
# output = RawEvent.re 
#---------------------------------------------------------------
include( "ByteStreamCnvSvc/WriteByteStream_jobOptions.py" )

include( "TileByteStream/WriteTileBS_jobOptions.py" )

theApp.EvtMax = 10
MessageSvc.OutputLevel = 2
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 3
