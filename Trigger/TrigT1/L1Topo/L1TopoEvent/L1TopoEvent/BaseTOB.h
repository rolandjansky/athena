//  BaseTOB.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__BaseTOB__
#define __TopoCore__BaseTOB__

#include <iostream>
#include "L1TopoCommon/Types.h"

namespace TCS {
   
   class BaseTOB {
   public:
      BaseTOB(int roiWord);
      virtual ~BaseTOB() {}

      virtual void print(std::ostream&) const = 0;

      virtual inputTOBType_t tobType() const = 0;
      
      uint32_t roiWord() const { return m_roiWord; }

   protected:
      int sizeCheck(int value, unsigned int size) const;
      int sizeCheckM(int value, unsigned int size) const;

      unsigned int sizeCheck(unsigned int value, unsigned int size) const;
      

   private:
      
      uint32_t m_roiWord;

   };
   
std::ostream & operator<< (std::ostream & o, const TCS::BaseTOB&);

} // end of namespace TCS


#endif /* defined(__TopoCore__BaseTOB__) */
