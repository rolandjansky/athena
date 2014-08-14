
# Load this file only ONCE
include.block("PrimaryDPDMaker/SetupTrigDecisionTool.py")

# To read files with trigger config 
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter("ReadPool")

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

#=============================================================================
#=========    Below is the OLD way of setting up TrigDecisionTool    =========
#=============================================================================

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod: 
#from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.configurationSourceList = ['ds']

# set up trigger config service
#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#cfg =  TriggerConfigGetter()


#=============================================================================
#=========    Below is the OLDER way of setting up TrigDecisionTool  =========
#=============================================================================

###---------------         Trigger_jobOptions.py         -----------------------

##import AthenaCommon.Constants as Lvl
##from AthenaCommon.AppMgr import theApp, ToolSvc as toolSvc, ServiceMgr as svcMgr

#### Setup IOVDb
##from AthenaCommon.GlobalFlags import GlobalFlags
###GlobalFlags.DetGeo.set_atlas()
##import IOVDbSvc.IOVDb
##from IOVDbSvc.CondDB import conddb
##conddb.addFolder("TRIGGER","/TRIGGER/HLT/Menu <tag>HEAD</tag>")
##conddb.addFolder("TRIGGER","/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>")
##conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>")
##conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Menu <tag>HEAD</tag>")
##conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Prescales <tag>HEAD</tag>")

#### Setup TrigDecisionTool
###from TrigDecision.TrigDecisionConf import TrigDec__TrigDecisionTool
##tdt = CfgMgr.TrigDec__TrigDecisionTool(OutputLevel=Lvl.INFO)
##toolSvc += tdt

##from TriggerJobOpts.TriggerFlags import TriggerFlags
##TriggerFlags.doTriggerConfigOnly = True

#### Setup TrigConfigSvc
##from TrigConfigSvc.TrigConfigSvcConfig import DSConfigSvc, SetupTrigConfigSvc
##dscfg = DSConfigSvc()
##dscfg.OutputLevel=Lvl.INFO
##svcMgr += dscfg

##trigcfg = SetupTrigConfigSvc()
##trigcfg.OutputLevel=Lvl.INFO
##trigcfg.SetStates('ds')
##trigcfg.InitialiseSvc()
####svcMgr += trigcfg

### use this if you want to setup from XML
###trigcfg = SetupTrigConfigSvc()
###trigcfg.OutputLevel=Lvl.INFO
###trigcfg.SetStates('xml')
###trigcfg.hltXmlFile = './HLTconfig_lumi1E31_no_Bphysics_no_prescale_14.2.20.xml'
###trigcfg.l1XmlFile  = './LVL1config_lumi1E31_no_Bphysics_no_prescale_14.2.20.xml'
###trigcfg.InitialiseSvc()

###---------------------------------------------------------------------
