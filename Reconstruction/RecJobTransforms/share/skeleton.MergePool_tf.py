###############################################################
#
# Skeleton top job options for ESD/AOD/DPD merging
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
rec.doAOD=False
rec.doESD=False

#from AthenaCommon.Logging import logging
import logging
recoLog = logging.getLogger('merge_pool')
recoLog.info( '****************** STARTING POOL FILE MERGING *****************' )

## Input
# Deal with generic case first of all
if hasattr(runArgs, "inputPOOL_MRG_INPUTFile"):
    if runArgs.inputPOOL_MRG_INPUTFileType is "AOD":
        runArgs.inputAODFile = runArgs.inputPOOL_MRG_INPUTFile
        runArgs.outputAOD_MRGFile = runArgs.outputPOOL_MRG_OUTPUTFile
    elif runArgs.inputPOOL_MRG_INPUTFileType is "ESD":
        runArgs.inputESDFile = runArgs.inputPOOL_MRG_INPUTFile
        runArgs.outputESD_MRGFile = runArgs.outputPOOL_MRG_OUTPUTFile
    else:
        recoLog.warning("No recognised data type for merge input and output ({0}). Merge setup may not be correct!".format(runArgs.inputPOOL_MRG_INPUTFileType))

# DAOD comes in many flavours, so automate transforming this into a "standard" AOD argument
DAOD_Input_Key = [ k for k in dir(runArgs) if k.startswith("inputDAOD") and k.endswith("File") ]
if len(DAOD_Input_Key) is 1:
    runArgs.inputAODFile = getattr(runArgs, DAOD_Input_Key[0])

DAOD_Output_Key = [ k for k in dir(runArgs) if k.startswith("outputDAOD") and k.endswith("_MRGFile") ]
if len(DAOD_Output_Key) is 1:
    runArgs.outputAOD_MRGFile = getattr(runArgs, DAOD_Output_Key[0])
        
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputAODFile"):
    from InDetRecExample.InDetKeys import InDetKeys;
    InDetFlags.keepAdditionalHitsOnTrackParticle.set_Value_and_Lock(True)
    rec.readAOD.set_Value_and_Lock( True )
    rec.doWriteAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )

if hasattr(runArgs,"inputESDFile"):
    rec.readESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )

## Output
if hasattr(runArgs,"outputAOD_MRGFile"): athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAOD_MRGFile )
if hasattr(runArgs,"outputESD_MRGFile"): athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESD_MRGFile )

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
        # Don't consider this step in the backwards navigation:
        StreamAOD.ExtendProvenanceRecord = False
        # Ignore the ItemList created by RecExCommon, and take everything
        # directly from the input file:
        StreamAOD.ItemList = []
        StreamAOD.TakeItemsFromInput = True
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

# Perfmon
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True
pmon_properties.PerfMonFlags.OutputFile = "ntuple_POOLMerge"

# For (all) DAOD merging make sure that the BPhysics metadata is propagated
# correctly as well:
if len( DAOD_Input_Key ) is 1:
    include( "DerivationFrameworkBPhys/SaveExtraMetadataInMerge_jobOFragment.py" )
    pass

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

