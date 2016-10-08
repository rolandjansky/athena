
## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "PrimaryManager" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = WARNING


# ************************
# Monitoring configuration
# ************************

## Setup the output file(s):
THistSvc = Service ("THistSvc")

## The string "TestMon" in the argument below is the 'FileKey'
## used by Athena to access the output file internally
THistSvc.Output += ["egammaMon DATAFILE='egammaMon.root' OPT='RECREATE'"]

## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
monMan = topSequence.PrimaryManager

## Add all the ManagedMonitorToolBase objects
## FIXME: should be done through ToolHandleArray
from egammaPerformance.egammaPerformanceConf import photonMonTool
phMonTool = photonMonTool(name= "phMonTool",
                          PhotonContainer		="PhotonAODCollection",
                          EMShowerContainer		="",
                          EMConvertContainer		="",
                          Photon_Trigger_Items = ["EF_g10","EF_g20"],
                          Photon_Selection_Items = ["all","tight"])
                          

ToolSvc+=phMonTool
monMan.AthenaMonTools += [ "photonMonTool/phMonTool" ]

from egammaPerformance.egammaPerformanceConf import electronMonTool
elMonTool = electronMonTool(name= "elMonTool",
                            ElectronContainer		="ElectronAODCollection",
                            EMTrackMatchContainer         ="",
                            EMShowerContainer		="",
                            EMConvertContainer		="",
                            Electron_Trigger_Items = ["EF_e10_medium"],
                            Electron_Selection_Items = ["all","loose","medium","tight"])
                          
ToolSvc+=elMonTool
monMan.AthenaMonTools += [ "electronMonTool/elMonTool" ]


from egammaPerformance.egammaPerformanceConf import forwardEgammaMonTool
fwdMonTool = forwardEgammaMonTool(name= "fwdMonTool",
                                  ElectronContainer        ="ElectronAODCollection",
                                  Electron_Selection_Items = ["all","tight","loose"])
ToolSvc+=fwdMonTool
monMan.AthenaMonTools += [ "forwardEgammaMonTool/fwdMonTool" ]


from egammaPerformance.egammaPerformanceConf import physicsMonTool
ZeeMonTool = physicsMonTool(name= "ZeeMonTool",
                            ElectronContainer		="ElectronAODCollection",
                            Trigger_Items = ["EF_e20_loose"],
                            ProcessName = "Zee",
                            Selection_Items = ["all"],
                            massShift = 91188,
                            massElectronClusterEtCut = 15000,
                            PhiBinning = 64,
                            massLowerCut = 70000,
                            massUpperCut = 110000)
                          
ToolSvc+=ZeeMonTool
monMan.AthenaMonTools += [ "physicsMonTool/ZeeMonTool" ]

from egammaPerformance.egammaPerformanceConf import physicsMonTool
JPsiMonTool = physicsMonTool(name= "JPsiMonTool",
                            ElectronContainer="ElectronAODCollection",
                            Trigger_Items = ["EF_2e5_medium"],
                            ProcessName = "JPsiee",
                            Selection_Items = ["all"],
                            massShift = 3097,
                            massElectronClusterEtCut = 3000 ,
                            PhiBinning = 40,
                            massLowerCut = 1000,
                            massUpperCut = 5000)
                          
ToolSvc+=JPsiMonTool
monMan.AthenaMonTools += [ "physicsMonTool/JPsiMonTool" ]

from egammaPerformance.egammaPerformanceConf import physicsMonTool
Upsilon1SMonTool = physicsMonTool(name= "Upsilon1SMonTool",
                                ElectronContainer="ElectronAODCollection",
                                Trigger_Items = ["EF_2e5_medium"],
                                ProcessName = "Upsilon1See",
                                Selection_Items = ["all"],
                                massShift = 9460,
                                massElectronClusterEtCut = 1000 ,
                                #PhiBinning = 64,
                                massLowerCut = 5000,
                                massUpperCut = 15000)
        
ToolSvc+=Upsilon1SMonTool
monMan.AthenaMonTools += [ "physicsMonTool/Upsilon1SMonTool" ]


from egammaPerformance.egammaPerformanceConf import physicsMonTool
Upsilon2SMonTool = physicsMonTool(name= "Upsilon2SMonTool",
                                ElectronContainer="ElectronAODCollection",
                                Trigger_Items = ["EF_2e5_medium"],
                                ProcessName = "Upsilon2See",
                                Selection_Items = ["all"],
                                massShift = 10023,
                                massElectronClusterEtCut = 1000 ,
                                #PhiBinning = 64,
                                massLowerCut = 5000,
                                massUpperCut = 15000)

ToolSvc+=Upsilon2SMonTool
monMan.AthenaMonTools += [ "physicsMonTool/Upsilon2SMonTool" ]



from egammaPerformance.egammaPerformanceConf import ephysicsMonTool
WenuMonTool = ephysicsMonTool(name= "WenuMonTool",
                                ElectronContainer="ElectronAODCollection",
                                JetContainer="Cone7TowerJets",
###                                Trigger_Items = ["EF_e25i_loose"],
                                metName = "MET_RefFinal",
                                ProcessName = "Wenu",
##                                Selection_Items = ["loose","tight"],
                                triggerXselection = ["EF_e25i_loose","medium","tight","tight","loose","loose"],
                                massShift = 9460,
                                massElectronClusterEtCut = 1000 ,
                                massLowerCut = 0,
                                massUpperCut = 200,
                                #PhiBinning = 64,
                                LeadingElectronClusterPtCut = 25000,
                                MissingParticleEtCut = 40000,
                                JetEnergyCut = 30000,
                                DeltaRCut = 1)
				
ToolSvc+=WenuMonTool
monMan.AthenaMonTools += [ "ephysicsMonTool/WenuMonTool" ]





## FileKey must match that given to THistSvc
monMan.FileKey = "egammaMon"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "user"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

## get a handle on the ToolSvc
from AthenaCommon.AppMgr import ToolSvc as toolSvc

