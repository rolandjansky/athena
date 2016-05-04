

### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
###############################
rec.doCBNT=False


doTrigger=True

TriggerModernConfig=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=True
rec.doAOD=False
rec.doDPD=False
rec.doESD=False 
doTAG=True

#rec.doTruth=True

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'undefined_no_prescale'
TriggerFlags.triggerMenuSetup = 'Physics_pp_v5'
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 
TriggerFlags.L1PrescaleSet = ''
TriggerFlags.HLTPrescaleSet = ''
TriggerFlags.doHLT=True
#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
#from RecExConfig.RecConfFlags import recConfFlags
#recConfFlags.AllowIgnoreConfigError=False
#athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

include("TriggerTest/TriggerTestCommon.py")

