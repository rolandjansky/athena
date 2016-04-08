###############################################################
#
# Skeleton top job options for AOD->TAG
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
rec.doWriteTAG=True

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('aod_to_tag')
recoLog.info( '****************** STARTING AOD->TAG MAKING *****************' )

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )

#set all output file names
if hasattr(runArgs,"outputTAGFile"):
    athenaCommonFlags.PoolTAGOutput.set_Value_and_Lock( runArgs.outputTAGFile )


if hasattr(runArgs,"outputNTUP_FASTMONFile"):
    rec.doFastPhysMonitoring.set_Value_and_Lock(True)
    rec.RootFastPhysMonOutput.set_Value_and_Lock(runArgs.outputNTUP_FASTMONFile)

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

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include("RecExCommon/RecExCommon_topOptions.py")

if rec.Commissioning:
    Service("GeoModelSvc").IgnoreTagDifference = True
    Service("GeoModelSvc").OutputLevel=6

if rec.doWriteAOD:
   if rec.readAOD():
       try:
           StreamAOD.ExtendProvenanceRecord = False
       except:
           print "StreamAOD was not defined, cannot set ExtendProvenanceRecord = False. Check your flags."

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

