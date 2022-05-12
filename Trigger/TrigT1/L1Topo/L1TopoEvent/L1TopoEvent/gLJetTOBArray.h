/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GLJETTOBARRAY_H
#define GLJETTOBARRAY_H

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/gLJetTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class gLJetTOBArray : public InputTOBArray, public DataArrayImpl<gLJetTOB> {
   public:
      // default constructor
      gLJetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<gLJetTOB>::size(); }

      TOBArray asTOBArray() const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
