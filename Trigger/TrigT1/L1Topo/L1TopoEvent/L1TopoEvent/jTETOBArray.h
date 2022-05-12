// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_jTETOBArray
#define L1TopoEvent_jTETOBArray

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/jTETOB.h"
#include <vector>

namespace TCS {
   
   class jTETOBArray : public InputTOBArray, public DataArrayImpl<jTETOB> {
   public:
      
      // default constructor
      jTETOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<jTETOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
