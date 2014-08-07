###############################################################
# Simple job options to:
#   read BS, run HLT, and write out BS identical but for new HLT fragments
# See ~aagaard/public/Reprocessing/testReproWrite.py
# 
# Simon George, November 2008
# Clemencia Mora, August 2009
#==============================================================

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('RawToRaw_WithHLT')
recoLog.info( '****************** STARTING BS->HLT->BS *****************' )

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from TriggerJobOpts.TriggerFlags import TriggerFlags

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

## max events
theApp.EvtMax=-1
if hasattr(runArgs,"maxEvents"): theApp.EvtMax = runArgs.maxEvents
EvtMax=theApp.EvtMax

## Input
if hasattr(runArgs,"inputBSFile"):
    BSRDOInput = runArgs.inputBSFile
    athenaCommonFlags.BSRDOInput = BSRDOInput
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
else:
    raise RuntimeError("runArgs should include inputBSFile - without input this transform makes no sense.")

## Output
if hasattr(runArgs,"outputBSFile"):
    BSRDOOutput = runArgs.outputBSFile
    athenaCommonFlags.BSRDOOutput = BSRDOOutput
else:
    raise RuntimeError("runArgs should include outputBSFile - without output this transform makes no sense.")


## Log level
if hasattr(runArgs,"loglevel"):
    from AthenaCommon.Constants import __all__ as validMessageLevels
    if runArgs.loglevel in validMessageLevels:
        myloglevel=validMessageLevels.index(runArgs.loglevel)
        HLTOutputLevel=myloglevel
        recoLog.info("set HLTOutputLevel to " + runArgs.loglevel + " " + str(myloglevel))

## Field and Geo Configuration
if hasattr(runArgs,"geometryVersion"):
    setDetDescr=runArgs.geometryVersion
    
elif not hasattr(runArgs,"autoConfiguration"):
    #use the setField flag from runHLT_standalone.py
    from PATJobTransforms.GeoConfig import SetFieldForHLT
    setField=SetFieldForHLT()
    recoLog.info("setField="+setField)


## Trigger Stream
if hasattr(runArgs,"trigStream"):
    from RecExConfig.RecFlags import rec
    rec.triggerStream=runArgs.trigStream

## Conditions Tag
if hasattr(runArgs,"conditionsTag"):
    setGlobalTag=runArgs.conditionsTag

## AutoConfigure stuff
if hasattr(runArgs,"autoConfiguration"):
    from RecExConfig.RecFlags import rec
    for key in runArgs.autoConfiguration:
        rec.AutoConfiguration.append(key)
        
    if 'FieldAndGeo' not in runArgs.autoConfiguration:
        from PATJobTransforms.GeoConfig import SetFieldForHLT
        setField=SetFieldForHLT()
        recoLog.info("setField="+setField)
        
    if 'ConditionsTag' in runArgs.autoConfiguration and not ( hasattr(runArgs,"trigStream") or 'TriggerStream' in runArgs.autoConfiguration):
        recoLog.warning("Cannot autoConfigure conditionsTag if trigStream is not known or autoConfigured as well")
        rec.AutoConfiguration.remove('ConditionsTag')
        
    from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
    ConfigureFromListOfKeys(rec.AutoConfiguration())


# turn on mufast monitoring
if hasattr(runArgs,"outputNTUP_MUFASTFile"):
    doMufastNtuple=True

## Trigger Menu configuration (using setMenu flag or testBlah=True)
if hasattr(runArgs,"triggerConfig"):
    #setup menu for runHLT_standalone, can be SomeThing:MenuName or just  MenuName format, careful with what this means for RAWtoESD and ESDtoAOD 
    setupForMC=False
    config=runArgs.triggerConfig.split(':')
    if len(config)>1:
        setMenu=config[1]
    else:
        setMenu=config[0]
    recoLog.info("setMenu="+setMenu)
else:
    # set Menu by default to Cosmic2009_v1
    recoLog.info("Set testCosmic2009V1=True")
    testCosmic2009V1=True

## no longer default in flags
TriggerFlags.outputHLTconfigFile="outputHLTconfig.xml"

##reduce output from TriggerMenuPython
from AthenaCommon.Include import excludeTracePattern,includeTracePattern
excludeTracePattern.append("*/TriggerMenuPython/*.py")
includeTracePattern.append("*/TriggerMenuPython/GenerateMenu.py")

## main job options 
include("TriggerRelease/runHLT_standalone.py")

# rename special root output files from trigger
if hasattr(runArgs,"outputNTUP_MUFASTFile"):
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='%s' OPT='NEW'" % runArgs.outputNTUP_MUFASTFile ]

if hasattr(runArgs,"outputHIST_TRIGEXPERTFile"):
    output=svcMgr.THistSvc.Output
    for idx in range(len(output)):
        if output[idx].count('EXPERT'):
            output[idx]="EXPERT DATAFILE='%s' TYPE='ROOT' OPT='RECREATE'" % runArgs.outputHIST_TRIGEXPERTFile
        svcMgr.THistSvc.Output=output

#add HLT results to output bytestream
from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")

# BS content definition
StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]

# set up special bs output service that just merged changed fragments with unchanged copy of original input bs.
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMergeOutputSvc
mergeSvc=ByteStreamMergeOutputSvc(ByteStreamOutputSvc='ByteStreamEventStorageOutputSvc', ByteStreamInputSvc='ByteStreamInputSvc', overWriteHeader=True)
svcMgr+=mergeSvc

StreamBSFileOutput.OutputFile = "ByteStreamMergeOutputSvc"
svcMgr.ByteStreamCnvSvc.ByteStreamOutputSvcList=['ByteStreamMergeOutputSvc']


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

