/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXMETROIAUXCONTAINER_V1_H 
#define XAODTRIGGER_VERSIONS_JFEXMETROIAUXCONTAINER_V1_H 

//System include(s):
extern "C"{
#include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
namespace xAOD{

  class jFexMETRoIAuxContainer_v1: public AuxContainerBase {

  public:
    //Default constructor
    jFexMETRoIAuxContainer_v1();

  private:
    std::vector<uint32_t> word0;
    std::vector<uint8_t> jFexNumber;
    std::vector<uint8_t> fpgaNumber;
    std::vector<int16_t> E_x;
    std::vector<int16_t> E_y;
    std::vector<uint8_t> Res;
    std::vector<uint8_t> Sat;
}; // class jFexMETRoIAuxContainer_v1

}//end of namespace xAOD 

//Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::jFexMETRoIAuxContainer_v1, xAOD::AuxContainerBase);

#endif //XAODTRIGGER_VERSIONS_JFEXMETROIAUXCONTAINER_V1_H
