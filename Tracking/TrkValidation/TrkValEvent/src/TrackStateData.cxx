/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackStateData.cxx
//   Source file for class TrackStateData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkValEvent/TrackStateData.h"

// default constructor
Trk::TrackStateData::TrackStateData():
m_trackStateOnSurface(0),
m_rot(0),
m_surface(0),
m_detTypeName("unidentified"),
m_detType(Trk::TrackState::unidentified) {}


// constructor with parameters
Trk::TrackStateData::TrackStateData(const Trk::TrackStateOnSurface* trackStateOnSurface,
                                    const Trk::RIO_OnTrack*         rot,
                                    const Trk::Surface*             surface,
                                    std::string                     detTypeName,
                                    TrackState::MeasurementType     detType):
 
            m_trackStateOnSurface(trackStateOnSurface),
            m_rot(rot),
            m_surface(surface),
            m_detTypeName(detTypeName),
            m_detType(detType)
{}


///  return Trk::TrackStateOnSurface pointer
const Trk::TrackStateOnSurface* Trk::TrackStateData::trackStateOnSurface() const {
    return m_trackStateOnSurface;
}
///  return Trk::RIO_OnTrack pointer
const Trk::RIO_OnTrack*         Trk::TrackStateData::rot() const {
    return m_rot;
}
///  return Trk::Surface pointer
const Trk::Surface*             Trk::TrackStateData::surface() const{
    return m_surface;
}
///  return detector type name
const std::string&              Trk::TrackStateData::detTypeName() const{
    return m_detTypeName;
}
///  return dectector type
const Trk::TrackState::MeasurementType&  Trk::TrackStateData::detType() const{
    return m_detType;
}
