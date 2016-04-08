from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

rec.readAOD=True

# different input files for ATN and RTT tests
if not acf.EvtMax.is_locked():
    acf.EvtMax=2000
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

if not ('FileList' in dir()):
    acf.FilesInput = ["/afs/cern.ch/user/o/okuprash/public/ATN_reference/AODexampleAtlasAnalysis207XVAL151124.root"]
else:
    acf.FilesInput = FileList

###############################

rec.doCBNT=False
rec.doTrigger=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False
rec.doESD=False

from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=True
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True

doTAG=False

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = EDMLibraries

#from TriggerMatchingTool.TriggerMatchingToolConf import MatchingTool
#ToolSvc += MatchingTool("MatchingTool")
#ToolSvc.MatchingTool.OutputLevel = VERBOSE

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += CfgMgr.AthenaTestHarness("Run1BStoxAODTrigger")

include("TriggerTest/TriggerTestCommon.py")
