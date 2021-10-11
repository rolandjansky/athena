###############################################################
#
# Skeleton top job options for RDO->BS
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

# disable ESD object making
rec.doESD.set_Value_and_Lock(False)

from AthenaConfiguration.AllConfigFlags import ConfigFlags

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('rdo_to_bs')
recoLog.info( '****************** STARTING RDO->BS MAKING *****************' )

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
    ConfigFlags.Input.Files = athenaCommonFlags.PoolRDOInput()

##Outputs
if hasattr(runArgs,"outputBSFile"):
    rec.doWriteBS.set_Value_and_Lock( True )   
    bsfilename=runArgs.outputBSFile
    athenaCommonFlags.BSRDOOutput.set_Value_and_Lock( runArgs.outputBSFile )


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
else: include( "RecExCommon/RecExCommon_topOptions.py" )

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
        
