/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  BaseTOB.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
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
