/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/FPVMAP.h"
#include <iostream>

#include "MuonGeoModel/Technology.h"

namespace MuonGM {


FPVMAP::FPVMAP()
{
    m_nreused=0;
//    std::cout<<"Creating the FPVMAP structure"<<std::endl;
}

FPVMAP::~FPVMAP()
{}

GeoVPhysVol* FPVMAP::GetDetector(std::string name) 
{
    if (m_Detectors.find(name)!=m_Detectors.end())
    {
        m_nreused++;
//         std::cout<<"FPVMAP:: the pointer to "<<name
//                  <<" is already stored; saving memory "<<m_nreused<<std::endl;
        return m_Detectors[name];
    }
    else return 0;
}

void FPVMAP::StoreDetector(GeoVPhysVol* s, std::string name)
{
//    std::cout<<"FPVMAP:: store the pointer to "<<name<<std::endl;
    m_Detectors[name]=s;
}

void FPVMAP::PrintAllDetectors()
{
    for (DetectorIterator it=m_Detectors.begin();it!=m_Detectors.end();it++)
    {
        std::string key=(*it).first;
        std::cout<<"---> A PhysVol corresponds to  "<<key<<std::endl;
    }
}

} // namespace MuonGM
