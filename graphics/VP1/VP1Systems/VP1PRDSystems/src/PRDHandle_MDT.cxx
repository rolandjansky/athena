/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_MDT.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1DetInfo.h"

#include "MuonReadoutGeometry/MuonStation.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/Surface.h"

#include "GeoPrimitives/GeoPrimitives.h"


//____________________________________________________________________
PRDHandle_MDT::PRDHandle_MDT(PRDCollHandle_MDT*collhandle, const Muon::MdtPrepData*dc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_driftcircle(dc)
{
}

//____________________________________________________________________
void PRDHandle_MDT::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(m_driftcircle->detectorElement()->surface(m_driftcircle->identify()).bounds()));
  assert(ccbo!=0);

  double radius = m_driftcircle->localPosition()[0];
  if (radius<0.15)
    radius = 0.0;//radius is so small it is better to collapse to line
  double halflength(ccbo->halflengthZ());
  PRDCollHandle_MDT::PROJECTION projection(static_cast<PRDCollHandle_MDT*>(collHandle())->projection());
  if (projection!=PRDCollHandle_MDT::NONE) {
    const double epsilon = 0.15;
    if (projection==PRDCollHandle_MDT::TOCHAMBERS) {
      double distanceToFirstEndPlane, distanceToSecondEndPlane;
      Amg::Transform3D transform(getTransform_CLHEP());
      Amg::Vector3D tubedir(0.0,0.0,1.0);
      MuonChamberProjectionHelper::applyTransformToVector( transform, tubedir);
      if (common()->muonChamberProjectionHelper()
        ->getDistancesToMDTChamberWallsAlongLine( parentMuonChamberPV(),
        transform * Amg::Vector3D(0.0,0.0,0.0), tubedir,
      distanceToFirstEndPlane, distanceToSecondEndPlane, radius )) {
        if (fabs(distanceToFirstEndPlane-distanceToSecondEndPlane)>0.1*epsilon)
          std::cout <<"PRDHandle_MDT: Warning tube is not centered in chamber! We should translate the prd object appropriately (todo)."<<std::endl;
        halflength = std::max(distanceToFirstEndPlane,distanceToSecondEndPlane)+epsilon;
      } else {
        std::cout <<"PRDHandle_MDT: Couldn't retrieve data for projections to chamber end planes."<<std::endl;
        halflength += 500.0;
      }
    } else {
      assert(projection==PRDCollHandle_MDT::TOTUBES);
      //To end of tubes.
      halflength += 56.0 + epsilon;
    }
  }

  shape_simple = common()->nodeManager()->getShapeNode_DriftTube( halflength, 0.0 );
  //Detailed shape uses tube (unless negligible radius):
  if (radius==0.0)
    shape_detailed = shape_simple;
  else
    shape_detailed = common()->nodeManager()->getShapeNode_DriftTube( halflength, radius );

  if ( m_driftcircle->rdoList().size() > 1)
    VP1Msg::message("Warning: MDT has additional elements in rdoList: NOT IMPLEMENTED");
}

//____________________________________________________________________
int PRDHandle_MDT::regionIndex()
{
  //Unique for each station.
  const MuonGM::MuonStation* station = m_driftcircle->detectorElement()->parentMuonStation();
  return station->getPhiIndex()-99999*station->getEtaIndex();//hopefully unique.
}

//____________________________________________________________________
QStringList PRDHandle_MDT::clicked() const
{
    
    if (!m_driftcircle)
      return QStringList("Null PRD");
    const MdtIdHelper * idhelper = VP1DetInfo::mdtIDHelper();
    if (!idhelper)
      return QStringList("Null PRD");

    std::ostringstream os;
    Identifier id = m_driftcircle->identify();

    os << "MdtPrepData with Identifier ["<<id.get_compact()  ;
    os << "] = [" << (idhelper->print_to_string(id)).c_str()  ;
    os << "]\n at global position = [" << m_driftcircle->globalPosition()<<"], local position = ["<<m_driftcircle->localPosition()<<"].";
    os <<"\nDrift radius = "<<m_driftcircle->localPosition()[0]<<" CLHEP::mm. ";
    os <<"ADC: "<<m_driftcircle->adc();
    os <<", TDC: "<<m_driftcircle->tdc();
    std::string status("Status: ");
    os<<status;
        
    QStringList l(QString(os.str().c_str())+driftCircleStatus());

    return l;
}

//____________________________________________________________________
QString PRDHandle_MDT::driftCircleStatus() const
{
  QString status;
  switch (m_driftcircle->status()){
    case  Muon::MdtStatusMasked:
      status+="Masked"; 
      break;
    case Muon::MdtStatusDriftTime:
      status+="Drift Time";
      break;
    case Muon::MdtStatusBeforeSpectrum:
      status+="Before Spectrum";
      break;    
    case Muon::MdtStatusAfterSpectrum:
      status+="After Spectrum";
      break;
    default:
      status+=" Unknown with value = "+m_driftcircle->status();
  }
  return status;
}

