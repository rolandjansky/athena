// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef __TopoCore__eEmTOBArray__
#define __TopoCore__eEmTOBArray__

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/eEmTOB.h"
#include <vector>

namespace TCS {
   
   class eEmTOBArray : public InputTOBArray, public DataArrayImpl<eEmTOB> {
   public:
      
      // default constructor
      eEmTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<eEmTOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
