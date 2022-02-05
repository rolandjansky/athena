// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef CTAUTOBARRAY_H
#define CTAUTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/cTauTOB.h"
#include <vector>

namespace TCS {
   
   class cTauTOBArray : public InputTOBArray, public DataArrayImpl<cTauTOB> {
   public:
      
      // default constructor
      cTauTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<cTauTOB>::size(); }

   private:
      virtual void print(std::ostream&) const;
   };
   
}

#endif
