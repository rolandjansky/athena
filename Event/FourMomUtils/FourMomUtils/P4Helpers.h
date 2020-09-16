///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4Helpers.h
// Header file for P4 helper functions
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_P4HELPERS_H
#define FOURMOMUTILS_P4HELPERS_H

/**
   P4Helpers provides static helper functions for kinematic calculation
on objects deriving from I4Momentum.

   @author David Rousseau rousseau@lal.in2p3.fr
   @author Sebastien Binet binet@cern.ch
   @author Tadashi Maeno
 */

#include <cmath>

namespace P4Helpers
{
  /** delta Phi in range [-pi,pi[ */
  inline
  double deltaPhi( double phiA, double phiB )
  {
    return  -remainder( -phiA + phiB, 2*M_PI );
  }
}

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS


// STL includes
#include <cmath>
#include <algorithm> // for std::sort
#include <limits>    // for std::numeric_limits

// CxxUtils includes
#include "CxxUtils/fpcompare.h" // for fpcompare::less

// EventKernel includes
#include "EventKernel/I4Momentum.h"

namespace P4Helpers
{
  /// Computes efficiently @f$ \Delta{\eta} @f$
  inline
  double deltaEta( const I4Momentum& p1, const I4Momentum& p2 )
  {
    double dEta(999);
    if ( p1.kind() == I4Momentum::P4PXPYPZE && p2.kind() == I4Momentum::P4PXPYPZE )
      {
        const double mom1 = p1.p(); const double pz1 = p1.pz();
        const double mom2 = p2.p(); const double pz2 = p2.pz();
        if ( ( mom1 + pz1 ) * ( mom2 - pz2 ) > 0 )
          dEta = -0.5 * std::log(   ( mom1 - pz1 ) / ( mom1 + pz1 )
                                    * ( mom2 + pz2 ) / ( mom2 - pz2 ) );
      }
    else if ( p1.kind() == I4Momentum::P4PXPYPZE )
      {
        const double mom1 = p1.p(); const double pz1 = p1.pz();
        if ( mom1 + pz1 > 0 )
          dEta = -0.5 * std::log(   ( mom1 - pz1 ) / ( mom1 + pz1 ) ) - p2.eta();
      }
    else if ( p2.kind() == I4Momentum::P4PXPYPZE )
      {
        const double mom2 = p2.p(); const double pz2 = p2.pz();
        if ( mom2 - pz2 > 0 )
          dEta = p1.eta() - 0.5 * std::log(  ( mom2 + pz2 ) / ( mom2 - pz2 ) );
      }
    else
      {
        dEta = p1.eta() - p2.eta();
      }
    return dEta;
  }

  /// Computes efficiently @f$ \Delta{\eta}^2 @f$
  inline
  double deltaEtaSq( const I4Momentum& p1, const I4Momentum& p2 )
  {
    double dEtaSq(999);
    if ( p1.kind() == I4Momentum::P4PXPYPZE && p2.kind() == I4Momentum::P4PXPYPZE )
      {
        const double mom1 = p1.p(); const double pz1 = p1.pz();
        const double mom2 = p2.p(); const double pz2 = p2.pz();
        if ( ( mom1 + pz1 ) * ( mom2 - pz2 ) > 0 )
          {
            dEtaSq = std::log( ( mom1 - pz1 ) / ( mom1 + pz1 )
                               * ( mom2 + pz2 ) / ( mom2 - pz2 ) );
            dEtaSq = 0.25 * dEtaSq * dEtaSq;
          }
      }
    else if ( p1.kind() == I4Momentum::P4PXPYPZE )
      {
        const double mom1 = p1.p(); const double pz1 = p1.pz();
        if ( mom1 + pz1 > 0 )
          {
            dEtaSq = - 0.5 * std::log(   ( mom1 - pz1 ) / ( mom1 + pz1 ) ) - p2.eta();
            dEtaSq = dEtaSq * dEtaSq;
          }
      }
    else if ( p2.kind() == I4Momentum::P4PXPYPZE )
      {
        const double mom2 = p2.p(); const double pz2 = p2.pz();
        if ( mom2 - pz2 > 0 )
          {
            dEtaSq = p1.eta() - 0.5 * std::log(  ( mom2 + pz2 ) / ( mom2 - pz2 ) );
            dEtaSq = dEtaSq * dEtaSq;
          }
      }
    else
      {
        dEtaSq = p1.eta() - p2.eta();
        dEtaSq = dEtaSq * dEtaSq;
      }
    return dEtaSq;
  }

  /// Computes efficiently @f$ \Delta{\eta} @f$, pointer args.
  inline
  double deltaEta( const I4Momentum * const p1, const I4Momentum * const p2 )
  {
    return deltaEta(*p1, *p2);
  }

  /** delta Phi in range [-pi,pi[ from one I4momentum reference */
  inline
  double deltaPhi( const I4Momentum& p4, const double phi )
  {
    return P4Helpers::deltaPhi( p4.phi(), phi );
  }

  /** delta Phi in range [-pi,pi[ from two I4momentum references */
  inline
  double deltaPhi( const I4Momentum & pA, const I4Momentum & pB )
  {
    double dPhi(999);
    if ( pA.kind() == I4Momentum::P4PXPYPZE && pB.kind() == I4Momentum::P4PXPYPZE )
      {
        double xp = pA.px() * pB.px() ;
        double xyp1 = 0;
        if ( xp != 0 )
          {
            xyp1 = 1.0 + pA.py() * pB.py() / xp ;
            dPhi = 0;
            if ( xyp1 != 0 )
              dPhi = atan( ( pA.py() / pA.px() - pB.py() / pB.px() ) / xyp1 );
            if ( xyp1 < 0 )
              {
                if ( pA.py() / pA.px() > 0 )
                  dPhi += M_PI;
                else
                  dPhi -= M_PI;
              }
          }
      }
    else
      {
        dPhi = remainder( -pA.phi() + pB.phi(), 2*M_PI );
      }
    return dPhi;
  }

  /** delta Phi squared in range ([-pi,pi[)^2 from two I4momentum references */
  inline
  double deltaPhiSq( const I4Momentum & pA, const I4Momentum & pB )
  {
    double dPhiSq(999);
    if ( pA.kind() == I4Momentum::P4PXPYPZE && pB.kind() == I4Momentum::P4PXPYPZE )
      {
        double cosPhi = ( pA.px() * pB.px() + pA.py() * pB.py() ) / pA.pt() / pB.pt();
        double phi = acos(cosPhi);
        dPhiSq = phi*phi;
      }
    else
      {
        dPhiSq = remainder( -pA.phi() + pB.phi(), 2*M_PI );
        dPhiSq = dPhiSq * dPhiSq;
      }
    return dPhiSq;
  }

  /** delta Phi in range [-pi,pi[ from two I4momentum pointers */
  inline
  double deltaPhi( const I4Momentum * const pA, const I4Momentum * const pB )
  { return deltaPhi( pA->phi(), pB->phi() ); }

  /// @f$ \Delta{R} @f$ from 1 @c I4Momentum
  inline
  double deltaR( const I4Momentum& p4, double eta, double phi )
  {
    using std::sqrt;
    const double dEta = p4.eta() - eta;
    const double dPhi = P4Helpers::deltaPhi( p4, phi );
    return sqrt( dEta*dEta + dPhi*dPhi );
  }

  /** delta R from two I4momentum reference */
  inline
  double deltaR( const I4Momentum& pA, const I4Momentum& pB )
  {
    using std::sqrt;
    const double dEtaSq = P4Helpers::deltaEtaSq( pA, pB );
    const double dPhiSq = P4Helpers::deltaPhiSq( pA, pB );
    return sqrt( dEtaSq + dPhiSq );
  }

  /** delta R from two I4momentum pointers */
  inline
  double deltaR( const I4Momentum * const pA, const I4Momentum * const pB )
  { return P4Helpers::deltaR( *pA, *pB ); }

  /// Check if 2 @c I4Momentum are in a @f$ \Delta{R} @f$ cone
  /// @param dR [in] @f$ \Delta{R} @f$
  /// @return true if they are
  inline
  bool isInDeltaR( const I4Momentum& p1, const I4Momentum& p2,
       double dR )
  {
    using std::abs;
    using std::sqrt;
    const double dPhi = abs( P4Helpers::deltaPhi(p1,p2) ); // in [-pi,pi)
    if ( dPhi > dR ) return false;                         // <==
    const double dEta = abs( P4Helpers::deltaEta(p1,p2) );
    if ( dEta > dR ) return false;                         // <==
    const double deltaR2 = dEta*dEta + dPhi*dPhi;
    if ( deltaR2 > dR*dR ) return false;                   // <==
    return true;
  }

  /** invariant mass from two I4momentum references */
  inline
  double invMass( const I4Momentum & pA, const I4Momentum & pB )
  { return (pA.hlv()+pB.hlv()).m(); }

  /** invariant mass from two I4momentum pointers */
  inline
  double invMass( const I4Momentum * const pA, const I4Momentum * const pB )
  { return invMass( *pA, *pB ); }

  /** invariant mass from four I4momentum references */
  inline
  double invMass( const I4Momentum & pA, const I4Momentum & pB,
      const I4Momentum & pC, const I4Momentum & pD )
  { return (pA.hlv()+pB.hlv()+pC.hlv()+pD.hlv()).m(); }

  /** invariant mass from four I4momentum pointers */
  inline
  double invMass( const I4Momentum * const pA, const I4Momentum * const pB,
      const I4Momentum * const pC, const I4Momentum * const pD )
  { return invMass( *pA, *pB, *pC, *pD ); }

  // -------------------------------------------------------------
  // --------------- Sorting functions ---------------------------
  // -------------------------------------------------------------

  /// sort a container according to the given predicate
  template<class Iterator_t, class Predicate_t>
  inline
  void sort( Iterator_t itr, Iterator_t itrEnd, Predicate_t p )
  {
    // Koenig's look-up at our rescue: handle correctly DataVector's
    // special sort method. => We inject the std::sort into our namespace
    using std::sort;
    return sort( itr, itrEnd, p );
  }

  /// sort a container according to the given predicate
  template<class Container_t, class Predicate_t>
  inline
  void sort( Container_t& container, Predicate_t p )
  {
    return P4Helpers::sort( container.begin(), container.end(), p );
  }

  // -------------------------------------------------------------
  // --------------- Matching functions --------------------------
  // -------------------------------------------------------------

  /// Find the closest element in a collection to an @c I4Momentum
  /// @param index [out] index of the closest element
  /// @param deltaR [out] @f$ \Delta{R} @f$
  /// @return true if found
  template <class Container_t>
  inline
  bool closestDeltaR( const double eta, const double phi,
          Container_t& coll, std::size_t& index, double& deltaR )
  {
    deltaR = std::numeric_limits<double>::max(); // big value
    bool l_return = false;
    std::size_t l_idx = 0;
    typename Container_t::const_iterator it  = coll.begin();
    typename Container_t::const_iterator itE = coll.end();
    for (; it != itE; ++it,++l_idx) {
      double rtu = P4Helpers::deltaR(**it, eta, phi);
      if ( CxxUtils::fpcompare::less(rtu, deltaR) ) {
        index  = l_idx;
        deltaR = rtu;
        l_return = true;
      }
    }
    return l_return;
  }

  /// Find the closest element in a collection to an @c I4Momentum
  /// @param index [out] index of the closest element
  /// @param deltaR [out] @f$ \Delta{R} @f$
  /// @return true if found
  template <class Container_t>
  inline
  bool closestDeltaR( const I4Momentum& p4,
          Container_t& coll, std::size_t& index, double& deltaR )
  {
    return P4Helpers::closestDeltaR( p4.eta(), p4.phi(),
                                     coll, index, deltaR );
  }

  /// find the closest element in R - with a condition on E
  /// @param index [out] index of the closest element
  /// @param deltaR [out] @f$ \Delta{R} @f$
  /// @param deltaE [out] @f$ \Delta{E} @f$
  /// @return true if found
  template <class Container_t>
  inline
  bool closestDeltaR( const double eta, const double phi, const double ene,
                      Container_t& coll, std::size_t& index,
                      double &deltaR, double &deltaE )
  {
    using std::abs;
    deltaR = deltaE = std::numeric_limits<double>::max(); // big value
    bool l_return = false;
    std::size_t l_idx = 0;
    typename Container_t::const_iterator it  = coll.begin();
    typename Container_t::const_iterator itE = coll.end();
    for (; it != itE; ++it,++l_idx) {
      const double dE  = abs( ene - (*it)->e() );
      if ( CxxUtils::fpcompare::less(dE, deltaE) ) {
        const double rtu = P4Helpers::deltaR(**it,eta,phi);
        if ( CxxUtils::fpcompare::less(rtu, deltaR) ) {
          index  = l_idx;
          deltaR = rtu;
          deltaE = dE;
          l_return = true;
        }
      }
    }
    return l_return;
  }

  /// find the closest element in R - with a condition on E
  /// @param index [out] index of the closest element
  /// @param deltaR [out] @f$ \Delta{R} @f$
  /// @param deltaE [out] @f$ \Delta{E} @f$
  /// @return false if found
  template <class Container_t>
  inline
  bool closestDeltaR( const I4Momentum& p4,
          Container_t& coll, std::size_t& index,
          double &deltaR, double &deltaE )
  {
    return P4Helpers::closestDeltaR( p4.eta(), p4.phi(), p4.e(),
                                     coll, index, deltaR, deltaE );
  }

} //> namespace P4Helpers


#endif

#endif // FOURMOMUTILS_P4HELPERS_H
