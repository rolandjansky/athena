from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from glob import glob

if not ('inputDir' in dir()):
     inputDir='/tmp/avishek/'

if ('FILE' in dir()):
    acf.FilesInput=[FILE]
else:    
    inputFiles = glob(inputDir+'*')
    acf.FilesInput=inputFiles

from RecExConfig.RecFlags import rec

if not 'RootNtupleOutput' in dir():
    rec.RootNtupleOutput="ntuple_fromAOD.root"

rec.doCBNT=False
rec.readESD=False
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False
rec.doDPD=False 
rec.doWriteTAG=False 

#doTrigger=True
#SkipEvents=840

if not acf.EvtMax.is_locked():
    acf.EvtMax=-1
if not ('OutputLevel' in dir()):
    #OutputLevel=DEBUG
    rec.OutputLevel=INFO

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

################ Mon Tools #################

#Global HLTMonTool

from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
HLTMon = HLTMonTool(name               = 'HLTMon',
                   histoPathBase      = "/Trigger/HLT");

#HLTMon.TrigDecisionTool = monTrigDecTool

ToolSvc += HLTMon;

HLTMonManager.AthenaMonTools += [ "HLTMonTool/HLTMon" ];



from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

