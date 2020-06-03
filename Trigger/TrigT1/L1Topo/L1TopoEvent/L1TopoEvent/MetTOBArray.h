/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MetTOBArray.h
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.

#ifndef __TopoCore__MetTOBArray__
#define __TopoCore__MetTOBArray__

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/MetTOB.h"
#include <vector>

namespace TCS {
   
   class MetTOBArray : public InputTOBArray, public DataArrayImpl<MetTOB> {
   public:
      
      // default constructor
      MetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<MetTOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__MetTOBArray__) */
