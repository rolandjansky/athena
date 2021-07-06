/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ServicesLayer.h"

#include <iostream>
#include <string>
#include <sstream>

ServicesLayer::ServicesLayer( double radius, double halfLength, int nStaves,
			      DetType::Type type, DetType::Part part, int num,
			      const std::string& suffix,
			      int nModulesPerStave, int nChipsPerModule) :
  m_radius(radius), m_zPos(0.), m_rMin(0.), m_rMax(0.), m_halfLength(halfLength), m_nStaves(nStaves),
  m_type(type), m_part(part), m_number(num),
  m_lastVolume(nullptr), m_suffix(suffix), 
  m_nModulesPerStave(nModulesPerStave), m_nChipsPerModule(nChipsPerModule)
{
  if (part == DetType::Barrel) {
    m_zPos = 0;
    m_rMin = radius;
    m_rMax = radius;
  }
  else {
    std::cout << "Wrong ServicesLayer constructor for barrel layer called " << std::endl;
  }
}

ServicesLayer::ServicesLayer( double zpos, double rmin, double rmax, int nStaves,
			      DetType::Type type, DetType::Part part, int num,
			      const std::string& suffix,
			      int nModulesPerStave, int nChipsPerModule) :
  m_radius(0.), m_zPos(zpos), m_rMin(rmin), m_rMax(rmax), m_halfLength(0.), m_nStaves(nStaves),
  m_type(type), m_part(part), m_number(num),
  m_lastVolume(nullptr), m_suffix(suffix), 
  m_nModulesPerStave(nModulesPerStave), m_nChipsPerModule(nChipsPerModule)
{
  if (part == DetType::Endcap) {
    m_radius = 0.5*(rmin+rmax);
    m_halfLength = rmax - rmin;
  }
  else {
    std::cout << "Wrong ServicesLayer constructor for endcap layer called " << std::endl;
  }
}

std::string ServicesLayer::name() const 
{
  std::ostringstream os;
  os << number();
  return DetType::name( type(), part()) + os.str();
}
