from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput =["user.gvolpi.5174418.EXT2._000001.RDO_FTK.pool.root"]  # new Ztautau sample
if not 'rec.doWriteESD' in dir():
    rec.doWriteESD=True

from RecExConfig.RecFlags import rec
doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_ReaderAlgo

FTK_RDO_Reader = FTK_RDO_ReaderAlgo( "FTK_RDO_ReaderAlgo" , OutputLevel=VERBOSE)

FTK_RDO_Reader.RDO_CollectionName="FTK_RDO_Tracks"
FTK_RDO_Reader.fillHists=True

alg += FTK_RDO_Reader  

from AthenaCommon.AppMgr import ServiceMgr
from FTK_DataProviderSvc.FTK_DataProviderSvc_Config import ConfiguredFTK_DataProviderSvc

FTK_DataProviderSvc = ConfiguredFTK_DataProviderSvc("ConfiguredFTK_DataProviderSvc",TrackFitter="GX2", OutputLevel=VERBOSE);

ServiceMgr += FTK_DataProviderSvc

