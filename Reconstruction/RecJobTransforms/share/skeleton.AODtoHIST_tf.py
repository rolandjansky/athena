###############################################################
#
# Skeleton top job options for AOD monitoring histograms
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('aod_to_hist')
recoLog.info( '****************** STARTING AOD->HIST MAKING *****************' )

from AthenaConfiguration.AllConfigFlags import ConfigFlags

from RecExConfig.RecFlags import rec
rec.readESD = False
rec.readAOD = True
rec.doDPD = False
rec.doHist = False
rec.doMonitoring = True

# Output log setting
OutputLevel = INFO

# Input and output for this substep
if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
    ConfigFlags.Input.Files = athenaCommonFlags.PoolAODInput()

if hasattr(runArgs,"outputHIST_AODFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputHIST_AODFile )


## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)

