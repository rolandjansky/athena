
PoolRDOInput=["/home/asbrizzi/data/athena/lucid/14.4.0/digi.pool.root"]
EvtMax      = 80
OutputLevel = DEBUG

if not ('doFastCaloAccess' in dir()):
    doFastCaloAccess=True

doCBNT = False

doTrigger = True
TriggerModernConfig = True

doWriteAOD = False
doWriteESD = False
doWriteTAG = False

doAOD = False
doESD = False
doTAG = False

doTruth = True
doCalo  = False

include("RecExCommon/RecExCommon_flags.py")

DetFlags.Lucid_setOn()
DetFlags.BCM_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = ['Validation', 'Time', 'Log']

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'lumi1E31'
#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
from RecExConfig.RecConfFlags import recConfFlags
recConfFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

TriggerFlags.Slices_all_setOff()
TriggerFlags.doLVL1 = True
TriggerFlags.doLVL2 = False
TriggerFlags.doEF   = False

TriggerFlags.doID.set_Off()
TriggerFlags.doMuon.set_Off()
TriggerFlags.doCalo.set_Off()
TriggerFlags.doBcm.set_Off()
TriggerFlags.doLucid.set_On()

TriggerFlags.Slices_all_setOff()
TriggerFlags.doLVL2 = False
TriggerFlags.doEF   = False

include("RecExCommon/RecExCommon_topOptions.py")

jobproperties.PerfMonFlags.doMonitoring = False

TriggerFlags.doCalo=False

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

Service ("StoreGateSvc").ActivateHistory    = False
Service ("GeoModelSvc").IgnoreTagDifference = True

from AthenaCommon.Configurable import Configurable
#Configurable.allConfigurables.get('CTPSimulation').OutputLevel=ERROR
Configurable.allConfigurables.get('CTPSimulation').OutputLevel=VERBOSE

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-00-00-00'

from TrigT1Lucid.TrigT1LucidConf import LVL1__TrigT1Lucid
Lucid = LVL1__TrigT1Lucid("LVL1::TrigT1Lucid")
Lucid.OutputLevel=ERROR
Lucid.qdcThreshold=400

from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc = THistSvc()
THistSvc.Output = ["AANT DATAFILE='/home/asbrizzi/data/athena/lucid/14.4.0/lvl1.root' OPT='RECREATE'"]
ServiceMgr += THistSvc

theApp.Dlls += ["RootHistCnv"]
theApp.HistogramPersistency = "ROOT"

from CBNT_Athena.CBNT_AthenaConf import *
from CBNT_Utils.CBNT_UtilsConf   import *

CBNT_AthenaAware = CBNT_AthenaAware()

topSequence += CBNT_AthenaAware

from TrigT1Lucid.TrigT1LucidConf import *

topSequence.CBNT_AthenaAware.Members += ["CBNTAA_EventInfo"]
topSequence.CBNT_AthenaAware.Members += ["LVL1CTP::CBNTAA_CTP_RDO/CTP_RDO"]
topSequence.CBNT_AthenaAware.Members += ["LVL1::CBNTAA_TrigT1Lucid/CBNTAA_TrigT1Lucid"] 

from AnalysisTools.AnalysisToolsConf import AANTupleStream

topSequence += AANTupleStream()

AANTupleStream = topSequence.AANTupleStream
AANTupleStream.ExistDataHeader = False

CBNTAA_TrigT1Lucid = Algorithm("LVL1::CBNTAA_TrigT1Lucid/CBNTAA_TrigT1Lucid")

