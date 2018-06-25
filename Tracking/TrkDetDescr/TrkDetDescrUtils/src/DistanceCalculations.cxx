/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceCalculations.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/DistanceCalculations.h"
#include "TrkDetDescrUtils/RealQuadraticEquation.h"
#include "CxxUtils/unused.h"

Trk::DistanceSolution Trk::DistanceCalculations::distanceToPlane(const Amg::Vector3D& position, const Amg::Vector3D& dir,
                                                                 const Amg::Vector3D& center, const Amg::Vector3D& normal) const
{ 
   double tol = 0.001;

    double S = center.dot(normal);
    double b = S < 0. ? -1 : 1 ;
    double d = (position-center).dot(normal);     // distance to surface

    double  A = b*dir.dot(normal);
    if(A==0.) {   // direction parallel to surface
      if ( fabs(d)<tol ) {
	return Trk::DistanceSolution(1,0.,true,0.);
      } else {
        return Trk::DistanceSolution(0,d,true,0.);
      }
    }
    
    return Trk::DistanceSolution(1,d,true,b*( S - ( position.dot(normal) ) )/A );
}

Trk::DistanceSolution Trk::DistanceCalculations::distanceToCylinder(const Amg::Vector3D& pos, const Amg::Vector3D& dir,
								    const Amg::Vector3D& center, const Amg::Vector3D& normal,
								    float radius) const
{ 
  double tol = 0.001;

  double sp     = pos.dot(normal);
  double sc     = center.dot(normal);
  double dp     = dir.dot(normal);

  Amg::Vector3D dx = center-pos-(sc-sp)*normal ;  //vector
  Amg::Vector3D ax = dir-dp*normal;          //vector

  double A  = ax.dot(ax);   // size of projected direction (squared)
  double B  = ax.dot(dx);   // dot product (->cos angle)
  double C  = dx.dot(dx);   // distance to axis (squared)
  double currDist = radius-sqrt(C);

  if (A==0.) {                      // direction parallel to cylinder axis
    if ( fabs(currDist) < tol ) {
      return Trk::DistanceSolution(1,0.,true,0.);   // solution at surface
    } else {
      return Trk::DistanceSolution(0,currDist,true,0.);      // point of closest approach without intersection
    }
  }
 
  // minimal distance to cylinder axis
  // The UNUSED declaration is to suppress redundant division checking here.
  // Even a tiny change in rmin (~1e-13) can cause huge changes in the
  // reconstructed output, so don't change how it's evaluated.
  const double UNUSED(rmin_tmp) = B*B/A;
  const double rmin2 = C - rmin_tmp;
  const double rmin = rmin2 < 0 ? 0 : sqrt(rmin2);
 
  if ( rmin > radius ) {          // no intersection
    double first = B/A;
    return Trk::DistanceSolution(0,currDist,true,first);      // point of closest approach without intersection
  } else {
    if ( fabs(rmin - radius) < tol ) {     // tangential 'intersection' - return double solution
      double first = B/A;
      return Trk::DistanceSolution(2, currDist,true,first,first);
    } else {
      // The UNUSED declaration here suppresses redundant division checking.
      // We don't want to rewrite how this is evaluated due to instabilities.
      const double UNUSED(b_a) = B/A;
      const double x = sqrt((radius-rmin)*(radius+rmin)/A);
      double first  = b_a - x;
      double second = b_a + x;
      if ( first >= 0. ) {
	return Trk::DistanceSolution(2,currDist,true,first,second);
      } else if ( second <= 0. ) {
	return Trk::DistanceSolution(2,currDist,true,second,first);
      } else {      // inside cylinder
	return Trk::DistanceSolution(2,currDist,true,second,first);
      }
    }
  }

}

Trk::DistanceSolution Trk::DistanceCalculations::distanceToCone(const Amg::Vector3D& pos, const Amg::Vector3D& dir,
								const Amg::Vector3D& center, const Amg::Vector3D& normal,
								float alpha) const
{
  double tol = 0.001;
  
  Amg::Vector3D dPos = pos - center;  
  double posLength = sqrt(dPos.dot(dPos));
  if (posLength < tol) // at origin of cone => on cone (avoid div by zero)
    return Trk::DistanceSolution(1,0.,true,0.);

  double posN = dPos.dot(normal);     
  Amg::Vector3D posP= pos-posN*normal;
  double posProjAngle = acos(posN/posLength);
  double currDist = posLength*sin(posProjAngle-alpha);
  // solution on the surface
  if (fabs(currDist) < tol) return Trk::DistanceSolution(1,currDist,true,0.);
  
  // solutions are in the form of a solution to a quadratic eqn.
  double dirN =  dir.dot(normal);
  Amg::Vector3D dirP= dir-dirN*normal;
  double  tan2Alpha = tan(alpha)*tan(alpha);
  double  A = dirP.dot(dirP) - tan2Alpha*dirN*dirN;
  double  B = 2*(dirP.dot(posP) - tan2Alpha*dirN*posN);
  double  C = posP.dot(posP) - tan2Alpha*posN*posN;

  if (A == 0.) return Trk::DistanceSolution(0,currDist,true,currDist); 

  Trk::RealQuadraticEquation solns(A,B,C);

  double s1=0.; double s2=0.;
  switch(solns.solutions) {
  case Trk::none:
    return Trk::DistanceSolution(0,currDist,true,0.,0.);
  case Trk::one:
    return Trk::DistanceSolution(1,currDist,true,solns.first);
  case Trk::two:
    s1 = fmin(solns.first,solns.second);
    s2 = fmax(solns.first,solns.second);
    if (s1>=0) return Trk::DistanceSolution(2,currDist,true,s1,s2);
    else return Trk::DistanceSolution(2,currDist,true,s2,s1);
  default:
    return Trk::DistanceSolution(0,currDist,true,0.,0.);
  }

  return Trk::DistanceSolution(0,currDist,true,0.,0.);
}
