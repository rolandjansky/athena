include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

#ROOT streaming service
include ("AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py")
StreamerSvc = Service ( "AthenaRootStreamerSvc" )
StreamerSvc.Streamers += [ "CscRawDataStreamer_p0" ]
