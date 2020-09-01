/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * -----------------------------------------
 *
 * Author       : Niels van Eldik, Zdenko van Kesteren 
 * Creation Date: 12 May 2004
 ***************************************************************************/

// std
#include <float.h>
// other packages

// this package
#include "MuonCalibEventBase/MdtCalibHitBase.h"

namespace MuonCalib {

MdtCalibHitBase::MdtCalibHitBase()
  : m_id(), m_tdc(0),m_adc(0), m_globalPosition(kNoValue,kNoValue,kNoValue), 
    m_localPosition(kNoValue,kNoValue,kNoValue),
    m_driftTime(kNoValue), m_driftRadius(kNoValue),m_sigma2DriftRadius(kNoValue),
    m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
    m_slewingTime(kNoValue),m_bFieldTime(kNoValue),m_propagationTime(kNoValue),
    m_timeOfFlight(kNoValue),m_distanceToReadout(kNoValue),
    m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue),
    m_wiresagTime(kNoValue), m_temperatureTime(kNoValue),m_backgroundTime(kNoValue), m_bFieldPerp(kNoValue), 
    m_bFieldPara(kNoValue), m_temperature(kNoValue), m_projSag(0), m_tube_t0(kNoValue), 
    m_tube_adccal(kNoValue), m_xtwin(kNoValue), m_sigmaXTwin(kNoValue), m_segmentT0Applied(false), m_tubeRadius(kNoValue)
{}

MdtCalibHitBase::MdtCalibHitBase( int tdc, int adc, const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos )  
  :  m_id(), 
     m_tdc(tdc), m_adc(adc), 
     m_globalPosition(globalPos), m_localPosition(localPos),
     m_driftTime(kNoValue), m_driftRadius(kNoValue), m_sigma2DriftRadius(kNoValue), 
     m_timeFromTrackDistance(kNoValue),m_sigma2TimeFromTrackDistance(kNoValue),
     m_slewingTime(kNoValue),m_bFieldTime(kNoValue),m_propagationTime(kNoValue),
     m_timeOfFlight(kNoValue),m_distanceToReadout(kNoValue),
     m_signedDistanceToTrack(kNoValue),m_sigma2DistanceToTrack(kNoValue),
     m_wiresagTime(kNoValue), m_temperatureTime(kNoValue),m_backgroundTime(kNoValue), m_bFieldPerp(kNoValue), 
     m_bFieldPara(kNoValue), m_temperature(kNoValue), m_projSag(0),m_tube_t0(kNoValue), 
     m_tube_adccal(kNoValue), m_xtwin(kNoValue), m_sigmaXTwin(kNoValue), m_segmentT0Applied(false), m_tubeRadius(kNoValue)
{} 

//static members
const double MdtCalibHitBase::kNoValue = FLT_MAX;

std::ostream& MdtCalibHitBase::dump(std::ostream &stream) const {
  stream << "MdtCalibHitBase: " << std::endl;
  stream << "   Drift time " << driftTime() << " radius " << driftRadius() 
	 << " DistRO " << distanceToReadout() << std::endl;
  stream << "   Radial residual " << radialResidual() << std::endl;
  stream << "   Global position " << globalPosition() 
	 << "   local position " << localPosition() << std::endl;
  return stream;
}

}  // namespace MuonCalib 

std::ostream& operator << (std::ostream &stream, const MuonCalib::MdtCalibHitBase &hit) {
  return hit.dump(stream);
}
