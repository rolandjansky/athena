/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * -----------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 17 October 2005
 * Last Update  : 17 October 2005
 ***************************************************************************/

// this package
#include "MuonCalibEventBase/CscCalibHitBase.h"

namespace MuonCalib {

  CscCalibHitBase::CscCalibHitBase() : m_nStrips(0), m_stripWidth(0.0), m_charge(0), m_error(0.0) 
  {}

  CscCalibHitBase::CscCalibHitBase( int nStrips, double stripWidth, int charge, double error, 
				    const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos ) 
  : m_nStrips(nStrips), m_stripWidth(stripWidth), m_charge(charge), m_error(error), m_globalPosition(globalPos),
    m_localPosition(localPos) 
  {}

  CscCalibHitBase::~CscCalibHitBase() 
  {}

  std::ostream& CscCalibHitBase::dump(std::ostream &stream) const {
    stream << "CscCalibHitBase: " << std::endl;
    stream << "   Number of Strips " << nStrips()
	   << "   Strip width " << stripWidth()
	   << "   Charge " << charge()
	   << "   Error " << error()
	   << "   Global position " << globalPosition() 
	   << "   Local position " << localPosition() << std::endl;
    return stream;
  }

  std::ostream& operator << (std::ostream &stream, const CscCalibHitBase &hit) {
    return hit.dump(stream);
  }

} // namespace MuonCalib


