from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from glob import glob

if ('FILE' in dir()):
    acf.FilesInput=[FILE]
if ('inputDir' in dir()):
    inputFiles = glob(inputDir+'*')
    acf.FilesInput=inputFiles
if not acf.EvtMax.is_locked():
    acf.EvtMax=-1
if ('NOV' in dir()):
     acf.EvtMax=NOV

from RecExConfig.RecFlags import rec

rec.doCBNT=False
rec.readESD=False
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False
rec.doDPD=False 
rec.doWriteTAG=False 

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList.set_Value_and_Lock( [ "ds" ] )

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

