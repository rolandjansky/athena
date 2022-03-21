/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

// Include xAOD headers here
#ifndef TRIGCONF_STANDALONE
#include "xAODTrigger/eFexTauRoIContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#endif

class TH2;

namespace TCS {
   
   class cTauMultiplicity : public CountingAlg {
   public:
      
      cTauMultiplicity(const std::string & name);
      virtual ~cTauMultiplicity();

      virtual StatusCode initialize() override;

      virtual StatusCode processBitCorrect( const TCS::InputTOBArray & input, Count & count ) override final ;

      virtual StatusCode process( const TCS::InputTOBArray & input, Count & count ) override final ;

      #ifndef TRIGCONF_STANDALONE
      // Matching function for HLT seeding
      static size_t cTauMatching( const xAOD::eFexTauRoI & eTau, const xAOD::jFexTauRoIContainer & jTauRoIs );
      #endif

   private:

      TrigConf::L1Threshold const * m_threshold{nullptr};
      // This function is used to map the ctau isolation working points into a common format with eFEX EM and taus.
      // This allows us to use same functionalities from ConfigurableAlg (L1TopoInterfaces) to apply isolation cuts in multiplicity algorithms for all flavour of TOBS 
      unsigned int convertIsoToBit( const TCS::cTauTOB * etauCand, const TCS::cTauTOB * jtauCand ) const; 

      // Matching function for L1Topo
      bool cTauMatching( const TCS::cTauTOB * etauCand, const TCS::cTauTOB * jtauCand ) const; 

   };

} 

#endif 
