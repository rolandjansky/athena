/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPrepRawData/PrepRawData.h"
#include "SiSpacePoint/PixelSpacePoint.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include <memory>

namespace InDet
{
  //-------------------------------------------------------------

  PixelSpacePoint::PixelSpacePoint()
    :
    SpacePoint()
  {}
    
  //-------------------------------------------------------------
  /** Constructor without globCovariance */
  
  PixelSpacePoint::PixelSpacePoint(  IdentifierHash elementId, 
                                     const Trk::PrepRawData* clus ) 
    :
    SpacePoint()
  {
    assert (clus!=nullptr);
    Trk::MeasurementBase::m_localParams = Trk::LocalParameters(clus->localPosition());
    Trk::MeasurementBase::m_localCovariance = clus->localCovariance();

    std::unique_ptr<const Amg::Vector3D> tmpPos{clus->detectorElement()->surface().localToGlobal(clus->localPosition())};
    assert (tmpPos) ;
    m_position = *tmpPos;

    m_clusList = {clus,nullptr};
    m_elemIdList.first = elementId ;
    m_elemIdList.second = 0 ;
    setupGlobalFromLocalCovariance();
  }
  
  //------------ -------------------------------------------------
  
  /** Constructor with globPosition and globCovariance */
  PixelSpacePoint::PixelSpacePoint( IdentifierHash elementId,
                                    const Trk::PrepRawData* clus,
                                    const Amg::Vector3D& globpos,
                                    const Amg::MatrixX& globcov)
    :
    SpacePoint()
  {
    assert (clus!=nullptr);
    m_position         = globpos;
    m_globalCovariance = globcov;
    m_clusList = {clus,nullptr};
    m_elemIdList.first = elementId ;
    m_elemIdList.second = 0 ;
  }
  
  // ------------------------------------------------------------------
  
  // copy constructor
  PixelSpacePoint::PixelSpacePoint(const PixelSpacePoint & PSP) 
    :
    SpacePoint(PSP)
  {}

 
  //-------------------------------------------------------------
  
  MsgStream&    PixelSpacePoint::dump( MsgStream& out ) const
  {
    out << "PixelSpacePoint  contains: " << std::endl;
    out << "Identifier Hash " << int(this->elementIdList().first) << std::endl; 
    out << "Global Position:  " << Amg::toString(this->globalPosition(),3) << std::endl;
    out << "Global Covariance Matrix " <<  Amg::toString(this->globCovariance(),3) << std::endl;
    out << "Local Parameters " << this->localParameters() << std::endl;
    out << "Local Covariance " << Amg::toString(this->localCovariance()) << std::endl; 
    out << "Cluster 1 :" << std::endl << (*this->clusterList().first) << std::endl;
    
    return out;
  }
  
  //-------------------------------------------------------------
  
  std::ostream& PixelSpacePoint::dump( std::ostream& out ) const
  {
    out << "PixelSpacePoint  contains: " << std::endl;
    out << "Identifier Hash " << int(this->elementIdList().first) << std::endl; 
    out << "Global Position:  " << Amg::toString(this->globalPosition(),3) << std::endl;
    out << "Global Covariance Matrix " <<  Amg::toString(this->globCovariance(),3) << std::endl;
    out << "Local Parameters " << this->localParameters() << std::endl;
    out << "Local Covariance " << Amg::toString(this->localCovariance()) << std::endl; 
    out << "Cluster 1 :" << std::endl << (*this->clusterList().first) << std::endl;
    
    return out;
  }
  

  // ------------------------------------------------------------------
  
  //assignment operator
  PixelSpacePoint& PixelSpacePoint::operator=(const PixelSpacePoint& PSP)
  {
    if (&PSP !=this) {
      Trk::SpacePoint::operator=(PSP);
    }
    
    return *this;
  }

  // ------------------------------------------------------------------

} // end of namespace

