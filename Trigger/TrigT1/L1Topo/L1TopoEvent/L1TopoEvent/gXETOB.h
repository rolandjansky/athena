// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_gXETOB
#define L1TopoEvent_gXETOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class gXETOB : public BaseTOB {
   public:
      
      // default constructor
      gXETOB();

      // constructor with initial values
      // input TOB type can be gXEJWOJ, gXENC, gXERHO or gMHT
      gXETOB(int ex, int ey, unsigned int et, inputTOBType_t tobType = NONE);

      // copy constructor
      gXETOB(const gXETOB & gxe);

      // destructor
      virtual ~gXETOB();

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
      
      // memory management
      static gXETOB* createOnHeap(const gXETOB& gxe);
      static void clearHeap();
      static const Heap<TCS::gXETOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }


   private:

      int m_Ex {0};
      int m_Ey {0};
      unsigned int m_Et {0};
      unsigned int m_Et2 {0};

      double m_ExDouble {0};
      double m_EyDouble {0};
      double m_EtDouble {0};

      inputTOBType_t  m_tobType { NONE };

      static thread_local Heap<TCS::gXETOB> fg_heap;
   };
}

#endif
