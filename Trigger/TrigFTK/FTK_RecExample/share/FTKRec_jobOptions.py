include.block('FTK_RecExample/FTKRec_jobOptions.py')

from RecExConfig.RecFlags import rec

if rec.doFTK():
  from FTK_RecExample.FTK_DataProviderSvc_Config import ConfiguredFTK_DataProviderSvc
  FTK_DataProviderSvc = ConfiguredFTK_DataProviderSvc("ConfiguredFTK_DataProviderSvc")
  ServiceMgr += FTK_DataProviderSvc
  ServiceMgr.ConfiguredFTK_DataProviderSvc.TrainingBeamspotX= -0.0497705
  ServiceMgr.ConfiguredFTK_DataProviderSvc.TrainingBeamspotY=1.06299

  from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_ReaderAlgo

  FTK_RDO_Reader = FTK_RDO_ReaderAlgo( "FTK_RDO_ReaderAlgo")
  FTK_RDO_Reader.FTK_DataProvider=FTK_DataProviderSvc
  FTK_RDO_Reader.RDO_CollectionName="FTK_RDO_Tracks"
  FTK_RDO_Reader.fillHists=False
  
  FTK_RDO_Reader.GetTracks=True
  FTK_RDO_Reader.GetTrackParticles=True
  FTK_RDO_Reader.GetRawVxVertex=True
  FTK_RDO_Reader.GetVxVertex=True
  FTK_RDO_Reader.GetVertex=True
  FTK_RDO_Reader.GetRefitTracks=True
  FTK_RDO_Reader.GetRefitTrackParticles=True
  FTK_RDO_Reader.GetRefitVxVertex=True
  FTK_RDO_Reader.GetRefitVertex=True

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



