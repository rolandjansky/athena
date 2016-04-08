/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TrackStateData
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for some data extracted from a TrackStateOnSurface
/////////////////////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRK_TRACKSTATEDATA_H
#define TRK_TRACKSTATEDATA_H

#include <string>
#include "TrkEventPrimitives/TrackStateDefs.h"

namespace Trk {
class TrackStateOnSurface;
class RIO_OnTrack;
class Surface;

/** @class TrackStateData
    Helper class for Trk::TrackDiff.
    @author Sebastian.Fleischmann@cern.ch
*/
class TrackStateData {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    /**  default constructor */
    TrackStateData();
    /**  explicit constructor */
    TrackStateData(const Trk::TrackStateOnSurface* trackStateOnSurface,
                   const Trk::RIO_OnTrack*         rot,
                   const Trk::Surface*             surface,
                   std::string                     detTypeName,
                   TrackState::MeasurementType     detType);

    /**  destructor */
    ~TrackStateData(){};
    //    /**  assignment operator */
    //    ResidualPull& operator  = (const ResidualPull&);

    ///////////////////////////////////////////////////////////////////
    // Main methods
    ///////////////////////////////////////////////////////////////////
    
    /**  return Trk::TrackStateOnSurface pointer */
    const Trk::TrackStateOnSurface* trackStateOnSurface() const;
    /**  return Trk::RIO_OnTrack pointer */
    const Trk::RIO_OnTrack*         rot() const;
    /**  return Trk::Surface pointer */
    const Trk::Surface*             surface() const;
    /**  return detector type name */
    const std::string&              detTypeName() const;
    /**  return dectector type */
    const TrackState::MeasurementType& detType() const;
    

protected:

    ///////////////////////////////////////////////////////////////////
    // Protected Data
    ///////////////////////////////////////////////////////////////////

    const Trk::TrackStateOnSurface* m_trackStateOnSurface;
    const Trk::RIO_OnTrack*         m_rot;
    const Trk::Surface*             m_surface;
    const std::string               m_detTypeName;
    const TrackState::MeasurementType m_detType;

};


} // end of name space

#endif // TRK_TRACKSTATEDATA_H
