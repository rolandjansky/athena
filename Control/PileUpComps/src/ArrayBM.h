/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_ARRAYBM
#define PILEUPCOMPS_ARRAYBM 1
/** @file ArrayBM.h
 * @brief A IBeamIntensity service configured with an intensity array
 * The FloatArrayProperty describes the intensity pattern that is 
 * repeated for the entire beam xing range.
 *
 * $Id: BkgStreamsCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */
#include "PileUpTools/IBeamIntensity.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
template <class TYPE> class SvcFactory;
class IAtRndmGenSvc;
namespace CLHEP {
  class RandGeneral;
}

class ArrayBM : virtual public IBeamIntensity, public AthService {
public:
  virtual ~ArrayBM();
  virtual void selectT0();
  virtual unsigned int getCurrentT0BunchCrossing() const { return m_t0Offset; }
  virtual unsigned int getBeamPatternLength() const { return m_ipLength; }
  virtual float largestElementInPattern() const { return m_largestElementInPattern; }
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
  friend class SvcFactory<ArrayBM>;
  ArrayBM(const std::string& name,ISvcLocator* svc);
private:
  /// max bunch crossings per orbit
  unsigned int m_maxBunchCrossingPerOrbit;
  /// offset of the t0 wrto our intensity pattern
  unsigned int m_t0Offset;
  /// user-defined intensity pattern
  FloatArrayProperty m_intensityPatternProp;
  /// lenght of the intensity pattern
  unsigned int m_ipLength;
  /// normalized intensity pattern. C array to make clhep RandGeneral happy
  double* m_intensityPattern;
  /// shoot random number proportionally to m_intensityPattern
  CLHEP::RandGeneral* m_biRandom;
  /// the service managing our random seeds/sequences
  ServiceHandle<IAtRndmGenSvc> m_atRndmGenSvc;
  /// The largest value in the pattern assuming that the pattern has
  /// mean value 1.0. Multiplying by this converts values in the
  /// m_intensityPattern from having max value 1.0 to having mean
  /// value 1.0.
  float m_largestElementInPattern;
};
#endif

