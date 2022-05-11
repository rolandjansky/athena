// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_jXETOBArray
#define L1TopoEvent_jXETOBArray

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/jXETOB.h"
#include <vector>

namespace TCS {
   
   class jXETOBArray : public InputTOBArray, public DataArrayImpl<jXETOB> {
   public:
      
      // default constructor
      jXETOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<jXETOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif