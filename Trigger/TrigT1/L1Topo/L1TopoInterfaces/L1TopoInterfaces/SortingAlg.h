/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  SortingAlg.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef TCS_SortingAlg
#define TCS_SortingAlg

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoCommon/StatusCode.h"

namespace TCS {

   class SortingAlg : public ConfigurableAlg {
   public:
      
      SortingAlg(const  std::string & name) :
      ConfigurableAlg(name, SORT),
      m_overflow(false)
      {};

      virtual ~SortingAlg();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output) = 0;
      virtual TCS::StatusCode sortBitCorrect(const InputTOBArray & input, TOBArray & output);
      /**
         @brief whether an overlflow occurred

         In practice propagate the overflow bits from the input collections
       */
      bool overflow() const { return m_overflow; }
      /// setter, to propagate bit from input event
      SortingAlg& setOverflow(const bool v) { m_overflow = v; return *this; }

   protected:

      virtual StatusCode doReset();
      bool m_overflow; ///< whether an overflow occurred during the current execution.
   };

}

#endif
