/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JJETTOBARRAY_H
#define JJETTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/jJetTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class jJetTOBArray : public InputTOBArray, public DataArrayImpl<jJetTOB> {
   public:
      // default constructor
      jJetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<jJetTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
