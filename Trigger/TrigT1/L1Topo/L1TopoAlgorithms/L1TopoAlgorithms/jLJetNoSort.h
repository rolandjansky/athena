/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jLJetNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JLJETNOSORT_H
#define L1TOPOALGORITHMS_JLJETNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLJetTOB.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jLJetNoSort : public SortingAlg {
   public:
      
      // constructor
      jLJetNoSort(const std::string & name);

      // destructor
      virtual ~jLJetNoSort();
      virtual TCS::StatusCode initialize() override;
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output) override final;    
    
   private:
   
      parType_t      m_numberOfjLJets = { 0 };
   
   };

} // end of namespace TCS

#endif
