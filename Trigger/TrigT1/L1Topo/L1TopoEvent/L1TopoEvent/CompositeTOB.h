/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  CompositeTOB.h
//  TopoCore
//  Created by Joerg Stelzer on 11/18/12.

#ifndef __TopoCore__CompositeTOB__
#define __TopoCore__CompositeTOB__

// class to hold composite trigger objects, for example such that are created by
// a topological algorithm
// ownership: the composite TOB never owns the candidates that it is composed of

#include <iostream>
#include <vector>
#include "L1TopoEvent/GenericTOB.h"

namespace TCS {
   
   class CompositeTOB : public GenericTOB {
   public:
      typedef std::vector<TCS::GenericTOB*> data_t;
      typedef data_t::iterator              iterator;
      typedef data_t::const_iterator        const_iterator;
      
      // default constructor
      CompositeTOB();

      // copy constructor
      CompositeTOB(const CompositeTOB&);

      // constructor from a list of TOBs
      CompositeTOB(const std::vector<GenericTOB*> &);
      
      // constructors for convencience
      CompositeTOB(GenericTOB*);
      CompositeTOB(GenericTOB*, GenericTOB*);

      // destructor
      virtual ~CompositeTOB();

      static CompositeTOB* createOnHeap(const CompositeTOB& tob);
      static void clearHeap();

      data_t & components() { return m_components; }
      const data_t & components() const { return m_components; }
      
      virtual void print(std::ostream &o) const;

      static unsigned int instances() { return fg_instances; }

      static const Heap<TCS::CompositeTOB>& heap() { return fg_heap; }

   private:
      data_t m_components;

      static unsigned int fg_instances;

      static Heap<TCS::CompositeTOB> fg_heap;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__CompositeTOB__) */
