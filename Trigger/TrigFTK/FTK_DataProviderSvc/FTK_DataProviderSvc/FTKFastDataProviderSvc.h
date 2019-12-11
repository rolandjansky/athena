/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKFASTDATAPROVIDERSVC_H
#define FTKFASTDATAPROVIDERSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrkTrack/TrackCollection.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk {
  class Track;
  class ITrackParticleCreatorTool;
  class IRIO_OnTrackCreator;
  class IVertexCollectionSortingTool;
}


namespace InDet {
  class IVertexFinder;
}

class IFTK_VertexFinderTool;


class FTKFastDataProviderSvc : public extends<AthService, IFTK_DataProviderSvc, IIncidentListener> {
    
 public:
 
 FTKFastDataProviderSvc(const std::string& name, ISvcLocator* sl);
 virtual ~FTKFastDataProviderSvc() = default;

 virtual StatusCode initialize() override;

 virtual const FTK_RawTrackContainer* getRawTracks() override;

 virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit) override;
 virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit, unsigned int& nErrors) override;

 virtual TrackCollection* getTracks(const bool withRefit) override;
 virtual TrackCollection* getTracks(const bool withRefit,unsigned int& nErrors) override;

 virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit) override;
 virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit, unsigned int& nErrors) override;

 virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit) override;
 virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit, unsigned int& nErrors) override;

 virtual  xAOD::VertexContainer* getFastVertices(const ftk::FTK_TrackType trackType=ftk::RawTrack) override;
 
 virtual xAOD::VertexContainer* getVertexContainer(const bool withRefit) override;
 
 virtual StatusCode getVertexContainer(xAOD::VertexContainer* vertex, const bool withRefit) override;

 virtual std::string getTrackParticleCacheName(const bool withRefit) override;

 virtual std::string getTrackCacheName(const bool withRefit) override;
 
 virtual std::string getVertexCacheName(const bool withRefit) override;

 virtual std::string getFastVertexCacheName(const bool withRefit) override;

 virtual Trk::Track* getCachedTrack(const unsigned int track, const bool withRefit) override;

 virtual void handle( const Incident &incident ) override;

 virtual unsigned int nRawTracks() override;
 virtual unsigned int nTracks(const bool withRefit) override;
 virtual unsigned int nTrackParticles(const bool withRefit) override;
 virtual unsigned int nTrackErrors(const bool withRefit) override;
 virtual unsigned int nTrackParticleErrors(const bool withRefit) override;
 
 virtual std::vector<unsigned int> nMissingSCTClusters() override;
 virtual std::vector<unsigned int> nMissingPixelClusters() override;
 virtual std::vector<unsigned int> nFailedSCTClusters() override;
 virtual std::vector<unsigned int> nFailedPixelClusters() override;

 private:

 StatusCode initTrackCache(bool do_refit);
 StatusCode initTrackParticleCache(bool do_refit);
 StatusCode fillTrackParticleCache(const bool withRefit);

 bool fillVertexContainerCache(bool withRefit, xAOD::TrackParticleContainer*);

 bool insideRoi(const IRoiDescriptor& roi, const Trk::Track *track);
 bool insideRoi(const IRoiDescriptor& roi, const xAOD::TrackParticle *track);

 private:

  ServiceHandle<StoreGateSvc> m_storeGate;

  ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
  ToolHandle< InDet::IVertexFinder > m_VertexFinderTool;
  ToolHandle<Trk::IVertexCollectionSortingTool > m_VertexCollectionSortingTool;
  ToolHandle< IFTK_VertexFinderTool > m_RawVertexFinderTool;

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
  xAOD::VertexContainer* m_fast_vertex_conv;
  xAOD::VertexContainer* m_fast_vertex_refit;

  bool m_got_conv_vertex;
  bool m_got_refit_vertex;
  bool m_got_fast_vertex_refit;
  bool m_got_fast_vertex_conv;
  
  std::string m_trackCacheName;
  std::string m_trackParticleCacheName;
  std::string  m_vertexCacheName;

  unsigned int  m_nErrors;
  std::vector<unsigned int> m_nFailedSCTClusters;
  std::vector<unsigned int> m_nFailedPixelClusters;
  std::vector<unsigned int> m_nMissingSCTClusters;
  std::vector<unsigned int> m_nMissingPixelClusters;

  bool m_doVertexing;
  bool m_doVertexSorting;

  bool m_done_conv_tp;
  bool m_done_refit_tp;
};


#endif
