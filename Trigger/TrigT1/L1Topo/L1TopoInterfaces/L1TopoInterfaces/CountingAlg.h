/*multi
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  CountingAlg.h
//  TopoCore
//  Created by Carlos Moreno on 15/01/21.

#ifndef TCS_CountingAlg
#define TCS_CountingAlg

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/CompositeTOB.h"
#include "L1TopoCommon/StatusCode.h"

#include "TrigConfData/L1Threshold.h"

#include <iostream>

namespace TCS { 

  class Count;
  
  class CountingAlg : public ConfigurableAlg {
  public:
     // default constructor
     CountingAlg(const std::string & name) :
        ConfigurableAlg(name, COUNT)
     {};

     virtual ~CountingAlg();

     virtual TCS::StatusCode process( const InputTOBArray & input, 
				      Count & count ) = 0;

     virtual TCS::StatusCode processBitCorrect( const InputTOBArray & input,
						Count & count ) = 0;

     unsigned int numberOutputBits() const { return m_numberOutputBits; }
     void setNumberOutputBits(unsigned int numberOutputBits) { m_numberOutputBits = numberOutputBits; }

     // Set the threshold
     void setThreshold(const TrigConf::L1Threshold & thr) { m_threshold = &thr; }

     // Access the threshold 
     const TrigConf::L1Threshold * getThreshold() { return m_threshold; }

  protected:

     std::vector<std::string> m_histAccept; // vector of strings for histogram names

  private:

     virtual StatusCode doReset();
     
     friend class TopoSteering;

     // generic parameter
     unsigned int m_numberOutputBits {1};

     // L1Threshold
     TrigConf::L1Threshold const * m_threshold{nullptr};
     
  };

}

#endif
