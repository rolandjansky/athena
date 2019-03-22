/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTP_SPECIAL_TRIGGER_H
#define TRIGT1CTP_SPECIAL_TRIGGER_H

// Base class:
#include "TrigT1CTP/ISpecialTrigger.h"

// LVL1 include(s):
#include "TrigConfL1Data/L1DataDef.h"
#include "CTPfragment/CTPdataformatVersion.h"


namespace LVL1CTP {

   /**
    *   @short Common class for internal triggers implementation
    *
    *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
    *
    * @version \$Id: SpecialTrigger.h,v 1.4 2007-06-15 16:14:01 efeld Exp $
    *
    */

   class SpecialTrigger : public ISpecialTrigger {

   public:

      //! default constructors
      SpecialTrigger();

      //! constructors setting the internal trigger type
      SpecialTrigger(TrigConf::L1DataDef::TriggerType tt, unsigned int thrnumber, unsigned int ctpVersionNumber);

      //! constructors setting the internal trigger type and rate
      SpecialTrigger(TrigConf::L1DataDef::TriggerType tt, unsigned int thrnumber,  unsigned int ctpVersionNumber, const unsigned int);
    
      //! destructor
      virtual ~SpecialTrigger();

      // access functions
      virtual std::string name() const;                   //!< @copydoc LVL1CTP::ISpecialTrigger::name
      virtual unsigned int rate() const;                  //!< @copydoc LVL1CTP::ISpecialTrigger::rate

      virtual unsigned int pit() const;                   //!< @copydoc LVL1CTP::ISpecialTrigger::pit

      virtual std::string print() const;                  //!< @copydoc LVL1CTP::ISpecialTrigger::print
      virtual std::string dump() const;                   //!< @copydoc LVL1CTP::ISpecialTrigger::dump

   protected:
    
      // data members
      unsigned int                         m_rate;        //!< rate for this internal trigger
      double                               m_counter;     //!< counter for rate
    
      TrigConf::L1DataDef::TriggerType     m_type;        //!< trigger type
      unsigned int                         m_thrNumber;   //!< threshold number (e.g. 0 or 1 for the PSCK triggers)
     
      unsigned int                         m_ctpVersionNumber;
      CTPdataformatVersion *               m_ctpVersion;
     

   }; // class SpecialTrigger

} // namespace LVL1CTP

#endif // TRIGT1CTP_SPECIAL_TRIGGER_H
