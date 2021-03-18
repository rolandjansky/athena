/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  EmMultiplicity.h
//  TopoCore
//  Created by Carlos Moreno on 05/05/20.

#ifndef __TopoCore__EMMultiplicity__
#define __TopoCore__EMMultiplicity__

#include <iostream>
#include <vector>
#include "L1TopoInterfaces/CountingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include "TrigConfData/L1Threshold.h"

class TH2;

namespace TCS { 
  
   class EMMultiplicity : public CountingAlg {
   public:
      EMMultiplicity(const std::string & name);
      virtual ~EMMultiplicity();

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
