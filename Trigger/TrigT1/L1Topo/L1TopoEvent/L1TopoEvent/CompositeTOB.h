// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_CompositeTOB
#define L1TopoEvent_CompositeTOB

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

      static const Heap<TCS::CompositeTOB>& heap() { return fg_heap; }

   private:
      data_t m_components;

      static Heap<TCS::CompositeTOB> fg_heap;
   };
}

#endif
