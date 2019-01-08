/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  InputTOBArray.h
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.

#ifndef __TopoCore__InputTOBArray__
#define __TopoCore__InputTOBArray__

#include <iostream>
#include "L1TopoEvent/DataArray.h"

namespace TCS {
   class InputTOBArray : public DataArray {
   public:
      // constructor
      InputTOBArray(const std::string & name) :
         DataArray(name)
      {}
      
      virtual unsigned int size() const = 0;
      

   private:
   };
   
}

#endif
