# Setup for just reading the trigger information

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
#
from RecExConfig.RecFlags import rec
rec.doTrigger=True
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=True
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True

rec.doCBNT=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False 
rec.doESD=False 
doTAG=False
#rec.doTruth=True
rec.readAOD=True

# Parse the command line options.  You can set
# the test to look for by setting TestType
if not acf.EvtMax.is_locked():
    acf.EvtMax=10

if not 'inputFile' in dir():
    print "testAthenaTrigAOD_TrigSlimValidation.py: FATAL you must set an input file via '-c inputFile=...'"
    import sys
    sys.exit(-1)
acf.FilesInput = [inputFile]

# Run the main job options
#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------


#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#cfg = TriggerConfigGetter()

# Setup the validation algorithm, TrigSlimValAlg
from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool
slimmer = HLT__TrigNavigationSlimmingTool()
ToolSvc += slimmer

from TrigValAlgs.TrigValAlgsConfig import TrigSlimValAlg
TrigSlimVal = TrigSlimValAlg()
TrigSlimVal.OutputLevel = INFO

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigSlimVal


include("TriggerTest/TriggerTestCommon.py")


