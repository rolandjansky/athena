#**************************************************************
#
# jopOptions file for reading TileCal ByteStream (both RDO and RIO)
# DD is read from ROOT
#
#==============================================================
#---------------------------------------------------------------
# Input = RawEvent.re
#---------------------------------------------------------------
include( "ByteStreamCnvSvc/ByteStreamSelector_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )

include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE 
# load detector description from ROOT
include( "TDRDetDescrAthenaRoot/TDRDetDescrAthenaRoot_jobOptions.py" )

DetDescrCnvSvc.ReadFromNova = FALSE 
include( "TileByteStream/ReadTileBS_jobOptions.py" )

include( "TileByteStream/ReadTileCellBS_jobOptions.py" )

# force creation of Converter in init
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamCnvSvc.InitCnvs += [
                                "TileRawChannelContainer",
                                "TileRawChannelCollection",
                                "TileCellCollection"
 ] 
theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 
theApp.EvtMax = 200
MessageSvc.OutputLevel = 2
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 3
