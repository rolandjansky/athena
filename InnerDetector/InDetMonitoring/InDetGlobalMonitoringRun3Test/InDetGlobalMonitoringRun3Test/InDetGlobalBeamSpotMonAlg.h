/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************************
// $Id:$
//
// InDetGlobalBeamSpotMonAlg is a module to monitor primary vertices and the beam spot in
// the context of package InnerDetector/InDetMonitoring/InDetGlobalMonitoring. A
// scaled-down version doing only primary vertex monitoring is available as module
// InDetGlobalPrimaryVertexMonTool (the reason for having two tools is that in
// InDetGlobalBeamSpotMonAlg monitoring is done wrt beam spot, while InDetGlobalPrimaryVertexMonAlg
// does not use the beam spot). Originally, this module was developed in package
// InDetAlignmentMonitoring under the name InDetAlignMonBeamSpot.
//
// Written in March 2008 by Juerg Beringer (LBNL)
// Adapted to AthenaMT 2020 by Per Johansson (Sheffield University) and Leonid Serkin (ICTP)
//
// ********************************************************************************

#ifndef InDetGlobalBeamSpotMonAlg_H
#define InDetGlobalBeamSpotMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "StoreGate/ReadHandleKey.h"

// xAOD
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Beam condition include(s):
#include "BeamSpotConditionsData/BeamSpotData.h"

//#include <vector>
#include <string>


class InDetGlobalBeamSpotMonAlg : public AthMonitorAlgorithm {
  
 public:
  
  InDetGlobalBeamSpotMonAlg( const std::string & name, ISvcLocator* pSvcLocator); 
  virtual ~InDetGlobalBeamSpotMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;
  std::string findComponentString(int bec, int ld) const;
  
 private:
  
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  std::string m_stream;
  
  bool m_useBeamspot;
  SG::ReadHandleKey<xAOD::VertexContainer> m_vxContainerName{this,"vxContainerName","PrimaryVertices","Vertex Container for Global Beamspot Monitoring"};
  bool m_vxContainerWithBeamConstraint;
  
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackContainerName{this,"trackContainerName","InDetTrackParticles","TrackParticle container for Global Beamspot Monitoring"};
  
  std::string m_histFolder;
  std::string m_triggerChainName;
  unsigned int m_minTracksPerVtx;
  float m_minTrackPt;
};

#endif
