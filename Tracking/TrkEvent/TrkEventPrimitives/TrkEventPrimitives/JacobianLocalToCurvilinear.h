/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCurvilinearToCurvilinear.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANLOCALTOCURVILINEAR_H
#define TRKEVENTPRIMITIVES_JACOBIANLOCALTOCURVILINEAR_H


// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

 // Trk
 class CurvilinearUVT;

/** @class JacobianLocalToCurvilinear
  
   This class represents the jacobian for transforming from a local to a curvilinear frame.
  
   The curvilinear frame is defined as follows:
   Given a direction @f$ \vec t = \frac{\vec p }{|\vec p |} @f$ of a track at a specific point,
   the plane perpenticular to the track may be defined by two vectors @f$ (\vec u, \vec v) @f$,
   denoting @f$ \vec z @f$ as the global z-axis and defining:
   @f$ \vec{u} = \frac {\vec t \times \vec z}{|\vec t \times \vec z|} @f$  and <br>
   @f$ \vec v =  \vec t \times \vec{u} @f$, <br>
   the unit vectors @f$ (\vec{curv_{i}}) = (\vec u, \vec v, \vec t) @f$ define at any point of
   the track a right-handed orthogonal coordinate system.<br>
   The local frame on a surface may be represented through two local vectors @f$ \vec{locX} @f$ and @f$ \vec{locY} @f$
   within the plane and one perpenticular vector @f$ \vec{locZ} @f$ pointing along the surface's normal.
  
   <b>Detailed Description</b></br>
   The non-zero elements of the jacobian for the transformation between the local representations of the track parameters
   @f$ (locX, locY, \phi, \theta, q/p) @f$ and the curvilinear representation @f$ (u, v, \phi', \theta', q/p') @f$
   can then be expressed as:<br>

   For the straight line case:

   @f$ \frac{\partial u}{\partial locX} = \vec{u} \cdot \vec{locX}   @f$ <br>
   @f$ \frac{\partial u}{\partial locY} = \vec{u} \cdot \vec{locY}  @f$ <br>
   @f$ \frac{\partial v}{\partial locX} = \vec v \cdot \vec{locX}  @f$ <br>
   @f$ \frac{\partial v}{\partial locY} = \vec v \cdot \vec{locY}  @f$ <br>
   @f$ \frac{\partial \phi'}{\partial \phi} = 1      @f$ <br>
   @f$ \frac{\partial \theta'}{\partial \theta} = 1 @f$ <br>
   @f$ \frac{\partial (q/p)'}{\partial (q/p)} = 1    @f$ <br>
  
   And additionally for the helical case :

   @f$ \frac{\partial \phi'}{\partial x} = \frac{1}{sin\theta'}\cdot(\alpha\cdot Q \cdot(\vec n \cdot u)\codt(\vec t\cdot \vec{locX) @f$<br>
   @f$ \frac{\partial \phi'}{\partial y} = \frac{1}{sin\theta'}\cdot(\alpha\cdot Q \cdot(\vec n \cdot u)\codt(\vec t\cdot \vec{locY) @f$<br>

   @f$ \frac{\partial \theta'}{\partial x} = - \cdot(\alpha\cdot Q \cdot(\vec n \cdot v)\codt(\vec t\cdot \vec{locX) @f$<br>
   @f$ \frac{\partial \theta'}{\partial y} = - \cdot(\alpha\cdot Q \cdot(\vec n \cdot v)\codt(\vec t\cdot \vec{locY) @f$<br>

   @author Andreas.Salzburger@cern.ch 
  
  */

 class JacobianLocalToCurvilinear : public AmgMatrix(5,5) {
    public:
     /**Constructor for straight line track model */
     JacobianLocalToCurvilinear(  const CurvilinearUVT& curvUVT,
                                  const Amg::Vector3D& locX,
                                  const Amg::Vector3D& locY);

     /**Constructor for helical track model */
     JacobianLocalToCurvilinear(const Amg::Vector3D& bfield,
                                double qOp,
                                double sinTheta,
                                const CurvilinearUVT& curvUVT,
                                const Amg::Vector3D& locX,
                                const Amg::Vector3D& locY);
 };

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const JacobianLocalToCurvilinear& jac);
std::ostream& operator << ( std::ostream& sl, const JacobianLocalToCurvilinear& jac); 
 
} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANLOCALTOCURVILINEAR_H
