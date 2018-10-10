/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  TOBArray.h
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.

#ifndef __TopoCore__TOBArray__
#define __TopoCore__TOBArray__

#include "L1TopoEvent/DataArrayImpl.h"
#include "L1TopoEvent/GenericTOB.h"

#include <iostream>
#include <vector>

namespace TCS {

   class GenericTOB;
   class CompositeTOB;

   typedef bool (sort_fnc)(GenericTOB* tob1, GenericTOB* tob2);

   class TOBArray : virtual public DataArray, virtual public DataArrayImpl<GenericTOB> {
   public:

      // default constructor
      TOBArray(const std::string & name = "TOBArray") :
         DataArray(name)
      {};

      using DataArrayImpl<GenericTOB>::push_back;
      void push_back(const CompositeTOB& tob);

      void sort(sort_fnc);

   private:
      // print method (can be invoked via <<)
      virtual void print(std::ostream&) const;
   };
   
} // end of namespace TCS

std::ostream & operator<< (std::ostream &, const TCS::TOBArray&);


#endif /* defined(__TopoCore__TOBArray__) */
