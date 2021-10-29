// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef JTAUTOB_H
#define JTAUTOB_H

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"


namespace TCS {
   
   class GenericTOB;
   
   class jTauTOB : public BaseTOB {
   public:
      
      // default constructor
      jTauTOB(uint32_t roiWord = 0, const std::string& tobName = "jTauTOB");

      // copy constructor
      jTauTOB(const jTauTOB & jtau);

      // constructor with initial values
      jTauTOB(unsigned int Et, unsigned int isolation, int eta, unsigned phi, uint32_t roiWord = 0, const std::string& tobName = "jTauTOB" );

      // destructor
      virtual ~jTauTOB();
      
      // accessors
      unsigned int energy() const { return m_Et; }
      unsigned int Et() const { return m_Et; }
      
      // accessors
      unsigned int isolation() const { return m_isolation; }    
      int eta() const { return m_eta; }
      unsigned phi() const { return m_phi; }
      
      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt(int energy) { m_Et = energy; }
      void setEta(int eta) { m_eta = eta; }
      void setPhi(int phi) { m_phi = phi; }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      inputTOBType_t tobType() const { return TCS::JTAU; }

      static jTauTOB* createOnHeap(const jTauTOB& jtau);
      static void clearHeap();

      static const Heap<TCS::jTauTOB>& heap() { return fg_heap; }

   private:

      static const unsigned int g_nBitsEt;
      static const unsigned int g_nBitsIsolation;
      static const unsigned int g_nBitsEta;
      static const unsigned int g_nBitsPhi;

      unsigned int m_Et { 0 };
      int m_eta { 0 };
      unsigned m_phi { 0 };

      unsigned int m_isolation {0};
      double m_EtDouble { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };

      virtual void print(std::ostream &o) const;

      static thread_local Heap<TCS::jTauTOB> fg_heap;
   };
   
}

#endif
