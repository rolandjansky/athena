// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_eTauTOB
#define L1TopoEvent_eTauTOB

#include "L1TopoEvent/Heap.h"
#include "L1TopoEvent/BaseTOB.h"

#include <iostream>

namespace TCS {
    
   class eTauTOB : public BaseTOB {
   public:
      
      static unsigned int nBitsEt() { return g_nBitsEt; }
      static unsigned int nBitsEta() { return g_nBitsEta; }
      static unsigned int nBitsPhi() { return g_nBitsPhi; }

      // default constructor
      eTauTOB(uint32_t roiWord = 0, const std::string& tobName = "eTauTOB");
      
      // constructor with individual values
      eTauTOB(unsigned int et, int eta, unsigned int phi, inputTOBType_t tobType = NONE, uint32_t roiWord = 0, const std::string& tobName = "eTauTOB");

      // constructor with initial values
      eTauTOB(const eTauTOB & eem);

      // destructor
      virtual ~eTauTOB();

      // accessors
      unsigned int Et() const { return m_Et; }                  // Et in units of 100 MeV
      int eta() const { return m_eta; }                         // eta in units of 0.025
      unsigned int phi() const { return m_phi; }                // phi in units of 0.05

      double EtDouble() const { return m_EtDouble; }            // float Et in units of GeV
      double etaDouble() const { return m_etaDouble; }          // float eta with granularity 0.025
      double phiDouble() const { return m_phiDouble; }          // float phi with granularity 0.05
      
      unsigned int rCore() const { return m_rCore; }            // rCore isolation
      unsigned int rHad() const { return m_rHad; }              // rHad isolation

      // setters
      void setEt(unsigned int et) { m_Et = sizeCheck(et, nBitsEt()); }
      void setEta(int eta) { m_eta = sizeCheck(eta, nBitsEta()); }
      void setPhi(unsigned int phi) { m_phi = sizeCheck(phi, nBitsPhi()); }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      void setRCore(unsigned int th) { m_rCore = th; }
      void setRHad(unsigned int th) { m_rHad = th; }
     
      // memory management
      static eTauTOB* createOnHeap(const eTauTOB& eem);
      static void clearHeap();
      static const Heap<TCS::eTauTOB>& heap() { return fg_heap; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:
      static const unsigned int g_nBitsEt;
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

      inputTOBType_t  m_tobType { NONE };

      static thread_local Heap<TCS::eTauTOB> fg_heap;
   };
}

#endif
