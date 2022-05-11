// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_cTauTOB
#define L1TopoEvent_cTauTOB

#include "L1TopoEvent/Heap.h"
#include "L1TopoEvent/BaseTOB.h"

#include <iostream>

namespace TCS {
    
   class cTauTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEt() { return g_nBitsEt; }
      static unsigned int nBitsEta() { return g_nBitsEta; }
      static unsigned int nBitsPhi() { return g_nBitsPhi; }

      // default constructor
      cTauTOB(uint32_t roiWord = 0, const std::string& tobName = "cTauTOB");
      
      // constructor with individual values
      cTauTOB(unsigned int et, int eta, unsigned int phi, inputTOBType_t tobType = NONE, uint32_t roiWord = 0, const std::string& tobName = "cTauTOB");

      // constructor with initial values
      cTauTOB(const cTauTOB & ctau);

      // destructor
      virtual ~cTauTOB();

      // accessors
      unsigned int Et() const { return m_Et; }                  // Et in units of 100 MeV
      int eta() const { return m_eta; }                         // eta in units of 0.025
      unsigned int phi() const { return m_phi; }                // phi in units of 0.05

      double EtDouble() const { return m_EtDouble; }            // float Et in units of GeV
      double etaDouble() const { return m_etaDouble; }          // float eta with granularity 0.025
      double phiDouble() const { return m_phiDouble; }          // float phi with granularity 0.05
      
      unsigned int RCore() const { return m_rCore; }            // eTau isolation
      unsigned int RHad() const { return m_rHad; }              // eTau isolation
    
      unsigned int EtIso() const { return m_EtIso; }            // jTau isolation energy 

      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(unsigned int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }
     
      void setRCore(unsigned int rCore) { m_rCore = rCore; }
      void setRHad(unsigned int rHad) { m_rHad = rHad; }
     
      void setEtIso(unsigned int etIso) { m_EtIso = etIso; }

      // memory management
      static cTauTOB* createOnHeap(const cTauTOB& ctau);
      static void clearHeap();
      static const Heap<TCS::cTauTOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:
      static const unsigned int g_nBitsEt;
      static const unsigned int g_nBitsIsolation;
      static const unsigned int g_nBitsEta;
      static const unsigned int g_nBitsPhi;
      
      unsigned int m_Et {0};
      int m_eta {0};
      unsigned int m_phi {0};

      double m_EtDouble {0};
      double m_etaDouble {0};
      double m_phiDouble {0};

      unsigned int m_rCore {0};
      unsigned int m_rHad {0};

      unsigned int m_EtIso {0};

      inputTOBType_t  m_tobType { NONE };

      static thread_local Heap<TCS::cTauTOB> fg_heap;
   };
}

#endif
