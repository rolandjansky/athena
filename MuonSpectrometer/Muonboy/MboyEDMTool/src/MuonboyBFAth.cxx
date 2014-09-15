/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyEDMTool/MuonboyBFAth.h"

MuonboyBFAth::MuonboyBFAth(){
  m_fieldService = 0;
}
 
MuonboyBFAth::~MuonboyBFAth(){}
 
void MuonboyBFAth::SetReady(){


}
void MuonboyBFAth::field(float* /*XYZ*/,float* /*BXYZ*/){
  
  std::cout << " please implement this function" << std::endl;
}
void MuonboyBFAth::fieldd(float* /*XYZ*/,float* /*BXYZ*/){


  std::cout << " please implement this function" << std::endl;
}
void MuonboyBFAth::field_tesla_cm(float* /*XYZ*/,float* /*BXYZ*/){


  std::cout << " please implement this function" << std::endl;
}
void MuonboyBFAth::fieldGradient_tesla_cm(float* /*XYZ*/,float* /*BXYZ*/){


  std::cout << " please implement this function" << std::endl;
}
