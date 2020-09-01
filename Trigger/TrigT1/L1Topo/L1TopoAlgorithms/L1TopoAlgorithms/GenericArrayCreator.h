// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TOPOALGORITHMS_GENERICARRAYCREATOR
#define L1TOPOALGORITHMS_GENERICARRAYCREATOR

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

#endif
