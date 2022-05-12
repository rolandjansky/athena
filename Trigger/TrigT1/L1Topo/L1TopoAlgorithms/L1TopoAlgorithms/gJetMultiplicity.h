/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  gJetMultiplicity.h
//  TopoCore

#ifndef L1TOPOALGORITHMS_GJETMULTIPLICITY_H
#define L1TOPOALGORITHMS_GJETMULTIPLICITY_H

#include <iostream>
#include <vector>
#include "L1TopoInterfaces/CountingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include "TrigConfData/L1Threshold.h"

class TH2;

namespace TCS { 
  
   class gJetMultiplicity : public CountingAlg {
   public:
      gJetMultiplicity(const std::string & name);
      virtual ~gJetMultiplicity();

      virtual StatusCode initialize() override;

      virtual StatusCode processBitCorrect( const TCS::InputTOBArray & input,
					    Count & count ) override final ;

      virtual StatusCode process( const TCS::InputTOBArray & input,
				  Count & count ) override final ;



   private:
     
      TrigConf::L1Threshold const * m_threshold{nullptr};
   };

}

#endif
