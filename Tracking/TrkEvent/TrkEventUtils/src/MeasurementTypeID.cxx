/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasurementTypeID.cxx,           (c) ATLAS Detector software  //
///////////////////////////////////////////////////////////////////

#include "TrkEventUtils/MeasurementTypeID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSegment/Segment.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkEventUtils/RoT_Extractor.h"

Trk::TrackState::MeasurementType 
Trk::MeasurementTypeID::defineType (const Trk::MeasurementBase* meas) const {

  if (meas == nullptr) { return Trk::TrackState::unidentified;
}
  const Trk::RIO_OnTrack* testROT=nullptr;
  Trk::RoT_Extractor::extract( testROT, meas); // get std and competing ROTs
  if (testROT!=nullptr) {
    if (!( testROT->identify().is_valid())) { return Trk::TrackState::unidentified;
}
    if (m_idHelper->is_pixel(testROT->identify())) { 
      return Trk::TrackState::Pixel;
    } if (m_idHelper->is_sct(testROT->identify())) { 
      return Trk::TrackState::SCT;
    } else if (m_idHelper->is_trt(testROT->identify())) {
      return Trk::TrackState::TRT;
    } else if (m_idHelper->is_mdt(testROT->identify())) {
      return Trk::TrackState::MDT;
    } else if (m_idHelper->is_tgc(testROT->identify())) { 
      return Trk::TrackState::TGC;
    } else if (m_idHelper->is_rpc(testROT->identify())) { 
      return Trk::TrackState::RPC;
    } else if (m_idHelper->is_csc(testROT->identify())) { 
      return Trk::TrackState::CSC;
    } else if (m_idHelper->is_stgc(testROT->identify())) { 
      return Trk::TrackState::STGC;
    } else if (m_idHelper->is_mm(testROT->identify())) { 
      return Trk::TrackState::MM;
    } else { return Trk::TrackState::unidentified;
}
  }

  const Trk::PseudoMeasurementOnTrack* testPseudo
    = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas);
  if (testPseudo!=nullptr) { return Trk::TrackState::Pseudo;
}
  const Trk::Segment* testSegment
    = dynamic_cast<const Trk::Segment*>(meas);
  if (testSegment!=nullptr) { return Trk::TrackState::Segment;
}
  const Trk::VertexOnTrack* testVertex
    = dynamic_cast<const Trk::VertexOnTrack*>(meas);
  if (testVertex!=nullptr) {  return Trk::TrackState::Vertex;
}
  const Trk::SpacePoint* testSpacePoint
    = dynamic_cast<const Trk::SpacePoint*>(meas);
  if (testSpacePoint!=nullptr) {  return Trk::TrackState::SpacePoint;
}
  return Trk::TrackState::unidentified;
}
