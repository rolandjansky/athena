/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXSRJETROIAUXCONTAINER_V1_H 
#define XAODTRIGGER_VERSIONS_JFEXSRJETROIAUXCONTAINER_V1_H 

//System include(s):
extern "C"{
#include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
namespace xAOD{

  class jFexSRJetRoIAuxContainer_v1: public AuxContainerBase {

  public:
    //Default constructor
    jFexSRJetRoIAuxContainer_v1();

  private:
    std::vector<uint32_t> word0;
    std::vector<uint8_t> jFexNumber;
    std::vector<uint8_t> fpgaNumber;
    std::vector<uint16_t> tobEt;
    std::vector<uint8_t> iEta;
    std::vector<uint8_t> iPhi;  
    std::vector<uint8_t> satFlag;
    std::vector<int8_t> globalEta;
    std::vector<uint8_t> globalPhi;
}; // class jFexSRJetRoIAuxContainer_v1

}//end of namespace xAOD 

//Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::jFexSRJetRoIAuxContainer_v1, xAOD::AuxContainerBase);

#endif //XAODTRIGGER_VERSIONS_JFEXSRJETROIAUXCONTAINER_V1_H
