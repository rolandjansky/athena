/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPrepRawData/PrepRawData.h"
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include <memory>
#include <ostream>
#include <sstream>

namespace InDet
{
  //-------------------------------------------------------------

  SCT_SpacePoint::SCT_SpacePoint()
    :
    Trk::SpacePoint()
  {
  }

  //-------------------------------------------------------------

  SCT_SpacePoint::SCT_SpacePoint(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
				 const Amg::Vector3D& position,
				 const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*> &clusList) 
    :
    Trk::SpacePoint()
  {
    setup(elementIdList,position,clusList);
    setupLocalCovarianceSCT();
    setupGlobalFromLocalCovariance();
  }
  
  //-------------------------------------------------------------
  
  void SCT_SpacePoint::setup(const std::pair<IdentifierHash, IdentifierHash>& elementIdList,  		    
			     const Amg::Vector3D& position,
			     const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>& clusList)
  {
    m_clusList = clusList ;
    m_position = position ;
    m_elemIdList.first = elementIdList.first ;
    m_elemIdList.second = elementIdList.second ;
    assert( (clusList.first!=0) && (clusList.second!=0) );
    assert(clusList.first->detectorElement()) ;
    std::optional<Amg::Vector2D> locpos{
      clusList.first->detectorElement()->surface().globalToLocal(position)
    };
    assert(locpos);
    Trk::MeasurementBase::m_localParams = Trk::LocalParameters(*locpos);
  }

  //-------------------------------------------------------------

  SCT_SpacePoint::SCT_SpacePoint(const SCT_SpacePoint &o)
    :
    Trk::SpacePoint(o)
  {
  }

  //-------------------------------------------------------------

  //---------------------------------------------------------------

  void SCT_SpacePoint::setupLocalCovarianceSCT()
  {

    /* For performance reason only, the error is assumed constant.
	numbers are taken from 
	Trigger/TrigTools/TrigOfflineSpacePointTool/OfflineSpacePointProviderTool */
    
    double deltaY = 0.0004;  // roughly pitch of SCT (80 mu) / sqrt(12)

    Amg::MatrixX cov(2,2);
    cov<<
      1.*deltaY,   25.*deltaY,
      25.*deltaY, 1600.*deltaY;
    
    Trk::MeasurementBase::m_localCovariance = cov;
  }
  
//-------------------------------------------------------------

MsgStream&    SCT_SpacePoint::dump( MsgStream& out ) const
{
  std::ostringstream os;
  dump(os);
  out << os.str();
  return out;
}

//-------------------------------------------------------------


std::ostream& SCT_SpacePoint::dump( std::ostream& out ) const
{
  const std::string lf{"\n"};//linefeed
  out << "SCT_SpacePoint  contains: " << lf;
  out << "Identifier Hashes ( " << int(this->elementIdList().first) << " , " ;
  out <<  int(this->elementIdList().second) << " ) " << lf ;
  out << "Global Position:  " << Amg::toString(this->globalPosition(),3) << lf;
  out << "Global Covariance Matrix " <<  Amg::toString(this->globCovariance(),3) << lf;
  out << "Local Parameters " << this->localParameters() << lf;
  out << "Local Covariance " << Amg::toString(this->localCovariance()) << lf; 
  out << "Cluster 1 :" << lf << (*this->clusterList().first) << lf;
  out << "Cluster 2 :" << lf << (*this->clusterList().second) << std::endl;
 
  return out;
}

  // ------------------------------------------------------------------
  
  //assignment operator
  SCT_SpacePoint& SCT_SpacePoint::operator=(const SCT_SpacePoint& o)
  {
    if (&o !=this) 
      {
	Trk::SpacePoint::operator=(o);
      }
    return *this;
  }

} // end of namespace InDet

