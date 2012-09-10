/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRampCumul.h"

#include <vector>

L1CaloRampCumul::L1CaloRampCumul(L1CaloRampRunPlan* pRampRunPlan) {
    std::vector<double> vSteps = pRampRunPlan->energySteps();
    std::vector<double>::iterator it = vSteps.begin();
    for(;it!=vSteps.end();++it) {
        RampStep rampStep(*it);
        m_mapRampSteps[*it]=rampStep;
    }
}

void L1CaloRampCumul::addEvent(double step, int energy) {
    std::map<double, RampStep>::iterator it = m_mapRampSteps.find(step);
    if(it!=m_mapRampSteps.end()) {
        (it->second).addEvent(energy);
    }
}
