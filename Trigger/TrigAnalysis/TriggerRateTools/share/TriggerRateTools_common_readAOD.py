#import new TrigDecisionTool 
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

# set up trigger config service
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
trigcfg = SetupTrigConfigSvc()
trigcfg.SetStates("ds") # this is for 13.0.40 or 14.X.0 data; for 13.0.30 data replace "ds" with "aod".
trigcfg.InitialiseSvc()

# set up metadata services ... NB comment if "aod" state is selected above
from EventInfoMgt.EventInfoMgtConf import TagInfoMgr
ServiceMgr += TagInfoMgr()
#ServiceMgr.TagInfoMgr.AddGeoModelTags = False
from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
ToolSvc += IOVDbMetaDataTool( "IOVDbMetaDataTool" )
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
ServiceMgr += MetaDataSvc( "MetaDataSvc" )
ServiceMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
ServiceMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]
import IOVDbSvc.IOVDb

# declare trigger metadata ... NB comment if "aod" state is selected above
ServiceMgr.IOVDbSvc.Folders += [ "/TRIGGER/HLT/Menu <tag>HEAD</tag>" ]
ServiceMgr.IOVDbSvc.Folders += [ "/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>" ]
ServiceMgr.IOVDbSvc.Folders += [ "/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>" ]
ServiceMgr.IOVDbSvc.Folders += [ "/TRIGGER/LVL1/Menu <tag>HEAD</tag>" ]
ServiceMgr.IOVDbSvc.Folders += [ "/TRIGGER/LVL1/Prescales <tag>HEAD</tag>" ]
# end work around


# Default TriggerRateTools output & config

from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()
from TriggerRateTools.TriggerRateToolsConf import Trig__TriggerRateTools
triggerRateTools = Trig__TriggerRateTools("TriggerRateTools")

# TriggerRateTools options
#ServiceMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='TriggerRates.root' OPT='RECREATE'"]
triggerRateTools.doTextOutput         = False
triggerRateTools.doVeto               = False
triggerRateTools.doRawTD              = False
triggerRateTools.OutputLevel          = 3
triggerRateTools.xSection             = 1.0 #barn^-1 s^-1
triggerRateTools.Luminosity           = 1.0 #barn
triggerRateTools.collsPerEvent	      = 1.0
triggerRateTools.IgnoreList           = ["L2_always","EF_always"]
triggerRateTools.CplxAndList          = []
triggerRateTools.CplxOrList           = []
triggerRateTools.PrescaleOverrideList = [["L1_RD0_FILLED","512"]]
triggerRateTools.CumulateList         = []
triggerRateTools.MenusList            = []
triggerRateTools.StreamsList          = []
triggerRateTools.doSlicesInfo         = False
triggerRateTools.ignorePassThrough    = False
triggerRateTools.doRetrieveItems      = False

TopAlg += triggerRateTools
