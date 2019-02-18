/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceSolution.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DISTANCESOLUTION_H
#define TRKSURFACES_DISTANCESOLUTION_H

//STD
#include <math.h>
#include <iostream>

namespace Trk {
          
  /**
   @class DistanceSolution
   Access to distance solutions. Driven by need to accomodate
   intersections with a cylinder into common interface. 

   @author Sarka.Todorova@cern.ch
   */
      
  class DistanceSolution {
    public:
      /**Default Constructor*/
      DistanceSolution();
      
      /**Constructor*/
      DistanceSolution(int num, double current=0., bool signedDist=false, double first=0., double second=0.);
            
      /**Destructor*/
      virtual ~DistanceSolution() = default;
      
      // methods to access solutions
      /** Number of intersection solutions*/
      int              numberOfSolutions() const;
      
      /** Distance to first intersection solution along direction*/
      double           first() const;
      
      /** Distance to second intersection solution along direction (for a cylinder surface)*/
      double           second() const;
      
      /** Absolute Distance to closest solution */
      double           absClosest() const;
      
      /** Distance to point of closest approach along direction*/
      double           toPointOfClosestApproach() const;
      
      /** Current distance to surface (spatial), signed (along/opposite to surface normal) if input argument true (absolute value by default)*/
      double           currentDistance(bool signedDist=false) const;
      
      /** This method indicates availability of signed current distance (false for Perigee and StraighLineSurface) */
      bool             signedDistance() const;
                    
    protected:
      int                    m_num;
      double                 m_first;
      double                 m_second;
      double                 m_current;
      bool                   m_signedDist;
  };
  
  inline int DistanceSolution::numberOfSolutions() const 
    { 
      return m_num;
    }

  inline double DistanceSolution::first() const 
    { 
      return m_first;
    }

  inline double DistanceSolution::second() const 
    { 
      return m_second;
    }
    
  inline double DistanceSolution::absClosest() const
  {
    if (m_num > 1)
      return ( m_first*m_first < m_second*m_second ) ? fabs(m_first) : fabs(m_second);
    else return fabs(m_first);
  }    
    
  inline double DistanceSolution::toPointOfClosestApproach() const 
    { 
      return m_first;
    }

  inline double DistanceSolution::currentDistance(bool signedDist) const 
    { 
      if (signedDist) return m_current;
      else return fabs(m_current);
    }

  inline bool DistanceSolution::signedDistance() const 
    { 
      return m_signedDist;
    }
                              
} // end of namespace

#endif // TRKSURFACES_DISTANCESOLUTION_H
