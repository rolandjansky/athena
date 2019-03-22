/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_PRESCALED_CLOCK_TRIGGER_H
#define TRIGT1CTP_PRESCALED_CLOCK_TRIGGER_H

// Base class:
#include "TrigT1CTP/SpecialTrigger.h"

namespace LVL1CTP {

  /**
   *
   *   @short Class for simulating the internal prescaled clock trigger
   *
   *     @see LVL1CTP::ISpecialTrigger
   *     @see LVL1CTP::SpecialTrigger
   *     @see LVL1CTP::BunchGroupTrigger
   *     @see LVL1CTP::RandomTrigger
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *
   * @version \$Id: PrescaledClockTrigger.h,v 1.4 2007-06-15 16:14:01 efeld Exp $
   *
   */

  class PrescaledClockTrigger : public SpecialTrigger {

  public:
    
    //! default constructor
    PrescaledClockTrigger();
    //! constructor setting the internal trigger type and rate
    PrescaledClockTrigger(unsigned int clock_number, const unsigned int, unsigned int ctpVersionNumber);
    
    virtual StatusCode execute(int /*bcid*/);         //!< @copydoc LVL1CTP::ISpecialTrigger::execute
    virtual bool evaluate() const;        //!< @copydoc LVL1CTP::ISpecialTrigger::evaluate

  private:

  }; // class PrescaledClockTrigger

} // namespace LVL1CTP

#endif // TRIGT1CTP_PRESCALED_CLOCK_TRIGGER_H
