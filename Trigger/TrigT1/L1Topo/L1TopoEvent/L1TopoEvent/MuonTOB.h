// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_MuonTOB
#define L1TopoEvent_MuonTOB

#include <iostream>
#include <string>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"

namespace TCS {
    
   class MuonTOB : public BaseTOB {
   public:
      
      // default constructor
      MuonTOB(uint32_t roiWord = 0, const std::string& tobName = "MuonTOB");
      
      // constructor with individual values
      MuonTOB(unsigned int et, unsigned int isolation, int eta, unsigned int phi, uint32_t roiWord = 0, const std::string& tobName = "MuonTOB");

      // constructor with initial values
      MuonTOB(const MuonTOB & muon);

      // destructor
      virtual ~MuonTOB();

      // accessors
      unsigned int nBitsEt() const { return m_nBitsEt; }
      unsigned int nBitsIsolation() const { return m_nBitsIsolation; }
      unsigned int nBitsEta() const { return m_nBitsEta; }
      unsigned int nBitsPhi() const { return m_nBitsPhi; }

      unsigned int Et() const { return m_Et; }                  // Et threshold in units of 100 MeV
      unsigned int isolation() const { return m_isolation; }
      int eta() const { return m_eta; }                         // Int eta for L1Topo, granularity = 0.025
      unsigned int phi() const { return m_phi; }                // Int phi for L1Topo, granularity = 0.05

      int bw2or3() const { return m_bw2or3; }
      int innerCoin() const { return m_innerCoin; }
      int goodMF() const { return m_goodMF; }
      int charge() const { return m_charge; }
      int is2cand() const { return m_is2cand; }

      double EtDouble() const { return m_EtDouble; }           // Et threshold in units of GeV
      double EtaDouble() const { return m_etaDouble; }         // Real muon eta
      double PhiDouble() const { return m_phiDouble; }         // Real muon phi
      
      inline bool isTGC() const {return m_isTGC;} // Additonal flag that identifies muons from TGC

      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setIsolation(unsigned int iso) { m_isolation = sizeCheck(iso, nBitsIsolation()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(unsigned int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setBW2or3(int bw2or3) { m_bw2or3 = bw2or3; }
      void setInnerCoin(int innerCoin) { m_innerCoin = innerCoin; }
      void setGoodMF(int goodMF) { m_goodMF = goodMF; }
      void setCharge(int charge) { m_charge = charge; }
      void setIs2cand(int is2cand) { m_is2cand = is2cand; }
      void setIsTGC(int isTGC) { m_isTGC = isTGC; }

      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      void setBitsEt(unsigned int nBitsEt) { m_nBitsEt = nBitsEt; }
      void setBitsIsolation(unsigned int nBitsIsolation) { m_nBitsIsolation = nBitsIsolation; }
      void setBitsEta(unsigned int nBitsEta) { m_nBitsEta = nBitsEta; }
      void setBitsPhi(unsigned int nBitsPhi) { m_nBitsPhi = nBitsPhi; }

      inputTOBType_t tobType() const { return MUON; }
      
      // memory management
      static MuonTOB* createOnHeap(const MuonTOB& cl);
      static void clearHeap();
      static const Heap<TCS::MuonTOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

   private:
      unsigned int m_nBitsEt{13};
      unsigned int m_nBitsIsolation{5};
      unsigned int m_nBitsEta{9};
      unsigned int m_nBitsPhi{7};

      unsigned int m_Et{0};
      unsigned int m_isolation{0};
      int m_eta{0};
      unsigned int m_phi{0};

      int m_bw2or3{0};
      int m_innerCoin{0};
      int m_goodMF{0};
      int m_charge{0};
      int m_is2cand{0};
      int m_isTGC{0};

      double m_EtDouble{0};
      double m_etaDouble{0};
      double m_phiDouble{0};

      static thread_local Heap<TCS::MuonTOB> fg_heap;
   };
}

#endif
