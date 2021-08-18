///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4Helpers.h
// Header file for P4 helper functions
// Author: karsten.koeneke@cern.ch
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_XAODP4HELPERS_H
#define FOURMOMUTILS_XAODP4HELPERS_H

/**
   P4Helpers provides static helper functions for kinematic calculation
on objects deriving from xAOD::IParticle (and xAOD::MissingET)

   @author Karsten Koeneke karsten.koeneke@cern.ch
 */


// STL includes
#include <cmath>

// CxxUtils includes
#include "CxxUtils/fpcompare.h" // for fpcompare::less and greater

// EDM includes
#include "xAODBase/IParticle.h"
#include "xAODMissingET/MissingET.h"


namespace xAOD
{
  namespace P4Helpers
  {
    /// Computes efficiently @f$ \Delta{y} @f$
    inline
    double deltaRapidity( const xAOD::IParticle& p1, const xAOD::IParticle& p2 )
    {
      return p1.rapidity() - p2.rapidity();
    }

    /// Computes efficiently @f$ \Delta{y} @f$, pointer args.
    inline
    double deltaRapidity( const xAOD::IParticle * const p1, const xAOD::IParticle * const p2 )
    {
      return xAOD::P4Helpers::deltaRapidity(*p1, *p2);
    }


    /// Computes efficiently @f$ \Delta{\eta} @f$
    inline
    double deltaEta( const xAOD::IParticle& p1, const xAOD::IParticle& p2 )
    {
      return p1.eta() - p2.eta();
    }

    /// Computes efficiently @f$ \Delta{\eta} @f$, pointer args.
    inline
    double deltaEta( const xAOD::IParticle * const p1, const xAOD::IParticle * const p2 )
    {
      return xAOD::P4Helpers::deltaEta(*p1, *p2);
    }


    /** delta Phi in range [-pi,pi[ */
    inline
    double deltaPhi( double phiA, double phiB )
    {
      return  -remainder( -phiA + phiB, 2*M_PI );
    }

    /** delta Phi in range [-pi,pi[ from one xAOD::IParticle reference */
    inline
    double deltaPhi( const xAOD::IParticle& p4, const double phi )
    {
      return xAOD::P4Helpers::deltaPhi( p4.phi(), phi );
    }

    /** delta Phi in range [-pi,pi[ from two xAOD::IParticle references */
    inline
    double deltaPhi( const xAOD::IParticle & pA, const xAOD::IParticle & pB )
    {
      return xAOD::P4Helpers::deltaPhi( pA.phi(), pB.phi() );
    }

    /** delta Phi in range [-pi,pi[ from two xAOD::IParticle pointers */
    inline
    double deltaPhi( const xAOD::IParticle * const pA, const xAOD::IParticle * const pB )
    { return xAOD::P4Helpers::deltaPhi( pA->phi(), pB->phi() ); }



    /** delta Phi in range [-pi,pi[ from an xAOD::IParticle references and an xAOD::MissingEt reference*/
    inline
    double deltaPhi( const xAOD::IParticle & pA, const xAOD::MissingET & met )
    {
      return xAOD::P4Helpers::deltaPhi( pA.phi(), met.phi() );
    }

    /** delta Phi in range [-pi,pi[ from an xAOD::IParticle pointers and an xAOD::MissingEt poiter*/
    inline
    double deltaPhi( const xAOD::IParticle * const pA, const xAOD::MissingET * const met )
    { return xAOD::P4Helpers::deltaPhi( pA->phi(), met->phi() ); }



    /// @f$ \Delta{R}^2 @f$ from bare rapidity,phi
    inline
    double deltaR2( double rapidity1, double phi1, double rapidity2, double phi2 )
    {
      const double dPhi = xAOD::P4Helpers::deltaPhi( phi1, phi2 );
      const double dRapidity = rapidity1-rapidity2;
      return dRapidity*dRapidity + dPhi*dPhi;
    }

    /// @f$ \Delta{R}^2 @f$ from 1 @c xAOD::IParticle
    inline
    double deltaR2( const xAOD::IParticle& p4, double rapidity, double phi, bool useRapidity=true )
    {
      if (useRapidity) {
        return xAOD::P4Helpers::deltaR2(p4.rapidity(),p4.phi(),rapidity,phi);
      }
      else {
        return xAOD::P4Helpers::deltaR2(p4.eta(),p4.phi(),rapidity,phi);
      }
    }

    /** delta R^2 from two xAOD::IParticle reference */
    inline
    double deltaR2( const xAOD::IParticle& pA, const xAOD::IParticle& pB, bool useRapidity=true )
    {
      if (useRapidity) {
        return xAOD::P4Helpers::deltaR2(pA.rapidity(),pA.phi(),pB.rapidity(),pB.phi());
      }
      else {
        return xAOD::P4Helpers::deltaR2(pA.eta(),pA.phi(),pB.eta(),pB.phi());
      }
    }

    /** delta R^2 from two xAOD::IParticle pointers */
    inline
    double deltaR2( const xAOD::IParticle * const pA, const xAOD::IParticle * const pB, bool useRapidity=true )
    { return xAOD::P4Helpers::deltaR2( *pA, *pB, useRapidity ); }


    /// @f$ \Delta{R} @f$ from bare bare rapidity,phi
    inline
    double deltaR( double rapidity1, double phi1, double rapidity2, double phi2 )
    { return std::sqrt( xAOD::P4Helpers::deltaR2( rapidity1, phi1, rapidity2, phi2 ) ); }

    /// @f$ \Delta{R} @f$ from 1 @c xAOD::IParticle
    inline
    double deltaR( const xAOD::IParticle& p4, double rapidity, double phi, bool useRapidity=true )
    { return std::sqrt( xAOD::P4Helpers::deltaR2( p4, rapidity, phi, useRapidity ) ); }

    /** delta R from two xAOD::IParticle reference */
    inline
    double deltaR( const xAOD::IParticle& pA, const xAOD::IParticle& pB, bool useRapidity=true )
    { return std::sqrt( xAOD::P4Helpers::deltaR2( pA, pB, useRapidity ) ); }

    /** delta R from two xAOD::IParticle pointers */
    inline
    double deltaR( const xAOD::IParticle * const pA, const xAOD::IParticle * const pB, bool useRapidity=true )
    { return std::sqrt( xAOD::P4Helpers::deltaR2( *pA, *pB, useRapidity ) ); }



    /// Check if 2 @c xAOD::IParticle are in a @f$ \Delta{R} @f$ cone
    /// @param dR [in] @f$ \Delta{R} @f$
    /// @return true if they are
    inline
    bool isInDeltaR( const xAOD::IParticle& p1, const xAOD::IParticle& p2,
                     double dR, bool useRapidity=true )
    {
      const double dPhi = std::abs( xAOD::P4Helpers::deltaPhi(p1,p2) ); // in [-pi,pi)
      if ( CxxUtils::fpcompare::greater(dPhi,dR) ) return false;        // <==
      if (useRapidity) {
        const double dRapidity = std::abs( xAOD::P4Helpers::deltaRapidity(p1,p2) );
        if ( CxxUtils::fpcompare::greater(dRapidity,dR) ) return false;        // <==
        const double deltaR2 = dRapidity*dRapidity + dPhi*dPhi;
        if ( CxxUtils::fpcompare::greater(deltaR2,dR*dR) ) return false;  // <==
        return true;
      }
      else {
        const double dEta = std::abs( xAOD::P4Helpers::deltaEta(p1,p2) );
        if ( CxxUtils::fpcompare::greater(dEta,dR) ) return false;        // <==
        const double deltaR2 = dEta*dEta + dPhi*dPhi;
        if ( CxxUtils::fpcompare::greater(deltaR2,dR*dR) ) return false;  // <==
        return true;
      }
    }



    /// Get the delta-|@f$ \vec{p} @f$| between two particles
    inline
    double deltaAbsP( const xAOD::IParticle& pA, const xAOD::IParticle& pB )
    {
      const double absPA = std::abs( pA.p4().P() );
      const double absPB = std::abs( pB.p4().P() );
      return absPA - absPB;
    }

    /// Get the delta-|@f$ \vec{p} @f$| between two particles using pointers
    inline
    double deltaAbsP( const xAOD::IParticle * const pA, const xAOD::IParticle * const pB )
    { return xAOD::P4Helpers::deltaAbsP( *pA, *pB ); }


  } //> namespace P4Helpers

} // End: namespace xAOD


#endif // FOURMOMUTILS_XAODP4HELPERS_H
