/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Create by: joao.victor.da.fonseca.pinto@cern.ch, 2014-10-17
#ifndef XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V1_H

// System include(s):
extern "C" {
  #include <stdint.h>
}

// xAOD include(s):
#include "AthContainers/AuxElement.h"

// std
#include <vector>


namespace xAOD{   
  class TrigRNNOutput_v1 : public SG::AuxElement{
       
  public:  
    /**
     * Class default constructor
     */
    TrigRNNOutput_v1();
      
    /** Acessor methods **/
    const std::vector<float>& decision() const; 
    long  RoIword() const;
    float et() const;
    
    void setDecision(const std::vector<float> &d);
    void setDecision(float d);
    void setRoiWord(long w);
    void setEt(float e);

  };
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRNNOutput_v1, SG::AuxElement );


#endif


