/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TIMEPOINTBETAFIT_H
#define MUON_TIMEPOINTBETAFIT_H

#include <vector>

namespace Muon {

  
  class TimePointBetaFit {
  public:
    /** simple struct holding the input to the fit */
    struct Hit {
      /** constructor, takes the distance of the point to the IP, the measured time with tof subtracted and the error on the measurement */
      Hit(float distance_, float time_, float error_ ) : distance(distance_), time(time_+distance*m_invSpeedOfLight), error(error_), useInFit(true), residual(0.) { weight2 = 1./(error*error); }

      /** data memebers */
      float distance;  /// distance from time reference point use to calculate TOF
      float time;      /// measured time
      float error;     /// error on the time 
      float weight2;   /// 1./error^2
      bool  useInFit;  /// flag to indicate whether to use the hit in the fit
      float residual;  /// residual in fit
    };
    typedef std::vector<Hit> HitVec; 

    /** simple struct holding the fit result */
    struct FitResult {
      /** constructors */
      FitResult() : status(0), beta(0), chi2(0), ndof(1) {}
      FitResult(int status_, float beta_, float chi2_, int ndof_) : status(status_), beta(beta_), chi2(chi2_), ndof(ndof_) {}
      
      /** chi2/ndof, return 0 if ndof == 0 or status == 0 */
      float chi2PerDOF() const { return (status == 0 || ndof == 0) ? 0 : chi2/ndof; }

      /** data members */
      int status;  /// status flag (0 = failed, 1 = ok)
      float beta;  /// fitted beta
      float chi2;  /// fit chi2
      int ndof;    /// number of degrees of freedom
    };

    /// constructor
    TimePointBetaFit() : m_debugLevel(-1) {}

    /// fit beta 
    FitResult fit( HitVec& hits ) const;

    /// fit beta with outlier logic 
    FitResult fitWithOutlierLogic( HitVec& hits ) const;

    /// set debug level
    void setDebugLevel( int level ) { m_debugLevel = level; }
  private:
    static float m_invSpeedOfLight; /// inverse speed of light
    int m_debugLevel;               /// debug level
  };

}

#endif
