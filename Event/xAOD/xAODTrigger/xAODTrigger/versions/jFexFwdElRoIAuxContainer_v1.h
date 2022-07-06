/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXFWDELROIAUXCONTAINER_V1_H 
#define XAODTRIGGER_VERSIONS_JFEXFWDELROIAUXCONTAINER_V1_H 

//System include(s):
extern "C"{
#include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
namespace xAOD{

  class jFexFwdElRoIAuxContainer_v1: public AuxContainerBase {

  public:
    //Default constructor
    jFexFwdElRoIAuxContainer_v1();

  private:
    std::vector<uint32_t> tobWord;
    std::vector<uint8_t>  jFexNumber;
    std::vector<uint8_t>  fpgaNumber;
    std::vector<uint16_t> tobEt;
    std::vector<uint8_t>  tobEMIso;
    std::vector<uint8_t>  tobEMf1;
    std::vector<uint8_t>  tobEMf2;
    std::vector<uint8_t>  tobLocalEta;
    std::vector<uint8_t>  tobLocalPhi;  
    std::vector<uint8_t>  tobSat;
    std::vector<int>      globalEta;
    std::vector<uint>     globalPhi;
    std::vector<float>    eta;
    std::vector<float>    phi;
    std::vector<int>      tobEtScale;
    std::vector< char>    isTOB;
    std::vector<uint16_t> EtEMiso;
    std::vector<uint16_t> EtEM;
    std::vector<uint16_t> EtHad1;
    std::vector<uint16_t> EtHad2;

}; // class jFexFwdElRoIAuxContainer_v1

}//end of namespace xAOD 

//Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::jFexFwdElRoIAuxContainer_v1, xAOD::AuxContainerBase);

#endif //XAODTRIGGER_VERSIONS_JFEXFWDELROIAUXCONTAINER_V1_H
