/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GJETTOBARRAY_H
#define GJETTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/gJetTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class gJetTOBArray : public InputTOBArray, public DataArrayImpl<gJetTOB> {
   public:
      // default constructor
      gJetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<gJetTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
