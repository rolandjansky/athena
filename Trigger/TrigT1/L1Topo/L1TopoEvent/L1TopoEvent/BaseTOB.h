// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#ifndef TopoCore_BaseTOB
#define TopoCore_BaseTOB

#include <iostream>
#include "L1TopoCommon/Types.h"

namespace TCS {
   
   class BaseTOB {
   public:
      BaseTOB(int roiWord);
      virtual ~BaseTOB();

      virtual void print(std::ostream&) const = 0;

      virtual inputTOBType_t tobType() const = 0;
      
      uint32_t roiWord() const { return m_roiWord; }

      static size_t instances() { return 0; }

   protected:
      int sizeCheck(int value, unsigned int size) const;
      int sizeCheckM(int value, unsigned int size) const;

      unsigned int sizeCheck(unsigned int value, unsigned int size) const;
      

   private:
      
      uint32_t m_roiWord;

   };
   
std::ostream & operator<< (std::ostream & o, const TCS::BaseTOB&);

}

#endif
