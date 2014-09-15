###############################################################
#
# Skeleton top job options for ESD->ESD (reprocessing)
#
#==============================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
rec.doESD=True

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('rdo_to_esd')
recoLog.info( '****************** STARTING RDO->ESD MAKING *****************' )

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )

## Outputs
if hasattr(runArgs,"outputESDFile"):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True ) 
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputESDFile )

if hasattr(runArgs,"outputCBNTFile"):
    rec.doCBNT.set_Value_and_Lock(True)
    rec.RootNtupleOutput.set_Value_and_Lock( runArgs.outputCBNTFile ) 

if hasattr(runArgs,"outputDQMonitorFile"):
    rec.doMonitoring.set_Value_and_Lock(True)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputDQMonitorFile )

if hasattr(runArgs,"outputNTUP_MUONCALIBFile"):
    from MuonRecExample.MuonRecFlags import muonRecFlags
    muonRecFlags.doCalib = True
    muonRecFlags.calibNtupleOutput.set_Value_and_Lock( runArgs.outputNTUP_MUONCALIBFile )

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        print "preInclude",fragment
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
        
