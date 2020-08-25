// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_MetTOB
#define L1TopoEvent_MetTOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class MetTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEx() { return g_nBitsEx; }
      static unsigned int nBitsEy() { return g_nBitsEy; }
      static unsigned int nBitsEt() { return g_nBitsEt; }


      // default constructor
      MetTOB();
      
      // constructor with individual values
      MetTOB(int ex, int ey, unsigned int et);

      // constructor with initial values
      MetTOB(const MetTOB & met);

      // destructor
      virtual ~MetTOB();

      // accessors
      int Ex() const { return m_Ex; }
      int Ey() const { return m_Ey; }
      unsigned int Et() const { return m_Et; }

      double ExDouble() const { return m_ExDouble; }
      double EyDouble() const { return m_EyDouble; }
      double EtDouble() const { return m_EtDouble; }
      
      // setters
      void setEx(int ex) { m_Ex = sizeCheckM(ex, nBitsEx()); }
      void setEy(int ey) { m_Ey = sizeCheckM(ey, nBitsEy()); }
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      
      void setExDouble(double ex) { m_ExDouble = ex; }
      void setEyDouble(double ey) { m_EyDouble = ey; }
      void setEtDouble(double et) { m_EtDouble = et; }
      
      inputTOBType_t tobType() const { return MET; }

      // memory management
      static MetTOB* createOnHeap(const MetTOB& cl);
      static void clearHeap();
      static const Heap<TCS::MetTOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:
      static const unsigned int g_nBitsEx;
      static const unsigned int g_nBitsEy;
      static const unsigned int g_nBitsEt;
      
      int m_Ex {0};
      int m_Ey {0};
      unsigned int m_Et {0};

      double m_ExDouble {0};
      double m_EyDouble {0};
      double m_EtDouble {0};

      static Heap<TCS::MetTOB> fg_heap;
   };
}

#endif
