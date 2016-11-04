from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from glob import glob

if ('FILE' in dir()):
    acf.FilesInput=[FILE]
elif ('inputDir' in dir()):
    inputFiles = glob(inputDir+'*')
    acf.FilesInput=inputFiles
elif 'RTT' in dir():
    rttfile='root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/'+RTT+'/x86_64-slc6-gcc49-opt/offline/TrigEgammaValidation/RDOtoAOD_MC_transform_Zee_25ns_pileup/AOD.Zee.25ns.pileup.pool.root'
    acf.FilesInput=[rttfile]
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
rec.doInDet=False
#rec.doMonitoring=True
#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList.set_Value_and_Lock( [ "ds" ] )

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

try:
    include ("AthenaMonitoring/DataQualityInit_jobOptions.py")
except Exception:
    treatException("Could not load AthenaMonitoring/DataQualityInit_jobOptions.py")

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
if ('derivation' in dir()):
    HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool(derivation=True)
else:
    HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
#
if ('derivation' in dir()):
    ToolSvc.TrigDecisionTool.Navigation.ReadonlyHolders=True
