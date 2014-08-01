####################################################################
#
# Skeleton top job options for RDO->RDOprime (RDO with trigger done)
#
#===================================================================

#Common job options disable most RecExCommon by default. Re-enable below on demand.
include("PATJobTransforms/CommonSkeletonJobOptions.py")
# disable ESD object making, but still enable trigger
rec.doESD.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(True)
rec.doInDet.set_Value_and_Lock(True)
rec.doMuon.set_Value_and_Lock(True)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
  
rec.doTrigger.set_Value_and_Lock(True)
rec.doRDOTrigger.set_Value_and_Lock(True)
recAlgs.doTrigger.set_Value_and_Lock(True)

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('rdo_to_rdotrigger')
recoLog.info( '****************** STARTING RDO->RDOTrigger MAKING *****************' )

## Input
if hasattr(runArgs,"inputFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
if hasattr(runArgs,"inputRDOFile"):
    rec.readRDO.set_Value_and_Lock( True )
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )

##Outputs
outputs = set(dir(runArgs)) & set(('tmpRDO', 'outputRDOFile'))
if len(outputs) > 1:
    recoLog.fatal("Both outputRDOFile and tmpRDO specified - this configuration should not be used!")
    raise SystemError
elif len(outputs) == 1:
    rec.doWriteRDO.set_Value_and_Lock( True )   
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( getattr(runArgs, outputs.pop()) )
else:
    recoLog.warning("No RDO output file specified")
    

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

oldRDOList=StreamRDO.ItemList
from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
theCKS2=CfgKeyStore("KeyStore")
CILMergeRDO=CfgItemList("RDOprime",allowWildCard=True)
CILMergeRDO.add(oldRDOList)
CILMergeRDO.add(theCKS2.streamRDO())

StreamRDO.ItemList = CILMergeRDO()


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
        
