/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCurvilinearToLocal.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANCURVILINEARTOLOCAL_H
#define TRKEVENTPRIMITIVES_JACOBIANCURVILINEARTOLOCAL_H


// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

 // Trk
 class CurvilinearUVT;


/** @class JacobianCurvilinearToLocal
  
   This class represents the jacobian for transforming from a curvilinear to a local frame
  
   The curvilinear frame is defined as follows:
   Given a direction @f$ \vec t = \frac{\vec p }{|\vec p |} @f$ of a track at a specific point,
   the plane perpenticular to the track may be defined by two vectors @f$ (\vec u, \vec v) @f$,
   denoting @f$ \vec z @f$ as the global z-axis and defining:
   @f$ \vec{u} = \frac {\vec z \times \vec{t}}{|\vec z \times \vec{t}|} @f$  and <br>
   @f$ \vec{v} =  \vec{t} \times \vec{u} @f$, <br>
   the unit vectors @f$ (\vec{curv_{i}}) = (\vec{u}, \vec{v}, \vec{t}) @f$ define at any point of
   the track a right-handed orthogonal coordinate system (the so-called curvilinear frame)<br>
   The local frame on a surface may be represented through two local vectors @f$ \vec{locX} @f$ and @f$ \vec{locY} @f$
   within the plane and one perpenticular vector @f$ \vec{locZ} @f$ pointing along the surface's normal.
  
  
   <b> Detailed Description </b> <br>
  
   The non-zero elements of the jacobian for the transformation between the curvilinear representation @f$ (u, v, \phi', \theta', q/p') @f$
   and the local representations of the track parameters
   @f$ (locX, locY, \phi, \theta, q/p) @f$
   can then be expressed as:<br>

   for the straight line case:

   @f$ \frac{\partial locX}{\partial u} = \frac{\vec{locY} \cdot \vec{v}}{\vec{locZ} \cdot \vec{t}}   @f$ <br>
   @f$ \frac{\partial locX}{\partial v} = - \frac{\vec{locY} \cdot \vec{u}}{\vec{locZ} \cdot \vec{t}}   @f$ <br>
   @f$ \frac{\partial locY}{\partial u} = - \frac{\vec{locX} \cdot \vec{v}}{\vec{locZ} \cdot \vec{t}}   @f$ <br>
   @f$ \frac{\partial locY}{\partial v} = \frac{\vec{locX} \cdot \vec{u}}{\vec{locZ} \cdot \vec{t}}   @f$ <br>
   @f$ \frac{\partial \phi}{\partial \phi'} = 1      @f$ <br>
   @f$ \frac{\partial \theta}{\partial \theta'} = 1 @f$ <br>
   @f$ \frac{\partial (q/p)}{\partial (q/p)'} = 1    @f$ <br>

   and additionally for the helix case:

   @f$ \frac{\partial\phi}{\partial u} =  \frac{\alpha\cdot Q}{\sin\theta} 
                                           \cdot (\vec{u}\cdot\vec{n})\frac{\vec{u}\cdot\vec{locZ}}{\vec{locZ} \cdot \vec{t}}  @f$ <br>
   @f$ \frac{\partial\phi{\partial v} =   \frac{\alpha\cdot Q}{\sin\theta} 
                                           \cdot (\vec{u}\cdot\vec{n})\frac{\vec{v}\cdot\vec{locZ}}{\vec{locZ} \cdot \vec{t}} @f$ <br>
   @f$ \frac{\partial\theta}{\partial u} = -\alpha\cdot Q \cot 
                                           (\vec{v}\cdot\vec{n})\frac{\vec{u}\cdot\vec{locZ}}{\vec{locZ} \cdot \vec{t}} @f$ <br>
   @f$ \frac{\partial\theta}{\partial v} = -\alpha\cdot Q \cot 
                                            (\vec{v}\cdot\vec{n})\frac{\vec{v}\cdot\vec{locZ}}{\vec{locZ} \cdot \vec{t}}@f$ <br>
 
  
   @author Andreas.Salzburger@cern.ch
  
  */

 class JacobianCurvilinearToLocal : public AmgMatrix(5,5) {
    public:
     /**Constructor for straight line track model */
     JacobianCurvilinearToLocal(  const Trk::CurvilinearUVT& curvUVT,
                                  const Amg::Vector3D& locX,
                                  const Amg::Vector3D& locY,
                                  const Amg::Vector3D& locZ
                                  );
     /**Constructor for helix track model
       */
     JacobianCurvilinearToLocal(
       const Amg::Vector3D& bfield,
       double qOp,
       double sinTheta, // only necessary if magnetic field is not along z
       const CurvilinearUVT& curvUVT,
       const Amg::Vector3D& locX,
       const Amg::Vector3D& locY,
       const Amg::Vector3D& locZ);
 };

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const JacobianCurvilinearToLocal& jac);
std::ostream& operator << ( std::ostream& sl, const JacobianCurvilinearToLocal& jac);  
 
} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANCURVILINEARTOLOCAL_H
