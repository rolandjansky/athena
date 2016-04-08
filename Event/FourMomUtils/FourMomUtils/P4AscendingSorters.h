///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4AscendingSorters.h
// Header file for 'ascending' Sorters
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_P4ASCENDINGSORTERS_H
#define FOURMOMUTILS_P4ASCENDINGSORTERS_H

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

// STL includes
#include <functional> // binary_function
#include <cmath>

// HepMC / CLHEP includes

// core includes
#include "CxxUtils/fpcompare.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

// Forward declaration

namespace P4Sorters {

typedef std::binary_function< const I4Momentum*,
            const I4Momentum*,
            bool
            >
        I4MomPredicate_t;

namespace Ascending {

class Px : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->px(), b->px() );
  }
};

class Py : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->py(), b->py() );
  }
};

class Pz : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->pz(), b->pz() );
  }
};


class Mass : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->m(), b->m() );
  }
};

class MassSquared : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->m2(), b->m2() );
    }
};

class Momentum : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->p(), b->p() );
  }
};

class MomentumSquared : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->p2(), b->p2() );
  }
};

class Eta : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->eta(), b->eta() );
  }
};

class AbsEta : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    using std::abs;
    return CxxUtils::fpcompare::less( abs(a->eta()), abs(b->eta()) );
  }
};

// not yet...
//   class Phi : public I4MomPredicate_t
//   {
//   public:
//     bool operator()( const I4Momentum* a, const I4Momentum* b ) {
//       return CxxUtils::fpcompare::less( a->phi(), b->phi() );
//     }
//   };

class Ene : public I4MomPredicate_t
{
  public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->e(), b->e() );
  }
};

class Et : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->et(), b->et() );
  }
};

class Pt : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->pt(), b->pt() );
  }
};

class InvPt : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->iPt(), b->iPt() );
  }
};

// Are those usefull anyway ??
class CosTh : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->cosTh(), b->cosTh() );
  }
};

class SinTh : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->sinTh(), b->sinTh() );
  }
};

class CotTh : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::less( a->cotTh(), b->cotTh() );
  }
};

} // > end namespace Ascending
} // > end namespace P4Sorters

#endif

#endif //> FOURMOMUTILS_P4ASCENDINGSORTERS_H
