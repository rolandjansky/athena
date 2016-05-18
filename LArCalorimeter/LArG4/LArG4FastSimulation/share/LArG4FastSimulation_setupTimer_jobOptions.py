#
# $Id: LArG4FastSimulation_setupTimer_jobOptions.py 747974 2016-05-18 12:25:25Z jchapman $
#
# job options for calculating timing information
#

#
# Include this file in your simulation job options to apply the timer
#

try:
    if (hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()):
        # this configures the MT
        from G4AtlasServices import G4AtlasServicesConfig
        G4AtlasServicesConfig.addAction('G4UA::TestActionTimerTool',['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'],False)
    else:
        # Post UserAction Migration (ATLASSIM-1752)
        try:
            from G4AtlasServices.G4AtlasUserActionConfig import UAStore
        except ImportError:
            from G4AtlasServices.UserActionStore import UAStore
            from AthenaCommon.CfgGetter import getPublicTool
            UAStore.addAction(getPublicTool('TestActionTimer'),['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
except:
    # Pre UserAction Migration
    def LArG4Timer_preG4Init():
        from G4AtlasApps import PyG4Atlas,AtlasG4Eng
        MyAction = PyG4Atlas.UserAction('G4ProfilingTools','TestActionTimer',['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
        AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
    simFlags.InitFunctions.add_function("preInitG4", LArG4Timer_preG4Init)

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output += ["simTime DATAFILE='timing_histos.root' OPT='RECREATE'"]

