/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileL2.cxx
//  Author   : Aranzazu Ruiz
//  Created  : March 2006
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    
//    
//*****************************************************************************

#include "TileEvent/TileL2.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

const float DELTA_PHI_DRAWER = M_PI / 32.;

TileL2::TileL2(int id)
  : m_ID  (id) 
{
    setphi();
}

TileL2::TileL2(int id, float /* phi */)
  : m_ID  (id) 
{
    setphi();
}

void TileL2::setphi()
{ 
  int drawer = (m_ID&0x3F);
  m_phi = (drawer<32?(drawer+0.5)*DELTA_PHI_DRAWER:(drawer-63.5)*DELTA_PHI_DRAWER);
  m_cosphi = cos(m_phi);
  m_sinphi = sin(m_phi);
}

void TileL2::print() const
{ 
    std::cout << (std::string) (*this) << std::endl;
} 

TileL2::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << whoami();
    text << " Id = " << std::hex << "0x" << m_ID << std::dec;
    text << " phi = " << m_phi;

    if (m_sumE.size()>0 ) {
      text << " sumEt = " << m_sumE[0];
      if (m_sumE.size()>1 ) {
        text << " sumEz = " << m_sumE[1];
        if (m_sumE.size()>2 ) {
          text << " sumE = " << m_sumE[2];
          if (m_sumE.size()>3 ) {
            text << " others = ";
            for (unsigned int i=3; i<m_sumE.size(); ++i) text << " " << m_sumE[i];
          }
        }
      }
    } else {
      text << " sumE: none";
    }
    
    text << " NMuons = " << m_eta.size();

    if (m_eta.size() ) {

      text << " eta =";
      for (unsigned int i=0; i<m_eta.size(); ++i) text << " " << m_eta[i];

      text << " enemu0 =";
      for (unsigned int i=0; i<m_enemu0.size(); ++i) text << " " << m_enemu0[i];

      text << " enemu1 =";
      for (unsigned int i=0; i<m_enemu1.size(); ++i) text << " " << m_enemu1[i];

      text << " enemu2 =";
      for (unsigned int i=0; i<m_enemu2.size(); ++i) text << " " << m_enemu2[i];

      text << " qual =";
      for (unsigned int i=0; i<m_quality_factor.size(); ++i) text << " " << m_quality_factor[i];

      text << " val =";
      for (unsigned int i=0; i<m_val.size(); ++i) text << " " << m_val[i];
    }
    
    return text.str();
}

void TileL2::clear()
{
  m_sumE.clear();
  if (m_eta.size()) {
    m_eta.clear();
    m_enemu0.clear();
    m_enemu1.clear();
    m_enemu2.clear();
    m_quality_factor.clear();
    m_val.clear();
  }
}
