/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  EmMultiplicity.h
//  TopoCore
//  Created by Carlos Moreno on 17/09/21.

#ifndef __TopoCore__eTauMultiplicity__
#define __TopoCore__eTauMultiplicity__

#include <iostream>
#include <vector>
#include "L1TopoInterfaces/CountingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include "TrigConfData/L1Threshold.h"

class TH2;

namespace TCS { 
  
   class eTauMultiplicity : public CountingAlg {
   public:
      eTauMultiplicity(const std::string & name);
      virtual ~eTauMultiplicity();

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
