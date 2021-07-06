// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaKernel/Units.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2015
 * @brief Wrapper to avoid constant divisions when using units.
 *
 * A typical use of the unit symbols is like
 *
 *   double pt_in_gev = pt / GeV;
 *
 * However, writing it like this results in a division by a constant being
 * generated.  It would be faster to rewrite this as multiplication by the
 * inverse, but the compiler won't do that by default as the results
 * are not guaranteed to be the same.
 *
 * Using the symbols defined here allow avoiding the divisions
 * (in optimized builds) without having to write the above explicitly
 * as
 *
 *   double pt_in_gev = pt * (1./GeV);
 *
 * which is pretty ugly.
 *
 * We do this by creating a static const object wrapping the value
 * of the unit and overriding operator/ to change the division
 * to multiplication by inverse.
 */


#ifndef ATHENAKERNEL_UNITS_H
#define ATHENAKERNEL_UNITS_H


#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"


namespace Athena {
namespace Units {


/**
 * @brief Wrapper to avoid constant divisions when using units.
 *        The Unit class defined here is a placeholder.
 *        A specialization will be generated for each unit by the UNIT
 *        macro below.  TAG is used to distinguish these specializations.
 *
 *        (We used to do this with a single, non-template, class,
 *        but that didn't work with clang when -ffp-exception-behavior=maytrap
 *        is used.)
 */
template <int TAG> class Unit {};
template <int TAG>
constexpr double operator/ (double x, const Unit<TAG> u)
{
  constexpr double recip = 1. / static_cast<double>(u);
  return x * recip;
}
template <int TAG>
constexpr float operator/ (float x, const Unit<TAG> u)
{
  constexpr float recip = 1. / static_cast<float>(u);
  return x * recip;
}


// Include a selection of units from SystemOfUnits.h
#define UNIT(NAME)                                                      \
template <> class Unit<__LINE__>                                        \
{                                                                       \
public:                                                                 \
  constexpr operator double() const { return Gaudi::Units::NAME; }      \
};                                                                      \
constexpr static const Unit<__LINE__> NAME

UNIT (millimeter);
UNIT (millimeter2);
UNIT (millimeter3);
UNIT (centimeter);
UNIT (centimeter2);
UNIT (centimeter3);
UNIT (meter);
UNIT (meter2);
UNIT (meter3);
UNIT (micrometer);
UNIT (um);
UNIT (mm);
UNIT (mm2);
UNIT (mm3);
UNIT (cm);
UNIT (cm2);
UNIT (cm3);
UNIT (radian);
UNIT (degree);
UNIT (rad);
UNIT (deg);
UNIT (picosecond);
UNIT (nanosecond);
UNIT (microsecond);
UNIT (millisecond);
UNIT (second);
UNIT (ns);
UNIT (s);
UNIT (ms);
UNIT (keV);
UNIT (MeV);
UNIT (GeV);
UNIT (TeV);
UNIT (gram);
UNIT (g);
UNIT (mole);
UNIT (c_light);
#undef UNIT


} // namespace Units
} // namespace Athena


#endif // not ATHENAKERNEL_ATLASUNITS_H
