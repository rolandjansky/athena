/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpaceTimePoint.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSPACETIMEPOINT_H
#define TRKSPACETIMEPOINT_H

#include "TrkSpaceTimePoint/SpaceTimePointBase.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include <ostream>
class MsgStream;

namespace Trk {

  /** SpaceTimePoint 

      This class holds a position and a time with associated error and weight.
      
      @author  Christian Ohm <christian.ohm@cern.ch>
      @author  Dominique Fortin <dominique.fortin@cern.ch>
  */  
  class SpaceTimePoint : public SpaceTimePointBase {
    public:

    /** T/P framework default constructor */
    SpaceTimePoint();

    /** constructor taking a time, an error on the time and a weight */
    SpaceTimePoint(const Amg::Vector3D& position, const float& t, const float& t_error,
                   const float& weight, const TrackState::MeasurementType detector);

    /** default destructor */
    virtual ~SpaceTimePoint() {};
      
    /** access to the global position */
    const Amg::Vector3D& globalPosition() const {return m_position;}
    
    /** return information about detector from which timing info originates */
    TrackState::MeasurementType measurementType() const {return m_detectorID;}
    
  private:
    
    /** global position of the point */
    Amg::Vector3D m_position;
    
    /** enum to denote origin of timing information */
    TrackState::MeasurementType m_detectorID;
    
  }; 

} // end of namespace

/** Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Trk::SpaceTimePoint& stp);

/** Overload of << operator for std::ostream for debug output*/
std::ostream& operator << ( std::ostream& sl, const Trk::SpaceTimePoint& stp);

#endif
