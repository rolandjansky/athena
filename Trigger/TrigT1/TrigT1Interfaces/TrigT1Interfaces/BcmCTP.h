// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_BCM_CTP_H
#define TRIGT1INTERFACES_BCM_CTP_H

// std include(s):
#include <stdint.h>
#include <string>

namespace LVL1 {

   /** @class BcmCTP
    *  @short BCM input class to the CTP simulation
    *
    *         A StoreGate class to contain the output status of the
    *         level 1 BCM trigger simulation for input into the CTP
    *         simulation.
    *
    * @author Bilge M. Demirkoz <demirkoz@cern.ch>
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class BcmCTP {

   public:
      BcmCTP( uint32_t word0 = 0 );
    
      /**
       * Returns an unsigned integer trigger word containing 9 bits of information
       */
      uint32_t cableWord0(void) const {
         return m_cableWord0;
      }

      //! dump raw object content to string
      const std::string dump() const;
      //! print object content in a human readable form to string
      const std::string print() const;

   private:
      /** A data member to contain 9 bits of information **/
      const uint32_t m_cableWord0;

   }; // class BcmCTP

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif

CLASS_DEF( LVL1::BcmCTP , 155355998 , 1 )

#endif // TRIGT1INTERFACES_BCM_CTP_H
