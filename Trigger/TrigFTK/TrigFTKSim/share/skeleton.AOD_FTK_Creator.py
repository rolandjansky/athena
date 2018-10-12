# steering file for RDO->AOD step

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKAODCreator')
ftkLog.info( '********** STARTING FTKStandaloneMerge AOD_FTK **********' )



## Pre-exec
if hasattr(runArgs,"preExec"):
    ftkLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        ftkLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if hasattr(runArgs,"inputRDO_FTKFile"):
    athenaCommonFlags.FilesInput = runArgs.inputRDO_FTKFile

elif hasattr(runArgs,"inputBS_FTKFile"):
    athenaCommonFlags.FilesInput = runArgs.inputBS_FTKFile

if hasattr(runArgs,"maxEvents"):
    ftkLog.info("Setting number of events: %d" % runArgs.maxEvents)
    athenaCommonFlags.EvtMax = runArgs.maxEvents
else:
    ftkLog.info("Running on all the events")
    athenaCommonFlags.EvtMax = -1


OutAODName = "AOD.pool.root"
if hasattr(runArgs, "outputAODFile"):
  OutAODName = runArgs.outputAODFile
athenaCommonFlags.PoolAODOutput.set_Value_and_Lock(OutAODName)

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOn()
DetFlags.FTK_setOn()


from RecExConfig.RecFlags import rec
rec.doAOD.set_Value_and_Lock(True)
rec.doESD = True
rec.doFTK.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(True)
rec.doWriteESD = False
rec.doWriteTAG = False
rec.doTagRawSummary = False


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

if hasattr(runArgs, "skipEvents"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.skipEvents)
elif hasattr(runArgs, "firstEvent"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.firstEvent)

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    for cmd in runArgs.postExec:
        exec(cmd)
