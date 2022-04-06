// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_jTETOB
#define L1TopoEvent_jTETOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class jTETOB : public BaseTOB {
   public:
      
      // default constructor
      jTETOB();

      // constructor with individual values
      jTETOB(int sumEt);

      // constructor with initial values
      jTETOB(const jTETOB & jte);

      // destructor
      virtual ~jTETOB();

      // accessors
      int sumEt() const { return m_sumEt; }

      double sumEtDouble() const { return m_sumEtDouble; }
      
      // setters
      void setSumEt(int sumEt) { m_sumEt = sumEt; }

      void setSumEtDouble(double sumEt) { m_sumEtDouble = sumEt; }
      
      inputTOBType_t tobType() const { return JTE; }    

      // memory management
      static jTETOB* createOnHeap(const jTETOB& jte);
      static void clearHeap();
      static const Heap<TCS::jTETOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:

      int m_sumEt {0};

      double m_sumEtDouble {0};

      static thread_local Heap<TCS::jTETOB> fg_heap;
   };
}

#endif
