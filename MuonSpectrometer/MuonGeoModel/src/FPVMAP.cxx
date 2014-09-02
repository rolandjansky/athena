/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/FPVMAP.h"
#include <iostream>

#include "MuonGeoModel/Technology.h"

namespace MuonGM {


FPVMAP* FPVMAP::thePointer=0;

FPVMAP::FPVMAP()
{
    m_nreused=0;
//    std::cout<<"Creating the FPVMAP structure"<<std::endl;
}

FPVMAP* FPVMAP::GetPointer()
{
    if (!thePointer) thePointer=new FPVMAP;
    return thePointer;
}

GeoVPhysVol* FPVMAP::GetDetector(std::string name) 
{
    if (Detectors.find(name)!=Detectors.end())
    {
        m_nreused++;
//         std::cout<<"FPVMAP:: the pointer to "<<name
//                  <<" is already stored; saving memory "<<m_nreused<<std::endl;
        return Detectors[name];
    }
    else return 0;
}

void FPVMAP::StoreDetector(GeoVPhysVol* s, std::string name)
{
//    std::cout<<"FPVMAP:: store the pointer to "<<name<<std::endl;
    Detectors[name]=s;
}

void FPVMAP::PrintAllDetectors()
{
    for (DetectorIterator it=Detectors.begin();it!=Detectors.end();it++)
    {
        std::string key=(*it).first;
        std::cout<<"---> A PhysVol corresponds to  "<<key<<std::endl;
    }
}

} // namespace MuonGM
