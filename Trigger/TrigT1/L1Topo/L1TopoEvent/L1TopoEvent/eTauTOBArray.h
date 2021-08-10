// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef ETAUTOBARRAY_H
#define ETAUTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/eTauTOB.h"
#include <vector>

namespace TCS {
   
   class eTauTOBArray : public InputTOBArray, public DataArrayImpl<eTauTOB> {
   public:
      
      // default constructor
      eTauTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<eTauTOB>::size(); }

   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
