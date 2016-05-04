### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")
###############################

doTrigger=True
TriggerModernConfig=True
rec.doWriteAOD=True
rec.doWriteESD=True
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False
rec.doESD=True 
doTAG=False
rec.doCBNT=False
#rec.doTruth=True

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

# set up trigger monitoring                                                                                                                                                        
if not ('RunningRTT' in dir()):
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' , 'Log' ]
else:
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' ]

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'default'
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 

TriggerFlags.doHLT=True
TriggerFlags.L1PrescaleSet  = '' 
TriggerFlags.HLTPrescaleSet = '' 
TriggerFlags.AODEDMSet="AODFULL"

#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
#from RecExConfig.RecConfFlags import recConfFlags
#recConfFlags.AllowIgnoreConfigError=False
#athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# increase verbosity of StreamAOD/ESD checks
StreamAOD.CheckNumberOfWrites = True
StreamESD.CheckNumberOfWrites = True

include("TriggerTest/TriggerTestCommon.py")

preExec='rec.doFloatingPointException=True'
