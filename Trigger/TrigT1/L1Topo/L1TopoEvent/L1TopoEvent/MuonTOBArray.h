/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MuonTOBArray.h
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.

#ifndef __TopoCore__MuonTOBArray__
#define __TopoCore__MuonTOBArray__

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/MuonTOB.h"
#include <vector>

namespace TCS {
   
   class MuonTOBArray : public InputTOBArray, public DataArrayImpl<MuonTOB> {
   public:
      
      // default constructor
      MuonTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<MuonTOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__MuonTOBArray__) */
