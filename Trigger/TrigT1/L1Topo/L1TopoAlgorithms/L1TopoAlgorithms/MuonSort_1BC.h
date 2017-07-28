//  MuonSort_1BC.h
//  TopoCore
//  Created by Sue Cheatham based on MuonSelect by Joerg Stelzer

#ifndef TCS__MuonSort_1BC
#define TCS__MuonSort_1BC

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class MuonSort_1BC : public SortingAlg {
   public:
      
      // constructor
      MuonSort_1BC(const std::string & name);

      // destructor
      virtual ~MuonSort_1BC();

      virtual TCS::StatusCode initialize();

      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);
      
   private:

      parType_t      m_numberOfMuons = { 0 };
      parType_t      m_minEta = { 0 };
      parType_t      m_maxEta = { 0 };

   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
