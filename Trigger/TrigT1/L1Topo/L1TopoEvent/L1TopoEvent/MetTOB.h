//  MetTOB.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__MetTOB__
#define __TopoCore__MetTOB__

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class MetTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEx() { return g_nBitsEx; }
      static unsigned int nBitsEy() { return g_nBitsEy; }


      // default constructor
      MetTOB();
      
      // constructor with individual values
      MetTOB(int ex, int ey);

      // constructor with initial values
      MetTOB(const MetTOB & met);

      // destructor
      virtual ~MetTOB();

      // accessors
      int Ex() const { return m_Ex; }
      int Ey() const { return m_Ey; }

      double ExDouble() const { return m_ExDouble; }
      double EyDouble() const { return m_EyDouble; }
      
      // setters
      void setEx(int ex) { m_Ex = sizeCheck(ex, nBitsEx()); }
      void setEy(int ey) { m_Ex = sizeCheck(ey, nBitsEy()); }
      
      void setExDouble(double ex) { m_ExDouble = ex; }
      void setEyDouble(double ey) { m_EyDouble = ey; }
      
      // memory management
      static MetTOB* createOnHeap(const MetTOB& cl);
      static void clearHeap();
      static const Heap<TCS::MetTOB>& heap() { return fg_heap; }

      static unsigned int instances() { return fg_instances; }

      virtual void print(std::ostream &o) const;

   private:
      static unsigned int g_nBitsEx;
      static unsigned int g_nBitsEy;
      
      int m_Ex = { 0 };
      int m_Ey = { 0 };

      double m_ExDouble = { 0 };
      double m_EyDouble = { 0 };

      static unsigned int fg_instances;

      static Heap<TCS::MetTOB> fg_heap;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__MetTOB__) */
