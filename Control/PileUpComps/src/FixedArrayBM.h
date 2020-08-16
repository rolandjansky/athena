/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_FIXEDARRAYBM
#define PILEUPCOMPS_FIXEDARRAYBM 1
/** @file FixedArrayBM.h
 * @brief A IBeamIntensity service configured with an intensity array
 * The Gaudi::Property<std::vector<float>> describes the intensity pattern that is
 * repeated for the entire beam xing range.
 * The T0Offset property specifies the fixed point in this intensity
 * pattern which will be used for the central bunch crossing.
 *
 * @author John Chapman - ATLAS Collaboration
 */
#include "Gaudi/Property.h"

#include "PileUpTools/IBeamIntensity.h"
#include "AthenaBaseComps/AthService.h"

class FixedArrayBM : public extends<AthService, IBeamIntensity>
{
public:
  /// \name Constructor and Destructor
  //@{
  FixedArrayBM(const std::string& name,ISvcLocator* svc);
  virtual ~FixedArrayBM();
  //@}
  /// \name AthService methods
  //@{
  virtual StatusCode initialize() override final;
  //@}
  /// \name IBeamIntensity methods
  //@{
  virtual float normFactor(int iXing) const override final;
  virtual float largestElementInPattern() const override final { return m_largestElementInPattern; }
  virtual void selectT0() override final { }
  virtual unsigned int getCurrentT0BunchCrossing() const override final { return m_t0Offset; }
  virtual unsigned int getBeamPatternLength() const override final { return m_ipLength; }
  //@}
private:
  virtual unsigned int getBCID(int iXing) const
  {
    //FIXME to be completely safe this should should probably depend on the bunch spacing too. Perhaps that concept should be deprecated though?
    return static_cast<unsigned int>((((iXing + static_cast<int>(m_t0Offset)) % static_cast<int>(m_maxBunchCrossingPerOrbit)) + static_cast<int>(m_maxBunchCrossingPerOrbit) )  % static_cast<int>(m_maxBunchCrossingPerOrbit));
  }
  /// max bunch crossings per orbit
  unsigned int m_maxBunchCrossingPerOrbit;
  /// offset of the t0 wrto our intensity pattern
  unsigned int m_t0Offset;
  /// allow t0 to be in an empty bunch crossing
  Gaudi::Property<bool> m_allowEmptyT0BunchCrossing;
  /// user-defined intensity pattern
  Gaudi::Property<std::vector<float>> m_intensityPatternProp;
  /// length of the intensity pattern
  unsigned int m_ipLength;
  /// normalized intensity pattern.
  double* m_intensityPattern;
  /// The largest value in the pattern assuming that the pattern has
  /// mean value 1.0. Multiplying by this converts values in the
  /// m_intensityPattern from having max value 1.0 to having mean
  /// value 1.0.
  float m_largestElementInPattern;
};
#endif
