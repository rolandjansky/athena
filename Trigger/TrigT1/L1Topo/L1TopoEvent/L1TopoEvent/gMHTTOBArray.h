// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_gMHTTOBArray
#define L1TopoEvent_gMHTTOBArray

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/gMHTTOB.h"
#include <vector>

namespace TCS {
   
   class gMHTTOBArray : public InputTOBArray, public DataArrayImpl<gMHTTOB> {
   public:
      
      // default constructor
      gMHTTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<gMHTTOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
