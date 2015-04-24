/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_DATAPROVIDERSVC_H
#define FTK_DATAPROVIDERSVC_H

#include "GaudiKernel/Service.h"
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
#include "VxVertex/VxContainer.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

//#include "TrkFitterInterfaces/ITrackFitter.h"
//#include "TrkFitterUtils/FitterTypes.h"

//#include "InDetPrepRawData/PixelClusterCollection.h"
//#include "InDetPrepRawData/SCT_ClusterCollection.h"
//#include "InDetPrepRawData/PixelClusterContainer.h"
//#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"

/// Forward Declarations ///
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;
class IPixelOfflineCalibSvc;
class IRoiDescriptor;
//class IFTK_UncertaintyTool;

namespace Trk {
  class Track;
  class ITrackFitter;
  class ITrackSummaryTool;
  class ITrackParticleCreatorTool;
  //  class VxCandidate;
  //class IVxCandidateXAODVertex;
}

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace InDet {
  class PixelCluster;
  class SCT_Cluster;
  class IVertexFinder;

}




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
 virtual TrackCollection* getTracks(const bool withRefit);
 virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit);
 virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit);
 
 virtual VxContainer* getVxContainer(const bool withRefit);
 virtual xAOD::VertexContainer* getVertexContainer(const bool withRefit);


 virtual void handle( const Incident &incident );

 StatusCode getFTK_RawTracksFromSG();
 Trk::Track* ConvertTrack(const unsigned int track);
 Trk::Track* getCachedTrack(const unsigned int track, const bool do_refit);
 StatusCode initTrackCache(bool do_refit);
 StatusCode initTrackParticleCache(bool do_refit);
 StatusCode fillTrackCache(bool do_refit);
 StatusCode fillTrackParticleCache(const bool withRefit);
 bool fillVxContainer(bool withRefit, TrackCollection* tracks);
 bool fillVertexContainerCache(bool withRefit, xAOD::TrackParticleContainer*);
 protected:

 InDet::PixelCluster* createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster, float eta);
 InDet::SCT_Cluster*  createSCT_Cluster( const FTK_RawSCT_Cluster&);

 private:



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

  const FTK_RawTrackContainer* m_ftk_tracks;

  // Track Cache
  TrackCollection* m_conv_tracks;
  TrackCollection* m_refit_tracks;

  //TrackParticle Cache
  xAOD::TrackParticleContainer* m_conv_tp;
  xAOD::TrackParticleContainer* m_refit_tp;
  xAOD::TrackParticleAuxContainer* m_conv_tpAuxCont;
  xAOD::TrackParticleAuxContainer* m_refit_tpAuxCont;

  // VxVertex cache 
  VxContainer* m_conv_vx;
  VxContainer* m_refit_vx;
  bool m_got_conv_vx;
  bool m_got_refit_vx;

  // xAOD vertex cache
  xAOD::VertexContainer* m_conv_vertex;
  xAOD::VertexAuxContainer* m_conv_vertexAux;
  xAOD::VertexContainer* m_refit_vertex;
  xAOD::VertexAuxContainer* m_refit_vertexAux;
  bool m_got_conv_vertex;
  bool m_got_refit_vertex;

  

  // maps from FTK track index to converted/refitted object index
  std::vector< int> m_conv_track_map;
  std::vector< int> m_refit_track_map;
  std::vector< int> m_conv_tp_map;
  std::vector< int> m_refit_tp_map;

  bool m_newEvent;
  bool m_gotRawTracks;
  std::string m_trackCacheName;
  std::string m_trackParticleCacheName;
  std::string m_VxContainerCacheName;
  std::string  m_VertexContainerCacheName;
  MsgStream *athenaLog;

};

#endif
