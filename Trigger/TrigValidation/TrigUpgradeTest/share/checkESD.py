import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1")
Stream1.OutputFile = "ESDout.pool.root"

svcMgr.EventSelector.InputCollections   = [ "myESD.pool.root" ]

from AthenaCommon.AppMgr import topSequence
from AthenaCommon.AppMgr import ToolSvc
from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigTool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
from TrigEDMConfig.TriggerEDM import EDMLibraries

cfgtool = TrigConf__xAODConfigTool('xAODConfigTool')
ToolSvc += cfgtool

tdt = Trig__TrigDecisionTool('TrigDecisionTool')
tdt.ConfigTool = cfgtool
tdt.NavigationFormat = "TrigComposite"
tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
ToolSvc += cfgtool

from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
MessageSvc.defaultLimit = 9999999
MessageSvc.useColors = True
checker                                 = TrigEDMChecker()
checker.OutputLevel                     = DEBUG
checker.doDumpAll                       = False
checker.doDumpxAODTrigElectronContainer = True
checker.doDumpxAODTrackParticle         = True
checker.doDumpTrigCompsiteNavigation    = True
checker.doDumpAllTrigComposite          = True
checker.doDumpStoreGate                 = False
checker.doTDTCheck                      = True
topSequence += checker
