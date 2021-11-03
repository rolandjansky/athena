/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GLARGERJETTOBARRAY_H
#define GLARGERJETTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/gLargeRJetTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class gLargeRJetTOBArray : public InputTOBArray, public DataArrayImpl<gLargeRJetTOB> {
   public:
      // default constructor
      gLargeRJetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<gLargeRJetTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
