// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_jXETOB
#define L1TopoEvent_jXETOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class jXETOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEx() { return g_nBitsEx; }
      static unsigned int nBitsEy() { return g_nBitsEy; }
      static unsigned int nBitsEt() { return g_nBitsEt; }


      // default constructor
      jXETOB();

      // constructor with individual values
      jXETOB(int ex, int ey, unsigned int et);

      // constructor with initial values
      jXETOB(const jXETOB & jxe);

      // destructor
      virtual ~jXETOB();

      // accessors
      int Ex() const { return m_Ex; }
      int Ey() const { return m_Ey; }
      unsigned int Et() const { return m_Et; }
      unsigned int Et2() const {return m_Et2; }

      double ExDouble() const { return m_ExDouble; }
      double EyDouble() const { return m_EyDouble; }
      double EtDouble() const { return m_EtDouble; }
      
      // setters
      void setEx(int ex) { m_Ex = ex; }
      void setEy(int ey) { m_Ey = ey; }
      void setEt(unsigned int et) { m_Et = et; }
      void setEt2(unsigned int et2) { m_Et2 = et2; }
      
      void setExDouble(double ex) { m_ExDouble = ex; }
      void setEyDouble(double ey) { m_EyDouble = ey; }
      void setEtDouble(double et) { m_EtDouble = et; }
      
      inputTOBType_t tobType() const { return JXE; }    

      // memory management
      static jXETOB* createOnHeap(const jXETOB& cl);
      static void clearHeap();
      static const Heap<TCS::jXETOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:
      static const unsigned int g_nBitsEx;
      static const unsigned int g_nBitsEy;
      static const unsigned int g_nBitsEt;
      
      int m_Ex {0};
      int m_Ey {0};
      unsigned int m_Et {0};
      unsigned int m_Et2 {0};

      double m_ExDouble {0};
      double m_EyDouble {0};
      double m_EtDouble {0};

      static thread_local Heap<TCS::jXETOB> fg_heap;
   };
}

#endif
