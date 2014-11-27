/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigT2ZdcSignals_V1_H
#define xAODTrigMinBias_TrigT2ZdcSignals_V1_H

#include "AthContainers/AuxElement.h"

namespace xAOD {

  class TrigT2ZdcSignals_v1 : public SG::AuxElement {
  public:
    ///default ctor
    TrigT2ZdcSignals_v1();

    //destructor
    ~TrigT2ZdcSignals_v1();

    /** Return the trigger energies of each counter */
    const std::vector<float> &triggerEnergies() const;
    
    void setTriggerEnergies(const std::vector<float> &trigEg);
    
    /** Return the relative times of the triggers */
    const std::vector<float> &triggerTimes() const;
    
    void setTriggerTimes(const std::vector<float> &trigTm);

    /**  Prints out data members to std::cout */
    void print(void) const;
   
    /** A data member to contain the number of ZDC signals */
    static const unsigned int NUM_ZDC = 8;
  };
}

#endif
