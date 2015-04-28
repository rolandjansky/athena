# steering file for RDO->RDO step
# see myTopOptions.py for more info

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput=["/afs/cern.ch/user/b/baines/work/public/data/mc12_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s1682_s1691_r4711_tid01323022_00/RDO.01323022._000057.pool.root.2"]

athenaCommonFlags.EvtMax=4

inputNTUP_FTKFile=["/afs/cern.ch/user/b/baines/work/public/data/mc12_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.NTUP_FTK.e2176_s1682_s1691_r4711_r4910_tid01342409_00/NTUP_FTK.01342409._000029.root.2"]

pmapFile="raw_12Libl.pmap"

from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_CreatorAlgo
FTK_RDO_CreatorAlgo = FTK_RDO_CreatorAlgo()
#FTK_RDO_CreatorAlgo.OutputLevel = VERBOSE
print "Adding FTK_RDO_CreatorAlgo to topSeq"

# Map path
from PyJobTransforms.trfUtils import findFile
import os.path

if os.path.exists(pmapFile) :
    FTK_RDO_CreatorAlgo.pmap_path = pmapFile
else :
    stdpath = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/' + pmapFile)
    if not os.path.exists(stdpath) :
        raise RuntimeError, 'PMAP %s file not find' % pmapFile
    FTK_RDO_CreatorAlgo.pmap_path = stdpath

FTK_RDO_CreatorAlgo.useStandalone   = True
FTK_RDO_CreatorAlgo.SingleProces    = True
FTK_RDO_CreatorAlgo.doMerging       = False

FTK_RDO_CreatorAlgo.NBanks        = 1
FTK_RDO_CreatorAlgo.NSubRegions     = 1
###Set Hit Warrior parameter
FTK_RDO_CreatorAlgo.HitWarrior      = 2  # Default 2
FTK_RDO_CreatorAlgo.KeepRejected    = 0  # >0 enable HW debug feature
FTK_RDO_CreatorAlgo.mergeTrackBName = "FTKMergedTracksStream"

FTK_RDO_CreatorAlgo.mergedTrackPaths = inputNTUP_FTKFile

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteRDO.set_Value_and_Lock(True)
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

#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

#StreamRDO=MSMgr.NewPoolStream("StreamRDO","FTKRDO.pool.root")
#StreamRDO.AddMetaDataItem(["IOVMetaDataContainer#*"])
#StreamRDO.AddItem( ["FTK_RawTrackContainer#*"] ) 

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamRDO = AthenaPoolOutputStream( "StreamRDO" )
StreamRDO.ItemList+=["FTK_RawTrackContainer#*"] 

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.addStreamRDO("FTK_RawTrackContainer","FTK_RDO_Tracks")


