/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpaceTimePoint.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSPACETIMEPOINTBASE_H
#define TRKSPACETIMEPOINTBASE_H
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

  /** SpaceTimePointBase 

      This base class for classes holding a position and a time with associated error and weight.
      
      @author  Christian Ohm <christian.ohm@cern.ch>
      @author  Dominique Fortin <dominique.fortin@cern.ch>
  */  
  class SpaceTimePointBase {
    public:
    /** constructor taking a time, an error on the time and a weight */
    SpaceTimePointBase(const float& t, const float& t_error, const float& weight);

    /** copy ctor*/
    SpaceTimePointBase(const SpaceTimePointBase& stpb);

    /** Assignment operator */
    SpaceTimePointBase& operator=(const SpaceTimePointBase& stpb);

    /** default destructor */
    virtual ~SpaceTimePointBase() {};
      
    /** access to the global position (implemented in concrete classes) */
    virtual const Amg::Vector3D& globalPosition() const = 0;
    
    /** access to the measured time */
    float time() const {return m_time;}
    
    /** access to the error on the measured time */
    float errorTime() const {return m_errorTime;}
    
    /** access to the weight of the space time point */
    float weight() const {return m_weight;}
    
  protected:
    
    /** measured time */
    float m_time;
    
    /** error on the time measurement */
    float m_errorTime;
    
    /** weight of the point */
    float m_weight;
    
  }; 

} // end of namespace

#endif
