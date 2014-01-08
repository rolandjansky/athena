/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyFieldStatus.h"

extern "C" void mboysetfieldstatus_(
                    int& getToroidBarrelOn ,
                    int& getToroidECTAOn   ,
                    int& getToroidECTCOn   ,
                    int& getSolenoidOn     ,
                    int& getAllToroidOn    ,
                    int& getFieldStatusOn 
);
extern "C" void f1setmattaginfo_( int& LongA, int& LongB, char* NAMEVARA, char* NAMEVARB);

MuonboyFieldStatus::MuonboyFieldStatus(){}

MuonboyFieldStatus::~MuonboyFieldStatus(){}

void MuonboyFieldStatus::SetFieldStatus(
                    int getToroidBarrelOn ,
                    int getToroidECTAOn   ,
                    int getToroidECTCOn   ,
                    int getSolenoidOn     ,
                    int getAllToroidOn    ,
                    int getFieldStatusOn 
){
//std::cout << " MuonboyFieldStatus::SetFieldStatus " << std::endl;
  mboysetfieldstatus_(
                     getToroidBarrelOn ,
                     getToroidECTAOn   ,
                     getToroidECTCOn   ,
                     getSolenoidOn     ,
                     getAllToroidOn    ,
                     getFieldStatusOn 
                     );
}

