###############################################################
#
# Job options file for running muFast algorithm
#
#==============================================================
#load the libraries for the Timing Service
theApp.Dlls +=["TrigTimeAlgs" ]
theApp.ExtSvc += [ "TrigTimerSvc" ]
# include the MDT cabling service
include( "MDTcablingJobOptions.py" )

# include the Roi Reconstruction Services
include( "TrigT1RPCRecRoiSvcJobOptions.py" )

include( "TrigT1TGCRecRoiSvcJobOptions.py" )

theApp.Dlls += [ "TrigT1TGC" ]
#load the libraries for the muFast algorithm
theApp.Dlls += [ "TrigmuFast" ]
theApp.ExtSvc += [ "MuRoadsSvc", "MuLUTSvc" ]

#configure the ROBmapper
if not "DetDescrVersion" in dir():
    DetDescrVersion = "DC2"

if DetDescrVersion == "DC1":
    muFast = Algorithm ( "muFast_1_L2" )
    muFast.ROBmapperFile = "RoiROBmap-P03.data"

#--------------------------------------------------------------
# Configuration of the Timer Service
#--------------------------------------------------------------
TrigTimerSvc = Service( "TrigTimerSvc" )
TrigTimerSvc.OutputLevel  = 3
