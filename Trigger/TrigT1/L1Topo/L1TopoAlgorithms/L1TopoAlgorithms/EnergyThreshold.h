/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//  EnergyThreshold.h
//  TopoCore
//  Created by Jack Harrison on 16/12/21.

#ifndef __TopoCore__EnergyThreshold__
#define __TopoCore__EnergyThreshold__

#include <vector>
#include <L1TopoInterfaces/CountingAlg.h>
#include "L1TopoEvent/TOBArray.h"
#include "TrigConfData/L1Threshold.h"


class TH2;


namespace TCS {

    class EnergyThreshold : public CountingAlg {

        public:

            EnergyThreshold(const std::string & name); 

            virtual ~EnergyThreshold() = default;

            virtual StatusCode initialize() override; 

            virtual StatusCode processBitCorrect(const TCS::InputTOBArray & input,
                                                Count & count) override final;

            virtual StatusCode process(const TCS::InputTOBArray & input, 
                                                Count & count) override final;

        private:

            TrigConf::L1Threshold const *m_threshold{nullptr};

    };
}

#endif
