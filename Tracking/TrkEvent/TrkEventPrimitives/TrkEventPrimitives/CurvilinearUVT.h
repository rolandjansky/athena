/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CurvilinearUVT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_CURVILINEARUVT_H
#define TRKEVENTPRIMITIVES_CURVILINEARUVT_H

// Amg 
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include <cmath>
#include <iostream>

class MsgStream;
namespace Trk {

/** @class CurvilinearUVT

   simple class that constructs the curvilinear vectors curvU and curvV
   from a given momentum direction (must be unit vector).

  The curvilinear frame @f$ (\vec u, \vec v, \vec t) @f$ at each point @f$ m @f$
  is constructed by:

  - @f$ \vec t = \vec{curvT} @f$ ... momentum direction at @f$ m @f$
  - @f$ \vec u = \vec{curvU} = \frac{\vec t \times \vec z}{||\vec t \times \vec
  z||} @f$ ... with @f$ \vec z @f$ the global z-axis
  - @f$ \vec v = \vec{curvV} = \vec t \times \fec u @f$ ... constructed so that
  frame is right-handed

   The picture shows the curvilinear frame for a straight line measurement

   @internal
   @image html CurvilinearFrameStraw.png
   @endinternal

   @author Andreas.Salzburger@cern.ch
  */
class CurvilinearUVT
{

public:

  /** Create from Amg::Vector3D */
  CurvilinearUVT(const Amg::Vector3D& dir);
  CurvilinearUVT() = default;

  /** Access methods */
  const Amg::Vector3D& curvU() const;
  const Amg::Vector3D& curvV() const;
  const Amg::Vector3D& curvT() const;

private:
  Amg::Vector3D m_curvU; //!< curvilinear U axis
  Amg::Vector3D m_curvV; //!< curvilinear V axis
  Amg::Vector3D m_curvT; //!< curvilinear T axis
};

  
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const CurvilinearUVT& uvt);
std::ostream& operator << ( std::ostream& sl, const CurvilinearUVT& uvt); 
}

#include "TrkEventPrimitives/CurvilinearUVT.icc"
#endif // TRKEVENTPRIMITIVES_CURVILINEARUV_H

