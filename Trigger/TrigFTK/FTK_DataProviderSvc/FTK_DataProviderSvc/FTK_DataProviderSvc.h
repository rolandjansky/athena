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

#include "IRegionSelector/IRoiDescriptor.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

/// Forward Declarations ///
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace InDet {
  class PixelCluster;
  class SCT_Cluster;
}

class FTK_DataProviderSvc : public virtual IFTK_DataProviderSvc,
  public virtual AthService {
			    //  public AthService {
    
 public:
 
 FTK_DataProviderSvc(const std::string& name, ISvcLocator* sl);
 virtual ~FTK_DataProviderSvc();

 virtual const InterfaceID& type() const;

 virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
 virtual StatusCode initialize();
 virtual StatusCode finalize();
 virtual StatusCode beginEvent();
 virtual StatusCode endEvent();
 virtual StatusCode GetTracksInRoi(TrackCollection&, const IRoiDescriptor&, bool);
 //virtual StatusCode GetTracksInRoi(TrackCollection&, const RoiDescriptor*, bool);
 virtual StatusCode GetTracksFullScan(TrackCollection&, bool);
 virtual StatusCode GetTracksFromStoreGate();
 virtual StatusCode InitializeTrackCache();
 virtual Trk::Track* ConvertTrack(const FTK_RawTrack &track, bool);

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

  ToolHandle<Trk::ITrackFitter> m_trackFitter;

  const FTK_RawTrackContainer* m_ftk_tracks;

  //std::map<const int, Trk::Track> m_cached_tracks; // Can't use this since Trk::Track doesn't have a < operator
  std::map<const int, int> m_cached_tracks;
  std::vector<Trk::Track*> m_cached_track_objects;

  MsgStream *athenaLog;

};

#endif
