// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_GeneralTOP
#define L1TopoEvent_GeneralTOP

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/JetTOB.h"
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/MuonTOB.h"
#include "L1TopoEvent/LateMuonTOB.h"
#include "L1TopoEvent/MuonNextBCTOB.h"
#include "L1TopoEvent/MetTOB.h"


// TODO implement sizecheck lile in ClusterTOB

namespace TCS {
   
   class GenericTOB : public BaseTOB {
   public:

      // default constructor
      GenericTOB(uint32_t roiWord = 0);

      // constructor from individual values
      GenericTOB(unsigned int Et, int eta, int phi, uint32_t roiWord = 0);

      // copy constructor
      GenericTOB(const GenericTOB & other);

      // constructor from jet
      GenericTOB(const JetTOB & jet, JetTOB::JetSize jetSize);

      // constructor from cluster
      GenericTOB(const ClusterTOB & cluster);

      // constructor from muon
      GenericTOB(const MuonTOB & muon);
      
      // constructor from lateMuon
      GenericTOB(const LateMuonTOB & lateMuon);

      // constructor from muonNextBC
      GenericTOB(const MuonNextBCTOB & muonNextBC);

      // constructor from met
      GenericTOB(const MetTOB & met);

      // destructor
      ~GenericTOB();

      static GenericTOB* createOnHeap(const GenericTOB &);
      static void clearHeap();

      static const Heap<TCS::GenericTOB>& heap() { return fg_heap; }

   public:
      unsigned int Et() const { return m_Et; }
      unsigned int EtWide() const { return m_EtWide; }
      unsigned int EtNarrow() const { return m_EtNarrow; }

      int Ex() const { return m_Ex; }
      int Ey() const { return m_Ey; }

      int eta() const { return m_eta; }
      int phi() const { return m_phi; }
      
      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }

      virtual void print(std::ostream &o) const;

      void setTobType(inputTOBType_t tobType) { m_tobType = tobType; }

      inputTOBType_t tobType() const { return m_tobType; }

   private:
      unsigned int m_Et { 0 };
      unsigned int m_EtNarrow { 0 };
      unsigned int m_EtWide { 0 };

      int m_Ex { 0 };
      int m_Ey { 0 };

      int m_eta { 0 };
      int m_phi { 0 };

      double m_EtDouble { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };
      
      inputTOBType_t   m_tobType { NONE };

      static Heap<TCS::GenericTOB> fg_heap;
   };
   
} // end of namespace TCS


#endif /* defined(__TopoCore__GeneralTOP__) */
