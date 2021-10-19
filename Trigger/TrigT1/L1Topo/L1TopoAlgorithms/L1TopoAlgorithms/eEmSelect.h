/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  eEmSelect.h
//  TopoCore

#ifndef TCS__eEmSelect
#define TCS__eEmSelect

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class eEmSelect : public SortingAlg {
   public:
      
      // constructor
      eEmSelect(const std::string & name);

      // destructor
      virtual ~eEmSelect();
      virtual TCS::StatusCode initialize();
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);

   private:
     
      parType_t      m_numberOfeEms = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };
      parType_t      m_et = { 0 };
      parType_t      m_minREta = { 0 };
      parType_t      m_minRHad = { 0 };
      parType_t      m_minWsTot = { 0 };
      
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
