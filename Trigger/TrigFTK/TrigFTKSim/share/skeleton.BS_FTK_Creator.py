# steering file for BS->BS step
# see myTopOptions.py for more info

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from PerfMonComps.PerfMonFlags import jobproperties as pmjp;
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKBSCreator')
#ftkLog.propagate = False
ftkLog.info( '********** STARTING FTKStandaloneMerge **********' )

athenaCommonFlags.BSRDOInput= runArgs.inputBSFile
if hasattr(runArgs, "outputBS_FTKFile") :
   athenaCommonFlags.BSRDOOutput=runArgs.outputBS_FTKFile

if hasattr(runArgs,"maxEvents"):
    ftkLog.info("Setting number of events: %d" % runArgs.maxEvents)
    athenaCommonFlags.EvtMax = runArgs.maxEvents
else:
    ftkLog.info("Running on all the events")
    athenaCommonFlags.EvtMax = -1

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

if hasattr(runArgs, "skipEvents"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.skipEvents)
elif hasattr(runArgs, "firstEvent"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.firstEvent)

from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
globalflags.InputFormat="bytestream"
if hasattr(runArgs,"geometryVersion"):
   globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)

if hasattr(runArgs,"conditionsTag"):
   globalflags.ConditionsTag.set_Value_and_Lock(runArgs.conditionsTag)
   
inputNTUP_FTKFile = runArgs.inputNTUP_FTKFile

from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_CreatorAlgo
FTK_RDO_CreatorAlgo = FTK_RDO_CreatorAlgo()
#FTK_RDO_CreatorAlgo.OutputLevel = VERBOSE
FTK_RDO_CreatorAlgo.mergeTrackBName = "FTKMergedTracksStream"
FTK_RDO_CreatorAlgo.mergedTrackPaths = inputNTUP_FTKFile
# By definition the direction of IBL locX is:
#      RAW      : reversed
#      NTUP_FTK : reversed
#      RDO      : normal
#      AOD      : normal
#  This FTK_RDO_CreatorAlgo is getting FTK tracks from NTUP_FTK and writing RAW
#  So we need to preserve IBL locX unchanged
#  (non-reversed RDO are created in Storegate as part of the process, but they are not written).
#  The reversal comes in the Reco job in FTK_DataProviderSvc 
FTK_RDO_CreatorAlgo.ReverseIBLlocX=False 

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
topSeq+=FTK_RDO_CreatorAlgo

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteRDO.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doWriteBS.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(False)

###
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrigger.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
###

rec.AutoConfiguration.set_Value_and_Lock(['everything'])

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArHVCorr.set_Value_and_Lock(False)

from LArROD.LArRODFlags import jobproperties
jobproperties.LArRODFlags.doLArFebErrorSummary.set_Value_and_Lock(False)

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

StreamBSFileOutput.ItemList = ["FTK_RawTrackContainer#*"]

# Merge with original bytestream
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMergeOutputSvc
svcMgr += ByteStreamMergeOutputSvc(ByteStreamOutputSvc='ByteStreamEventStorageOutputSvc',
          ByteStreamInputSvc='ByteStreamInputSvc',
          overWriteHeader=True)
svcMgr.ByteStreamCnvSvc.ByteStreamOutputSvcList=['ByteStreamMergeOutputSvc']
StreamBSFileOutput.OutputFile = "ByteStreamMergeOutputSvc"

###from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

pmjp.PerfMonFlags.OutputFile = 'ntuple_BSFTKCreator.pmon.gz'

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    ftkLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        ftkLog.info(cmd)
        exec(cmd)
