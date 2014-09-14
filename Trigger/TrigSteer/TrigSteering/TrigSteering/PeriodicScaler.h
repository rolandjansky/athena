/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : PeriodicScaler
 *
 * @brief : implementation of Prescale/PassThrough engine using periodic algorithm
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: PeriodicScaler.h,v 1.5 2008-05-06 09:19:19 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_PeriodicScaler_H
#define TRIGSTEERING_PeriodicScaler_H

#include "TrigSteering/Scaler.h"

#include <vector>

namespace HLT {

  /**
     @class PeriodicScaler
     Utility class to determine if a chain should be prescaled/passed-through using its PS/PT factor,
     implemented with a periodic algorithm (take every [factor] event).

     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */
  class PeriodicScaler : public Scaler {

  public:

    PeriodicScaler();
    virtual ~PeriodicScaler() { }

    /** @brief make decision based on given factor
	@param factor take every [factor] event
	@return true if event is taken
    */
    virtual bool decision(float factor);

    /** @brief Reset scaler to initial state.
     */
    virtual void reset();
    
  protected:
    unsigned int m_counter; //!< event counter
    unsigned int m_last;    //!< last integer
  };

  
  /**
     @class PeriodicScaler
     Utility class to determine if a chain should be prescaled/passed-through using its PS/PT factor,
     implemented with a periodic algorithm (take every [factor] event and always 1st event)
     
     @author Frank Winklmeier
  */
  class PeriodicScalerTake1st : public PeriodicScaler {
  public:
    PeriodicScalerTake1st();
    virtual ~PeriodicScalerTake1st() { }

    /** @brief make decision based on given factor (but always take first event)
	@param factor take every [factor] event
	@return true if event is taken
    */
    virtual bool decision(float factor);
  };
  
}
#endif
