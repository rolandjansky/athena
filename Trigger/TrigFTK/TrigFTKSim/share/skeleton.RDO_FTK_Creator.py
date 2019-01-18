# steering file for RDO->RDO step
# see myTopOptions.py for more info

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from PerfMonComps.PerfMonFlags import jobproperties as pmjp;
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKRDOCreator')
#ftkLog.propagate = False
ftkLog.info( '********** STARTING FTKStandaloneMerge RDO_FTK **********' )

from AthenaCommon.DetFlags import DetFlags
DetFlags.makeRIO.pixel_setOn()
DetFlags.makeRIO.SCT_setOn()
DetFlags.detdescr.all_setOn()
DetFlags.geometry.all_setOn()

athenaCommonFlags.FilesInput = runArgs.inputRDOFile

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

inputNTUP_FTKFile = runArgs.inputNTUP_FTKFile


from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_CreatorAlgo
FTK_RDO_CreatorAlgo = FTK_RDO_CreatorAlgo()
#FTK_RDO_CreatorAlgo.OutputLevel = VERBOSE
print "Adding FTK_RDO_CreatorAlgo to topSeq"

# Map path
from PyJobTransforms.trfUtils import findFile
import os.path

FTK_RDO_CreatorAlgo.mergeTrackBName = "FTKMergedTracksStream"
FTK_RDO_CreatorAlgo.mergedTrackPaths = inputNTUP_FTKFile
# By definition the direction of IBL locX is:
#      RAW      : reversed
#      NTUP_FTK : reversed
#      RDO      : normal
#      AOD      : normal
#  This FTK_RDO_CreatorAlgo is getting the FTK tracks from NTUP_FTK and writing RDO so
#  the direction of IBL locX needs to be reversed 
FTK_RDO_CreatorAlgo.ReverseIBLlocX=True 

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doWriteRDO.set_Value_and_Lock(True)


from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

topSeq+=FTK_RDO_CreatorAlgo

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

theApp.Dlls += [ "TrigFTKSim" ]

#from AthenaCommon.AlgSequence import AlgSequence
#topSeq = AlgSequence()
#if hasattr(topSeq, "FTKMergerAlgo"):
#FTKMergerA = getattr(topSeq, "FTKMergerAlgo")
#FTKMergerA.OutputLevel = VERBOSE

OutName = "RDO.pool.root"
if hasattr(runArgs, "outputRDO_FTKFile") :
    OutName = runArgs.outputRDO_FTKFile

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

#StreamRDO=MSMgr.NewPoolStream("StreamRDO",OutName)
StreamRDO=MSMgr.GetStream("StreamRDO")
StreamRDO.SetOutputFileName(OutName)
StreamRDO.AddMetaDataItem(["IOVMetaDataContainer#*"])
StreamRDO.AddItem( ["FTK_RawTrackContainer#*"] )

#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

#StreamRDO = AthenaPoolOutputStream( "StreamRDO", OutName, True)
#StreamRDO.ItemList+=["FTK_RawTrackContainer#*"]

#from RecExConfig.ObjKeyStore import objKeyStore
#objKeyStore.addStreamRDO("FTK_RawTrackContainer","FTK_RDO_Tracks")

pmjp.PerfMonFlags.OutputFile = 'ntuple_RDOFTKCreator.pmon.gz'

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    for cmd in runArgs.postExec:
        exec(cmd)
