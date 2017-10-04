/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_DATAPROVIDERSVC_H
#define FTK_DATAPROVIDERSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentListener.h"
//#include "IRegionSelector/IRoiDescriptor.h"
//#include "TrkTrack/TrackCollection.h"
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

//#include "TrkFitterInterfaces/ITrackFitter.h"
//#include "TrkFitterUtils/FitterTypes.h"
//#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
//#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"
#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"

/// Forward Declarations ///
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;
class IPixelOfflineCalibSvc;
class IRoiDescriptor;
//class IFTK_UncertaintyTool;
class PRD_MultiTruthCollection;
class McEventCollection;

namespace Trk {
  class Track;
  class ITrackFitter;
  class ITrackSummaryTool;
  class ITrackParticleCreatorTool;
  class IRIO_OnTrackCreator;
  //  class RIO_OnTrack;
  //  class VxCandidate;
  //class IVxCandidateXAODVertex;
}

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace InDet {
  class PixelCluster;
  class PixelClusterOnTrack;
  class SCT_Cluster;
  class IVertexFinder;
}

class IFTK_VertexFinderTool;


class FTK_DataProviderSvc : public virtual IFTK_DataProviderSvc, virtual public IIncidentListener,
  public virtual AthService {
			    //  public AthService {
    
 public:
 
 FTK_DataProviderSvc(const std::string& name, ISvcLocator* sl);
 virtual ~FTK_DataProviderSvc();

 virtual const InterfaceID& type() const;

 virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
 virtual StatusCode initialize();
 virtual StatusCode finalize();
 virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit);
 virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit, unsigned int& nErrors);

 virtual TrackCollection* getTracks(const bool withRefit);
 virtual TrackCollection* getTracks(const bool withRefit,unsigned int& nErrors);

 virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit);
 virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit, unsigned int& nErrors);

 virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit);
 virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit, unsigned int& nErrors);

 virtual  xAOD::VertexContainer* getFastVertices(const ftk::FTK_TrackType trackType=ftk::RawTrack);
 
 virtual xAOD::VertexContainer* getVertexContainer(const bool withRefit);
 
 virtual StatusCode getVertexContainer(xAOD::VertexContainer* vertex, const bool withRefit);

 virtual std::string getTrackParticleCacheName(const bool withRefit);

 virtual std::string getTrackCacheName(const bool withRefit);
 
 virtual std::string getVertexCacheName(const bool withRefit);

 virtual std::string getFastVertexCacheName(const bool withRefit);

 virtual void handle( const Incident &incident );

 virtual unsigned int nRawTracks();
 virtual unsigned int nTracks(const bool withRefit);
 virtual unsigned int nTrackParticles(const bool withRefit);
 virtual unsigned int nTrackErrors(const bool withRefit);
 virtual unsigned int nTrackParticleErrors(const bool withRefit);
 
 virtual std::vector<unsigned int> nMissingSCTClusters();
 virtual std::vector<unsigned int> nMissingPixelClusters();
 virtual std::vector<unsigned int> nFailedSCTClusters();
 virtual std::vector<unsigned int> nFailedPixelClusters();
 

 private:

 void getFTK_RawTracksFromSG();
 Trk::Track* ConvertTrack(const unsigned int track);
 Trk::Track* getCachedTrack(const unsigned int track, const bool do_refit);
 StatusCode initTrackCache(bool do_refit);
 StatusCode initTrackParticleCache(bool do_refit);
 StatusCode fillTrackCache(bool do_refit);
 StatusCode fillTrackParticleCache(const bool withRefit);

 bool fillVertexContainerCache(bool withRefit, xAOD::TrackParticleContainer*);

 
 const Trk::RIO_OnTrack* createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster,  const Trk::TrackParameters& trkPerigee);
 const Trk::RIO_OnTrack* createSCT_Cluster( const FTK_RawSCT_Cluster& raw_sct_cluster, const Trk::TrackParameters& trkPerigee);
 
 StatusCode getTruthCollections();
 void createSCT_Truth(Identifier id, int barCode);
 void createPixelTruth(Identifier id, int barCode);

 InDet::SCT_ClusterCollection*  getSCT_ClusterCollection(IdentifierHash hashId);
 InDet::PixelClusterCollection* getPixelClusterCollection(IdentifierHash hashId);



 private:

 float dphi(const float phi1, const float phi2) const;


  std::string m_RDO_key;
  StoreGateSvc* m_storeGate;
  ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;
  const PixelID* m_pixelId;
  const SCT_ID*  m_sctId;
  
  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager*  m_SCT_Manager;

  const AtlasDetectorID* m_id_helper;

  ToolHandle<IFTK_UncertaintyTool> m_uncertaintyTool;
  ToolHandle<Trk::ITrackFitter> m_trackFitter;
  ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool;
  ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
  ToolHandle< InDet::IVertexFinder > m_VertexFinderTool;
  ToolHandle< IFTK_VertexFinderTool > m_RawVertexFinderTool;
  ToolHandle< Trk::IRIO_OnTrackCreator >      m_ROTcreator;
  ToolHandle< IFTK_DuplicateTrackRemovalTool > m_DuplicateTrackRemovalTool;

  double m_trainingBeamspotX;
  double m_trainingBeamspotY;
  double m_trainingBeamspotZ;
  double m_trainingBeamspotTiltX;
  double m_trainingBeamspotTiltY;
  
  bool m_remove_duplicates;
  
  const FTK_RawTrackContainer* m_ftk_tracks;

  // Track Cache
  TrackCollection* m_conv_tracks;
  TrackCollection* m_refit_tracks;

  //TrackParticle Cache
  xAOD::TrackParticleContainer* m_conv_tp;
  xAOD::TrackParticleContainer* m_refit_tp;
  xAOD::TrackParticleAuxContainer* m_conv_tpAuxCont;
  xAOD::TrackParticleAuxContainer* m_refit_tpAuxCont;

  // xAOD vertex cache
  xAOD::VertexContainer* m_conv_vertex;
  xAOD::VertexContainer* m_refit_vertex;

  // for fast vertexing algorithm
  xAOD::VertexContainer* m_fast_vertex_raw;
  xAOD::VertexContainer* m_fast_vertex_conv;
  xAOD::VertexContainer* m_fast_vertex_refit;

  bool m_got_conv_vertex;
  bool m_got_refit_vertex;
  bool m_got_fast_vertex_refit;
  bool m_got_fast_vertex_conv;
  bool m_got_fast_vertex_raw;
  
  bool m_correctPixelClusters;
  bool m_correctSCTClusters;
  bool m_broadPixelErrors;
  bool m_broadSCT_Errors;

  // maps from FTK track index to converted/refitted object index
  std::vector< int> m_conv_track_map;
  std::vector< int> m_refit_track_map;
  std::vector< int> m_conv_tp_map;
  std::vector< int> m_refit_tp_map;

  bool m_newEvent;
  bool m_gotRawTracks;
  std::string m_trackCacheName;
  std::string m_trackParticleCacheName;
  std::string  m_vertexCacheName;
  bool m_doTruth;
  std::string m_ftkPixelTruthName;
  std::string m_ftkSctTruthName;
  std::string m_mcTruthName;              

  const McEventCollection*  m_mcEventCollection;
  bool m_collectionsReady;
  PRD_MultiTruthCollection* m_ftkPixelTruth;
  PRD_MultiTruthCollection* m_ftkSctTruth;


  // collection of FTK clusters
  std::string m_PixelClusterContainerName; // name of the collection
  InDet::PixelClusterContainer* m_PixelClusterContainer; // pixel container object
  std::string m_SCT_ClusterContainerName; // name of the collection
  InDet::SCT_ClusterContainer* m_SCT_ClusterContainer; // SCT container object

  double m_pTscaleFactor;

  bool m_rejectBadTracks;
  float m_dPhiCut;
  float m_dEtaCut;
  std::vector<float> m_pixelBarrelPhiOffsets;
  std::vector<float>  m_pixelBarrelEtaOffsets;
  std::vector<float>  m_pixelEndCapPhiOffsets;
  std::vector<float> m_pixelEndCapEtaOffsets;

  unsigned int  m_nErrors;
  std::vector<unsigned int> m_nFailedSCTClusters;
  std::vector<unsigned int> m_nFailedPixelClusters;
  std::vector<unsigned int> m_nMissingSCTClusters;
  std::vector<unsigned int> m_nMissingPixelClusters;

};

inline bool compareFTK_Clusters (const Trk::RIO_OnTrack* cl1, const Trk::RIO_OnTrack* cl2) {
   
  //  double r1 = cl1->globalPosition().x()*cl1->globalPosition().x() + cl1->globalPosition().y()*cl1->globalPosition().y();
  //double r2 = cl2->globalPosition().x()*cl2->globalPosition().x() + cl2->globalPosition().y()*cl2->globalPosition().y();
  //r1+=cl1->globalPosition().z()*cl1->globalPosition().z();
  //r2+=cl2->globalPosition().z()*cl2->globalPosition().z();

  return (cl1->globalPosition().mag()<cl2->globalPosition().mag());
  
}


#endif
