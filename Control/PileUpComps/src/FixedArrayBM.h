/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_FIXEDARRAYBM
#define PILEUPCOMPS_FIXEDARRAYBM 1
/** @file FixedArrayBM.h
 * @brief A IBeamIntensity service configured with an intensity array
 * The FloatArrayProperty describes the intensity pattern that is 
 * repeated for the entire beam xing range.
 * The T0Offset property specifies the fixed point in this intensity 
 * pattern which will be used for the central bunch crossing.
 *
 * @author John Chapman - ATLAS Collaboration
 */
#include "PileUpTools/IBeamIntensity.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Property.h"
template <class TYPE> class SvcFactory;

class FixedArrayBM : virtual public IBeamIntensity, public AthService {
public:
  virtual ~FixedArrayBM();
  virtual void selectT0() { }
  virtual unsigned int getCurrentT0BunchCrossing() const { return m_t0Offset; }
  virtual unsigned int getBeamPatternLength() const { return m_ipLength; }
  virtual float largestElementInPattern() const { return m_largestElementInPattern; }
  virtual unsigned int getBCID(int iXing) const {
    //FIXME to be completely safe this should should probably depend on the bunch spacing too. Perhaps that concept should be deprecated though?
    return static_cast<unsigned int>((((iXing + static_cast<int>(m_t0Offset)) % static_cast<int>(m_maxBunchCrossingPerOrbit)) + static_cast<int>(m_maxBunchCrossingPerOrbit) )  % static_cast<int>(m_maxBunchCrossingPerOrbit));
  }
  virtual float normFactor(int iXing) const { 
    unsigned int index = static_cast<unsigned int>((((iXing + static_cast<int>(m_t0Offset)) % static_cast<int>(m_ipLength)) + static_cast<int>(m_ipLength) )  % static_cast<int>(m_ipLength));
    //The array itself has max-value 1.0, but we want it to have mean value 1.0 (for filled bunch crossings), so we multiple by the m_largestElementInPattern.
    ATH_MSG_VERBOSE("normFactor for BCID " << iXing 
		    << " (offset " << m_t0Offset 
		    << " index " << index
		    << ") is = " <<  m_largestElementInPattern*m_intensityPattern[ index ]);
    return m_largestElementInPattern*m_intensityPattern[ index ]; 
  }
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
protected:
  friend class SvcFactory<FixedArrayBM>;
  FixedArrayBM(const std::string& name,ISvcLocator* svc);
private:
  /// max bunch crossings per orbit
  unsigned int m_maxBunchCrossingPerOrbit;
  /// offset of the t0 wrto our intensity pattern
  unsigned int m_t0Offset;
  /// allow t0 to be in an empty bunch crossing
  BooleanProperty m_allowEmptyT0BunchCrossing;
  /// user-defined intensity pattern
  FloatArrayProperty m_intensityPatternProp;
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

