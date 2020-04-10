/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONBETACALCULATIONUTILS_H
#define MUON_MUONBETACALCULATIONUTILS_H

#include "GaudiKernel/PhysicalConstants.h"

namespace Muon {

  class MuonBetaCalculationUtils {
  public:
    
    MuonBetaCalculationUtils(float mass=0.) : m_mass2(mass*mass) {}

    float calculateBetaTrack( float p ) {
      return fabs(p)/sqrt(p*p+m_mass2);
    }

    float calculateBeta( float time, float dist ) {
      return dist*m_inverseSpeedOfLight/time;
    }

    float calculateBetaError( float time, float error, float dist ) {
      return dist*m_inverseSpeedOfLight/(time*time)*error;
    }

    float calculateInverseBeta( float time, float dist ) {
      return time/(dist*m_inverseSpeedOfLight);
    }

    float calculateInverseBetaError( float error, float dist ) {
      return error/(dist*m_inverseSpeedOfLight);
    }

    float calculateTof( float beta, float dist ){
      return dist*m_inverseSpeedOfLight/beta;
    }

  private:
    const double m_inverseSpeedOfLight = 1 / Gaudi::Units::c_light; // need 1/299.792458 inside calculateTof()/calculateBeta()
    float m_mass2;
  };

}


#endif