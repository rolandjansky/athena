/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_RANDOM_TRIGGER_H
#define TRIGT1CTP_RANDOM_TRIGGER_H

// Base class:
#include "TrigT1CTP/SpecialTrigger.h"


// Forward include(s):
namespace CLHEP {
  class HepRandomEngine;
}

namespace LVL1CTP {

  /**
   *
   *   @short Class for simulating the internal random trigger
   *
   *     @see LVL1CTP::ISpecialTrigger
   *     @see LVL1CTP::SpecialTrigger
   *     @see LVL1CTP::BunchGroupTrigger
   *     @see LVL1CTP::PrescaledClockTrigger
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *
   * @version \$Id: RandomTrigger.h,v 1.6 2007-06-15 16:14:01 efeld Exp $
   *
   */

  class RandomTrigger : public SpecialTrigger {

  public:

    //! default constructor
    RandomTrigger();
    //! constructor setting the internal trigger type and random number engine
    RandomTrigger(unsigned int rand_number, const unsigned int,unsigned int ctpVersionNumber, CLHEP::HepRandomEngine* randEng = 0);

    //! destructor
    virtual ~RandomTrigger();

    virtual StatusCode execute(int /*bcid*/);         //!< @copydoc LVL1CTP::ISpecialTrigger::execute
    virtual bool evaluate() const;        //!< @copydoc LVL1CTP::ISpecialTrigger::evaluate

  private:

    //! initialisation of random trigger
    virtual void Init();

    //! random generator 
    CLHEP::HepRandomEngine* m_randEngine;

  }; // class RandomTrigger

} // namespace LVL1CTP

#endif // TRIGT1CTP_RANDOM_TRIGGER_H
