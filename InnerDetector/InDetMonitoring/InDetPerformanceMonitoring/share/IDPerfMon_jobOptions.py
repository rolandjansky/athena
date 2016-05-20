## set to true in case you would like to write out your own IDPerfMon.root
IDPerfMonDoOutput = False

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonZee
IDPerfMonZee_noTrig = IDPerfMonZee (name = "IDPerfMonZee_noTrig",
                                   tracksName=InDetKeys.xAODTrackParticleContainer())# tracksName = "InDetTrackParticles")#InDetKeys.TrackParticles())
ToolSvc += IDPerfMonZee_noTrig
if (InDetFlags.doPrintConfigurables()):
    print IDPerfMonZee_noTrig

from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonWenu
IDPerfMonWenu_noTrig = IDPerfMonWenu (name = "IDPerfMonWenu_noTrig",
                                      tracksName=InDetKeys.xAODTrackParticleContainer()) #     tracksName = "InDetTrackParticles")#InDetKeys.TrackParticles())
ToolSvc += IDPerfMonWenu_noTrig
if (InDetFlags.doPrintConfigurables()):
    print IDPerfMonWenu_noTrig

## only do trigger-aware monitoring if DQMonFlags.useTrigger is true
if not DQMonFlags.useTrigger():
    print "IDPerfMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
else:
    print 'IDPerfMon_jobOptions.py : following express stream trigger menu (pp_v5_menu) -> https://twiki.cern.ch/twiki/bin/view/Atlas/ExpressStream#Run_2_Physics_Physics_pp_v5_menu'

    #
    # for pp collisions
    #
    if not rec.doHeavyIon():
        IDPerfMonZee_e24_medium_L1EM18VH = IDPerfMonZee (name = "IDPerfMonZee_e24_medium_L1EM18VH",
                                                        tracksName=InDetKeys.xAODTrackParticleContainer(),#         tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "e24_medium_L1EM18VH")
    
        print 'IDPerfMon_jobOptions.py : highLumi Zee triggers'

        IDPerfMonZee_e24_tight_L1EM20VH  = IDPerfMonZee (name = "IDPerfMonZee_e24_tight_L1EM20VH",
                                                        tracksName=InDetKeys.xAODTrackParticleContainer(),#       tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "e24_tight_L1EM20VH")
        
        print 'IDPerfMon_jobOptions.py : Wenu triggers'
        
        IDPerfMonWenu_e24_medium_L1EM18VH = IDPerfMonWenu (name = "IDPerfMonWenu_e24_medium_L1EM18VH",
                                                        tracksName=InDetKeys.xAODTrackParticleContainer(),#     tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "e24_medium_L1EM18VH")
    
        IDPerfMonWenu_e24_tight_L1EM20VH  = IDPerfMonWenu (name = "IDPerfMonWenu_e24_tight_L1EM20VH",
                                                        tracksName=InDetKeys.xAODTrackParticleContainer(),#   tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "e24_tight_L1EM20VH")
    
    
        IDPerfMonZee_e24_medium_L1EM18VH.TrigDecisionTool             = monTrigDecTool
        IDPerfMonZee_e24_tight_L1EM20VH.TrigDecisionTool             = monTrigDecTool
        IDPerfMonWenu_e24_medium_L1EM18VH.TrigDecisionTool             = monTrigDecTool
        IDPerfMonWenu_e24_tight_L1EM20VH.TrigDecisionTool             = monTrigDecTool
    
    
        IDPerfMonZee_e24_medium_L1EM18VH.TriggerChain             = "HLT_e24_medium_L1EM18VH"
        IDPerfMonZee_e24_tight_L1EM20VH.TriggerChain             = "HLT_e24_tight_L1EM20VH"
    
        IDPerfMonWenu_e24_medium_L1EM18VH.TriggerChain             = "HLT_e24_medium_L1EM18VH"
        IDPerfMonWenu_e24_tight_L1EM20VH.TriggerChain             = "HLT_e24_tight_L1EM20VH"

    
        ToolSvc += IDPerfMonZee_e24_medium_L1EM18VH
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonZee_e24_medium_L1EM18VH
        ToolSvc += IDPerfMonZee_e24_tight_L1EM20VH
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonZee_e24_tight_L1EM20VH
    
        ToolSvc += IDPerfMonWenu_e24_medium_L1EM18VH
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonWenu_e24_medium_L1EM18VH
        ToolSvc += IDPerfMonWenu_e24_tight_L1EM20VH
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonWenu_e24_tight_L1EM20VH
    
    #
    # Heavy ion collisions
    #
    if rec.doHeavyIon():
        IDPerfMonZee_e15_loose_ion = IDPerfMonZee (name = "IDPerfMonZee_e15_loose_ion",
                                                    tracksName=InDetKeys.xAODTrackParticleContainer(),#         tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "e15_loose_ion")
    
        print 'IDPerfMon_jobOptions.py : highLumi Zee triggers (HI)'
    
        IDPerfMonZee_2e10_loose_ion  = IDPerfMonZee (name = "IDPerfMonZee_2e10_loose_ion",
                                                        tracksName=InDetKeys.xAODTrackParticleContainer(),#       tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "2e10_loose_ion")
    
        print 'IDPerfMon_jobOptions.py : Wenu triggers (HI)'
    
        IDPerfMonWenu_e15_loose_ion = IDPerfMonWenu (name = "IDPerfMonWenu_e15_loose_ion",
                                                        tracksName=InDetKeys.xAODTrackParticleContainer(),#     tracksName = "InDetTrackParticles",#InDetKeys.TrackParticles(),
                                                        triggerChainName = "e15_loose_ion")
    
    
    
        IDPerfMonZee_e15_loose_ion.TrigDecisionTool             = monTrigDecTool
        IDPerfMonZee_2e10_loose_ion.TrigDecisionTool             = monTrigDecTool
        IDPerfMonWenu_e15_loose_ion.TrigDecisionTool             = monTrigDecTool



        IDPerfMonZee_e15_loose_ion.TriggerChain             = "HLT_e15_loose_ion"
        IDPerfMonZee_2e10_loose_ion.TriggerChain             = "HLT_2e10_loose_ion"

        IDPerfMonWenu_e15_loose_ion.TriggerChain             = "HLT_e15_loose_ion"

        
        ToolSvc += IDPerfMonZee_e15_loose_ion
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonZee_e15_loose_ion
        ToolSvc += IDPerfMonZee_2e10_loose_ion
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonZee_2e10_loose_ion

        ToolSvc += IDPerfMonWenu_e15_loose_ion
        if (InDetFlags.doPrintConfigurables()):
            print IDPerfMonWenu_e15_loose_ion

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

if not DQMonFlags.useTrigger():
    print "IDPerfMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
else:
    if not rec.doHeavyIon():
        print 'INFO: adding trigger aware monitoring to tool'
        IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_e24_medium_L1EM18VH]
        IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_e24_tight_L1EM20VH ]
        
        IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_e24_medium_L1EM18VH ]
        IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_e24_tight_L1EM20VH ]
    if rec.doHeavyIon():        
        print 'INFO: adding trigger aware monitoring to tool (HI)'
        IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_e15_loose_ion]
        IDPerfMonManager.AthenaMonTools += [ IDPerfMonZee_2e10_loose_ion ]

        IDPerfMonManager.AthenaMonTools += [ IDPerfMonWenu_e15_loose_ion ]
    


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
