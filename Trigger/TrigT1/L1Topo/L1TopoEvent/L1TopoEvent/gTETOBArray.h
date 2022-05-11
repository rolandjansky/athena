// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_gTETOBArray
#define L1TopoEvent_gTETOBArray

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/gTETOB.h"
#include <vector>

namespace TCS {
   
   class gTETOBArray : public InputTOBArray, public DataArrayImpl<gTETOB> {
   public:
      
      // default constructor
      gTETOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<gTETOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
