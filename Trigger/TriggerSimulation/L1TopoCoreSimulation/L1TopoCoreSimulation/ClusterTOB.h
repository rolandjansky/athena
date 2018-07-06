/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ClusterTOB.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#ifndef __TopoCore__ClusterTOB__
#define __TopoCore__ClusterTOB__

#include <iostream>

#include "L1TopoCoreSimulation/BaseTOB.h"
#include "L1TopoCoreSimulation/Heap.h"

namespace TCS {
    
   class ClusterTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEt() { return g_nBitsEt; }
      static unsigned int nBitsIsolation() { return g_nBitsIsolation; }
      static unsigned int nBitsEta() { return g_nBitsEta; }
      static unsigned int nBitsPhi() { return g_nBitsPhi; }


      // default constructor
      ClusterTOB();
      
      // constructor with individual values
      ClusterTOB(int et, int isolation, int eta, int phi);

      // constructor with initial values
      ClusterTOB(const ClusterTOB & cluster);

      // destructor
      virtual ~ClusterTOB();

      // accessors
      int Et() const { return m_Et; }
      int isolation() const { return m_isolation; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }
      
      // setters
      void setEt(int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setIsolation(int et) { m_isolation = sizeCheck(et, nBitsIsolation()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      // memory management
      static ClusterTOB* createOnHeap(const ClusterTOB& cl);
      static void clearHeap();
      static const Heap<TCS::ClusterTOB>& heap() { return fg_heap; }

      static unsigned int instances() { return fg_instances; }

      virtual void print(std::ostream &o) const;

   private:
      static unsigned int g_nBitsEt;
      static unsigned int g_nBitsIsolation;
      static unsigned int g_nBitsEta;
      static unsigned int g_nBitsPhi;
      
      int m_Et;
      int m_isolation;
      int m_eta;
      int m_phi;

      static unsigned int fg_instances;

      static Heap<TCS::ClusterTOB> fg_heap;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__ClusterTOB__) */
