/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_IBEAMINTENSITY_H
#define PILEUPTOOLS_IBEAMINTENSITY_H
/** @file IBeamIntensity.h
 * @brief provides the relative beam intensity as a function of the
 * bunch xing.
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "GaudiKernel/IService.h"
class IBeamIntensity : virtual public IService {
public:
  ///a scale factor (average value 1.0) for the beam intensity at a given
  ///xing. Note how the xing is a signed int, relative to the t0 xing
  virtual float normFactor(int bunchXing) const = 0;

  ///the largest element in the beam intensity pattern. Required by the
  ///BkgStreamsCaches to ensure the background caches are large enough.
  virtual float largestElementInPattern() const = 0;

  ///randomly select in which bunch the current t0 is wrto the beam intensity
  /// distribution. This should be done proportionally to the distribution...
  virtual void selectT0() =0;

  ///return the bunch crossing selected to be the current t0 bunch crossing
  virtual unsigned int getCurrentT0BunchCrossing() const =0;

  ///return the length of the beam pattern
  virtual unsigned int getBeamPatternLength() const =0;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IBeamIntensity, 1, 0 );
};
#endif // PILEUPTOOLS_IBEAMINTENSITY_H
