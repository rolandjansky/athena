//  DecisionAlg.h
//  TopoCore
//  Created by Joerg Stelzer on 11/15/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef TCS_DecisionAlg
#define TCS_DecisionAlg

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/CompositeTOB.h"
#include "L1TopoCommon/StatusCode.h"

#include <iostream>

namespace TCS {

   class Decision;

   class DecisionAlg : public ConfigurableAlg {
   public:
      // default constructor
      DecisionAlg(const  std::string & name) :
         ConfigurableAlg(name, DECISION)
      {};

      virtual ~DecisionAlg();

      virtual TCS::StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                       const std::vector<TCS::TOBArray *> & output,
                                       Decision & decison ) = 0;
    
      virtual TCS::StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                       const std::vector<TCS::TOBArray *> & output,
                                       Decision & decison ) = 0;

      unsigned int numberOutputBits() const { return m_numberOutputBits; }
      void setNumberOutputBits(unsigned int numberOutputBits) { m_numberOutputBits = numberOutputBits; }
      /**
         @brief propagate the hardware decision bits for each output bit of this algo

         These bits are used only to fill the monitoring histograms.
         They do not have any use in the L1TopoSimulation itself.         
       */
      DecisionAlg& setHardwareBits(const unsigned int &bitNumber,
                                   const bool &valueDecision,
                                   const bool &valueOverflow);
      ///! reset all hardware bits for this algo
      DecisionAlg& resetHardwareBits();
      ///! get one hardware decision bit from this algo
      bool getDecisionHardwareBit(const unsigned int &bitNumber) const;
      ///! get one hardware overflow bit from this algo
      bool getOverflowHardwareBit(const unsigned int &bitNumber) const;
   private:

      virtual StatusCode doReset();

      friend class TopoSteering;

      // generic parameter
      unsigned int m_numberOutputBits {1};
      ///! decision bits from hardware (assume the algo won't have >31 output bits)
      uint32_t m_hardwareDecisionBits {0};
      ///! overflow bits from hardware (assume the algo won't have >31 output bits)
      uint32_t m_hardwareOverflowBits {0};
   };

}

#endif
