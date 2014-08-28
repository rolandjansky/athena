//  ClusterTOBArray.h
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__ClusterTOBArray__
#define __TopoCore__ClusterTOBArray__

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/ClusterTOB.h"
#include <vector>

namespace TCS {
   
   class ClusterTOBArray : public InputTOBArray, public DataArrayImpl<ClusterTOB> {
   public:
      
      // default constructor
      ClusterTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<ClusterTOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
