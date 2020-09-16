// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_MetTOBArray
#define L1TopoEvent_MetTOBArray

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
   
}

#endif
