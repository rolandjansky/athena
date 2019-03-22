/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_BUNCH_GROUP_TRIGGER_H
#define TRIGT1CTP_BUNCH_GROUP_TRIGGER_H

// Base class:
#include "TrigT1CTP/SpecialTrigger.h"

// STL include(s):
#include <vector>
#include <string>

namespace LVL1CTP {

  /**
   *
   *   @short Class for simulating the internal bunch group trigger
   *   Note that a dedicated class to deal with the real bunch group trigger is in TrigT1Interfaces
   *     @see LVL1CTP::ISpecialTrigger
   *     @see LVL1CTP::SpecialTrigger
   *     @see LVL1CTP::PrescaledClockTrigger
   *     @see LVL1CTP::RandomTrigger
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *
   * @version \$Id: BunchGroupTrigger.h,v 1.4 2008-09-24 16:15:20 efeld Exp $
   *
   */

  class BunchGroupTrigger : public SpecialTrigger {

  public:

    //! default constructor    
    BunchGroupTrigger();
    //! constructor setting the internal trigger type and rate
    BunchGroupTrigger(unsigned int bgrp_number, const std::vector<int>&,unsigned int ctpVersionNumber);
    
    //! virtual empty destructor
    virtual ~BunchGroupTrigger() {}

    virtual StatusCode execute(int bcid);    //!< @copydoc LVL1CTP::ISpecialTrigger::execute
    virtual bool evaluate() const;           //!< @copydoc LVL1CTP::ISpecialTrigger::evaluate

    virtual std::string print() const;       //!< @copydoc LVL1CTP::SpecialTrigger::print
    virtual std::string dump() const;        //!< @copydoc LVL1CTP::SpecialTrigger::dump

  private:

    //! list of bunch IDs
    std::vector<int> m_bunches;

  }; // class BunchGroupTrigger

} // namespace LVL1CTP

#endif // TRIGT1CTP_BUNCH_GROUP_TRIGGER_H
