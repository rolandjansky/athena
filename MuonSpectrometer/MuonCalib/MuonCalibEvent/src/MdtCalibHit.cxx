/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 12 May 2004
 ***************************************************************************/

// std
#include <float.h>
// other packages

// this package
#include "MuonCalibEvent/MdtCalibHit.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

//static members
const float MdtCalibHit::kNoValue = FLT_MAX;

// MdtCalibHit::hitBase copies data from a MdtCalibHit instance to a MdtCalibHitBase
// MdtCalibHit and MdtCalibHitBase are nearly identical except that 
// MdtCalibHit uses an Athena identifier and MdtCalibHitBase uses Muonfixedid
MuonCalib::MdtCalibHitBase* MdtCalibHit::hitBase(const MuonCalib::IIdToFixedIdTool& rIdToFixed) const {
  MuonCalib::MdtCalibHitBase *hb = new MuonCalib::MdtCalibHitBase(); 

  hb->setIdentifier(rIdToFixed.idToFixedId(m_id));
  hb->setTdc(m_tdc);
  hb->setAdc(m_adc);
  hb->setGlobalPos(m_globalPosition);
  hb->setLocalPos(m_localPosition);
  hb->setDriftTime(m_driftTime);
  if(m_sigma2DriftRadius>0)
    hb->setDriftRadius(m_driftRadius,sqrt(m_sigma2DriftRadius));
  else
    hb->setDriftRadius(m_driftRadius,0.);
  if(m_sigma2DistanceToTrack>0)
    hb->setDistanceToTrack(m_signedDistanceToTrack, sqrt(m_sigma2DistanceToTrack));
  else
    hb->setDistanceToTrack(m_signedDistanceToTrack,0.);
  if(m_sigma2TimeFromTrackDistance>0)
    hb->setTimeFromTrackDistance(m_timeFromTrackDistance, sqrt(m_sigma2TimeFromTrackDistance));
  else
    hb->setTimeFromTrackDistance(m_timeFromTrackDistance,0.);
  hb->setSlewingTime(m_slewingTime);
  hb->setPropagationTime(m_propagationTime);
  hb->setTimeOfFlight(m_timeOfFlight);
  hb->setDistanceToReadout(m_distanceToReadout);
  hb->setBFieldTime(m_lorentzTime);
  
  hb->setWiresagTime(m_wiresagTime);
  hb->setTemperatureTime(m_temperatureTime);
  hb->setBackgroundTime(m_backgroundTime);
  hb->setBFieldPerp(m_bFieldPerp);
  hb->setBFieldPara(m_bFieldPara);
  hb->setTemperature(m_temperature);
  hb->setProjSag(m_projSag);
  
  hb->setTubeT0(m_tube_t0);
  hb->setTubeAdcCal(m_tube_adccal);
  hb->setGlobalPointOfClosestApproach(m_globalPointOfClosestApproach);
  hb->setLocalPointOfClosestApproach(m_localPointOfClosestApproach);
  hb->setLocXtwin(m_xtwin);
  if(m_sigma2XTwin>0)
    hb->setSigmaLocXtwin( sqrt(m_sigma2XTwin));
  else
    hb->setSigmaLocXtwin( 0.);
  /////////////////////////////////////////////////////////////////// 
  // WARNING: these quantities, foreseen for MdtCalibHitBase, are not
  // available from MdtCalibHit
  /////////////////////////////////////////////////////////////////// 
  // hb->setWiresagTime(??);
  // hb->setTemperatureTime(??);
  // hb->setBackgroundTime(??);
    
  return hb;
}  //end MdtCalibHit::hitBase

std::ostream& MdtCalibHit::dump(std::ostream &stream) const {
  stream << "MdtMdtCalibHit: " << std::endl;
  stream << "   Drift time " << driftTime() << " radius " << driftRadius() 
	 << " DistRO " << distanceToReadout() << std::endl;
  stream << "   Radial residual " << radialResidual() << std::endl;
  stream << "   Global position " << globalPosition() 
	 << "   local position " << localPosition() << std::endl;
  return stream;
}

// overload of ostream operator
MsgStream& MdtCalibHit::dump( MsgStream &sl ) const {
  sl << "MdtMdtCalibHit: " << std::endl;
  sl << "   Drift time " << driftTime() << " radius " << driftRadius() 
     << " DistRO " << distanceToReadout() << std::endl;
  sl << "   Radial residual " << radialResidual() << std::endl;
  sl << "   Global position (" << globalPosition().x() 
     << "," << globalPosition().y() << "," << globalPosition().z() << ") "
     << "   local position (" << localPosition().x() 
     << "," << localPosition().y() << "," << localPosition().z() << ") " << std::endl;
  return sl;
}

std::ostream& operator << (std::ostream& stream, const MdtCalibHit &hit) {
  return hit.dump(stream);
}

MsgStream& operator << ( MsgStream &sl, const MdtCalibHit &hit) {
  return hit.dump(sl);
}
