/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyMaterialTagInfo.h"

extern "C" void resetmattaginfo_();
extern "C" void f1setmattaginfo_( int& LongA, int& LongB, char* NAMEVARA, char* NAMEVARB);

MuonboyMaterialTagInfo::MuonboyMaterialTagInfo(){}

MuonboyMaterialTagInfo::~MuonboyMaterialTagInfo(){}

void MuonboyMaterialTagInfo::SetMatiereTagInfo(std::vector< std::pair<std::string,std::string> > TagInfoXML)
{

  std::cout << " MuonboyMaterialTagInfo::SetMatiereTagInfo " 
            << " One gets from TagInfoXML " << TagInfoXML.size() << " items "
            << std::endl;

  resetmattaginfo_();

  for (unsigned int Item = 0; Item < TagInfoXML.size(); Item++) {

    std::string TheStringA  = TagInfoXML[Item].first;   
    char NAMEVARA[100];
    int LongA = TheStringA.size();
    if( LongA > 100 ){ LongA = 100; }
    for(int I=0; I<LongA; I++){ NAMEVARA[I] = TheStringA[I]; }
    
    std::string TheStringB  = TagInfoXML[Item].second;  
    char NAMEVARB[100];
    int LongB = TheStringB.size();
    if( LongB > 100 ){ LongB = 100; }
    for(int I=0; I<LongB; I++){ NAMEVARB[I] = TheStringB[I]; }

    f1setmattaginfo_( LongA, LongB, NAMEVARA,NAMEVARB);

  }

}
