###############################################################
#
# Skeleton top job options for AOD->xAOD 
#
###############################################################
# $Id: skeleton.AODtoAOD_tf.py 588353 2014-03-18 22:38:59Z graemes $
###############################################################

# Common job options disable most RecExCommon by default. Re-enable below on demand.
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

# Options to setup AOD to AOD (re)processing
rec.doAOD=True
rec.readAOD=True
rec.doWriteAOD=True
rec.doWritexAOD=True

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('AOD_to_AOD')
recoLog.info( '****************** STARTING AOD Reprocessing *****************' )

from AthenaConfiguration.AllConfigFlags import ConfigFlags

## Input
if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
    ConfigFlags.Input.Files = athenaCommonFlags.PoolAODInput()
else:
    raise RuntimeError("No AOD input file given")

## Outputs
if hasattr(runArgs,"outputAOD_RPRFile"):
    #for TAG->AOD->skimmedAOD
    rec.doWriteAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolAODOutput.set_Value_and_Lock( runArgs.outputAOD_RPRFile )
else:
    raise RuntimeError("No AOD_RPR output file given")


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

