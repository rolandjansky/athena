/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  LateMuonTOBArray.h
//  TopoCore
//  Based on MuonTOBArray Created by Joerg Stelzer on 11/17/12.

#ifndef __TopoCore__LateMuonTOBArray__
#define __TopoCore__LateMuonTOBArray__

#include <iostream>
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/LateMuonTOB.h"
#include <vector>

namespace TCS {
   
   class LateMuonTOBArray : public InputTOBArray, public DataArrayImpl<LateMuonTOB> {
   public:
      
      // default constructor
      LateMuonTOBArray(const std::string & name, unsigned int reserve);

      virtual unsigned int size() const { return DataArrayImpl<LateMuonTOB>::size(); }
      
   private:
      virtual void print(std::ostream&) const;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__LateMuonTOBArray__) */
