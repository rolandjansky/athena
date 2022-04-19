// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_gMHTTOB
#define L1TopoEvent_gMHTTOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class gMHTTOB : public BaseTOB {
   public:
      
      // default constructor
      gMHTTOB();

      // constructor with individual values
      gMHTTOB(int ex, int ey, unsigned int et);

      // constructor with initial values
      gMHTTOB(const gMHTTOB & gmht);

      // destructor
      virtual ~gMHTTOB();

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
      
      inputTOBType_t tobType() const { return GMHT; }    

      // memory management
      static gMHTTOB* createOnHeap(const gMHTTOB& gmht);
      static void clearHeap();
      static const Heap<TCS::gMHTTOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:

      int m_Ex {0};
      int m_Ey {0};
      unsigned int m_Et {0};
      unsigned int m_Et2 {0};

      double m_ExDouble {0};
      double m_EyDouble {0};
      double m_EtDouble {0};

      static thread_local Heap<TCS::gMHTTOB> fg_heap;
   };
}

#endif
