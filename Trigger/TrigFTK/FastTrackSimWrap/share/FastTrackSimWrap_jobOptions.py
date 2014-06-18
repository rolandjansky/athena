###############################################################
#
# DumpSp job options file
#
#==============================================================
# top algorithm to be run and component library to be loaded. if
# InDetTruthToTrack does not exist, you haven't configured the 
# rest of the job correctly. see the job options inside
# submit_torque.pl
#theApp.TopAlg += [ "DumpSp" ]
#theApp.Dlls   += [ "FastTrackSimWrap" ]
#DumpSp = Algorithm( "DumpSp" )
#DumpSp.maxEta                  = 3.3
#DumpSp.minPt                   = 0.*GeV
#DumpSp.OutFileName             = "ftksim_wrap.dat.bz2"
#DumpSp.OutFileNameRawHits      = "ftksim_wrap_raw.dat.bz2"
#DumpSp.OutputLevel             = 2
#DumpSp.tracksName = "Tracks"


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


from FastTrackSimWrap.FastTrackSimWrapConf import DumpSp

if hasattr(runArgs,"outputTXT_FTKIPFile") :
    OutputTXT_FTKIPFile = runArgs.outputTXT_FTKIPFile
else :
    OutputTXT_FTKIPFile = 'ftksim_wrap_raw.dat.bz2'


theJob += DumpSp(OutputLevel             = INFO, 
                 maxEta                  = 3.3,
                 minPt                   = 0.8*GeV,
                 DoData                  = False,
                 OutFileName             = "ftksim_wrap.dat.bz2",
                 OutFileNameRawHits      = OutputTXT_FTKIPFile,
                 tracksName              = "Tracks")

print theJob
	
	

###############################################################
