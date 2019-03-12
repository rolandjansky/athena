/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PsMap.h"
#include <iostream>
#include <sstream>
#ifndef LARG4_STAND_ALONE
#include "PathResolver/PathResolver.h"
#endif

PsMap* PsMap::s_thePointer=nullptr;

PsMap::PsMap()
{
  m_directory="/afs/cern.ch/atlas/offline/data/lar/calo_data";

#ifndef LARG4_STAND_ALONE
  //std::string larLocation = PathResolver::find_directory("lar","DATAPATH");
  std::string larLocation = PathResolver::find_directory("LArG4Barrel","ATLASCALDATA");
#endif

  double xnorm=15.9;  // nA/MeV normalisation for PS maps

  for (int imap=0;imap<5;imap++) {
    // accordion folds
    std::ostringstream fn;
    fn << "presampler_"<<imap<<".map";
    std::string filename = fn.str();
    std::string fileLocation;
#ifdef LARG4_STAND_ALONE
    fileLocation=m_directory+"/"+filename;
#else
    //fileLocation=larLocation+"/calo_data/"+filename;
    fileLocation=larLocation+"/"+filename;
#endif
    CurrMap* cm = new CurrMap(fileLocation,xnorm);
    int code=imap;
    m_theMap[code]=cm;
  }

}

PsMap* PsMap::GetPsMap()
{
  if (s_thePointer==nullptr) s_thePointer=new PsMap();
  return s_thePointer;
}

void PsMap::Reset()
{
  curr_map::iterator it=m_theMap.begin();
  while (it != m_theMap.end()) {
    delete (*it).second;
    m_theMap.erase(it++);
  }
}

CurrMap* PsMap::GetMap(int module) const
{
  // module 0 and 1 have their own maps (code = 0 and 1)
  // module 2 and 3 have the same map (same geometry) with code 2
  // module 4 and 5 have the same map (same geometry) with code 3
  // module 6 and 7 have the same map (same geometry) with code 4
  int code = -1;
  if (module==0 || module==1) code=module;
  if (module > 1 && module < 8) code=(module-2)/2 + 2;
  auto it = m_theMap.find(code);
  if (it != m_theMap.end())
    return  it->second; 
  else {
    std::cout << " Code " << module << " not found in map ..." << std::endl;
    return nullptr;
  }
}

void PsMap::SetDirectory(std::string dir)
{
  m_directory=dir;
}
