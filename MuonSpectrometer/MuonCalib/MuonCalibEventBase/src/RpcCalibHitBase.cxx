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
#include "MuonCalibEventBase/RpcCalibHitBase.h"

namespace MuonCalib{ 

  RpcCalibHitBase::RpcCalibHitBase() : m_nStrips(0), m_stripWidth(0.0), m_stripLength(0.), m_time(0.0), 
				       m_error(0.0), m_distanceToRO(0.) {
  }

  RpcCalibHitBase::RpcCalibHitBase( int nStrips, double stripWidth, double time, double error, 
				    const Amg::Vector3D &globalPos, const Amg::Vector3D &localPos ) 
    : m_nStrips(nStrips), m_stripWidth(stripWidth), m_stripLength(0.), m_time(time), 
      m_error(error), m_globalPosition(globalPos), m_localPosition(localPos), m_distanceToRO(0.) {
  }

  RpcCalibHitBase::RpcCalibHitBase( int nStrips, double stripWidth, double stripLength, double time, 
				    double error, const Amg::Vector3D &globalPos , const Amg::Vector3D &localPos ) 
    : m_nStrips(nStrips), m_stripWidth(stripWidth), m_stripLength(stripLength),  m_time(time), 
      m_error(error), m_globalPosition(globalPos), m_localPosition(localPos), m_distanceToRO(0.) {
  }

  RpcCalibHitBase::~RpcCalibHitBase() 
  {}

  std::ostream& RpcCalibHitBase::dump(std::ostream &stream) const {
    stream << "RpcCalibHitBase: " << std::endl;
    stream << "   Number of Strips " << nStrips()
	   << "   Strip width " << stripWidth()
	   << "   Strip length " << stripLength()
	   << "   Time " << time()
	   << "   Error " << error()
	   << "   Global position " << globalPosition() 
	   << "   local position " << localPosition() 
	   << "   distance to readout " << distanceToRO() << std::endl;
    return stream;
  }

  std::ostream& operator << (std::ostream &stream, const RpcCalibHitBase &hit) {
    return hit.dump(stream);
  }

} // namespace MuonCalib



