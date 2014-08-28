//  MuonTOB.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__MuonTOB__
#define __TopoCore__MuonTOB__

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class MuonTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEt() { return g_nBitsEt; }
      static unsigned int nBitsIsolation() { return g_nBitsIsolation; }
      static unsigned int nBitsEta() { return g_nBitsEta; }
      static unsigned int nBitsPhi() { return g_nBitsPhi; }


      // default constructor
      MuonTOB();
      
      // constructor with individual values
      MuonTOB(int et, int isolation, int eta, int phi);

      // constructor with initial values
      MuonTOB(const MuonTOB & cluster);

      // destructor
      virtual ~MuonTOB();

      // accessors
      unsigned int Et() const { return m_Et; }
      unsigned int isolation() const { return m_isolation; }
      unsigned int eta() const { return m_eta; }
      unsigned int phi() const { return m_phi; }

      double EtDouble() const { return m_EtDouble; }
      double EtaDouble() const { return m_etaDouble; }
      double PhiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setIsolation(int et) { m_isolation = sizeCheck(et, nBitsIsolation()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }
      
      // memory management
      static MuonTOB* createOnHeap(const MuonTOB& cl);
      static void clearHeap();
      static const Heap<TCS::MuonTOB>& heap() { return fg_heap; }

      static unsigned int instances() { return fg_instances; }

      virtual void print(std::ostream &o) const;

   private:
      static unsigned int g_nBitsEt;
      static unsigned int g_nBitsIsolation;
      static unsigned int g_nBitsEta;
      static unsigned int g_nBitsPhi;
      
      unsigned int m_Et;
      unsigned int m_isolation;
      unsigned int m_eta;
      unsigned int m_phi;

      double m_EtDouble;
      double m_etaDouble;
      double m_phiDouble;

      static unsigned int fg_instances;

      static Heap<TCS::MuonTOB> fg_heap;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__MuonTOB__) */
