/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  GenericArrayCreator.h
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 12/10/12.

#ifndef __L1TopoCoreSimulation__GenericArrayCreator__
#define __L1TopoCoreSimulation__GenericArrayCreator__

#include <iostream>

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/JetTOB.h"


namespace TCS {
   class GenericArrayCreator : public SortingAlg {
   public:
      GenericArrayCreator(const std::string & name);
      virtual ~GenericArrayCreator(){};

      virtual StatusCode sort(const InputTOBArray & input, TOBArray & output);

      void setJetWidth(JetTOB::JetSize jetsize) { m_jetsize = jetsize; }

   private:
      JetTOB::JetSize m_jetsize;
   };
}

#endif /* defined(__L1TopoCoreSimulation__GenericArrayCreator__) */
