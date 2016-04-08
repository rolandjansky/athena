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
//#include "VxVertex/VxContainer.h"
//#include "VxVertex/VxCandidate.h"
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "IRegionSelector/IRoiDescriptor.h"
class VxContainer;
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
    enum FTK_TrackType{RawTrackType, ConvertedTrackType, RefittedTrackType};
}

static const InterfaceID IID_IFTK_DataProviderSvc("IFTK_DataProviderSvc",1,0);

class IFTK_DataProviderSvc : virtual public IInterface {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IFTK_DataProviderSvc; }
  
  virtual TrackCollection* getTracks(const bool withRefit) = 0;
  virtual TrackCollection* getTracksInRoi(const IRoiDescriptor&, const bool withRefit) = 0;

  virtual xAOD::TrackParticleContainer* getTrackParticles(const bool withRefit) = 0;
  virtual xAOD::TrackParticleContainer* getTrackParticlesInRoi(const IRoiDescriptor&, const bool withRefit) = 0;

  virtual VxContainer* getVxContainer(const ftk::FTK_TrackType)=0;

  // methods to create and return a view container. DataProviderSvc retains ownership 
  virtual xAOD::VertexContainer* getVertexContainer(const bool withRefit)=0;

  // method to fill container. Ownership passes to caller 
  virtual StatusCode getVertexContainer(xAOD::VertexContainer* vertex, const bool withRefit)=0;
};

#endif
