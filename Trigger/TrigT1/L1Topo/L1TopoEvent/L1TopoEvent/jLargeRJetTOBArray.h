/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JLARGERJETTOBARRAY_H
#define JLARGERJETTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/jLargeRJetTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class jLargeRJetTOBArray : public InputTOBArray, public DataArrayImpl<jLargeRJetTOB> {
   public:
      // default constructor
      jLargeRJetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<jLargeRJetTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
