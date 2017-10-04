/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ==============================================================================
 * Drift Time Correction Function for Wire Sag Effects
 * ---------------------------------------------------
 * ==============================================================================
 */

/// Includes

#include <cmath>
#include "MdtCalibData/WireSagCorFunc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

/// Namespace

using namespace MuonCalib;

/// initialization method (might eventually pass coefficients this way)
void WireSagCorFunc::init(const CalibFunc::ParVec& params) {
  MsgStream log(Athena::getMessageSvc(), "WireSagCorFunc");
  m_params = params;
  if (m_params.size() != 0) {
    log << MSG::ERROR << "Wrong number of parameters: " << m_params.size() << endmsg;
  }
}

/// name method
std::string WireSagCorFunc::name() const {
  return std::string("WireSagCorFunc");
}

/// correction function
double WireSagCorFunc::correction(double signedDriftRadius, double effectiveSag) const {
  /// polynomial coefficients

  double coeff[6] = {-4.47741E-3, 1.75541E-2, -1.32913E-2,
                     2.57938E-3, -4.55015E-5, -1.70821E-7};

  /// scale factor needed, as correction function was normalized to 100um wire sag
  double scaleFactor = effectiveSag/100.;

  /// Calculate dT using polynomial coefficients
  double dT = 0;
  for (int i = 0; i < 5; i++) {
    dT = dT + coeff[i]*std::pow(signedDriftRadius,i);
  }

  return scaleFactor*dT;
}
