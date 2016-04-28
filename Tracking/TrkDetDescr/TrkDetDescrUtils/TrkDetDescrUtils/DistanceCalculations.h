/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceCalculations.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_DISTANCECALCULATIONS_H
#define TRKDETDESCRUTILS_DISTANCECALCULATIONS_H

//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkDetDescrUtils/DistanceSolution.h"

namespace Trk {

/** @class DistanceSolution
   
    distance calculations   
     
    @author sarka.todorova@cern.ch 
  */
   
   class DistanceCalculations {
        
   public:
     Trk::DistanceSolution distanceToPlane(const Amg::Vector3D& position, const Amg::Vector3D& dir,
					   const Amg::Vector3D& center, const Amg::Vector3D& normal) const; 
     
     Trk::DistanceSolution distanceToCylinder(const Amg::Vector3D& pos, const Amg::Vector3D& dir,
					      const Amg::Vector3D& center, const Amg::Vector3D& normal,
					      float radius) const;
     
     Trk::DistanceSolution distanceToCone(const Amg::Vector3D& pos, const Amg::Vector3D& dir,
					  const Amg::Vector3D& center, const Amg::Vector3D& normal,
					  float alpha) const;

   private:
     float m_tol;
   };
}

#endif // TRKDETDESCRUTILS_DISTANCECALCULATIONS_H
