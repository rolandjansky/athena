/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  JetTOBArray.h
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.

#ifndef __TopoCore__JetTOBArray__
#define __TopoCore__JetTOBArray__

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/JetTOB.h"
#include <vector>

namespace TCS {

   class TOBArray;

   class JetTOBArray : public InputTOBArray, public DataArrayImpl<JetTOB> {
   public:
      // default constructor
      JetTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<JetTOB>::size(); }

      TOBArray asTOBArray(JetTOB::JetSize jetsize) const;

   private:      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;
   };
   
}

#endif
