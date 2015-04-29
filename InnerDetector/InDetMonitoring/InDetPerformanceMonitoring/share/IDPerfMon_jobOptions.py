## set to true in case you would like to write out your own IDPerfMon.root
IDPerfMonDoOutput = False

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
    print 'IDPerfMon_jobOptions.py : following express stream trigger menu (pp_v5_menu) -> https://twiki.cern.ch/twiki/bin/view/Atlas/ExpressStream#Run_2_Physics_Physics_pp_v5_menu'
    print 'IDPerfMon_jobOptions.py : lowLumi Zee triggers'
    IDPerfMonZee_2e12_loose = IDPerfMonZee (name = "IDPerfMonZee_2e12_loose",
                                            tracksName = InDetKeys.TrackParticles())
    IDPerfMonZee_e24_medium_L1EM18VH = IDPerfMonZee (name = "IDPerfMonZee_e24_medium_L1EM18VH",
                                            tracksName = InDetKeys.TrackParticles())
    print 'IDPerfMon_jobOptions.py : highLumi Zee triggers'
    IDPerfMonZee_2e15_loose = IDPerfMonZee (name = "IDPerfMonZee_2e15_loose",
                                            tracksName = InDetKeys.TrackParticles())
    IDPerfMonZee_e24_tight_L1EM20VH  = IDPerfMonZee (name = "IDPerfMonZee_e24_tight_L1EM20VH",
                                            tracksName = InDetKeys.TrackParticles())
    print 'IDPerfMon_jobOptions.py : Wenu triggers'
    IDPerfMonWenu_g25_loose_xe35 = IDPerfMonWenu (name = "IDPerfMonWenu_g25_loose_xe35",
                                            tracksName = InDetKeys.TrackParticles())
    IDPerfMonWenu_e24_medium_L1EM18VH = IDPerfMonWenu (name = "IDPerfMonWenu_e24_medium_L1EM18VH",
                                            tracksName = InDetKeys.TrackParticles())
    IDPerfMonWenu_e24_tight_L1EM20VH  = IDPerfMonWenu (name = "IDPerfMonWenu_e24_tight_L1EM20VH",
                                            tracksName = InDetKeys.TrackParticles())


    IDPerfMonZee_2e12_loose.TrigDecisionTool             = monTrigDecTool
    IDPerfMonZee_e24_medium_L1EM18VH.TrigDecisionTool             = monTrigDecTool
    IDPerfMonZee_2e15_loose.TrigDecisionTool             = monTrigDecTool
    IDPerfMonZee_e24_tight_L1EM20VH.TrigDecisionTool             = monTrigDecTool
    IDPerfMonWenu_g25_loose_xe35.TrigDecisionTool             = monTrigDecTool
    IDPerfMonWenu_e24_medium_L1EM18VH.TrigDecisionTool             = monTrigDecTool
    IDPerfMonWenu_e24_tight_L1EM20VH.TrigDecisionTool             = monTrigDecTool

    IDPerfMonZee_2e12_loose.TriggerChain = "2e12_loose"
    IDPerfMonZee_e24_medium_L1EM18VH.TriggerChain             = "e24_medium_L1EM18VH"
    IDPerfMonZee_2e15_loose.TriggerChain             = "2e15_loose"
    IDPerfMonZee_e24_tight_L1EM20VH.TriggerChain             = "e24_tight_L1EM20VH"

    IDPerfMonWenu_g25_loose_xe35.TriggerChain             = "g25_loose_xe35"
    IDPerfMonWenu_e24_medium_L1EM18VH.TriggerChain             = "e24_medium_L1EM18VH"
    IDPerfMonWenu_e24_tight_L1EM20VH.TriggerChain             = "e24_tight_L1EM20VH"

    ToolSvc += IDPerfMonZee_2e12_loose
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonZee_2e12_loose
    ToolSvc += IDPerfMonZee_e24_medium_L1EM18VH
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonZee_e24_medium_L1EM18VH
    ToolSvc += IDPerfMonZee_2e15_loose
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonZee_2e15_loose
    ToolSvc += IDPerfMonZee_e24_tight_L1EM20VH
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonZee_e24_tight_L1EM20VH

    ToolSvc += IDPerfMonWenu_g25_loose_xe35
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonWenu_g25_loose_xe35
    ToolSvc += IDPerfMonWenu_e24_medium_L1EM18VH
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonWenu_e24_medium_L1EM18VH
    ToolSvc += IDPerfMonWenu_e24_tight_L1EM20VH
    if (InDetFlags.doPrintConfigurables()):
        print IDPerfMonWenu_e24_tight_L1EM20VH


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
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_2e12_loose ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_e24_medium_L1EM18VH]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_2e15_loose ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_e24_tight_L1EM20VH ]

    IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_g25_loose_xe35 ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_e24_medium_L1EM18VH ]
    IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_e24_tight_L1EM20VH ]


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
