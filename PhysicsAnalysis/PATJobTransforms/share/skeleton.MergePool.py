###############################################################
#
# Skeleton top job options for ESD/AOD/DPD merging
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
rec.doAOD=False
rec.doESD=False

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('merge_pool')
recoLog.info( '****************** STARTING POOL FILE MERGING *****************' )

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputAODFile"):
    rec.readAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )

if hasattr(runArgs,"inputESDFile"):
    rec.readESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )

## Output
if hasattr(runArgs,"outputAODFile"): athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAODFile )
if hasattr(runArgs,"outputESDFile"): athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )

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

#Lock doAOD/ESD before starting RecExCommon, so they are not overwritten by the auto-configuration
rec.doAOD.lock()
rec.doESD.lock()

include( "RecExCommon/RecExCommon_topOptions.py" )

if rec.doWriteAOD:
    if rec.readAOD(): 
        StreamAOD.ExtendProvenanceRecord = False
    else:
        print "StreamAOD was not defined, cannot set ExtendProvenanceRecord = False. Check your flags."

if rec.doWriteESD:
    if rec.readESD(): 
        StreamESD.ExtendProvenanceRecord = False
    else:
        print "StreamESD was not defined, cannot set ExtendProvenanceRecord = False. Check your flags."

# Fast merge options
if hasattr(runArgs,"fastPoolMerge") and runArgs.fastPoolMerge == True:
     recoLog.info("Using CopyEventStreamInfo")
     from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import CopyEventStreamInfo
     if rec.doWriteAOD: stream = StreamAOD
     if rec.doWriteESD: stream = StreamESD
     stream.HelperTools = []
     streamInfoToolKey = stream.name()
     streamInfoTool = CopyEventStreamInfo( streamInfoToolKey+"_CopyEventStreamInfo" )
     streamInfoTool.Key = streamInfoToolKey
     ToolSvc += streamInfoTool
     ServiceMgr.MetaDataSvc.MetaDataTools += [ streamInfoTool ]


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

