readAOD=True

EvtMax=2
PoolAODInput=["/tmp/klemetti/data09_900GeV.00141811.physics_MinBias.merge.AOD.f187_m304/data09_900GeV.00141811.physics_MinBias.merge.AOD.f187_m304._lb0074-lb0093.1"]
OutputLevel=INFO
DetDescrVersion="ATLAS-GEO-02-01-00"


doTrigger=False

TriggerModernConfig=True

doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False 
doESD=False 
doTAG=False

doCBNT=False
doTruth=False

include("RecExCommon/RecExCommon_topOptions.py")
include("TriggerRateTools/TriggerRateTools_common_readAOD.py")

triggerRateTools.OutputLevel = 3
ServiceMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='TriggerRates.root' OPT='RECREATE'"]
triggerRateTools.xSection = 1.0 # barns
triggerRateTools.Luminosity = 1.0 # barns^-1 sec^-1
#triggerRateTools.collsPerEvent = 5.1 # for pike-up
triggerRateTools.doSlicesInfo = False
triggerRateTools.doRawTD = True
triggerRateTools.doTextOutput = True
#triggerRateTools.ignorePassThrough = True
