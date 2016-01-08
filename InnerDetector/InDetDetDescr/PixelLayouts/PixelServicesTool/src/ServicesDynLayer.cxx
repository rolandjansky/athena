/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/ServicesDynLayer.h"

#include <iostream>
#include <string>
#include <sstream>

ServicesDynLayer::ServicesDynLayer( double radius, double halfLength, int nStaves,
			      DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
			      const std::string& suffix,
			      int nModulesPerStave, int nChipsPerModule) :
  m_radius(radius), m_zPos(0.), m_rMin(0.), m_rMax(0.), m_halfLength(halfLength), m_nStaves(nStaves),
  m_type(type), m_part(part), m_number(num),
  m_lastVolume(0), m_suffix(suffix)
{
  m_nModulesPerStave.push_back(nModulesPerStave);
  m_nChipsPerModule.push_back(nChipsPerModule);

  if (part == DetTypeDyn::Barrel) {
    m_zPos = 0;
    m_rMin = radius;
    m_rMax = radius;
  }
  else {
    std::cout << "Wrong ServicesDynLayer constructor for barrel layer called " << std::endl;
  }
}

ServicesDynLayer::ServicesDynLayer( double radius, double halfLength, int nStaves,
				    DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
				    const std::string& suffix,
				    std::vector<int> nModulesPerStave, std::vector<int> nChipsPerModule) :
  m_radius(radius), m_zPos(0.), m_rMin(0.), m_rMax(0.), m_halfLength(halfLength), m_nStaves(nStaves),
  m_type(type), m_part(part), m_number(num),
  m_lastVolume(0), m_suffix(suffix), 
  m_nModulesPerStave(nModulesPerStave), m_nChipsPerModule(nChipsPerModule)
{
  if (part == DetTypeDyn::Barrel) {
    m_zPos = 0;
    m_rMin = radius;
    m_rMax = radius;
  }
  else {
    std::cout << "Wrong ServicesDynLayer constructor for barrel layer called " << std::endl;
  }
}

ServicesDynLayer::ServicesDynLayer( double zpos, double rmin, double rmax, int nStaves,
			      DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
			      const std::string& suffix,
			      int nModulesPerStave, int nChipsPerModule) :
  m_radius(0.), m_zPos(zpos), m_rMin(rmin), m_rMax(rmax), m_halfLength(0.), m_nStaves(nStaves),
  m_type(type), m_part(part), m_number(num),
  m_lastVolume(0), m_suffix(suffix)
{
  m_nModulesPerStave.push_back(nModulesPerStave);
  m_nChipsPerModule.push_back(nChipsPerModule);

  if (part == DetTypeDyn::Endcap) {
    m_radius = 0.5*(rmin+rmax);
    m_halfLength = rmax - rmin;
  }
  else {
    std::cout << "Wrong ServicesDynLayer constructor for endcap layer called " << std::endl;
  }
}

std::string ServicesDynLayer::name() const 
{
  std::ostringstream os;
  os << number();
  return DetTypeDyn::name( type(), part()) + os.str();
}

int ServicesDynLayer::modulesPerStaveTot() const
{
  int modTot=0;
  for(int i=0; i<(int)m_nModulesPerStave.size(); i++) modTot+=m_nModulesPerStave[i];
  return modTot;
}
