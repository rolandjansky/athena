/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSurfaceIntersection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_TRACKSURFACEINTERSECTION_H
#define TRKEXUTILS_TRACKSURFACEINTERSECTION_H

//Trk
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

      /** @class TrackSurfaceIntersection
         
         An intersection with a Surface is given by
         - a global position
         - a momentum direction (unit vector)
         - the pathlength to go there from the starting point
          
          @author Andreas.Salzburger@cern.ch 
        */
      class TrackSurfaceIntersection {
       
      public:
	  /**Constructor*/
	  TrackSurfaceIntersection(const Amg::Vector3D& pos, const Amg::Vector3D& dir, double path);
	  /**Destructor*/
	  virtual ~TrackSurfaceIntersection() = default;

          TrackSurfaceIntersection (const TrackSurfaceIntersection& other);
          TrackSurfaceIntersection& operator= (const TrackSurfaceIntersection& other) = default;

	  /** Method to retrieve the position of the Intersection */
	  const Amg::Vector3D& position() const;
	        Amg::Vector3D& position();
        
	  /** Method to retrieve the direction at the Intersection */
	  const Amg::Vector3D& direction() const;
	        Amg::Vector3D& direction();
        
	  /** Method to retrieve the pathlength propagated till the Intersection */
	  double  pathlength() const;
	  double& pathlength();
        
	  /** Method to retrieve the object serial number (needed for speed optimization) */
	  unsigned long long serialNumber() const;

      private:
	  static unsigned long long	s_serialNumber;
	  unsigned long long		m_serialNumber;
	  Amg::Vector3D			m_position;
	  Amg::Vector3D			m_direction;
	  double			m_pathlength;
                  
      };

  inline const Amg::Vector3D& TrackSurfaceIntersection::position() const
  { return m_position; }

  inline Amg::Vector3D& TrackSurfaceIntersection::position()
  { return m_position; }

  inline const Amg::Vector3D& TrackSurfaceIntersection::direction() const
  { return m_direction; }

  inline Amg::Vector3D& TrackSurfaceIntersection::direction()
  { return m_direction; }

  inline double TrackSurfaceIntersection::pathlength() const
  { return m_pathlength; }
  
  inline double& TrackSurfaceIntersection::pathlength()
  { return m_pathlength; }
  
  inline unsigned long long TrackSurfaceIntersection::serialNumber() const
  { return m_serialNumber; }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const TrackSurfaceIntersection& tsfi);
std::ostream& operator << ( std::ostream& sl, const TrackSurfaceIntersection& tsfi); 
        
} // end of namespace

#endif // TRKEXUTILS_TRACKSURFACEINTERSECTION_H
