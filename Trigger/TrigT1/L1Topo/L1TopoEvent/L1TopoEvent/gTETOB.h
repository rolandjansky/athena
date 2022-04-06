// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_gTETOB
#define L1TopoEvent_gTETOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class gTETOB : public BaseTOB {
   public:
      
      // default constructor
      gTETOB();

      // constructor with individual values
      gTETOB(int sumEt);

      // constructor with initial values
      gTETOB(const gTETOB & gte);

      // destructor
      virtual ~gTETOB();

      // accessors
      int sumEt() const { return m_sumEt; }

      double sumEtDouble() const { return m_sumEtDouble; }
      
      // setters
      void setSumEt(int sumEt) { m_sumEt = sumEt; }

      void setSumEtDouble(double sumEt) { m_sumEtDouble = sumEt; }
      
      inputTOBType_t tobType() const { return GTE; }    

      // memory management
      static gTETOB* createOnHeap(const gTETOB& gte);
      static void clearHeap();
      static const Heap<TCS::gTETOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:

      int m_sumEt {0};

      double m_sumEtDouble {0};

      static thread_local Heap<TCS::gTETOB> fg_heap;
   };
}

#endif
