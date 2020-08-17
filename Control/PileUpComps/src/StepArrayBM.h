/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_STEPARRAYBM
#define PILEUPCOMPS_STEPARRAYBM 1
/** @file StepArrayBM.h
 * @brief A IBeamIntensity service configured with an intensity array and an optional signal pattern array
 * The Gaudi::Property<std::vector<float>> IntensityPattern describes the intensity pattern that is
 * repeated for the entire beam xing range.
 * The Gaudi::Property<std::vector<float>> SignalPattern describes the positions at which signal events will be placed (and hence which xings are simulated)
 * This BeamIntensity service sequentially steps through the bunch crossings of the provided pattern
 * $Id: BkgStreamsStepCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Will Buttinger - ATLAS Collaboration
 */
#include "Gaudi/Property.h"

#include "PileUpTools/IBeamIntensity.h"
#include "AthenaBaseComps/AthService.h"

class StepArrayBM : public extends<AthService, IBeamIntensity>
{
public:
  /// \name Constructor
  //@{
  StepArrayBM(const std::string& name,ISvcLocator* svc);
  virtual ~StepArrayBM();
  //@}
  /// \name AthService methods
  //@{
  virtual StatusCode initialize() override final;
  //@}
  /// \name IBeamIntensity methods
  //@{
  virtual float normFactor(int iXing) const override final
  {
    unsigned int index = static_cast<unsigned int>((((iXing + static_cast<int>(m_t0Offset)) % static_cast<int>(m_ipLength)) + static_cast<int>(m_ipLength) )  % static_cast<int>(m_ipLength));
    //The array itself has max-value 1.0, but we want it to have mean value 1.0 (for filled bunch crossings), so we multiple by the m_largestElementInPattern.
    ATH_MSG_VERBOSE("normFactor for BCID " << iXing
                    << " (offset " << m_t0Offset
                    << " index " << index
                    << ") is = " <<  m_largestElementInPattern*m_intensityPattern[ index ]);
    return m_largestElementInPattern*m_intensityPattern[ index ];
  }
  virtual float largestElementInPattern() const override final { return m_largestElementInPattern; }
  virtual void selectT0() override final;
  virtual unsigned int getCurrentT0BunchCrossing() const override final { return m_t0Offset; }
  virtual unsigned int getBeamPatternLength() const override final { return m_ipLength; }
  //@}
private:
  /// max bunch crossings per orbit
  unsigned int m_maxBunchCrossingPerOrbit;
  /// offset of the t0 wrto our intensity pattern
  unsigned int m_t0Offset;
  /// offset of the current xing wrto the signal pattern
  unsigned int m_signalOffset;
  /// user-defined intensity pattern
  Gaudi::Property<std::vector<float>> m_intensityPatternProp;
  /// user-defined signal pattern - non zero numbers means "Do Signal"
  Gaudi::Property<std::vector<float>> m_signalPatternProp;
  /// lenght of the intensity pattern
  unsigned int m_ipLength;
  /// length of the signal pattern
  unsigned int m_spLength;
  /// normalized intensity pattern. C array to make clhep RandGeneral happy
  double* m_intensityPattern;
  /// locally stored siganlPattern
  bool* m_signalPattern;
  /// The largest value in the pattern assuming that the pattern has
  /// mean value 1.0. Multiplying by this converts values in the
  /// m_intensityPattern from having max value 1.0 to having mean
  /// value 1.0.
  float m_largestElementInPattern;
};
#endif
