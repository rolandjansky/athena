###############################################################
#
# Job options file for running muComb algorithm
#
#==============================================================
#load the libraries for the Timing Service
theApp.Dlls +=["TrigTimeAlgs" ]
theApp.ExtSvc += [ "TrigTimerSvc" ]
# include the MDT cabling service
#include( "MDTcablingJobOptions.py" )

# include the Roi Reconstruction Services
#include( "TrigT1RPCRecRoiSvcJobOptions.py" )

#include( "TrigT1TGCRecRoiSvcJobOptions.py" )

#theApp.Dlls += [ "TrigT1TGC" ]
#load the libraries for the muComb algorithm
theApp.Dlls += [ "TrigmuComb" ]
#theApp.ExtSvc += [ "MuRoadsSvc", "MuLUTSvc" ]


#--------------------------------------------------------------
# Configuration of the Timer Service
#--------------------------------------------------------------
TrigTimerSvc = Service( "TrigTimerSvc" )
TrigTimerSvc.OutputLevel  = 3
