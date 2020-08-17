/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePoint.cxx Implementation file for class SpacePoint
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
// Version 2.0   /12/2005 Martin Siebel
///////////////////////////////////////////////////////////////////

#include "TrkSpacePoint/SpacePoint.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"
#include <cmath>
#include <new>

namespace Trk
{
  
  
  // ------------------------------------------------------------------
  
  // Default constructor
  SpacePoint::SpacePoint()
    :
    m_clusList(nullptr, nullptr),
    m_elemIdList(0,0),
    m_position(),
    m_globalCovariance()
  {
  } 
  
  // ------------------------------------------------------------------
  
  
  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
  MsgStream& operator << ( MsgStream& sl, const Trk::SpacePoint& spacePoint)
  { 
    return spacePoint.dump(sl);
  }
  
  // ------------------------------------------------------------------
  
  std::ostream& operator << ( std::ostream& sl, const Trk::SpacePoint& spacePoint)
  {
    return spacePoint.dump(sl);
  }

  /** set up the global covariance matrix by rotating the local one */
  
   void SpacePoint::setupGlobalFromLocalCovariance()
  {
    const Amg::MatrixX& lc = this->localCovariance();

    Amg::MatrixX cov(3,3);

    cov<<
      lc(0,0),lc(0,1),0.,
      lc(1,0),lc(1,1),0.,
      0.     ,0.     ,0.;

    const Amg::RotationMatrix3D& R = associatedSurface().transform().rotation();
    m_globalCovariance = R*cov*R.transpose();
    
  }

  const Surface& SpacePoint::associatedSurface() const
    { 
      assert(m_clusList.first->detectorElement());
      return m_clusList.first->detectorElement()->surface();
    }

} // end of namespace


