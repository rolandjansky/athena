include.block('FTK_RecExample/FTKRec_jobOptions.py')

print 'FTKRec_jobOptions.py'


from FTK_DataProviderSvc.FTK_DataProviderSvc_Config import ConfiguredFTK_DataProviderSvc
FTK_DataProviderSvc = ConfiguredFTK_DataProviderSvc("ConfiguredFTK_DataProviderSvc")
ServiceMgr += FTK_DataProviderSvc
ServiceMgr.ConfiguredFTK_DataProviderSvc.TrainingBeamspotX= -0.0497705
ServiceMgr.ConfiguredFTK_DataProviderSvc.TrainingBeamspotY=1.06299
#########ServiceMgr.ConfiguredFTK_DataProviderSvc.OutputLevel=DEBUG


from RecExConfig.RecFlags import rec

  

if rec.doWriteAOD() or rec.doWriteESD():
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
  print ' include FTK_RecExample/ConfiguredFTK_TrackTruth.py'
  include ('FTK_RecExample/ConfiguredFTK_TrackTruth.py')
  FTK_TracksTruth = ConfiguredFTK_TrackTruth(Tracks="FTK_TrackCollection",
                                              TracksTruth = "FTK_Tracks_TruthCollection",
                                              DetailedTruth = "FTK_Tracks_DetailedTruthCollection")
  FTK_RefitTracksTruth = ConfiguredFTK_TrackTruth(Tracks="FTK_TrackCollectionRefit",
                                              TracksTruth = "FTK_RefitTracks_TruthCollection",
                                              DetailedTruth = "FTK_RefitTracks_DetailedTruthCollection")



# Moved to WriteFTK_ESD.py and WriteFTK_AOD.py
#
#from RecExConfig.ObjKeyStore import objKeyStore

#objKeyStore.addStreamAOD("xAOD::TrackParticleContainer", "FTK_TrackParticleContainer")
#objKeyStore.addStreamAOD(    "xAOD::TrackParticleAuxContainer","FTK_TrackParticleContainerAux.")
#objKeyStore.addStreamAOD(    "xAOD::TrackParticleContainer","FTK_TrackParticleContainerRefit")
#objKeyStore.addStreamAOD(    "xAOD::TrackParticleAuxContainer","FTK_TrackParticleContainerRefitAux.")
#objKeyStore.addStreamAOD(    "xAOD::VertexContainer","FTK_VertexContainer")
#objKeyStore.addStreamAOD(    "xAOD::VertexAuxContainer","FTK_VertexContainerAux.")
#objKeyStore.addStreamAOD(    "xAOD::VertexContainer","FTK_VertexContainerRefit")
#objKeyStore.addStreamAOD(    "xAOD::VertexAuxContainer","FTK_VertexContainerRefitAux.")

#objKeyStore.addStreamESD("TrackCollection","FTK_TrackCollection")
#objKeyStore.addStreamESD("TrackCollection","FTK_TrackCollectionRefit")
#objKeyStore.addStreamESD("VxContainer","FTK_VxContainer")
#objKeyStore.addStreamESD("VxContainer","FTK_VxContainerRefit")
#objKeyStore.addStreamESD("xAOD::TrackParticleContainer","FTK_TrackParticleContainer")
#objKeyStore.addStreamESD("xAOD::TrackParticleAuxContainer","FTK_TrackParticleContainerAux.")
#objKeyStore.addStreamESD("xAOD::TrackParticleContainer","FTK_TrackParticleContainerRefit")
#objKeyStore.addStreamESD("xAOD::TrackParticleAuxContainer","FTK_TrackParticleContainerRefitAux.")
#objKeyStore.addStreamESD("xAOD::VertexContainer","FTK_VertexContainer")
#objKeyStore.addStreamESD("xAOD::VertexAuxContainer","FTK_VertexContainerAux.")
#objKeyStore.addStreamESD("xAOD::VertexContainer","FTK_VertexContainerRefit")
#objKeyStore.addStreamESD("xAOD::VertexAuxContainer","FTK_VertexContainerRefitAux.")
#objKeyStore.addStreamESD("InDet::PixelClusterContainer","FTK_PixelClusterContainer")
#objKeyStore.addStreamESD("InDet::SCT_ClusterContainer","FTK_SCT_ClusterContainer")

#from RecExConfig.RecFlags import rec
#if rec.doTruth():
#  objKeyStore.addStreamESD("PRD_MultiTruthCollection","FTK_PRD_MultiTruthPixel")
#  objKeyStore.addStreamESD("PRD_MultiTruthCollection","FTK_PRD_MultiTruthSCT")

#print "After addition of FTK collectios AOD and ESD Output list is :"
#print objKeyStore


