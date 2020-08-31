// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef XAOD_TRACKPARTICLEXAODHELPERS_H
#define XAOD_TRACKPARTICLEXAODHELPERS_H

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include <cmath>


namespace xAOD {

  namespace TrackingHelpers {
    ///@brief convenience method to calculate the square of a value.
    inline
    double sqr(double a) { return a*a; }

    ///@brief Get the impact parameter significance of a track particle in the r-phi plane.
    ///@return the impact parameter significance of the track particle.
    ///@throw this method may throw an exception in case the uncertainty is zero or the covariance matrix does not exist.
    ///The impact parameter and uncertainty are those stored in the track particle. Will perform input (tp, cov) validity checks. 
    double d0significance(const xAOD::TrackParticle *tp);

    ///@brief Unsafe version of @ref d0significance
    ///@return the impact parameter significance of the track particle or an undefined value
    ///In case the covariance matrix does not exist or the uncertainty is zero, this method will return an undefined value.
    ///In case the track particle does not exist the behaviour is undefined (segmentation fault or random return value).
    ///To validated the inputs use @ref hasValidCovD0, @ref hasValidCovD0andZ0, or @ref checkPVReference.
    inline
    double d0significanceUnsafe(const xAOD::TrackParticle *tp) {
      double d0 = tp->d0();
      // elements in definingParametersCovMatrixVec should be : sigma_d0^2, sigma_d0_z0, sigma_z0^2
      double sigma_d0 = std::sqrt( tp->definingParametersCovMatrixVec()[0] );
      return d0/sigma_d0;
    }

    ///@brief calculate the squared d0 uncertainty component due to the size of the beam spot.
    ///@param track_phi0 the phi angle of the track particle at the perigee wrt. the average beamspot position.
    ///@param beam_sigma_x the width of the beamspot in the x-direction e.g. @ref IBeamCondSvc:: beamSigma(0).
    ///@param beam_sigma_y the width of the beamspot in the y-direction e.g. @ref IBeamCondSvc:: beamSigma(1).
    ///@param beam_sigma_xy the beamspot xy correlation e.g. @ref IBeamCondSvc:: beamSigmaXY.
    ///@return the squared d0 uncertainty component due to the size of the beam spot.
    inline 
    double d0UncertaintyBeamSpot2(double track_phi0,  double beam_sigma_x,double beam_sigma_y, double beam_sigma_xy) {
      double sin_phi = sin(track_phi0);
      double cos_phi = cos(track_phi0);
      double d0_uncert2= sin_phi * ( sin_phi * sqr(beam_sigma_x)
                                    -cos_phi * beam_sigma_xy)
                        +cos_phi * ( cos_phi * sqr(beam_sigma_y)
                                    -sin_phi * beam_sigma_xy);
      return d0_uncert2;
    }


    ///@brief Get the impact parameter significance of a track particle in the r-phi plane where the d0 uncertainty takes the finite beamspot width into account.
    ///@param beam_sigma_x the width of the beamspot in the x-direction e.g. @ref IBeamCondSvc:: beamSigma(0).
    ///@param beam_sigma_y the width of the beamspot in the y-direction e.g. @ref IBeamCondSvc:: beamSigma(1).
    ///@param beam_sigma_xy the beamspot xy correlation e.g. @ref IBeamCondSvc:: beamSigmaXY.
    ///@return the impact parameter significance of the track particle where the d0 uncertainty takes the finite beamspot width into account.
    ///@throw this method may throw an exception in case the uncertainty is zero or the covariance matrix does not exist or the beamspot covariance matrix is invalid.
    ///The impact parameter and uncertainty are those stored in the track particle. Will perform input (tp, cov) validity checks.
    double d0significance(const xAOD::TrackParticle *tp, double beam_sigma_x, double beam_sigma_y, double beam_sigma_xy);

    ///@brief Unsafe version of @ref d0significance with beam spot uncertainty.
    ///@param tp pointer to a track particle.
    ///@param beam_sigma_x the width of the beamspot in the x-direction e.g. @ref IBeamCondSvc:: beamSigma(0).
    ///@param beam_sigma_y the width of the beamspot in the y-direction e.g. @ref IBeamCondSvc:: beamSigma(1).
    ///@param beam_sigma_xy the beamspot xy correlation e.g. @ref IBeamCondSvc:: beamSigmaXY.
    ///@return the impact parameter significance of the track particle where the d0 uncertainty takes the finite beamspot width into account, or an undefined value
    ///In case the covariance matrix does not exist or the uncertainty is zero, this method will return an undefined value.
    ///In case the track particle does not exist the behaviour is undefined (segmentation fault or random return value).
    // In case beam_sigma_xy is larger than the square of beam_sigma_x and the square of beam_sigma_y the result is undefined.
    ///To validated the inputs use @ref hasValidCovD0, @ref hasValidCovD0andZ0, or @ref checkPVReference or @ref checkBeamSpotSigma.
    inline
    double d0significanceUnsafe(const xAOD::TrackParticle *tp, double beam_sigma_x,double beam_sigma_y, double beam_sigma_xy) {
      double d0 = tp->d0();
      // elements in definingParametersCovMatrixVec should be : sigma_d0^2, sigma_d0_z0, sigma_z0^2
      double sigma_d0 = std::sqrt( tp->definingParametersCovMatrixVec()[0] + d0UncertaintyBeamSpot2(tp->phi(),beam_sigma_x, beam_sigma_y, beam_sigma_xy) );
      return d0/sigma_d0;
    }

    ///@brief Get the impact parameter significance of a track particle in the z direction.
    ///@param tp a pointer to a track particle.
    ///@param vx a pointer to a primary vertex with respect to which z0 is expressed or NULL. 
    ///@return the impact parameter significance of the track particle.
    ///@throw this method may throw an exception in case the uncertainty is zero or the covariance matrix does not exist.
    ///The impact parameter and uncertainty are those stored in the track particle. Will perform input (tp, cov) validity checks.
    ///If the given vertex results from a fit which includes the track particle tp, then this z0 impact parameter significance
    ///will be biased. 
    double z0significance(const xAOD::TrackParticle *tp, const xAOD::Vertex *vx=NULL);

    ///@brief Unsafe version of @ref z0significance
    ///@return the impact parameter significance of the track particle or an undefined value
    ///In case the covariance matrix does not exist or the uncertainty is zero or smaller, this method will return an undefined value.
    ///In case the track particle does not exist the behaviour is undefined (segmentation fault or random return value).
    inline
    double z0significanceUnsafe(const xAOD::TrackParticle *tp) {
      double z0 = tp->z0();
      // elements in definingParametersCovMatrixVec should be : sigma_z0^2, sigma_d0_z0, sigma_z0^2
      double sigma_z0 = std::sqrt( tp->definingParametersCovMatrixVec()[2] );
      return z0/sigma_z0;
    }

    ///@brief Unsafe version of @ref z0significance, which uses z0 relative to a given primary vertex.
    ///@param tp a valid pointer to a track particle.
    ///@param vx a valid pointer to a primary vertex with respect to which z0 is expressed.
    ///@return the impact parameter significance of the track particle or an undefined value
    ///In case the covariance matrix does not exist or the uncertainty is zero or smaller, this method will return an undefined value.
    ///In case the track particle or vertex does not exist the behaviour is undefined (segmentation fault or random return value).
    ///To validated the inputs use @ref hasValidCovZ0 or @ref hasValidCovD0andZ0, and @ref checkPVReference.
    ///If the given vertex results from a fit which includes the track particle tp, this z0 impact parameter significance
    ///will be biased. 
    inline
    double z0significanceUnsafe(const xAOD::TrackParticle *tp, const xAOD::Vertex *vx) {
      // use z0 relative to the given primary vertex.
      double z0 = tp->z0() - vx->z();
      // assume that z0 was expressed relative to the associated vertex.
      if (tp->vertex()) {
	z0 += tp->vertex()->z();
      }
      // elements in definingParametersCovMatrixVec should be : sigma_z0^2, sigma_d0_z0, sigma_z0^2
      double sigma_z0 = std::sqrt( tp->definingParametersCovMatrixVec()[2] );
      return z0/sigma_z0;
    }


    ///@brief Check whether the given track particle is valid and has a covariance matrix of the defining parameters.
    ///@return true if the track particle is valid and has a covariance matrix of the defining parameters.
    bool hasValidCov(const xAOD::TrackParticle *tp);

    ///@brief Check whether the given track particle is valid and has a valid d0 uncertainty.
    ///@return true if the track particle is valid, has a covariance matrix, and a valid d0 uncertainty.
    inline
    bool hasValidCovD0(const xAOD::TrackParticle *tp ) {
          if (hasValidCov(tp)) {
 	     if  ( !tp->definingParametersCovMatrixVec().empty()
                   && tp->definingParametersCovMatrixVec()[0]>0.) {
                return true;
             }
          }
          return false;
    }

    ///@brief Check whether the given track particle is valid and has a valid z0 uncertainty.
    ///@return true if the track particle is valid, has a covariance matrix, and a valid z0 uncertainty.
    inline
    bool hasValidCovZ0(const xAOD::TrackParticle *tp) {
          if (hasValidCov(tp)) {
             if  ( ( tp->definingParametersCovMatrixVec().size() > 2 )
                   && ( tp->definingParametersCovMatrixVec()[2] > 0. ) ) {
                return true;
             }
          }
          return false;
    }

    ///@brief Check whether the given track particle is valid and has a valid d0 and z0 uncertainty.
    ///@return true if the track particle is valid, has a covariance matrix, and a valid d0 and z0 uncertainty.
    inline
    bool hasValidCovD0andZ0( const xAOD::TrackParticle *tp) {
          if (hasValidCov(tp)) {
             if  ( ( tp->definingParametersCovMatrixVec().size() > 2 ) 
                   && ( tp->definingParametersCovMatrixVec()[0] > 0. )
                   && ( tp->definingParametersCovMatrixVec()[2] > 0. ) ) {
                return true;
             }
          }
          return false;
    }


    ///@brief test whether the given primary vertex has a significant displacement in r-phi wrt. d0 uncertainty.
    ///@param tp pointer to a track particle.
    ///@param vx pointer to a vertex.
    ///@param max_pv_dxy_sqr maximum deviation of the primary vertex in units of the d0 uncertainty squared.
    ///@return true if the given primary vertex has no significant displacement in r-phi wrt. d0 uncertainty.
    ///The method will also return false if the track particle or vertex are invalid or if the track particle
    ///does not have a valid d0 uncertainty.
    inline
    bool checkPVReference(const xAOD::TrackParticle *tp, const xAOD::Vertex *vx, const double max_pv_dxy_sqr=0.5*0.5) {
        if (hasValidCovD0(tp) && vx) {
	     return std::abs( sqr(vx->x())+ sqr(vx->y())) <  max_pv_dxy_sqr * tp->definingParametersCovMatrixVec()[0];
        }
        return false;
    }

    ///@brief check that the beamspot covariance matrix is valid
    ///@param beam_sigma_x the width of the beamspot in the x-direction e.g. @ref IBeamCondSvc:: beamSigma(0).
    ///@param beam_sigma_y the width of the beamspot in the y-direction e.g. @ref IBeamCondSvc:: beamSigma(1).
    ///@param beam_sigma_xy the beamspot xy correlation e.g. @ref IBeamCondSvc:: beamSigmaXY.
    ///@return true if the beamspot covariance matrix is valid.
    inline
    bool checkBeamSpotSigma(double beam_sigma_x,double beam_sigma_y, double beam_sigma_xy) {
      return sqr(beam_sigma_x)+sqr(beam_sigma_y)>=2*std::abs(beam_sigma_xy);
    }

    ///@brief compute the uncertainty of pt squared.
    ///@param tp a pointer to a track particle.
    ///@throw will throw an exception if the track particle is not valid, no covariance matrix of the defining parameters is set or the covariance matrix has wrong dimension.
    double pTErr2(const xAOD::TrackParticle *tp);

    ///@brief compute the uncertainty of pt.
    ///@param tp a pointer to a track particle.
    ///@throw will throw an exception if the track particle is not valid, no covariance matrix of the defining parameters is set or the covariance matrix has wrong dimension.
    inline
    double pTErr(const xAOD::TrackParticle *tp) {
      return sqrt( pTErr2(tp) );
    }


    ///@brief compute the uncertainty of pt squared.
    ///@param tp a valid pointer to a track particle for which the defining parameters covariance matrix is set and valid
    ///undefined behaviour if tp is invalid or no valid covariance matrix is set for the defining parameters.
    inline
    double pTErr2Unsafe(const xAOD::TrackParticle *tp) {

      //            /   d                       \2    /   d                        \2          d       d
      // pt_err^2 =|  ------   (pt) *sigma_q/p  |  + | --------   pt * sigma_theta |    +    ----- pt ------- pt * sigma_theta_qp
      //            \  d q/p                    /     \ d theta                    /          d qp    d theta

      //                d             / d pt           2   d pt                     \   / d pt                  \2
      //          =   ------   (pt) * | ----- sigma_q/p  + ------- * sigma_theta_qp | + | ------- * sigma_theta |
      //               d q/p          \ d q/p              d theta                  /   \ d theta               /

      //   d                pt
      // ------ pt  =  - -------------
      //  d q/p          fabs(qOverP)

      //   d                pt
      //  ------- pt =   -----------
      //  d theta         tan(theta)

      double pt = tp->pt();
      double diff_qp =  - pt / std::abs(tp->qOverP());

      double diff_theta = pt / tan( tp->theta() );

      // since 1/tan (thata) defined for all floats 0..M_PI, don't have to protect :
      //
      // double diff_theta = ( std::abs(tp->theta()-M_PI_2) > std::numeric_limits<float>::epsilon() * 2
      //                      ? pt / tan( tp->theta() )
      //                      : 0 )

      
      const std::vector<float>&cov= tp->definingParametersCovMatrixVec();
      // elements in definingParametersCovMatrixVec should be : 0: sigma_d0^2,
      //                                                        1: sigma_d0_z0,  sigma_z0^2,
      //                                                        3: sigma_d0_phi, sigma_z0_phi, sigma_phi^2
      //                                                        6: sigma_d0_th,  sigma_z0_th,  sigma_phi_th, sigma_th^2
      //                                                       10: sigma_d0_qp,  sigma_z0_qp,  sigma_phi_qp, sigma_th_qp, sigma_qp^2

      double pt_err2 = diff_qp * (diff_qp * cov[14] + diff_theta * cov[13] ) + sqr(diff_theta) * cov[9];
      return pt_err2;
    }

    ///@brief compute the uncertainty of pt.
    ///@param tp a valid pointer to a track particle for which the defining parameters covariance matrix is set and valid
    ///undefined behaviour if tp is invalid or no valid covariance matrix is set for the defining parameters.
    inline
    double pTErrUnsafe(const xAOD::TrackParticle *tp) {
      return sqrt(pTErr2Unsafe(tp));
    }

    /// return true if the covariance matrix of the defining parameters is set, has enough elements and the q/p is valid
    /// @TODO also check theta ? 
    inline
    bool hasValidCovQoverP(const xAOD::TrackParticle *tp ) {
          if (hasValidCov(tp)) {
 	     if  ( tp->definingParametersCovMatrixVec().size()>=15 && std::abs(tp->qOverP())>0.) {
                return true;
             }
          }
          return false;
    }

  }// TrackParticleHelpers

} // namespace xAOD
  
#endif // XAOD_TRACKPARTICLEXAODHELPERS_H
