/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jTauNoSort.h
// TopoCore

#ifndef L1TOPOALGORITHMS_JTAUNOSORT_H
#define L1TOPOALGORITHMS_JTAUNOSORT_H

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class jTauNoSort : public SortingAlg {
   public:
      
      // constructor
      jTauNoSort(const std::string & name);

      // destructor
      virtual ~jTauNoSort();
      virtual TCS::StatusCode initialize() override;
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output) override final;

   private:

      parType_t      m_numberOfjTaus = { 0 };
      //parType_t      m_iso = { 0 };
   };

} // end of namespace TCS

#endif
