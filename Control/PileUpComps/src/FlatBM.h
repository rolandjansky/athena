/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_FLATBM
#define PILEUPCOMPS_FLATBM 1
/** @file FlatBM.h
 * @brief dummy (flat beam) implementation of IBeamIntensity service
 *
 * $Id: BkgStreamsCache.h,v 1.10 2008-08-28 01:11:06 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */
#include "PileUpTools/IBeamIntensity.h"
#include "GaudiKernel/Service.h"
template <class TYPE> class SvcFactory;

class FlatBM : virtual public IBeamIntensity, public Service {
public:
  inline virtual void selectT0() {}
  inline virtual float largestElementInPattern() const {
    return 1.0;
  }
  inline virtual unsigned int getCurrentT0BunchCrossing() const { 
    //The first filled bunch crossing is always BCID 1
    return 1; 
  }
  inline virtual unsigned int getBeamPatternLength() const { 
    return 1; 
  }
  inline virtual float normFactor(int /*iXing*/) const { return 1.0; }
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
 protected:
  friend class SvcFactory<FlatBM>;
  FlatBM(const std::string& name,ISvcLocator* svc): Service(name,svc) { }
};
#endif

