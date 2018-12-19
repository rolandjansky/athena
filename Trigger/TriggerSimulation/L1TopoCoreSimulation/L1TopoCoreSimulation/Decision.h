/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  Decision.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/16/12.
//

#ifndef __TopoCore__Decision__
#define __TopoCore__Decision__

#include <iostream>
#include "L1TopoCoreSimulation/StatusCode.h"
#include <stdint.h>

namespace TCS {

   class Decision {
   public:
      
      // default constructor
      Decision() :
         m_decision(0)
         , m_firstBit(0)
         , m_nBits(1)
      {};

      Decision(unsigned int firstBit, unsigned int nBits) :
         m_decision(0)
         , m_firstBit(firstBit)
         , m_nBits(nBits)
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
      uint64_t     m_decision;
      unsigned int m_firstBit;
      unsigned int m_nBits;
   };
   
} // end of namespace TCS

std::ostream & operator<<(std::ostream&, const TCS::Decision &);

#endif /* defined(__TopoCore__Decision__) */
