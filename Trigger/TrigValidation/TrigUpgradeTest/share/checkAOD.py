import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1")
Stream1.OutputFile = "ESDout.pool.root"

svcMgr.EventSelector.InputCollections   = [ "/eos/atlas/atlascerngroupdisk/data-art/grid-output/21.0/Athena/x86_64-slc6-gcc62-opt/2018-12-08T2201/TrigAnalysisTest/test_physics_pp_v7_rdotoesdaod_grid/AOD.pool.root" ]
#svcMgr.EventSelector.InputCollections   = [ "myESD.pool.root" ]
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
MessageSvc.defaultLimit = 9999999
MessageSvc.useColors = True
checker                                 = TrigEDMChecker()
checker.OutputLevel                     = DEBUG
checker.doDumpAll                       = False
checker.doDumpStoreGate                 = False
checker.doDumpNavigation                = True
checker.doDumpTrigCompsiteNavigation    = True

checker.Decisions = "exportTest"
checker.dumpTrigCompositeContainers = [ "exportTest" ]


from AthenaCommon.AppMgr import topSequence
topSequence += checker

theApp.EvtMax = 10
svcMgr.EventSelector.SkipEvents = 9
