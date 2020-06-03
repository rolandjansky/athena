/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetSortingAlgSize2.h
//  TopoCore
//  Created by Christoper Willis 12/6/12

#ifndef __TopoCore__JetSortingAlgSize2__
#define __TopoCore__JetSortingAlgSize2__

#include "L1TopoAlgorithms/JetSortingAlg.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class JetSortingAlgSize2 : public JetSortingAlg {
   public:
      
    // constructor
    JetSortingAlgSize2(const std::string & name);

    // destructor
    virtual ~JetSortingAlgSize2();
    
  private:
    
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__JetSortingAlgSize2__) */
