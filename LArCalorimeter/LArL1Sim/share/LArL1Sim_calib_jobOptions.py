# +===========================================================================+
# +                                                                           +
# + Author ........: F. Ledroit                                               +
# + Institut ......: LPSC Grenoble                                             +
# + Creation date .: 21/04/2004      Version 0.01                             +
# + Subject .......: Job Option file to make calibration of the TTL1s         +
# +===========================================================================+ 
include( "LArL1Sim/LArL1Sim_testG4_jobOptions.py" )

theApp.TopAlg += [ "LArTTL1Calib"]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
LArTTL1Calib = Algorithm( "LArTTL1Calib" )
#LArTTL1Calib.OutputLevel = 2
svcMgr.MessageSvc.useColors   = TRUE
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit = 100000

theLArTTL1Maker=LArTTL1Maker()
theLArTTL1Maker.OutputLevel  = 3

theApp.EvtMax = 100000

#--------------------------------------------------------------
# Select HBOOK or ROOT persistency
#--------------------------------------------------------------
theApp.HistogramPersistency = "HBOOK"
#--------------------------------------------------------------
# Histogram output file 
#--------------------------------------------------------------
# Specify the appropriate output file type
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
#HistogramPersistencySvc.OutputFile  = "histo.rt"
#--------------------------------------------------------------
# Ntuples
#--------------------------------------------------------------
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output     = [ "FILE1 DATAFILE='tuple1.hbook' OPT='NEW'" ]
#
#
# ............ if true, noise added in all subdetectors (default = true)
#
theLArTTL1Maker.NoiseOnOff = FALSE
#
# ....... if true, NO calibration is applied to EM towers (for calibration coeffs computation !)
#.................(default = false )
#
#theLArTTL1Maker.NoEmCalibrationMode = TRUE
#
# ....... if true, NO calibration is applied to HEC towers (for calibration coeffs computation !)
#.................(default = false )
#
#theLArTTL1Maker.NoHadCalibrationMode = TRUE
#
# ............ transverse energy threshold for debug printout (default = 5000 MeV)
#
#theLArTTL1Maker.DebugThreshold = 1000.
#==============================================================
#
# End of job options file
#
###############################################################
