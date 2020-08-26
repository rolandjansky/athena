// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_MuonNextBCTOB
#define L1TopoEvent_MuonNextBCTOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class MuonNextBCTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEt() { return g_nBitsEt; }
      static unsigned int nBitsIsolation() { return g_nBitsIsolation; }
      static unsigned int nBitsEta() { return g_nBitsEta; }
      static unsigned int nBitsPhi() { return g_nBitsPhi; }


      // default constructor
      MuonNextBCTOB(uint32_t roiWord = 0);
      
      // constructor with individual values
      MuonNextBCTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord = 0);

      // constructor with initial values
      MuonNextBCTOB(const MuonNextBCTOB & muonNextBC);

      // destructor
      virtual ~MuonNextBCTOB();

      // accessors
      unsigned int Et() const { return m_Et; }
      unsigned int isolation() const { return m_isolation; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }

      double EtDouble() const { return m_EtDouble; }
      double EtaDouble() const { return m_etaDouble; }
      double PhiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setIsolation(unsigned int et) { m_isolation = sizeCheck(et, nBitsIsolation()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      inputTOBType_t tobType() const { return MUONNEXTBC; }
      
      // memory management
      static MuonNextBCTOB* createOnHeap(const MuonNextBCTOB& cl);
      static void clearHeap();
      static const Heap<TCS::MuonNextBCTOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:
      static const unsigned int g_nBitsEt;
      static const unsigned int g_nBitsIsolation;
      static const unsigned int g_nBitsEta;
      static const unsigned int g_nBitsPhi;
      
      unsigned int m_Et{0};
      unsigned int m_isolation{0};
      int m_eta{0};
      int m_phi{0};

      double m_EtDouble{0};
      double m_etaDouble{0};
      double m_phiDouble{0};

      static thread_local Heap<TCS::MuonNextBCTOB> fg_heap;
   };
}

#endif
