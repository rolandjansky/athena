// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_gXETOBArray
#define L1TopoEvent_gXETOBArray

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/gXETOB.h"
#include <vector>

namespace TCS {
   
   class gXETOBArray : public InputTOBArray, public DataArrayImpl<gXETOB> {
   public:
      
      // default constructor
      gXETOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<gXETOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
