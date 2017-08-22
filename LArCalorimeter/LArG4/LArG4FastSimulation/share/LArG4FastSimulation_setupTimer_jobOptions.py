#
# $Id: LArG4FastSimulation_setupTimer_jobOptions.py 747974 2016-05-18 12:25:25Z jchapman $
#
# job options for calculating timing information
#

#
# Include this file in your simulation job options to apply the timer
#
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::TestActionTimerTool',['Run','Event','Step'])

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output += ["simTime DATAFILE='timing_histos.root' OPT='RECREATE'"]

