/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LocalDirection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_LOCALDIRECTION_H
#define TRKEVENTPRIMITIVES_LOCALDIRECTION_H

#include <iostream>

class MsgStream;

/** standard namespace for Tracking*/
namespace Trk {

/** @class LocalDirection
    @brief represents the three-dimensional global direction with respect
           to a planar surface frame.

   <b>LocalDirection Class</b><br>
   Class to represent the three-dimensional global direction with respect to a
   planar surface frame.

   The two angles @f$ \alpha_{xz} @f$ and @f$ \alpha_{yz} @f$ describe the
  angles of the momentum within the two projections onto the xz plane
  respectively the yz plane of the surface-intrinsic (x,y,z) cartesian frame.

   The transformation between local and global direction coordinates is done as
  follows: May @f$ T @f$ denote the transformation of the local frame to the
  global frame, such that a point
   @f$ \lambda @f$ expressed in the local frame coordinates transforms to a
  global point @f$ \gamma @f$ by the use of @f$ T @f$ like:

   @f$ \gamma = T(\lambda) @f$ and similarily
   @f$ \lambda = T^{-1} (\gamma) @f$.

   1) Local direction to global direction:
   Given @f$ \alpha_{xz} @f$ and @f$ \alpha_{yz} @f$ in the local frame, the
  3-dim direciton in the local frame can be expressed as:

   @f$ \vec d = (d_{z}/tan(\alpha_{xz}), d_{z}/tan(\alpha_{yz}), d_{z}) @f$

   using the constraint that the direciton is normalized:

   @f$ d^{2} = 1 = \frac{d_{z}^{2}}{tan^{2}(\alpha_{xz}} +
  \frac{d_{z}^{2}}{tan^{2}(\alpha_{yz}} + d_{z}^{2} @f$,
   @f$ d_{z} @f$ can be evaluated as:

   @f$ d_{z} =
  \frac{tan(\alpha_{xz})tan(\alpha_{yz})}{\sqrt{tan^{2}(\alpha_{xz})+
                                                            tan^{2}(\alpha_{yz})+
                                                           tan^{2}(\alpha_{xz})tan^{2}(\alpha_{yz})}}
  @f$

  The global direcion @f$ D @f$ is then, using @f$ T @f$:
       @f$ D = T(d) @f$

  2) Global direction to local direcion:
  Constructing the global direction from the local one is straight forwardly
  done, such that @f$ D @f$ is transformed into the local frame by using @f$
  T^{-1} @f$:

    @f$ d = T^{-1}(D) @f$

  and @f$ \alpha_{xz} @f$ respectively @f$ \alpha_{yz} @f$ are then determined
  as:

  @f$ \alpha_{xz} = atan(d_{z}, d_{x}) @f$, respectively @f$ \alpha_{xz} =
  atan(d_{z}, d_{y}) @f$.


   \todo  add image of Jean-Francois

   @author Andreas.Salzburger@cern.ch
  */

class LocalDirection
{

public:
  LocalDirection()=default;
  LocalDirection(double axz, double ayz)
    : m_angleXZ(axz)
    , m_angleYZ(ayz)
  {}

  /** access method for angle of local XZ projection */
  double angleXZ() const;

  /** access method for angle of local YZ projection */
  double angleYZ() const;

protected:
  double m_angleXZ = 0.;
  double m_angleYZ = 0.;

  };

/** inline access method */
inline double LocalDirection::angleXZ() const { return m_angleXZ; }

/** inline access method */
inline double LocalDirection::angleYZ() const { return m_angleYZ; }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const LocalDirection& lomo);
std::ostream& operator << ( std::ostream& sl, const LocalDirection& lomo); 
    
}// end of namespace  

#endif // TRKEVENTPRIMITIVES_LOCALDIRECTION_H
