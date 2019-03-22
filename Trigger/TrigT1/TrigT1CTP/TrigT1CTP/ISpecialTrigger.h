/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_ISPECIAL_TRIGGER_H
#define TRIGT1CTP_ISPECIAL_TRIGGER_H

// STL include(s):
#include <string>
#include <map>

// LVL1 include(s):
#include "TrigConfL1Data/L1DataDef.h"

// Forward includes:
class StatusCode;

namespace LVL1CTP {

  /**
   *   @short Interface for internal trigger classes
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *
   * @version \$Id: ISpecialTrigger.h,v 1.7 2009-01-29 20:54:31 efeld Exp $
   *
   */

  class ISpecialTrigger {

  public:

    //! virtual empty destructor
    virtual ~ISpecialTrigger() {}

    virtual unsigned int rate() const = 0;         //!< get rate of internal trigger
    virtual std::string name() const = 0;          //!< get name of internal trigger
    virtual unsigned int pit() const = 0;          //!< get signal position on PIT bus

    virtual StatusCode execute(int bcid=1) = 0;      //!< execute trigger simulation 

    virtual bool evaluate() const = 0;             //!< test, if internal trigger fires
    virtual std::string print() const = 0;         //!< print configuration to string
    virtual std::string dump() const = 0;          //!< dump raw configuration to string

  private:

  }; // class ISpecialTrigger

  /// typedef for internal trigger type
  //typedef TrigConf::L1DataDef::InternalTriggerType InternalTriggerType;
  /// typedef for map between internal trigger type and internal trigger class
  typedef std::map< std::pair<TrigConf::L1DataDef::TriggerType, unsigned int>, ISpecialTrigger*> InternalTriggerMap;

} // namespace LVL1CTP

#endif // TRIGT1CTP_ISPECIAL_TRIGGER_H
