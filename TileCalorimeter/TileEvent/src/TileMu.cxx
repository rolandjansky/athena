/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//========================================
// file TileMu.cxx
// 
//========================================

#include "TileEvent/TileMu.h"

#include <iostream>
#include <sstream>
#include <iomanip>

TileMu::TileMu ( float eta, float phi, std::vector<float> & ener, float qual )
  : m_eta(eta)
  , m_phi(phi)
  , m_energy_deposited(ener)
  , m_quality_factor(qual)
{ 
}

void TileMu::Set( float eta, float phi, std::vector<float> & ener, float qual )
{
  m_eta = eta;
  m_phi = phi;
  m_energy_deposited = ener;
  m_quality_factor = qual;
}

void TileMu::print() const
{
  std::cout << (std::string) (*this) << std::endl;
}

TileMu::operator std::string() const
{
  std::ostringstream text(std::ostringstream::out);

  text << whoami();
  text << " eta=" << m_eta;
  text << " phi=" << m_phi;
  for (unsigned int i=0; i<m_energy_deposited.size(); ++i)
    text << " ene[" << i << "]=" << m_energy_deposited[i];
  text << " qual=" << m_quality_factor;

  return text.str();
}

