//  GeneralTOP.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __TopoCore__GeneralTOP__
#define __TopoCore__GeneralTOP__

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/JetTOB.h"
#include "L1TopoEvent/ClusterTOB.h"

// TODO implement sizecheck lile in ClusterTOB

namespace TCS {
   
   class GenericTOB : public BaseTOB {
   public:

      // default constructor
      GenericTOB();

      // constructor from individual values
      GenericTOB(int Et, int eta, int phi);

      // copy constructor
      GenericTOB(const GenericTOB & other);

      // constructor from jet
      GenericTOB(const JetTOB & jet, JetTOB::JetSize jetSize);

      // constructor from cluster
      GenericTOB(const ClusterTOB & cluster);

      // destructor
      ~GenericTOB();


      static GenericTOB* createOnHeap(const GenericTOB &);
      static void clearHeap();

      static const Heap<TCS::GenericTOB>& heap() { return fg_heap; }

   public:
      int Et() const { return m_Et; }
      int eta() const { return m_eta; }
      int phi() const { return m_phi; }
      
      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }

      virtual void print(std::ostream &o) const;

      static unsigned int instances() { return fg_instances; }

   private:
      int m_Et { 0 };
      int m_eta { 0 };
      int m_phi { 0 };

      double m_EtDouble { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };

      static unsigned int fg_instances;

      static Heap<TCS::GenericTOB> fg_heap;
   };
   
} // end of namespace TCS


#endif /* defined(__TopoCore__GeneralTOP__) */
