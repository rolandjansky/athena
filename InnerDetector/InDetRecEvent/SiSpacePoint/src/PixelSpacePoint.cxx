/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPrepRawData/PrepRawData.h"
#include "SiSpacePoint/PixelSpacePoint.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "Identifier/IdentifierHash.h"

#include <memory>
#include <ostream>
#include <sstream>

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
    m_position = clus->detectorElement()->surface().localToGlobal(clus->localPosition());

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
    std::ostringstream os;
    dump(os);
    out << os.str();
    return out;
  }
  
  //-------------------------------------------------------------
  
  std::ostream& PixelSpacePoint::dump( std::ostream& out ) const
  {
    const std::string lf{"\n"};//linefeed
    out << "PixelSpacePoint  contains: " << lf;
    out << "Identifier Hash " << int(this->elementIdList().first) << lf; 
    out << "Global Position:  " << Amg::toString(this->globalPosition(),3) << lf;
    out << "Global Covariance Matrix " <<  Amg::toString(this->globCovariance(),3) << lf;
    out << "Local Parameters " << this->localParameters() << lf;
    out << "Local Covariance " << Amg::toString(this->localCovariance()) << lf; 
    out << "Cluster 1 :" << lf << (*this->clusterList().first) << std::endl;
    
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

