
#from AthenaCommon.AlgSequence import AlgSequence
#topSeq = AlgSequence()
#if hasattr(topSeq, "FTKMergerAlgo"):
#FTKMergerA = getattr(topSeq, "FTKMergerAlgo")
#FTKMergerA.OutputLevel = VERBOSE

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_CreatorAlgo
FTK_RDO_CreatorAlgo = FTK_RDO_CreatorAlgo()
FTK_RDO_CreatorAlgo.OutputLevel = VERBOSE
print "Adding FTK_RDO_CreatorAlgo to topSeq"
topSeq+=FTK_RDO_CreatorAlgo

#FTK_RDO_CreatorAlgo = Algorithm( "FTK_RDO_CreatorAlgo",  OutputLevel=VERBOSE)

# Map path
from PyJobTransforms.trfUtils import findFile
import os.path
print runArgs
if hasattr(runArgs, 'pmap_path'):
    if os.path.exists(runArgs.pmap_path) :
        FTK_RDO_CreatorAlgo.pmap_path = runArgs.pmap_path
    else :
        stdpath = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/' + runArgs.pmap_path)
        if not os.path.exists(stdpath) :
            raise RuntimeError, 'PMAP %s file not find' % runArgs.pmap_path
        FTK_RDO_CreatorAlgo.pmap_path = stdpath
else:
    raise RuntimeError, "PMAP wasn't not set. Abort"

#FTK_RDO_CreatorAlgo.useStandalone   = True
#FTK_RDO_CreatorAlgo.SingleProces    = True
#FTK_RDO_CreatorAlgo.doMerging       = False

FTK_RDO_CreatorAlgo.NBanks        = 1
FTK_RDO_CreatorAlgo.NSubRegions     = 1
###Set Hit Warrior parameter
FTK_RDO_CreatorAlgo.HitWarrior      = 2  # Default 2
FTK_RDO_CreatorAlgo.KeepRejected    = 0  # >0 enable HW debug feature
if hasattr(runArgs,"FTKOldFormat") :
   FTK_RDO_CreatorAlgo.mergeTrackBName = "FTKBankMerged"
else :
   FTK_RDO_CreatorAlgo.mergeTrackBName = "FTKMergedTracksStream"

# Set the input FTK tracks file here
if hasattr(runArgs, 'inputNTUP_FTKFile'):
    FTK_RDO_CreatorAlgo.mergedTrackPaths = runArgs.inputNTUP_FTKFile
else:
    raise RuntimeError, 'Failed to find mandatory inputNTUP_FTKFile runtime argument for transform '


from RecExConfig.RecFlags import rec
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doWriteRDO.set_Value_and_Lock(True)


theApp.Dlls += [ "TrigFTKSim" ]


#from AthenaCommon.AlgSequence import AlgSequence
#topSeq = AlgSequence()
#if hasattr(topSeq, "FTKMergerAlgo"):
#FTKMergerA = getattr(topSeq, "FTKMergerAlgo")
#FTKMergerA.OutputLevel = VERBOSE

#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

#StreamRDO=MSMgr.NewPoolStream("StreamRDO","FTKRDO.pool.root")
#StreamRDO.AddMetaDataItem(["IOVMetaDataContainer#*"])
#StreamRDO.AddItem( ["FTK_RawTrackContainer#*"] ) 

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamRDO = AthenaPoolOutputStream( "StreamRDO" )
StreamRDO.ItemList+=["FTK_RawTrackContainer#*"] 

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.addStreamRDO("FTK_RawTrackContainer","FTK_RDO_Tracks")
