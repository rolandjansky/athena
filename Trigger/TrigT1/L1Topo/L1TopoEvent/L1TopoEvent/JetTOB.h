// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef TopoCore_JetTOB
#define TopoCore_JetTOB

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"


// TODO implement sizecheck lile in ClusterTOB

namespace TCS {
   
   class GenericTOB;
   
   class JetTOB : public BaseTOB {
   public:
      
      enum JetSize { JS1, JS2 };
      
      // default constructor
      JetTOB(uint32_t roiWord = 0);

      // copy constructor
      JetTOB(const JetTOB & jet);

      // constructor with initial values
      JetTOB(unsigned int Et1, unsigned int Et2, int eta, int phi, uint32_t roiWord = 0 );

      // destructor
      virtual ~JetTOB();
      
      // accessors
      int energy(JetSize js ) const { return js==JS1?m_Et1:m_Et2; }
      int Et(JetSize js ) const { return js==JS1?m_Et1:m_Et2; }
      
      // accessors
      unsigned int Et1()      const { return m_Et1; }
      unsigned int EtWide()   const { return m_Et1; }
      unsigned int Et2()      const { return m_Et2; }
      unsigned int EtNarrow() const { return m_Et2; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }
      
      double EtDouble(JetSize js ) const { return js==JS1?m_Et1Double:m_Et2Double; }
      double Et1Double() const { return m_Et1Double; }
      double Et2Double() const { return m_Et2Double; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt1(int energy) { m_Et1 = energy; }
      void setEtWide(int energy) { m_Et1 = energy; }
      void setEt2(int energy) { m_Et2 = energy; }
      void setEtNarrow(int energy) { m_Et2 = energy; }
      void setEta(int eta) { m_eta = eta; }
      void setPhi(int phi) { m_phi = phi; }
      
      void setEtWideDouble(double et) { m_Et1Double = et; }
      void setEtNarrowDouble(double et) { m_Et2Double = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      inputTOBType_t tobType() const { return JET; }

      static JetTOB* createOnHeap(const JetTOB& jet);
      static void clearHeap();

      static const Heap<TCS::JetTOB>& heap() { return fg_heap; }

   private:

      static const unsigned int g_nBitsEt;
      static const unsigned int g_nBitsIsolation;
      static const unsigned int g_nBitsEta;
      static const unsigned int g_nBitsPhi;

      unsigned int m_Et1 { 0 };
      unsigned int m_Et2 { 0 };
      int m_eta { 0 };
      int m_phi { 0 };

      double m_Et1Double { 0 };
      double m_Et2Double { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };

      virtual void print(std::ostream &o) const;

      static thread_local Heap<TCS::JetTOB> fg_heap;
   };
   
}

#endif
