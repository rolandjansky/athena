include("AthenaPoolDataModelTest/esdtoesd_base.py")

svcMgr.EventSelector.InputCollections   = [ "myESD.pool.root" ]

from AthenaCommon.AppMgr import topSequence

#
# TOOLS AND SERVICES
#

# Add the TDT, set it to use the Conf SVC rather than xAOD metadata (not ready for that yet)
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
ToolSvc.TrigDecisionTool.TrigConfigSvc = "Trig::TrigConfigSvc/TrigConfigSvc"

# Note that for now we don't properly configure the trigger configuration service yet. So we give the "run3_dummy" property here first 
# which will load some hard-coded items from HLTConfigSvc.cxx for testing.
from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvc
ServiceMgr += TrigConfigSvc("TrigConfigSvc")
ServiceMgr.TrigConfigSvc.PriorityList = ["run3_dummy", "ds", "xml"]

#
# ALGS
#

# At runtime it claims it currently does not need this but it's wrong
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()
Stream1.ItemList += [ "xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux." ]

# Add the trigger decision creation algorithm
from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMakerMT
topSequence+=TrigDecisionMakerMT()
topSequence.TrigDecMakerMT.OutputLevel = VERBOSE
Stream1.ItemList += [ "xAOD::TrigDecision#xTrigDecision", "xAOD::TrigDecisionAuxInfo#xTrigDecisionAux." ]

# Add the trigger menu conversion algorithm:
from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriter
xAODMenuWriter = TrigConf__xAODMenuWriter("xAODMenuWriter")
topSequence += xAODMenuWriter
topSequence.xAODMenuWriter.OutputLevel = VERBOSE
Stream1.ItemList += [ "xAOD::TrigConfKeys#TrigConfKeys" ] # Note: no aux!
Stream1.MetadataItemList += [ "xAOD::TriggerMenuContainer#TriggerMenu", "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ]

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
