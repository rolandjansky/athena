/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Create by: joao.victor.da.fonseca.pinto@cern.ch, 2014-10-17
#ifndef XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V1_H

// xAOD include(s):
#include "AthContainers/AuxElement.h"

//#include "CLIDSvc/CLASS_DEF.h"
//#include "GaudiKernel/MsgStream.h"

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
    const std::vector<float>& decision() const; // expand to more outputs!
    //float decision() const;
    
    long  RoIword() const;
    float et() const;
    
    void setDecision(const std::vector<float> &d);
    void setDecision(float d);
    
    void setRoIword(long w); // hold the roiword
    void setEt(float e);
  };
} // namespace xAOD

#endif // XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUT_V1_H


