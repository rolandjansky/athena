# steering file for BS->ESD step -- data configuration
# see myTopOptions.py for more info
#doCBNT=False
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import glob

if not ('EvtMax' in dir()):
   acf.EvtMax=10


if not 'BSRDOInput' in dir():
      acf.BSRDOInput=["../testAllPT_data/EF._0001.data"]

for i, f in enumerate(BSRDOInput):
    if not glob.glob(f):
        BSRDOInput[i] = "/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/data"+f[2:]


if not 'doWriteESD' in dir():
    rec.doWriteESD=True

#testCosmicV1=True

rec.doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False
rec.doESD=False
rec.doWriteTAG=False 

rec.doCBNT=False

doTrigger=True


#doTrigger=False
#-------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_data()
#GlobalFlags.InputFormat.set_bytestream()

readBS=True

#from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
#svcMgr+=DBReplicaSvc(UseCOOLSQLite=False)


#useCOMCONDDB=True
#setDetDescr = 'ATLAS-GEO-04-00-00'
#setGlobalTag = 'COMCOND-HLTC-000-00'
#EvtMax=25
#setModifiers = ['noCSCReadout',
#                'enableHotIDMasking',
#                'disableCaloAllSamples',
#                'softTRTsettings',
#                'openThresholdRPCCabling',
                                  #special streaming setup
#                'enable7BitL1TTStreaming']

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doLVL2= False
TriggerFlags.doEF = False

#include ("RecExCommon/RecExCommon_flags.py")
include.block("RecExCond/RecExCommon_flags.py")
TriggerFlags.doHLTpersistency=True
TriggerFlags.writeBS=False 
TriggerFlags.abortOnConfigurationError=True


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-04-00-00')
globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-HLTC-000-00')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DataSource.set_Value_and_Lock('data')


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#TriggerFlags.MuonSlice.doMuonCalibrationStream = athenaCommonFlags.isOnline()
athenaCommonFlags.BSRDOInput=BSRDOInput

# should be done afterwards so that TriggerFlags are configured ok

# has been run at RDO->BS step (even EF ?)
# doTrigger=False
#from RecExConfig.RecFlags import recAlgs
#recAlgs.doTrigger=False


# main jobOption
#include ("RecExCommon/RecExCommon_topOptions.py")
include("TriggerRelease/Trigger_topOptions_standalone.py")


# the correct tag should be specified

#from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
#svcMgr+=DBReplicaSvc(UseCOOLSQLite=False)



ServiceMgr.IOVDbSvc.GlobalTag="COMCOND-HLTC-000-00"
#ServiceMgr.IOVDbSvc.GlobalTag="OFLCOND-CSC-00-01-00"

