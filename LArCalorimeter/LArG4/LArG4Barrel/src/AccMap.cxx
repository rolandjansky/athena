/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AccMap.h"
#include <iostream>
#include <sstream>
#ifndef LARG4_STAND_ALONE
#include "PathResolver/PathResolver.h"
#endif

AccMap* AccMap::s_thePointer=nullptr;

AccMap::AccMap()
{
  int i1[10]={0,0,3,2,9,12,10,9,0,2};      // first fold
  int i2[10]={2,1,12,12,13,13,13,13,1,4};  // last fold for 10 electronic regions
  m_directory="/afs/cern.ch/atlas/offline/data/lar/calo_data";

  m_nmax=14;
  m_xmin.resize(m_nmax);
  m_xmax.resize(m_nmax);
  m_ymin.resize(m_nmax);
  m_ymax.resize(m_nmax);

  double xnorm=14.1591;   // nA/MeV normalisation for accordion maps

#ifndef LARG4_STAND_ALONE
  //std::string larLocation = PathResolver::find_directory("lar","DATAPATH");
  std::string larLocation = PathResolver::find_directory("LArG4Barrel","ATLASCALDATA");
#endif

  for (int iregion=0;iregion<10;iregion++) {
    // accordion folds
    for (int ifold=i1[iregion]; ifold<=i2[iregion]; ifold++) {
      std::ostringstream fn;
      fn << "fold"<<ifold<<"_region"<<iregion<<".map";
      std::string filename = fn.str();
      std::string fileLocation;
#ifdef LARG4_STAND_ALONE
      fileLocation=m_directory+"/"+filename;
#else
      //fileLocation=larLocation+"/calo_data/"+filename;
      fileLocation=larLocation+"/"+filename;
#endif
      //       std::cout << " try to open map " << fileLocation << std::endl;
      CurrMap* cm = new CurrMap(fileLocation,xnorm);
      int code=10*ifold+iregion;
      m_theMap[code]=cm;

      // add some rounding safety in edges of map
      m_xmin[ifold]=cm->GetXmin()+0.1;
      m_xmax[ifold]=cm->GetXmax()-0.1;
      m_ymin[ifold]=cm->GetYmin()+0.1;
      m_ymax[ifold]=cm->GetYmax()-0.1;
    }
    // straight section
    for (int istr=1; istr<=2; istr++) {
      std::ostringstream fn;
      fn << "straight"<<istr<<"_region"<<iregion<<".map";
      std::string filename = fn.str();
      std::string fileLocation;
#ifdef LARG4_STAND_ALONE
      fileLocation=m_directory+"/"+filename;
#else
      //fileLocation=larLocation+"/calo_data/"+filename;
      fileLocation=larLocation+"/"+filename;
#endif
      //       std::cout << " try to open map " << fileLocation << std::endl;
      CurrMap* cm = new CurrMap(fileLocation,xnorm);
      int code=10*(20+istr)+iregion;
      m_theMap[code]=cm;
    }
  }

}

AccMap* AccMap::GetAccMap()
{
  if (s_thePointer==nullptr) s_thePointer=new AccMap();
  return s_thePointer;
}

void AccMap::Reset()
{
  curr_map::iterator it = m_theMap.begin();
  while (it != m_theMap.end()) {
    delete (*it).second;
    m_theMap.erase(it++);
  }
}

CurrMap* AccMap::GetMap(int ifold, int region, int sampling, int eta) const
{
  return this->GetMap(ifold,this->Region(region,sampling,eta));
}

CurrMap* AccMap::GetMap(int ifold, int ielecregion) const
{
  const int code=10*ifold+ielecregion;
  // std::cout << " code is " << code << std::endl;
  const auto mapIter = m_theMap.find(code);
  if (mapIter != m_theMap.end())
    return mapIter->second;
  else {
    std::cout << " Code " << code << " not found in map ..." << std::endl;
    return nullptr;
  }
}

int AccMap::Region(int region, int sampling, int eta) const
{
  int elecregion=0;
  // logic to compute region vs eta and sampling...
  if (region==0) {
    if (sampling==1) {
      if (eta<256) elecregion=0;
      else         elecregion=1;
    }
    else if (sampling==2) {
      if (eta<32) elecregion=2;
      else        elecregion=3;
    }
    else {
      if (eta<9 || eta==26) elecregion=4;
      if ((eta>8 && eta<13) || (eta>15 && eta<19)) elecregion=5;
      if ((eta>12 && eta < 16) || (eta>18 && eta<21)) elecregion=6;
      if ((eta>20 && eta < 26)) elecregion=7;
    }
  }
  else {
    if (sampling==1) elecregion=8;
    else             elecregion=9;
  }
  return elecregion;
}

void AccMap::SetDirectory(std::string dir)
{
  m_directory=dir;
}
