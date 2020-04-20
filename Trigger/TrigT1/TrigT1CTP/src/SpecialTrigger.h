/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CTP_ISPECIAL_TRIGGER_H
#define TRIGT1CTP_ISPECIAL_TRIGGER_H

#include "CTPfragment/CTPdataformatVersion.h"

// STL include(s):
#include <vector>
#include <string>
#include <map>

namespace LVL1CTP {

   class ISpecialTrigger;
   typedef std::map< std::string, const ISpecialTrigger*> InternalTriggerMap;


   /* @brief Interface for internal trigger classes */
   class ISpecialTrigger {
   public:
      virtual ~ISpecialTrigger() {}
      virtual std::string name() const = 0;          //!< get name of internal trigger
      virtual size_t pitPos() const = 0;             //!< get signal position on PIT bus
   };



   class RandomTrigger : public ISpecialTrigger {
   public:
      RandomTrigger() = delete;
      RandomTrigger( size_t rndmNr, const CTPdataformatVersion * ctpFormat);
      virtual ~RandomTrigger() {}
      std::string name() const 
         { return "RNDM" + std::to_string(m_rndmNr); }
      size_t pitPos() const 
      {return m_pitPos; }
   private:
      size_t m_rndmNr;    //!< nr of the bunchgroup 0..15
      size_t m_pitPos;    //!< global bit in the TIP vector
   };




   /** @brief Class for simulating the internal bunch group trigger */
   class BunchGroupTrigger : public ISpecialTrigger {
   public:
      BunchGroupTrigger() = delete;
      BunchGroupTrigger( size_t bgrpNr,
                         const std::vector<unsigned int>& bunches,
                         const CTPdataformatVersion * ctpFormat);
      virtual ~BunchGroupTrigger() {}
      std::string name() const
         { return "BGRP" + std::to_string(m_bgrpNr); }
      size_t pitPos() const 
      {return m_pitPos; }
   private:
      std::vector<unsigned int> m_bunches;   //!< list of bunch IDs
      size_t                    m_bgrpNr;    //!< nr of the bunchgroup 0..15
      size_t                    m_pitPos;    //!< global bit in the TIP vector
   };

}



















#endif
