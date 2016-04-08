//
//  BaseTOB.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#ifndef __TopoCore__BaseTOB__
#define __TopoCore__BaseTOB__

#include <iostream>

namespace TCS {
   
   class BaseTOB {
   public:
      virtual void print(std::ostream&) const = 0;
      
   protected:
      int sizeCheck(int value, unsigned int size);
   };
   
} // end of namespace TCS

std::ostream & operator<< (std::ostream & o, const TCS::BaseTOB&);

#endif /* defined(__TopoCore__BaseTOB__) */
