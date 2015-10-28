/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONBETACALCULATIONUTILS_H
#define MUON_MUONBETACALCULATIONUTILS_H


namespace Muon {

  class MuonBetaCalculationUtils {
  public:
    
    MuonBetaCalculationUtils(float mass=0.) : m_inverseSpeedOfLight(1./299.792458), m_mass2(mass*mass) {}

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
    float m_inverseSpeedOfLight;
    float m_mass2;
  };

}


#endif
