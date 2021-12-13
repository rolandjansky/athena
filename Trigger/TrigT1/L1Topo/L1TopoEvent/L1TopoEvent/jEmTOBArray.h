/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JEMTOBARRAY_H
#define JEMTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/jEmTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class jEmTOBArray : public InputTOBArray, public DataArrayImpl<jEmTOB> {
   public:
      // default constructor
      jEmTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<jEmTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
