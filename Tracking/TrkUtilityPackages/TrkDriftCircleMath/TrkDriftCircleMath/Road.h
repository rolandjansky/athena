/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_DRIFT_CICLE_MATH__ROAD_H__
#define __TRK_DRIFT_CICLE_MATH__ROAD_H__

#include "TrkDriftCircleMath/LocPos.h"

namespace TrkDriftCircleMath
{

  /**
     @brief TrkDriftCircleMath::Road - encodes the road given to the segment finder in station coordinates.
  */
  class Road {
  public:
    
    /** constructor
	@param roadPos          position of the road in local AMDB chamber coordinates
	@param roadAngle        angle of the road in local AMDB chamber coordinates
	@param chamberAngle     angle of the line from the chamber center to the IP in local AMDB chamber coordinates
	@param roadAngularWidth opening angle of the road in the precision coordinate of the chamber
    */
    Road (const TrkDriftCircleMath::LocPos& roadPos, double roadAngle, double chamberAngle, double roadAngularWidth);
          
    /** destructor */
    ~Road();
    
    /** returns the position of the road in local coordinates */
    const TrkDriftCircleMath::LocPos&   pos() const { return  m_pos; }

    /** returns the angle of the road in local coordinates */
    double angle() const { return  m_angle; }

    /** returns the angle of the line from the chamber center to the IP in local AMDB chamber coordinates */
    double chamberAngle() const { return  m_chamberAngle; }

    /** returns the width angle of the road */
    double width() const { return  m_width; }
    
  private:
    TrkDriftCircleMath::LocPos  m_pos;
    double                      m_angle;
    double                      m_chamberAngle;
    double                      m_width;
  };

}

#endif // __TRK_DRIFT_CICLE_MATH__ROAD_H__
