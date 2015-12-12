# steering file for RDO->RDO step
# see myTopOptions.py for more info

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput=[
    #"/afs/cern.ch/user/b/baines/work/public/data/mc12_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s1682_s1691_r4711_tid01323022_00/RDO.01323022._000057.pool.root.2"
"/afs/cern.ch/user/b/baines/work/public/data/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r6630_tid05584674_00/RDO.05584674._000005.pool.root.1"
    ]

athenaCommonFlags.EvtMax=4

inputNTUP_FTKFile=[
    #"/afs/cern.ch/user/b/baines/work/public/data/mc12_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.NTUP_FTK.e2176_s1682_s1691_r4711_r4910_tid01342409_00/NTUP_FTK.01342409._000029.root.2"
    "/afs/cern.ch/user/b/baines/work/public/data/user.jahreda.mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.RDO_FTK.e3698_s2608_s2183_r6630.test.v2_EXT0.39080317/user.jahreda.6259805.EXT0._000005.NTUP_FTK.root"
    ]

from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_CreatorAlgo
FTK_RDO_CreatorAlgo = FTK_RDO_CreatorAlgo()
FTK_RDO_CreatorAlgo.OutputLevel = VERBOSE
print "Adding FTK_RDO_CreatorAlgo to topSeq"

# Map path
from PyJobTransforms.trfUtils import findFile
import os.path

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

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamRDO = AthenaPoolOutputStream( "StreamRDO" )
StreamRDO.ItemList+=["FTK_RawTrackContainer#*"] 

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.addStreamRDO("FTK_RawTrackContainer","FTK_RDO_Tracks")


