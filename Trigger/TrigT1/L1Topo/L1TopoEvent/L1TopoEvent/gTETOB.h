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

      // constructor with initial values
      // input TOB type can be gTE
      gTETOB(unsigned int sumEt, inputTOBType_t tobType = NONE);

      // copy constructor
      gTETOB(const gTETOB & gte);

      // destructor
      virtual ~gTETOB();

      // accessors
      unsigned int sumEt() const { return m_sumEt; }

      double sumEtDouble() const { return m_sumEtDouble; }
      
      // setters
      void setSumEt(unsigned int sumEt) { m_sumEt = sumEt; }

      void setSumEtDouble(double sumEt) { m_sumEtDouble = sumEt; }
      
      // memory management
      static gTETOB* createOnHeap(const gTETOB& gte);
      static void clearHeap();
      static const Heap<TCS::gTETOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:

      unsigned int m_sumEt {0};

      double m_sumEtDouble {0};

      inputTOBType_t  m_tobType { NONE };

      static thread_local Heap<TCS::gTETOB> fg_heap;
   };
}

#endif
