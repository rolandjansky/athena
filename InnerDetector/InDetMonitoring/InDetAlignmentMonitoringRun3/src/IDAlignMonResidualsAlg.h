/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonResiduals.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Ben Cooper, John Alison, Pierfrancesco Butti
// Adapted to AthenaMT 2021 by Per Johansson
// **********************************************************************

#ifndef IDAlignMonResidualsAlg_H
#define IDAlignMonResidualsAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/EventContext.h"
#include "CommissionEvent/ComTime.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"

#include <string>
#include <vector>
#include <vector>

class ITRT_CalDbTool;
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace InDetDD{
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace Trk {
  class IUpdator;
  class IPropagator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;
  class IResidualPullCalculator; 
}

class IInDetAlignHitQualSelTool; 

class IDAlignMonResidualsAlg :  public AthMonitorAlgorithm {

 public:
  
  IDAlignMonResidualsAlg( const std::string & name, ISvcLocator* pSvcLocator ); 
  virtual ~IDAlignMonResidualsAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  /** Convert from an int to a string */
  static std::string intToString(int input);

 private:
  StatusCode setupTools(); //PJ setup managers and helpers, etc.
	
  bool isEdge(const Trk::RIO_OnTrack*) const;	
  
  std::pair<const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> findOverlapHit(const Trk::Track*, const Trk::RIO_OnTrack*) const;
  StatusCode getSiResiduals(const Trk::Track*, const Trk::TrackStateOnSurface*, bool, double*) const;
  std::unique_ptr <Trk::TrackParameters> getUnbiasedTrackParameters(const Trk::Track*, const Trk::TrackStateOnSurface*) const;

  bool trackRequiresRefit(const Trk::Track*) const;

  float m_maxPtEC{}; // threshold for low-pt EC distributions
  
  //tools
  const AtlasDetectorID*                m_idHelper{};
  const InDetDD::PixelDetectorManager*  m_PIX_Mgr{}; 
  const InDetDD::SCT_DetectorManager*   m_SCT_Mgr{};
  const PixelID*                        m_pixelID{};
  const SCT_ID*                         m_sctID{}; 
  const TRT_ID*                         m_trtID{}; 

  SG::ReadHandleKey<TrackCollection> m_tracksKey {this,"TrackName", "CombinedInDetTracks", "track data key"};
  SG::ReadHandleKey<TrackCollection> m_tracksName {this,"TrackName2","CombinedInDetTracks", "track data key"};
  SG::ReadHandleKey<ComTime> m_comTimeObjectName    {this, "ComTimeObjectName", "TRT_Phase"};

  ToolHandle<ITRT_CalDbTool> m_trtcaldbTool;
  ToolHandle<Trk::IUpdator>             m_iUpdator;
  ToolHandle<Trk::IPropagator>          m_propagator;
  ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool handle
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelection; // baseline
  ToolHandle<IInDetAlignHitQualSelTool>  m_hitQualityTool;

  std::string m_Pixel_Manager;
  std::string m_SCT_Manager;
  bool  m_extendedPlots;
  bool m_doHitQuality = false;
  int m_checkrate {};
  bool m_doPulls {};
  const std::string m_layers[4]{"b0", "b1", "b2", "b3"}; //
  
};

#endif
