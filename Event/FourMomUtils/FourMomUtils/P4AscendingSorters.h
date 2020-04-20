///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <cmath>

// core includes
#include "CxxUtils/fpcompare.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

namespace P4Sorters {

namespace Ascending {

class Px
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->px(), b->px() );
  }
};

class Py
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->py(), b->py() );
  }
};

class Pz
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->pz(), b->pz() );
  }
};


class Mass
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->m(), b->m() );
  }
};

class MassSquared
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->m2(), b->m2() );
    }
};

class Momentum
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->p(), b->p() );
  }
};

class MomentumSquared
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->p2(), b->p2() );
  }
};

class Eta
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->eta(), b->eta() );
  }
};

class AbsEta
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    using std::abs;
    return CxxUtils::fpcompare::less( abs(a->eta()), abs(b->eta()) );
  }
};

class Ene
{
  public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->e(), b->e() );
  }
};

class Et
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->et(), b->et() );
  }
};

class Pt
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->pt(), b->pt() );
  }
};

class InvPt
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->iPt(), b->iPt() );
  }
};

// Are those usefull anyway ??
class CosTh
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->cosTh(), b->cosTh() );
  }
};

class SinTh
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->sinTh(), b->sinTh() );
  }
};

class CotTh
{
public:
  bool operator()( const I4Momentum* a, const I4Momentum* b ) const {
    return CxxUtils::fpcompare::less( a->cotTh(), b->cotTh() );
  }
};

} // > end namespace Ascending
} // > end namespace P4Sorters

#endif

#endif //> FOURMOMUTILS_P4ASCENDINGSORTERS_H
