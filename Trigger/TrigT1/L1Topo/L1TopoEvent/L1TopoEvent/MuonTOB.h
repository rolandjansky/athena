// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_MuonTOB
#define L1TopoEvent_MuonTOB

#include <iostream>
#include <string>

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
      MuonTOB(uint32_t roiWord = 0);
      
      // constructor with individual values
      MuonTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord = 0);

      // constructor with initial values
      MuonTOB(const MuonTOB & muon);

      // destructor
      virtual ~MuonTOB();

      // accessors
      unsigned int Et() const { return m_Et; }
      unsigned int isolation() const { return m_isolation; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }

      std::string sectorName() const { return m_sectorName; }
      bool bw2or3() const { return m_bw2or3; }
      bool innerCoin() const { return m_innerCoin; }
      bool goodMF() const { return m_goodMF; }
      int charge() const { return m_charge; }
      bool is2cand() const { return m_is2cand; }

      double EtDouble() const { return m_EtDouble; }
      double EtaDouble() const { return m_etaDouble; }
      double PhiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setIsolation(unsigned int et) { m_isolation = sizeCheck(et, nBitsIsolation()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setSectorName(std::string sectorName) { m_sectorName = sectorName; }
      void setBW2or2(bool bw2or3) { m_bw2or3 = bw2or3; }
      void setInnerCoin(bool innerCoin) { m_innerCoin = innerCoin; }
      void setGoodMF(bool goodMF) { m_goodMF = goodMF; }
      void setCharge(int charge) { m_charge = charge; }
      void setIs2cand(bool is2cand) { m_is2cand = is2cand; }

      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      inputTOBType_t tobType() const { return MUON; }
      
      // memory management
      static MuonTOB* createOnHeap(const MuonTOB& cl);
      static void clearHeap();
      static const Heap<TCS::MuonTOB>& heap() { return fg_heap; }

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

      std::string m_sectorName{""};
      bool m_bw2or3{0};
      bool m_innerCoin{0};
      bool m_goodMF{0};
      int m_charge{0};
      bool m_is2cand{0};

      double m_EtDouble{0};
      double m_etaDouble{0};
      double m_phiDouble{0};

      static thread_local Heap<TCS::MuonTOB> fg_heap;
   };
}

#endif
