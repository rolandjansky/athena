# JO for quick testing Bphys monitoring.
# When running on AOD produced by RTT TriggerTest, 
# JpsiFinder histograms will be empty as there are no
# offline muon containers.

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not ('fileList' in dir()):
  fileList=['/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_0/21.1/build/x86_64-slc6-gcc62-opt/AthenaP1/TriggerTest/Bphysics_toAOD/AOD.pool.root']
  #fileList=['/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_0/21.1/build/x86_64-slc6-gcc62-opt/AthenaP1/TriggerTest/BphysExo_toAOD/AOD.pool.root']

# allow this to work in RTT
if not fileList == []:
    acf.FilesInput=fileList


if not 'RootNtupleOutput' in dir():
    rec.RootNtupleOutput="ntuple_fromAOD.root"

rec.doCBNT=False
rec.readESD=True
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

include("TrigBphysMonitoring/RunJpsiFinder.py")

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

from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
HLTMonManager.AthenaMonTools += TrigBphysMonitoringTool()

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

