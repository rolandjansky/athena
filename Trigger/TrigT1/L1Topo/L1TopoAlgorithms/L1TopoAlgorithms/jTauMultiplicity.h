/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  jTauMultiplicity.h
//  TopoCore

#ifndef __TopoCore__jTauMultiplicity__
#define __TopoCore__jTauMultiplicity__

#include <iostream>
#include <vector>
#include "L1TopoInterfaces/CountingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include "TrigConfData/L1Threshold.h"

class TH2;

namespace TCS { 
  
   class jTauMultiplicity : public CountingAlg {
   public:
      jTauMultiplicity(const std::string & name);
      virtual ~jTauMultiplicity();

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
