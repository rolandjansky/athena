/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  JetTOB.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#ifndef __TopoCore__JetTOB__
#define __TopoCore__JetTOB__

#include <iostream>

#include "L1TopoCoreSimulation/BaseTOB.h"
#include "L1TopoCoreSimulation/Heap.h"


// TODO implement sizecheck lile in ClusterTOB

namespace TCS {
   
   class GenericTOB;
   
   class JetTOB : public BaseTOB {
   public:
      
      enum JetSize { JS1, JS2 };
      
      // default constructor
      JetTOB();

      // copy constructor
      JetTOB(const JetTOB & jet);

      // constructor with initial values
      JetTOB(int Et1, int Et2, int eta, int phi);

      // destructor
      virtual ~JetTOB();
      
      // accessors
      // TODO default value to be removed
      int energy(JetSize js = JS1) const { return js==JS1?m_Et1:m_Et2; }
      int Et(JetSize js = JS1) const { return js==JS1?m_Et1:m_Et2; }
      
      // accessors
      int Et1() const { return m_Et1; }
      int Et2() const { return m_Et2; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }
      
      // setters
      void setEt1(int energy) { m_Et1 = energy; }
      void setEt2(int energy) { m_Et2 = energy; }
      void setEta(int eta) { m_eta = eta; }
      void setPhi(int phi) { m_phi = phi; }
      
      static JetTOB* createOnHeap(const JetTOB& jet);
      static void clearHeap();

      static const Heap<TCS::JetTOB>& heap() { return fg_heap; }

      static unsigned int instances() { return fg_instances; }

   private:

      static unsigned int g_nBitsEt;
      static unsigned int g_nBitsIsolation;
      static unsigned int g_nBitsEta;
      static unsigned int g_nBitsPhi;

      int m_Et1;
      int m_Et2;
      int m_eta;
      int m_phi;

      virtual void print(std::ostream &o) const;

      static unsigned int fg_instances;

      static Heap<TCS::JetTOB> fg_heap;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__JetTOB__) */
