/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughHit.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "TrkDetElementBase/TrkDetElementBase.h"  
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

MuonHoughHit::MuonHoughHit(const Trk::PrepRawData* prd):m_orig_weight(1.)
{
  const Trk::TrkDetElementBase* element = prd->detectorElement();
  const Identifier identifier  = prd->identify();

  const Amg::Vector3D globalpos = element->surface(identifier).Trk::Surface::localToGlobal(prd->localPosition());
  
  m_hitx = globalpos.x(); // does this what i expect?
  m_hity = globalpos.y();
  m_hitz = globalpos.z();


  double hitr2 = m_hitx*m_hitx+m_hity*m_hity;
  m_radius = std::sqrt(hitr2);
  m_abs = std::sqrt(hitr2+m_hitz*m_hitz);
  m_theta = std::atan2(m_radius,m_hitz); 
  m_phi = std::atan2(m_hity,m_hitx);
  m_barrel = (std::abs(m_radius/m_hitz)<MuonHough::tan_barrel) ? 0 : 1;
  
  m_phi_sector = calcPhiSector();
  m_magnetic_trackratio = calcMagneticTrackRatio();
  
  m_prd = prd;

  m_weight = 1;
  m_probability = 1;
  m_associated = false;
  m_id = -1;

  m_detector_id = MuonHough::MDT;

  m_measures_phi = false;

}

MuonHoughHit::MuonHoughHit(double x, double y, double z, bool measures_phi, MuonHough::DetectorTechnology detector_id, double prob, double weight, const Trk::PrepRawData* prd, int id):m_orig_weight(weight)
{
  m_detector_id=detector_id;
  m_measures_phi=measures_phi;
  m_hitx=x;
  m_hity=y;
  m_hitz=z;
  m_probability = prob;
  m_weight = weight;
  m_prd = prd;
  m_associated = false;
  m_id = id;

  double hitr2 = m_hitx*m_hitx+m_hity*m_hity;
  m_radius = std::sqrt(hitr2);
  m_abs = std::sqrt(hitr2+m_hitz*m_hitz);
  m_theta = std::atan2(m_radius,m_hitz); 
  m_phi = std::atan2(m_hity,m_hitx);
  m_barrel = (std::abs(m_radius/m_hitz)<MuonHough::tan_barrel) ? 0 : 1;
  m_phi_sector = calcPhiSector();
  m_magnetic_trackratio = calcMagneticTrackRatio();
}

std::string MuonHoughHit::getWhichDetector()const
{
  MsgStream log(Athena::getMessageSvc(),"MuonHoughHit::getWhichDetector");
  std::string detector_name;
  switch (m_detector_id)
    {
    case MuonHough::MDT:
      detector_name="MDT";
      break;
    case MuonHough::CSC:
      detector_name="CSC";
      break;
    case MuonHough::RPC:
      detector_name="RPC";
      break;
    case MuonHough::TGC:
      detector_name="TGC";
      break;
    default: if(log.level()<=MSG::WARNING) log << MSG::WARNING << "MuonHoughHit:: no valid detector_id" << endmsg;
    }
  return detector_name;
}

std::vector <double> MuonHoughHit::getPosition()const
{
  std::vector <double> position(3);
  position[0]=m_hitx;
  position[1]=m_hity;
  position[2]=m_hitz;

  return position;
}

int MuonHoughHit::calcPhiSector()const
{
  double phi = m_phi; // [-Pi , Pi]
  phi += MuonHough::half_phisector;

  if(phi<0) phi += MuonHough::two_Pi;  // [ 0 , 2*Pi ]
  int sectorhit = static_cast<int> (phi / (2*MuonHough::half_phisector)) ; // 0..15
  if (sectorhit == MuonHough::phisectors) sectorhit += -1; // could happen in rare cases
  return sectorhit;
}

double MuonHoughHit::calcMagneticTrackRatio()const
{
  // for formulas see muonhoughmathutils, only here for cpu improvements
  double magnetic_trk_ratio = 0.;

  if (m_barrel) {
    
    // Barrel hypothesis

    if ( m_radius >= MuonHough::radius_cylinder) {
      double diff_b = m_radius-MuonHough::radius_cylinder;
      magnetic_trk_ratio = diff_b*diff_b/m_abs;
    }
  }
  else {
    int sign = 1;
    if (m_hitz < 0) sign = -1;
    // Endcap hypothesis
    if (std::abs(m_hitz) < MuonHough::z_cylinder) {magnetic_trk_ratio = 0.;}
    else if (std::abs(m_hitz) < MuonHough::z_end) {
      // Inside Toroid
      double diff_e = m_hitz-sign*MuonHough::z_cylinder; 
      magnetic_trk_ratio = diff_e*diff_e/m_abs;
      
    } else {
      // Outside Toroid
      magnetic_trk_ratio = (-MuonHough::z_magnetic_range_squared + sign*2*m_hitz*MuonHough::z_magnetic_range)/m_abs;
    }
  }
  
  return magnetic_trk_ratio;
}
