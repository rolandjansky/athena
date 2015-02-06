/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFTK_DATAPROVIDERSVC_H
#define IFTK_DATAPROVIDERSVC_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include <string>

namespace InDet {
  class PixelCluster;
  class SCT_Cluster;
}

namespace Trk {
  class Track;
}

class IRoiDescriptor;

static const InterfaceID IID_IFTK_DataProviderSvc("IFTK_DataProviderSvc",1,0);

class IFTK_DataProviderSvc : virtual public IInterface {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IFTK_DataProviderSvc; }
  
  virtual Trk::Track* ConvertTrack(const FTK_RawTrack &track, bool) = 0;
  virtual StatusCode GetTracksFullScan(TrackCollection& track_collection, bool) = 0;
  virtual StatusCode GetTracksInRoi(TrackCollection &track_collection, const IRoiDescriptor&, bool) = 0;
  virtual StatusCode beginEvent() = 0;
  virtual StatusCode endEvent() = 0;

};

#endif
