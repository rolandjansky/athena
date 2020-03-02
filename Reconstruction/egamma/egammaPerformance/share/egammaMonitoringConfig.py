#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
    
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
from AthenaMonitoring.DQMonFlags import DQMonFlags

if  DQMonFlags.monManEnvironment() in ('tier0','tier0ESD','online', 'AOD'):

# dual use of Run2 and Run3 monitoring

    from AthenaCommon.Configurable import Configurable

    if Configurable.configurableRun3Behavior == 1 : # Configure Run3 monitoring
 
        try:
            from egammaPerformance.egammaPerformanceConf import MonitorElectronConfig
            from egammaPerformance.egammaPerformanceConf import MonitorPhotonConfig
            # Set the Athena configuration flags
            from AthenaConfiguration.AllConfigFlags import ConfigFlags
            ConfigFlags.lock()
            # Initialize configuration object, add accumulator, merge, and run.
            from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
            from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
            cfg = MainServicesSerialCfg()
            cfg.merge(PoolReadCfg(ConfigFlags))

            MonitorElectronAcc = MonitorElectronConfig(ConfigFlags)
            cfg.merge(MonitorElectronAcc)

            MonitorPhotonAcc = MonitorPhotonConfig(ConfigFlags)
            cfg.merge(MonitorPhotonAcc)

        except Exception:
            from AthenaCommon.Resilience import treatException
            treatException("egammaMonitoring_jobOptions.py: exception when setting up Egamma monitoring for Run III")
            
    else : # Configure Run2 monitoring

        import TrigEgammaMonitoring.TrigEgammaMonitCategory as egammaConf
        try:

            # Set the Athena configuration flags
            from AthenaCommon.AthenaCommonFlags import jobproperties
            from AthenaCommon.Constants import INFO


            monManEgamma = AthenaMonManager(name="EgammaMonManager",
                                            FileKey             = DQMonFlags.monManFileKey(),
                                            Environment         = DQMonFlags.monManEnvironment(),
                                            ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                            DataType            = DQMonFlags.monManDataType())
            topSequence += monManEgamma
        
            from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
            from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool

            ### Setup which objects will be monitored
            
            ## Commenting in/out Mon tools for now
            egammaMonitorUpsilon1S  = False
            egammaMonitorUpsilon2S  = False
            # egammaMonitorJPsi  = False
            # egammaMonitorZee  = False
            # egammaMonitorFwdEg  = False
            # egammaMonitorPhotons  = False
            # egammaMonitorElectrons  = False

            if not ('egammaMonitorPhotons' in dir()):
                egammaMonitorPhotons  = True
            if not ('egammaMonitorElectrons' in dir()):
                egammaMonitorElectrons  = True
            if not ('egammaMonitorFwdEg' in dir()):
                egammaMonitorFwdEg  = True
            if not ('egammaMonitorZee' in dir()):
                egammaMonitorZee  = True
            if not ('egammaMonitorJPsi' in dir()):
                egammaMonitorJPsi  = True
            if not ('egammaMonitorUpsilon1S' in dir()):
                egammaMonitorUpsilon1S  = True
            if not ('egammaMonitorUpsilon2S' in dir()):
                egammaMonitorUpsilon2S  = True

            photonTrigItems = []
            electronTrigItems = []
            mySingleElectronTrigger = []
            myDiElectronTrigger = [] 
            ZeeTrigItems = []
            JPsiTrigItems = []
            UpsilonTrigItems = []
            FrwdETrigItems = []
            MyTrigDecisionTool = ""

            # do trigger by default for at least one electron and photon monitor

            BypassphotonTrigItems = []
            BypassmySingleElectronTrigItems = []
            # Force the trigger with a trigger found in the test file for test purpose
            # BypassphotonTrigItems = ['L1_EM20VH']
            # BypassmySingleElectronTrigItems = ['L1_EM20VH']

            BypassphotonTrigItems += egammaConf.primary_double_pho
            BypassmySingleElectronTrigItems += egammaConf.primary_single_ele
            print("egamma electron trigger = %s" % BypassmySingleElectronTrigItems)

            BypassMyTrigDecisionTool = monTrigDecTool if DQMonFlags.useTrigger() else None

            ## Commenting in/out Trigger for now
            MyDoTrigger = False
            #MyDoTrigger = DQMonFlags.useTrigger()

            if (MyDoTrigger):
                photonTrigItems += egammaConf.primary_double_pho
                mySingleElectronTrigger += egammaConf.primary_single_ele
                myDiElectronTrigger += egammaConf.primary_double_ele
                JPsiTrigItems += egammaConf.monitoring_Jpsiee
                FrwdETrigItems += egammaConf.primary_single_ele
                ZeeTrigItems += egammaConf.monitoring_Zee 
                MyTrigDecisionTool = monTrigDecTool
            
            egammaMonOutputLevel = INFO
            #egammaMonOutputLevel = VERBOSE
            #egammaMonOutputLevel = DEBUG
        
            if(egammaMonitorPhotons):
                from egammaPerformance.egammaPerformanceConf import photonMonTool
                phMonTool = photonMonTool(name= "phMonTool",
                                      EgTrigDecisionTool = MyTrigDecisionTool,
                                      EgUseTrigger = MyDoTrigger,
                                      EgTrigger = photonTrigItems,
                                      EgGroupExtension = "",
                                      PhotonContainer = "Photons",
                                      OutputLevel = egammaMonOutputLevel)            
                if jobproperties.Beam.beamType()=='collisions':
                    phMonTool.FilterTools += [ GetLArBadLBFilterTool() ]
                    phMonTool.FilterTools += [ GetFilledBunchFilterTool() ]
                monManEgamma.AthenaMonTools += [ phMonTool ]    
                print(phMonTool)

                if DQMonFlags.useTrigger():
                    phMonToolWithTrigger = photonMonTool(name= "phMonToolWithTrigger",
                                                     EgTrigDecisionTool = BypassMyTrigDecisionTool,
                                                     EgUseTrigger = True,
                                                     EgTrigger = BypassphotonTrigItems,
                                                     EgGroupExtension = "WithTrigger",
                                                     PhotonContainer = "Photons",
                                                     OutputLevel = egammaMonOutputLevel)
                    if jobproperties.Beam.beamType()=='collisions':
                        phMonToolWithTrigger.FilterTools += [ GetLArBadLBFilterTool() ]
                        phMonToolWithTrigger.FilterTools += [ GetFilledBunchFilterTool() ]
                    monManEgamma.AthenaMonTools += [ phMonToolWithTrigger ]    
                    print(phMonToolWithTrigger)
                

            if(egammaMonitorElectrons):
                from egammaPerformance.egammaPerformanceConf import electronMonTool
                elMonTool = electronMonTool(name= "elMonTool",
                                        EgTrigDecisionTool = MyTrigDecisionTool,
                                        EgUseTrigger = MyDoTrigger,
                                        EgTrigger = mySingleElectronTrigger,
                                        EgGroupExtension = "",
                                        ElectronContainer = "Electrons",
                                        OutputLevel = egammaMonOutputLevel)
                if jobproperties.Beam.beamType()=='collisions':
                    elMonTool.FilterTools += [ GetLArBadLBFilterTool() ]
                    elMonTool.FilterTools += [ GetFilledBunchFilterTool() ]
                monManEgamma.AthenaMonTools += [ elMonTool ]
                print(elMonTool)

                if DQMonFlags.useTrigger():
                    elMonToolWithTrigger = electronMonTool(name= "elMonToolWithTrigger",
                                                           EgTrigDecisionTool = BypassMyTrigDecisionTool,
                                                           EgUseTrigger = True,
                                                           EgTrigger = BypassmySingleElectronTrigItems,
                                                           EgGroupExtension = "WithTrigger",
                                                           ElectronContainer = "Electrons",
                                                           OutputLevel = egammaMonOutputLevel)
                    if jobproperties.Beam.beamType()=='collisions':
                        elMonToolWithTrigger.FilterTools += [ GetLArBadLBFilterTool() ]
                        elMonToolWithTrigger.FilterTools += [GetFilledBunchFilterTool()]
                    monManEgamma.AthenaMonTools += [ elMonToolWithTrigger ]
                    print(elMonToolWithTrigger)

            if(egammaMonitorFwdEg):
                from egammaPerformance.egammaPerformanceConf import forwardElectronMonTool
                fwdMonTool = forwardElectronMonTool(name= "fwdMonTool",
                                                EgTrigDecisionTool = MyTrigDecisionTool,
                                                EgUseTrigger = MyDoTrigger,
                                                EgTrigger = FrwdETrigItems,
                                                ForwardElectronContainer ="ForwardElectrons",
                                                OutputLevel = egammaMonOutputLevel)
                if jobproperties.Beam.beamType()=='collisions':
                    fwdMonTool.FilterTools += [ GetLArBadLBFilterTool() ]
                    fwdMonTool.FilterTools += [ GetFilledBunchFilterTool() ]
                monManEgamma.AthenaMonTools += [ fwdMonTool ]    
                print(fwdMonTool)

            if(egammaMonitorZee):
                from egammaPerformance.egammaPerformanceConf import ZeeTaPMonTool
                ZeeMonTool = ZeeTaPMonTool(name= "ZeeMonTool",
                                           EgTrigDecisionTool = MyTrigDecisionTool,
                                           EgUseTrigger = MyDoTrigger,
                                           EgTrigger = ZeeTrigItems,
                                           EgGroupExtension="Z",
                                           ElectronContainer  ="Electrons",
                                           massPeak = 91188,
                                           electronEtCut = 20000,
                                           massLowerCut = 60000,
                                           massUpperCut = 120000,
                                           #PhiBinning = 64,
                                           OutputLevel = egammaMonOutputLevel,
                                           TrigDecisionTool = MyTrigDecisionTool,
                                           UseTrigger = MyDoTrigger)
                if jobproperties.Beam.beamType()=='collisions':
                    ZeeMonTool.FilterTools += [ GetLArBadLBFilterTool() ]
                    ZeeMonTool.FilterTools += [ GetFilledBunchFilterTool() ]
                monManEgamma.AthenaMonTools += [ ZeeMonTool ]
                print(ZeeMonTool)

            if(egammaMonitorJPsi):
                from egammaPerformance.egammaPerformanceConf import ZeeTaPMonTool
                JPsiMonTool = ZeeTaPMonTool(name= "JPsiMonTool",
                                        EgTrigDecisionTool = MyTrigDecisionTool,
                                        EgUseTrigger = MyDoTrigger,
                                        EgTrigger = JPsiTrigItems,
                                        EgGroupExtension="JPsi",
                                        ElectronContainer="Electrons",
                                        massPeak = 3097,
                                        electronEtCut = 3000,
                                        massLowerCut = 2000,
                                        massUpperCut = 5000,
                                        #PhiBinning = 40,
                                        OutputLevel = egammaMonOutputLevel,
                                        TrigDecisionTool = MyTrigDecisionTool,
                                        UseTrigger = MyDoTrigger)
                if jobproperties.Beam.beamType()=='collisions':
                    JPsiMonTool.FilterTools += [ GetLArBadLBFilterTool() ]                
                    JPsiMonTool.FilterTools += [ GetFilledBunchFilterTool() ]       
                monManEgamma.AthenaMonTools += [ JPsiMonTool ]
                print(JPsiMonTool)

            if(egammaMonitorUpsilon1S):
                from egammaPerformance.egammaPerformanceConf import physicsMonTool
                Upsilon1SMonTool = physicsMonTool(name= "Upsilon1SMonTool",
                                              ElectronContainer="Electrons",
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
                                              UseTrigger = MyDoTrigger)
                if jobproperties.Beam.beamType()=='collisions':
                    Upsilon1SMonTool.FilterTools += [ GetLArBadLBFilterTool() ]                
                    Upsilon1SMonTool.FilterTools += [ GetFilledBunchFilterTool() ]       
                monManEgamma.AthenaMonTools += [ Upsilon1SMonTool ]
                print(Upsilon1SMonTool)

            if(egammaMonitorUpsilon2S):
                from egammaPerformance.egammaPerformanceConf import physicsMonTool
                Upsilon2SMonTool = physicsMonTool(name= "Upsilon2SMonTool",
                                              ElectronContainer="Electrons",
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
                                              UseTrigger = MyDoTrigger)
                if jobproperties.Beam.beamType()=='collisions':
                    Upsilon2SMonTool.FilterTools += [ GetLArBadLBFilterTool() ]                
                    Upsilon2SMonTool.FilterTools += [ GetFilledBunchFilterTool() ]       
                monManEgamma.AthenaMonTools += [ Upsilon2SMonTool ]
                print(Upsilon2SMonTool)

        except Exception:
            from AthenaCommon.Resilience import treatException
            treatException("egammaMonitoring_jobOptions.py: exception when setting up Egamma monitoring for Run II")
