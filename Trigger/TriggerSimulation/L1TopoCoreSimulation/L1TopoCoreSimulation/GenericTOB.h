/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  GeneralTOP.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#ifndef __TopoCore__GeneralTOP__
#define __TopoCore__GeneralTOP__

#include <iostream>

#include "L1TopoCoreSimulation/BaseTOB.h"
#include "L1TopoCoreSimulation/JetTOB.h"
#include "L1TopoCoreSimulation/ClusterTOB.h"

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
      
      virtual void print(std::ostream &o) const;

      static unsigned int instances() { return fg_instances; }

   private:
      int m_Et;
      int m_eta;
      int m_phi;

      static unsigned int fg_instances;

      static Heap<TCS::GenericTOB> fg_heap;
   };
   
} // end of namespace TCS


#endif /* defined(__TopoCore__GeneralTOP__) */
