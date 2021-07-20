/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonGenericTracksAlg_H
#define IDAlignMonGenericTracksAlg_H

// **********************************************************************
// IDAlignMonGenericTracksAlg.h
// AUTHORS: Beate Heinemann, Tobias Golling
// Adapted to AthenaMT 2021 by Per Johansson
// **********************************************************************

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/EventContext.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "StoreGate/ReadHandleKey.h"

//------------TrackMon------------
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include <vector>
#include <map>

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace Trk  { 
  class Track;
  class VxTrackAtVertex;
}

class IInDetAlignHitQualSelTool; 

class IDAlignMonGenericTracksAlg : public AthMonitorAlgorithm {

public:

  IDAlignMonGenericTracksAlg( const std::string & name, ISvcLocator* pSvcLocator ); 
  virtual ~IDAlignMonGenericTracksAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

protected:

 bool m_hasBeamCondSvc;

private:

  bool fillVertexInformation(std::map<const xAOD::TrackParticle*, const xAOD::Vertex*>& trackVertexMapTP, const EventContext& ctx ) const;
  const Trk::Track*   getTrkTrack(const Trk::VxTrackAtVertex*)const;
  const xAOD::Vertex* findAssociatedVertexTP(const std::map<const xAOD::TrackParticle*, const xAOD::Vertex*>& trackVertexMapTP, const xAOD::TrackParticle *) const;

  const AtlasDetectorID*                m_idHelper;
  const PixelID*                        m_pixelID;
  const SCT_ID*                         m_sctID; 
  const TRT_ID*                         m_trtID; 
  
  float m_barrelEta;
  bool  m_extendedPlots;
  float m_d0Range;
  float m_d0BsRange;
  float m_pTRange;
  float m_z0Range;
  float m_etaRange;
  int   m_NTracksRange;
  bool  m_doIP;
  bool  m_doHitQuality;
  
  SG::ReadHandleKey<xAOD::VertexContainer> m_VxPrimContainerName{this,"vxPrimContainerName","PrimaryVertices","Primary Vertices for Alignment Monitoring"};
  SG::ReadHandleKey<TrackCollection> m_tracksKey  {this, "TrackName", "ExtendedTracks", "track data key"};
  SG::ReadHandleKey<TrackCollection> m_tracksName {this, "TrackName2", "ExtendedTracks", "track data key"};
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelection; // baseline
  ToolHandle<IInDetAlignHitQualSelTool>  m_hitQualityTool;
  PublicToolHandle< Trk::ITrackToVertexIPEstimator >  m_trackToVertexIPEstimator
     {this,"TrackToVertexIPEstimator","Trk::TrackToVertexIPEstimator",""};
  
  std::string m_Pixel_Manager;
  std::string m_SCT_Manager;
  std::string m_TRT_Manager;

};

#endif
