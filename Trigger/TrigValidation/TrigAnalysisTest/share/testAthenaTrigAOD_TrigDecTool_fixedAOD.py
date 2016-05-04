from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not ('fileList' in dir()):
     fileList=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e380_s764_r1295/AOD.134900._000001.pool.root.1']

if not fileList == []:
    acf.FilesInput=fileList

rec.readAOD=True


if not acf.EvtMax.is_locked():
    acf.EvtMax=2000
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

###############################
rec.doCBNT=False


#doTrigger=False
rec.doTrigger=False
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=False


rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False 
rec.doESD=False 
doTAG=False

#rec.doTruth=True


#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False


from TrigValAlgs.TrigValAlgsConfig import TrigDecisionChecker

TrigDecChecker = TrigDecisionChecker("TrigDecChecker")
TrigDecChecker.WriteEventDecision=False
TrigDecChecker.MonitoringBlock=100
TrigDecChecker.MonitoredChains=['EF_e24_medium_iloose ','EF_mu24']  
#TrigDecChecker.MonitoredChains=['L2_e10_medium','EF_e10_medium','L2_mu10','EF_mu10']  


from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigDecChecker


include("TriggerTest/TriggerTestCommon.py")


# get the name auditor
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors  += [ "NameAuditor"]


