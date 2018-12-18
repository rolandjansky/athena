/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetSortingAlgSize1.h
//  TopoCore
//  Created by Christoper Willis 12/6/12

#ifndef TCS_JetSortingAlgSize1
#define TCS_JetSortingAlgSize1

#include "L1TopoAlgorithms/JetSortingAlg.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetSortingAlgSize1 : public JetSortingAlg {
   public:
      
      // constructor
      JetSortingAlgSize1(const std::string & name);

      // destructor
      virtual ~JetSortingAlgSize1();
    
   private:
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__JetSortingAlgSize1__) */
