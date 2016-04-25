///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4DescendingSorters.h
// Header file for 'descending' Sorters
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_P4DESCENDINGSORTERS_H
#define FOURMOMUTILS_P4DESCENDINGSORTERS_H

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

namespace Descending {

class Px : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->px(), b->px() );
  }
};

class Py : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->py(), b->py() );
  }
};

class Pz : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->pz(), b->pz() );
  }
};


class Mass : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->m(), b->m() );
  }
};

class MassSquared : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->m2(), b->m2() );
    }
};

class Momentum : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->p(), b->p() );
  }
};

class MomentumSquared : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->p2(), b->p2() );
  }
};

class Eta : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->eta(), b->eta() );
  }
};

class AbsEta : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    using std::abs;
    return CxxUtils::fpcompare::greater( abs(a->eta()), abs(b->eta()) );
  }
};

// not yet...
//   class Phi : public I4MomPredicate_t
//   {
//   public:
//     bool operator()( const I4Momentum* a, const I4Momentum* b ) {
//       return CxxUtils::fpcompare::greater( a->phi(), b->phi() );
//     }
//   };

class Ene : public I4MomPredicate_t
{
  public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->e(), b->e() );
  }
};

class Et : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->et(), b->et() );
  }
};

class Pt : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->pt(), b->pt() );
  }
};

class InvPt : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->iPt(), b->iPt() );
  }
};

// Are those usefull anyway ??
class CosTh : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->cosTh(), b->cosTh() );
  }
};

class SinTh : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->sinTh(), b->sinTh() );
  }
};

class CotTh : public I4MomPredicate_t
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) {
    return CxxUtils::fpcompare::greater( a->cotTh(), b->cotTh() );
  }
};

} // > end namespace Descending
} // > end namespace P4Sorters

#endif

#endif //> FOURMOMUTILS_P4DESCENDINGSORTERS_H
