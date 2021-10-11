/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JTAUTOBARRAY_H
#define JTAUTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/jTauTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class jTauTOBArray : public InputTOBArray, public DataArrayImpl<jTauTOB> {
   public:
      // default constructor
      jTauTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<jTauTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
