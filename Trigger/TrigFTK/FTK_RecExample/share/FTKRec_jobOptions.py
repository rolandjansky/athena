include.block('FTK_RecExample/FTKRec_jobOptions.py')


from RecExConfig.RecFlags import rec
from AthenaCommon.GlobalFlags import GlobalFlags

if rec.doFTK() and globalflags.InputFormat() == 'bytestream':
    ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc")
    ByteStreamAddressProviderSvc.TypeNames += [ "FTK_RawTrackContainer/FTK_RDO_Tracks"]
    
if rec.doFTK():
    
    from TrigFTK_RecExample.TrigFTK_DataProviderSvc_Config import TrigFTK_DataProviderSvc
    theFTK_DataProviderSvc = TrigFTK_DataProviderSvc("TrigFTK_DataProviderSvc")
    ServiceMgr += theFTK_DataProviderSvc
    
    from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_ReaderAlgo
    
    FTK_RDO_Reader = FTK_RDO_ReaderAlgo( "FTK_RDO_ReaderAlgo")
    FTK_RDO_Reader.FTK_DataProvider=theFTK_DataProviderSvc
    FTK_RDO_Reader.RDO_CollectionName="FTK_RDO_Tracks"
    FTK_RDO_Reader.fillHists=False
    FTK_RDO_Reader.fillTree=False
    FTK_RDO_Reader.GetTracks=True
    FTK_RDO_Reader.GetTrackParticles=True
    FTK_RDO_Reader.GetVertex=True
    FTK_RDO_Reader.GetRefitTracks=True
    FTK_RDO_Reader.GetRefitTrackParticles=True
    FTK_RDO_Reader.GetRefitVertex=True
    FTK_RDO_Reader.GetTruthVertex=False
    
    from AthenaCommon.AlgSequence import AlgSequence
    
    alg = AlgSequence() 
    alg += FTK_RDO_Reader 
    
    
    if rec.doTruth():
        include ('FTK_RecExample/ConfiguredFTK_TrackTruth.py')
        FTK_TracksTruth = ConfiguredFTK_TrackTruth(Tracks="FTK_TrackCollection",
                                                   TracksTruth = "FTK_Tracks_TruthCollection",
                                                   DetailedTruth = "FTK_Tracks_DetailedTruthCollection")
        FTK_RefitTracksTruth = ConfiguredFTK_TrackTruth(Tracks="FTK_TrackCollectionRefit", 
                                                     TracksTruth = "FTK_RefitTracks_TruthCollection",
                                                     DetailedTruth = "FTK_RefitTracks_DetailedTruthCollection")
