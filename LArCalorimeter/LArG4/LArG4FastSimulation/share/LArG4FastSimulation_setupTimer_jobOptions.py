#
# $Id: LArG4FastSimulation_setupTimer_jobOptions.py 619286 2014-09-30 23:27:14Z zmarshal $
#
# job options for calculating timing information
#

#
# Include this file in your simulation job options to apply the timer
#

def LArG4Timer_preG4Init():
    from G4AtlasApps import PyG4Atlas,AtlasG4Eng
    MyAction = PyG4Atlas.UserAction('LArG4FastSimulationTestActions','TestActionTimer',['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
simFlags.InitFunctions.add_function("preInitG4", LArG4Timer_preG4Init)

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output += ["simTime DATAFILE='timing_histos.root' OPT='RECREATE'"]

