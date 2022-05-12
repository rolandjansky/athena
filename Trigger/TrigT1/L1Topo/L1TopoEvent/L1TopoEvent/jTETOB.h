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

      // constructor with initial values
      // input TOB type can be jTE, jTEC, jTEFWD, jTEFWDA, jTEFWDC
      jTETOB(unsigned int sumEt, inputTOBType_t tobType = NONE);

      // copy constructor
      jTETOB(const jTETOB & jte);

      // destructor
      virtual ~jTETOB();

      // accessors
      unsigned int sumEt() const { return m_sumEt; }

      double sumEtDouble() const { return m_sumEtDouble; }
      
      // setters
      void setSumEt(unsigned int sumEt) { m_sumEt = sumEt; }

      void setSumEtDouble(double sumEt) { m_sumEtDouble = sumEt; }
      
      // memory management
      static jTETOB* createOnHeap(const jTETOB& jte);
      static void clearHeap();
      static const Heap<TCS::jTETOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:

      unsigned int m_sumEt {0};

      double m_sumEtDouble {0};

      inputTOBType_t  m_tobType { NONE };

      static thread_local Heap<TCS::jTETOB> fg_heap;
   };
}

#endif
