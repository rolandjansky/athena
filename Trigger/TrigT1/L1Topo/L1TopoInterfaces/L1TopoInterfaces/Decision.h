//  Decision.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__Decision__
#define __TopoCore__Decision__

#include "L1TopoCommon/StatusCode.h"

#include <iostream>
#include <vector>
#include <stdint.h>

namespace TCS {

   class Decision {
   public:
      
      Decision(unsigned int firstBit = 0, unsigned int nBits = 1) :
         m_firstBit(firstBit),
         m_nBits(nBits)
      {};

      // destructor
      ~Decision(){};

      // setters
      StatusCode reset() { m_decision = 0; return StatusCode::SUCCESS; }
      void setBit(unsigned int index, bool value);

      void setFirstBit(unsigned int startBit) { m_firstBit = startBit; }
      void setNBits(unsigned int nBits) { m_nBits = nBits; }

      // getters
      bool bit(unsigned int index) const { return (m_decision & (0x1<<index)) != 0; }
      uint64_t decision() const { return m_decision; }
      unsigned int firstBit() const { return m_firstBit; }
      unsigned int lastBit() const { return m_firstBit + m_nBits - 1; }
      unsigned int nBits() const { return m_nBits; }

   private:
      uint32_t     m_decision {0};
      unsigned int m_firstBit {0};
      unsigned int m_nBits {1};

   };
   
}

std::ostream & operator<<(std::ostream&, const TCS::Decision &);

#endif
