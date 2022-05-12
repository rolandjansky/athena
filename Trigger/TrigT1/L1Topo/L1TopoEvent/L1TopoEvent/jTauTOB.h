// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
      jTauTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord = 0, const std::string& tobName = "jTauTOB" );

      // destructor
      virtual ~jTauTOB();
      
      // accessors
      unsigned int Et() const { return m_Et; }
      int eta() const { return m_eta; }
      unsigned phi() const { return m_phi; }
      
      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }
 
      unsigned int EtIso() const { return m_EtIso; } // jTau isolation energy 

      // setters
      void setEt(int et) { m_Et = et; }
      void setEta(int eta) { m_eta = eta; }
      void setPhi(int phi) { m_phi = phi; }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      void setEtIso(unsigned int etIso) { m_EtIso = etIso; }

      inputTOBType_t tobType() const { return TCS::JTAU; }

      static jTauTOB* createOnHeap(const jTauTOB& jtau);
      static void clearHeap();

      static const Heap<TCS::jTauTOB>& heap() { return fg_heap; }

   private:

      static const unsigned int g_nBitsEt;
      static const unsigned int g_nBitsEta;
      static const unsigned int g_nBitsPhi;

      unsigned int m_Et { 0 };
      int m_eta { 0 };
      unsigned m_phi { 0 };

      double m_EtDouble { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };

      unsigned int m_EtIso { 0 };

      virtual void print(std::ostream &o) const;

      static thread_local Heap<TCS::jTauTOB> fg_heap;
   };
   
}

#endif
