/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFTK_DATAPROVIDERSVC_H
#define IFTK_DATAPROVIDERSVC_H

// Include Files
#include <string>
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
namespace InDet {
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {
  class Track;
}
class IRoiDescriptor;

namespace ftk{
    enum FTK_TrackType{RawTrack, ConvertedTrack, RefittedTrack};
}

static const InterfaceID IID_IFTK_DataProviderSvc("IFTK_DataProviderSvc",1,0);

class IFTK_DataProviderSvc : virtual public IInterface {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IFTK_DataProviderSvc; }

  virtual const FTK_RawTrackContainer* getRawTracks() = 0;
  
  virtual TrackCollection* getTracks(const bool withRefit) = 0;
  virtual TrackCollection* getTracks(const bool withRefit, unsigned int& nErrors) = 0;

  virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit) = 0;
  virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit, unsigned int& nErrors) = 0;

  virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit) = 0;
  virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit, unsigned int& nErrors) = 0;
  virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit) = 0;
  virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit, unsigned int& nErrors) = 0;

  virtual xAOD::VertexContainer* getFastVertices(const ftk::FTK_TrackType trackType=ftk::RawTrack) =0;

  // methods to create and return a view container. DataProviderSvc retains ownership 
  virtual xAOD::VertexContainer* getVertexContainer(const bool withRefit)=0;

  // method to fill container. Ownership passes to caller 
  virtual StatusCode getVertexContainer(xAOD::VertexContainer* vertex, const bool withRefit)=0;

  virtual std::string getTrackParticleCacheName(const bool withRefit)=0;

  virtual std::string getTrackCacheName(const bool withRefit)=0;

  virtual std::string getVertexCacheName(const bool withRefit)=0;

  virtual std::string getFastVertexCacheName(const bool withRefit)=0;

  virtual std::vector<unsigned int> nMissingSCTClusters()=0;
  virtual std::vector<unsigned int> nMissingPixelClusters()=0;
  virtual std::vector<unsigned int> nFailedSCTClusters()=0;
  virtual std::vector<unsigned int> nFailedPixelClusters()=0;

  virtual unsigned int nRawTracks()=0;
  virtual unsigned int nTracks(const bool withRefit)=0;
  virtual unsigned int nTrackParticles(const bool withRefit)=0;
  virtual unsigned int nTrackErrors(const bool withRefit)=0;
  virtual unsigned int nTrackParticleErrors(const bool withRefit)=0;
 
  virtual Trk::Track* getCachedTrack(const unsigned int track, const bool do_refit)=0;

};

#endif
