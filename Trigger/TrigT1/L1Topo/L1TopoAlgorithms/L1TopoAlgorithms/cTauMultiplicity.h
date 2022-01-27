/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  cTauMultiplicity.h
//  TopoCore

#ifndef __TopoCore__cTauMultiplicity__
#define __TopoCore__cTauMultiplicity__

#include <iostream>
#include <vector>
#include "L1TopoInterfaces/CountingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "TrigConfData/L1Threshold.h"

class TH2;

namespace TCS {
   
   class cTauMultiplicity : public CountingAlg {
   public:
      
      cTauMultiplicity(const std::string & name);
      virtual ~cTauMultiplicity();

      virtual StatusCode initialize() override;

      virtual StatusCode processBitCorrect( const TCS::InputTOBArray & input, Count & count ) override final ;

      virtual StatusCode process( const TCS::InputTOBArray & input, Count & count ) override final ;
      
   private:

      TrigConf::L1Threshold const * m_threshold{nullptr};
      // This function is used to map the ctau isolation working points into a common format with eFEX EM and taus.
      // This allows us to use same functionalities from ConfigurableAlg (L1TopoInterfaces) to apply isolation cuts in multiplicity algorithms for all flavour of TOBS 
      unsigned int convertIsoToBit(const TCS::GenericTOB & input_etau, const TCS::GenericTOB & input_jtau); 

   };

} 

#endif 
