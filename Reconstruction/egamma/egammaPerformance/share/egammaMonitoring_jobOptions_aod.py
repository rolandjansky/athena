if DQMonFlags.monManEnvironment() == 'tier0ESD':

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
 
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    from AthenaMonitoring.DQMonFlags import DQMonFlags
 
    try:

        monManEgamma = AthenaMonManager(name="EgammaMonManager",
                                        FileKey             = DQMonFlags.monManFileKey(),
                                        Environment         = DQMonFlags.monManEnvironment(),
                                        ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                        DataType            = DQMonFlags.monManDataType())
        topSequence += monManEgamma

        from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
        monbadlb = GetLArBadLBFilterTool()
        
        egammaMonOutputLevel = INFO
### Setup which objects will be monitored
        if not ('egammaMonitorPhotons' in dir()):
            egammaMonitorPhotons  = True
            
        if not ('egammaMonitorElectrons' in dir()):
            egammaMonitorElectrons  = True

        if not ('egammaMonitorFwdEg' in dir()):
            egammaMonitorFwdEg  = True

        if not ('egammaMonitorSoftEg' in dir()):
            egammaMonitorSoftEg  = True
                    
        if not ('egammaMonitorZee' in dir()):
            egammaMonitorZee  = True

        if not ('egammaMonitorJPsi' in dir()):
            egammaMonitorJPsi  = True

        if not ('egammaMonitorUpsilon1S' in dir()):
            egammaMonitorUpsilon1S  = True

        if not ('egammaMonitorUpsilon2S' in dir()):
            egammaMonitorUpsilon2S  = True

        if not ('egammaMonitorWenu' in dir()):
            egammaMonitorWenu  = True

        if not ('egammaMonitorTop' in dir()):
            egammaMonitorTop  = True    

## Commenting out Top tool for now
        egammaMonitorTop = False
        egammaMonitorJPsi  = False
        egammaMonitorUpsilon1S  = False
        egammaMonitorUpsilon2S  = False
            
###


        photonTrigItems = []
        electronTrigItems = []
        ZeeTrigItems = []
        JPsiTrigItems = []
        UpsilonTrigItems = []
        SoftETrigItems = []
        FrwdETrigItems = []
        WenutriggerXselection = ["tight","tight","loose","loose"]
       # WenutriggerXselection = []
        MyTrigDecisionTool = ""

        if DQMonFlags.useTrigger():

            photonTrigItems = ["EF_g35_loose_g25_loose"]
            electronTrigItems = ["EF_e24vh_medium1","EF_2e12Tvh_loose1"]
            UpsilonTrigItems = ["EF_2e12Tvh_loose1"]
            SoftETrigItems = ["EF_2e12Tvh_loose1"]
            FrwdETrigItems = ["EF_e24vh_medium1"]
            ZeeTrigItems = ["EF_e24vh_medium1"]
            JPsiTrigItems = ["EF_2e12Tvh_loose1"]
            WenutriggerXselection = ["EF_e24vh_medium1","medium","tight","tight","loose","loose"]
            MyTrigDecisionTool = "Trig::TrigDecisionTool/TrigDecisionTool"

        if(egammaMonitorPhotons):
            from egammaPerformance.egammaPerformanceConf import photonMonTool
            phMonTool = photonMonTool(name= "phMonTool",
                                      PhotonContainer		="PhotonAODCollection",
                                      EMShowerContainer		="",
                                      EMConvertContainer		="",
                                      Photon_Trigger_Items = photonTrigItems,
                                      Photon_Selection_Items = ["all","loose","tight"],
                                      Photon_Divison_Items = ["PHOB","PHOC","PHOE"],
                                      OutputLevel = egammaMonOutputLevel,
                                      TrigDecisionTool = MyTrigDecisionTool,
                                      UseTrigger = DQMonFlags.useTrigger() )

            phMonTool.FilterTools += [ monbadlb ]

            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                phMonTool.FilterTools.append(monFilledBunchFilterTool)
            ToolSvc+=phMonTool
            monManEgamma.AthenaMonTools += [ "photonMonTool/phMonTool" ]

        if(egammaMonitorElectrons):
            from egammaPerformance.egammaPerformanceConf import electronMonTool
            elMonTool = electronMonTool(name= "elMonTool",
                                        ElectronContainer		="ElectronAODCollection",
                                        EMTrackMatchContainer         ="",
                                        EMShowerContainer		="",
                                        EMConvertContainer		="",
                                        Electron_Trigger_Items = electronTrigItems,
                                        Electron_Selection_Items = ["all","loose","medium","tight"],
                                        Electron_Divison_Items = ["EIDB","EIDC","EIDE","EIDA"],
                                        OutputLevel = egammaMonOutputLevel,
                                        TrigDecisionTool = MyTrigDecisionTool,
                                        UseTrigger = DQMonFlags.useTrigger())

            elMonTool.FilterTools += [ monbadlb ]

            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                elMonTool.FilterTools.append(monFilledBunchFilterTool)
            ToolSvc+=elMonTool
            monManEgamma.AthenaMonTools += [ "electronMonTool/elMonTool" ]


        if(egammaMonitorFwdEg):
            from egammaPerformance.egammaPerformanceConf import forwardEgammaMonTool
            fwdMonTool = forwardEgammaMonTool(name= "fwdMonTool",
                                              ElectronContainer        ="ElectronAODCollection",
                                              Electron_Trigger_Items = FrwdETrigItems,
                                              Electron_Selection_Items = ["all","tight","loose"],
                                              TrigDecisionTool = MyTrigDecisionTool,
                                              UseTrigger = DQMonFlags.useTrigger())

            fwdMonTool.FilterTools += [ monbadlb ]

            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                fwdMonTool.FilterTools.append(monFilledBunchFilterTool)
            ToolSvc+=fwdMonTool
            monManEgamma.AthenaMonTools += [ "forwardEgammaMonTool/fwdMonTool" ]    

        if(egammaMonitorSoftEg):
            from egammaPerformance.egammaPerformanceConf import softEMonTool
            sfMonTool = softEMonTool(name= "sfMonTool",
                                     ElectronContainer               ="ElectronAODCollection",
                                     EMTrackMatchContainer         ="",
                                     EMShowerContainer               ="",
                                     Electron_Trigger_Items = SoftETrigItems,
                                     Electron_Selection_Items = ["all","loose","tight"],
                                     Electron_Divison_Items = ["softEIDB","softEIDC","softEIDE"],
                                     OutputLevel = egammaMonOutputLevel,
                                     TrigDecisionTool = MyTrigDecisionTool,
                                     UseTrigger = DQMonFlags.useTrigger())

            sfMonTool.FilterTools += [ monbadlb ]

            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                sfMonTool.FilterTools.append(monFilledBunchFilterTool) 
            ToolSvc+=sfMonTool
            monManEgamma.AthenaMonTools += [ "softEMonTool/sfMonTool" ]


        if(egammaMonitorZee):
            from egammaPerformance.egammaPerformanceConf import physicsMonTool
            ZeeMonTool = physicsMonTool(name= "ZeeMonTool",
                                        ElectronContainer		="ElectronAODCollection",
                                        Trigger_Items = ZeeTrigItems,
                                        ProcessName = "Zee",
                                        Selection_Items = ["all"],
                                        massShift = 91188,
                                        massElectronClusterEtCut = 20000,
                                        massLowerCut = 60000,
                                        massUpperCut = 120000,
                                        PhiBinning = 64,
                                        OutputLevel = egammaMonOutputLevel,
                                        TrigDecisionTool = MyTrigDecisionTool,
                                        UseTrigger = DQMonFlags.useTrigger())
        
            ToolSvc+=ZeeMonTool
            monManEgamma.AthenaMonTools += [ "physicsMonTool/ZeeMonTool" ]

        if(egammaMonitorJPsi):
            from egammaPerformance.egammaPerformanceConf import physicsMonTool
            JPsiMonTool = physicsMonTool(name= "JPsiMonTool",
                                         ElectronContainer="ElectronAODCollection",
                                         Trigger_Items = JPsiTrigItems,
                                         ProcessName = "JPsiee",
                                         Selection_Items = ["all"],
                                         massShift = 3097,
                                         massElectronClusterEtCut = 3000 ,
                                         massLowerCut = 2500,
                                         massUpperCut = 3500,
                                         PhiBinning = 40,
                                         OutputLevel = egammaMonOutputLevel,
                                         TrigDecisionTool = MyTrigDecisionTool,
                                         UseTrigger = DQMonFlags.useTrigger())
        
            ToolSvc+=JPsiMonTool
            monManEgamma.AthenaMonTools += [ "physicsMonTool/JPsiMonTool" ]


        if(egammaMonitorUpsilon1S):
            from egammaPerformance.egammaPerformanceConf import physicsMonTool
            Upsilon1SMonTool = physicsMonTool(name= "Upsilon1SMonTool",
                                              ElectronContainer="ElectronAODCollection",
                                              Trigger_Items = UpsilonTrigItems,
                                              ProcessName = "Upsilon1See",
                                              Selection_Items = ["all"],
                                              massShift = 9460,
                                              massElectronClusterEtCut = 1000 ,
                                              massLowerCut = 5000,
                                              massUpperCut = 15000,
                                              #PhiBinning = 64,
                                              OutputLevel = egammaMonOutputLevel,
                                              TrigDecisionTool = MyTrigDecisionTool,
                                              UseTrigger = DQMonFlags.useTrigger())
            
            ToolSvc+=Upsilon1SMonTool
            monManEgamma.AthenaMonTools += [ "physicsMonTool/Upsilon1SMonTool" ]


        if(egammaMonitorUpsilon2S):
            from egammaPerformance.egammaPerformanceConf import physicsMonTool
            Upsilon2SMonTool = physicsMonTool(name= "Upsilon2SMonTool",
                                              ElectronContainer="ElectronAODCollection",
                                              Trigger_Items = UpsilonTrigItems,
                                              ProcessName = "Upsilon2See",
                                              Selection_Items = ["all"],
                                              massShift = 10023,
                                              massElectronClusterEtCut = 1000 ,
                                              massLowerCut = 5000,
                                              massUpperCut = 15000,
                                              #PhiBinning = 64,
                                              OutputLevel = egammaMonOutputLevel,
                                              TrigDecisionTool = MyTrigDecisionTool,
                                              UseTrigger = DQMonFlags.useTrigger())
            
            ToolSvc+=Upsilon2SMonTool
            monManEgamma.AthenaMonTools += [ "physicsMonTool/Upsilon2SMonTool" ]

        if(egammaMonitorWenu):
            from egammaPerformance.egammaPerformanceConf import ephysicsMonTool
            WenuMonTool = ephysicsMonTool(name= "WenuMonTool",
                                          ElectronContainer="ElectronAODCollection",
                                          #JetContainer="Cone7TowerJets",
                                          JetContainer="AntiKt4TopoEMJets",
                                          metName = "MET_RefFinal",
                                          ProcessName = "Wenu",
                                          triggerXselection = WenutriggerXselection,
                                          massShift = 9460,
                                          massElectronClusterEtCut = 1000 ,
                                          massLowerCut = 0,
                                          massUpperCut = 200,
                                          #PhiBinning = 64,
                                          LeadingElectronClusterPtCut = 20000,
                                          MissingParticleEtCut = 25000,
                                          JetEnergyCut = 1000000,
                                          DeltaRCut = 10,
                                          OutputLevel = egammaMonOutputLevel,
                                          TrigDecisionTool = MyTrigDecisionTool,
                                          UseTrigger = DQMonFlags.useTrigger())
            
            ToolSvc+=WenuMonTool
            monManEgamma.AthenaMonTools += [ "ephysicsMonTool/WenuMonTool" ]


        if(egammaMonitorTop):
            from egammaPerformance.egammaPerformanceConf import TopphysicsMonTool
            TopMonTool = TopphysicsMonTool(name= "TopMonTool",
                                          ElectronContainer="ElectronAODCollection",
                                          JetContainer="AntiKt4TopoEMJets",
                                          metName = "MET_RefFinal",
                                          ProcessName = "Topww",
                                          triggerXselection = WenutriggerXselection,
                                          Selection_Items = ["all","loose","tight","medium"],
                                          massShift = 9460,
                                          massElectronClusterEtCut = 1000 ,
                                          massLowerCut = 0,
                                          massUpperCut = 200,
                                          #PhiBinning = 64,
                                          LeadingElectronClusterPtCut = 25000,
                                          MissingParticleEtCut = 40000,
                                          JetEnergyCut = 1000000,
                                          DeltaRCut = 10,
                                          OutputLevel = egammaMonOutputLevel,
                                          TrigDecisionTool = MyTrigDecisionTool,
                                          UseTrigger = DQMonFlags.useTrigger())
            
            ToolSvc+=TopMonTool
            monManEgamma.AthenaMonTools += [ "TopphysicsMonTool/TopMonTool" ]
        
            

    except Exception:
        from AthenaCommon.Resilience import treatException
        treatException("egammaMonitoring_jobOptions.py: exception when setting up Egamma monitoring")
 
