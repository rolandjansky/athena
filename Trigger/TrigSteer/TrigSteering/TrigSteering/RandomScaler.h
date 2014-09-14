/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : RandomScaler
 *
 * @brief Random prescaler
 *
 * This scaler is used for the random HLT prescales. It uses CLHEP's Ranlux64 via
 * the athena AtRanluxGenSvc. By default the random number stream is configured
 * via the AtRanluxGenSvc.Seeds property. Use the configurable in TrigSteeringConfig
 * to configure this class properly. In addition it has these options for non-default
 * seed settings:
 *
 * - useNodeIDSeed: Derive initial seed from node ID (for partition running)
 * - useEventSeed:  Re-seed the RNG on every event based on event time stamp and
 *                  the initial seed (as specified via AtRanluxGenSvc.Seeds)
 *
 * For a study on the prescale 'quality' when re-seeding is used, see:
 * http://indico.cern.ch/getFile.py/access?contribId=2&resId=0&materialId=slides&confId=108184
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Frank Winklmeier - CERN
 *
 * $Id: RandomScaler.cxx,v 1.5 2008-05-06 09:19:19 tbold Exp $
 **********************************************************************************/
#ifndef TRIGSTEERING_RandomScaler_H
#define TRIGSTEERING_RandomScaler_H

#include "TrigSteering/Scaler.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

namespace CLHEP {
  class HepRandomEngine;
  class Ranlux64Engine;
}

namespace HLT {

  /**
     @class RandomScaler
     Utility class to determine if a chain should be prescaled/passed-through using its PS/PT factor,
     using a random procedure (random number).

     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */
  class RandomScaler : public ReusableScaler,
                       public IIncidentListener
  {

  public:

    RandomScaler( const std::string& type,
                  const std::string& name,
                  const IInterface* parent ); //!< std Gaudi constructor

    virtual ~RandomScaler() { } //!< virtual destructor

    StatusCode initialize(); //!< initialize random number generators with seeds

    void handle(const Incident& inc);  //!< incident handler
    
    /** @brief make decision based on given factor
	@param factor to compare agains random number
	@return true if event is taken, ie factor < 1/randomNumber
    */
    bool decision(float factor);

    /** @brief Reset scaler to initial state.
	This doesn't do anything for a random scaler
     */	
    void reset() {}
    
  private:

    void setSeedFromDataflow();
    
    CLHEP::Ranlux64Engine* m_engine;  //!< CLHEP random engine

    enum {
      SEED_FIXED = 0,
      SEED_SEC = 1,
      SEED_NSEC = 2,
      N_SEED_INPUTS = 3
    };    
    long m_seedInput[N_SEED_INPUTS];   //!< input to hash-based seed
    
    // Properties
    int32_t m_seed;     //!< seed for random number engine
    bool m_nodeIDSeed;  //!< use node ID as seed
    bool m_eventSeed;   //!< derive seed from event quantities
  };
}

#endif
