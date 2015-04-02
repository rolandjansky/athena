## set to true in case you would like to write out your own IDPerfMon.root
IDPerfMonDoOutput = True

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZee
IDPerfMonZee_noTrig = IDPerfMonZee (name = "IDPerfMonZee_noTrig",
                                    tracksName = InDetKeys.TrackParticles())
ToolSvc += IDPerfMonZee_noTrig
if (InDetFlags.doPrintConfigurables()):
    print IDPerfMonZee_noTrig

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonWenu
IDPerfMonWenu_noTrig = IDPerfMonWenu (name = "IDPerfMonWenu_noTrig",
                                      tracksName = InDetKeys.TrackParticles())
ToolSvc += IDPerfMonWenu_noTrig
if (InDetFlags.doPrintConfigurables()):
    print IDPerfMonWenu_noTrig

#include( "TrackIsolationTools/TrackIsolationTool_jobOptions.py" )

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonKshort
IDPerfMonKshort_noTrig = IDPerfMonKshort (name = "IDPerfMonKshort_noTrig",
                                         VxContainerName = InDetKeys.SecVertices(),
                                          tracksName = InDetKeys.TrackParticles(),
                                          VxPrimContainerName = InDetKeys.PrimaryVertices())

ToolSvc += IDPerfMonKshort_noTrig
if (InDetFlags.doPrintConfigurables()):
    print IDPerfMonKshort_noTrig

## only do trigger-aware monitoring if monTrigDecTool known by ToolSvc
if not hasattr(ToolSvc, 'monTrigDecTool'):
    print "IDPerfMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
else:
    IDPerfMonZee_2e20 = IDPerfMonZee (name = "IDPerfMonZee_2e20",
                                      tracksName = InDetKeys.TrackParticles())
    IDPerfMonZee_e25i_tight = IDPerfMonZee (name = "IDPerfMonZee_e25i_tight",
                                            tracksName = InDetKeys.TrackParticles())
    IDPerfMonWenu_g25_xe30 = IDPerfMonWenu (name = "IDPerfMonWenu_g25_xe30",
                                            tracksName = InDetKeys.TrackParticles())
    IDPerfMonWenu_e25i_tight = IDPerfMonWenu (name = "IDPerfMonWenu_e25i_tight",
                                              tracksName = InDetKeys.TrackParticles())

    IDPerfMonZee_2e20.TrigDecisionTool             = monTrigDecTool
    IDPerfMonZee_e25i_tight.TrigDecisionTool       = monTrigDecTool
    IDPerfMonWenu_g25_xe30.TrigDecisionTool        = monTrigDecTool
    IDPerfMonWenu_e25i_tight.TrigDecisionTool      = monTrigDecTool

    IDPerfMonZee_2e20.TriggerChain             = "EF_2e20" # alternative "EF_e25i_tight"
    IDPerfMonZee_e25i_tight.TriggerChain       = "EF_e25i_tight"
    IDPerfMonWenu_g25_xe30.TriggerChain        = "EF_g25_xe30" # alternative "EF_e25i_tight"
    IDPerfMonWenu_e25i_tight.TriggerChain      = "EF_e25i_tight"

    IDPerfMonZee_2e20.triggerChainName             = "EF_2e20" # alternative "EF_e25i_tight"
    IDPerfMonZee_e25i_tight.triggerChainName       = "EF_e25i_tight"
    IDPerfMonWenu_g25_xe30.triggerChainName        = "EF_g25_xe30" # alternative "EF_e25i_tight"
    IDPerfMonWenu_e25i_tight.triggerChainName      = "EF_e25i_tight"

    ToolSvc += IDPerfMonZee_2e20
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonZee_2e20
    ToolSvc += IDPerfMonZee_e25i_tight
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonZee_e25i_tight
    ToolSvc += IDPerfMonWenu_g25_xe30
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonWenu_g25_xe30
    ToolSvc += IDPerfMonWenu_e25i_tight
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonWenu_e25i_tight

    ###########################

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
IDPerfMonManager = AthenaMonManager( name = "IDPerfMonManager",
                                      FileKey = "GLOBAL",
                                      ManualDataTypeSetup = True,
                                      DataType            = "userDefined", # use this for collision data for now
                                      Environment         = "user",
                                      ManualRunLBSetup    = True,
#                                      maxPrescale=50,
                                      Run                 = 1,
                                      LumiBlock           = 1)
IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_noTrig ]
IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_noTrig ]
IDPerfMonManager.AthenaMonTools += [ IDPerfMonKshort_noTrig ]
if not hasattr(ToolSvc, 'monTrigDecTool'):
    print "IDPerfMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
else:
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_2e20 ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_e25i_tight ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_g25_xe30 ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_e25i_tight ]

    # no trigger requirement for Kshorts

## Setup the output histogram file(s):
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
if IDPerfMonDoOutput:
    THistSvc = Service( "THistSvc" )
    histOutput = "IDPerfMon DATAFILE='./IDPerfMon.root' OPT='RECREATE'"
    THistSvc.Output += [histOutput]
    IDPerfMonManager.FileKey = "IDPerfMon"


topSequence += IDPerfMonManager
if (InDetFlags.doPrintConfigurables()):
    print IDPerfMonManager
