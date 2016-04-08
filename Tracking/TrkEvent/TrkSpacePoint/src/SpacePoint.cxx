/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePoint.cxx Implementation file for class SpacePoint
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
// Version 2.0   /12/2005 Martin Siebel
///////////////////////////////////////////////////////////////////

#include <new>
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include <math.h>

namespace Trk
{
  
  unsigned int SpacePoint::s_numberOfInstantiations=0;

  
  // Destructor:
  SpacePoint::~SpacePoint()
  {
    delete m_clusList;
#ifndef NDEBUG
    s_numberOfInstantiations--; // delete SpacePoint, so decrement total count
#endif
  }
  
  // ------------------------------------------------------------------
  
  // Default constructor
  SpacePoint::SpacePoint()
    :
    m_clusList(0),
    m_elemIdList(0,0),
    m_position(),
    m_globalCovariance()
  {
#ifndef NDEBUG
    s_numberOfInstantiations++; // new SpacePoint, so increment total count
#endif
  } 
  
  // ------------------------------------------------------------------
  
  // copy constructor
  SpacePoint::SpacePoint(const SpacePoint & SP) :
    Trk::MeasurementBase(SP)
  {
    m_elemIdList = SP.m_elemIdList;
    m_position = SP.m_position;
    if (SP.m_clusList){
      m_clusList = new std::pair<const PrepRawData*, const PrepRawData*>(*SP.m_clusList);
    } else {
      m_clusList=nullptr;
    }    
    m_globalCovariance = SP.m_globalCovariance; 
#ifndef NDEBUG
    s_numberOfInstantiations++; // new SpacePoint, so increment total count
#endif
  }
  
  // ------------------------------------------------------------------
  
  //assignment operator
  SpacePoint& SpacePoint::operator=(const SpacePoint& SP)
  {
    if (&SP !=this) 
    {
			Trk::MeasurementBase::operator=(SP);
			delete m_clusList;
			m_elemIdList = SP.m_elemIdList;
			m_position = SP.m_position;
			if (SP.m_clusList){
			  m_clusList = new std::pair<const PrepRawData*, const PrepRawData*>(*SP.m_clusList);
			}  else {
        m_clusList=nullptr;
      }  
			m_globalCovariance = SP.m_globalCovariance; 
    }
    return *this;
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
      assert(m_clusList->first->detectorElement()); 
      return m_clusList->first->detectorElement()->surface(); 
    }

  // ------------------------------------------------------------------
  
  unsigned int SpacePoint::numberOfInstantiations()
  {
    return s_numberOfInstantiations;
  }


} // end of namespace


