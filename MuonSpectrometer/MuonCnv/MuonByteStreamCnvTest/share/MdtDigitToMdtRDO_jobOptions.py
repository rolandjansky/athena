# the source of the digits
include( "MuonDigitZebraCnv/G3MuonDigit_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# use detector store conversion service
include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
# amdb
include( "AmdbAth/AmdbAth_jobOptions.py" )

#load relevant libraries
theApp.Dlls += [ "MDTcabling"]
theApp.Dlls += [ "MuonByteStreamCnvTest" ]
theApp.ExtSvc += [ "MDTcablingSvc" ]
#top algorithms to be run
theApp.TopAlg += [ "MdtDigitToMdtRDO" ]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 2
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = FALSE;  #true will dump data store contents
MDTcablingSvc = Service( "MDTcablingSvc" )
MDTcablingSvc.MapFlag = TRUE
MDTcablingSvc.GeoFlag = FALSE
MDTcablingSvc.CheFlag = FALSE
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 200
#
# End of AthExStoreGateExample job options file
#
###############################################################
