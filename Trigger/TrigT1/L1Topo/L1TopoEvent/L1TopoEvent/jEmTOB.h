// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef JEMTOB_H
#define JEMTOB_H

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"


namespace TCS {
   
   class GenericTOB;
   
   class jEmTOB : public BaseTOB {
   public:
      
      // default constructor
      jEmTOB(uint32_t roiWord = 0, const std::string& tobName = "jEm");

      // copy constructor
      jEmTOB(const jEmTOB & cluster);

      // constructor with initial values
      jEmTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord = 0, const std::string& tobName = "jEmTOB" );

      // destructor
      virtual ~jEmTOB();
      
      // accessors
      unsigned int Et() const { return m_Et; }
      int eta() const { return m_eta; }
      unsigned phi() const { return m_phi; }
      
      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }

      unsigned int isolation() const { return m_isolation; }
      unsigned int frac1() const { return m_frac1; }
      unsigned int frac2() const { return m_frac2; }
      
      // setters
      void setEt(int et) { m_Et = et; }
      void setEta(int eta) { m_eta = eta; }
      void setPhi(int phi) { m_phi = phi; }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      void setIsolation(unsigned int isolation) { m_isolation = isolation; }
      void setFrac1(unsigned int frac1) { m_frac1 = frac1; }
      void setFrac2(unsigned int frac2) { m_frac2 = frac2; }

      static jEmTOB* createOnHeap(const jEmTOB& cluster);
      static void clearHeap();

      static const Heap<TCS::jEmTOB>& heap() { return fg_heap; }

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:

      unsigned int m_Et { 0 };
      int m_eta { 0 };
      unsigned m_phi { 0 };

      double m_EtDouble { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };

      unsigned int m_isolation{ 0 };
      unsigned int m_frac1{ 0 };
      unsigned int m_frac2{ 0 };

      virtual void print(std::ostream &o) const;

      inputTOBType_t  m_tobType { NONE };

      static thread_local Heap<TCS::jEmTOB> fg_heap;
   };
   
}

#endif
