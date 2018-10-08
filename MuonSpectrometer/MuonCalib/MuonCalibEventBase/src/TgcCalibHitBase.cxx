/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Input Event Data Model
 * -----------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 17 October 2005
 ***************************************************************************/

// this package
#include "MuonCalibEventBase/TgcCalibHitBase.h"

namespace MuonCalib {

  TgcCalibHitBase::TgcCalibHitBase() :  m_nStrips(0), m_stripWidth(0.), m_stripLength(0.), m_error(0.) 
  {}

  TgcCalibHitBase::TgcCalibHitBase( int nStrips, double stripWidth, double error, 
				    const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos ) 
    :  m_nStrips(nStrips), m_stripWidth(stripWidth), m_stripLength(0.), m_error(error), m_globalPosition(globalPos), m_localPosition(localPos) 
  {}

  TgcCalibHitBase::~TgcCalibHitBase() 
  {}

  std::ostream& TgcCalibHitBase::dump(std::ostream &stream) const {
    stream << "TgcCalibHitBase: " << std::endl;
    stream << "   Number of Strips " << nStrips()
	   << "   Strip width " << stripWidth() 
	   << "   Strip length " << stripLength()
	   << "   Error " << error()
	   << "   Global position " << globalPosition() 
	   << "   local position " << localPosition() << std::endl;
    return stream;
  }

  // All the other ostream operators are outside namespace MuonCalib, does it matter?
  std::ostream& operator << (std::ostream &stream, const TgcCalibHitBase &hit) {
    return hit.dump(stream);
  }

} // namespace MuonCalib
  
