/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTTL1Cell.cxx
//  Author   : Monica Dunford
//  Created  : Nov, 2009
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//     
//
//  BUGS:
//    
//
//*****************************************************************************

#include "TileEvent/TileTTL1Cell.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/TileID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// get cabling
namespace {
const TileCablingService * const s_cabling = TileCablingService::getInstance();
}

//=========================
// Constructors
//=========================
TileTTL1Cell::TileTTL1Cell(const Identifier& id,
			   float energy, float time, float correction_factor, 
			   uint16_t quality)
  : m_ID(id) 
  , m_eneTower(energy)
  , m_timeAve(time)
  , m_corrFactor(correction_factor)
  , m_quality(quality)
{ 
}

//=========================
// Set attributes
//=========================
void TileTTL1Cell::setEnergy(float ene)
{ 
   m_eneTower = ene;
}
void TileTTL1Cell::setTime(float tave)
{ 
   m_timeAve = tave;
}
void TileTTL1Cell::setCorrectionFactor(float corrfact)
{ 
   m_corrFactor = corrfact;
}
void TileTTL1Cell::setQuality(uint16_t qual)
{ 
  m_quality = qual;
}

//=========================
// Return attributes
//=========================
/* return Identifier */
const Identifier & TileTTL1Cell::TTL1_ID() const {
  return m_ID;
}

//=========================
// Supporting functions
//=========================

void TileTTL1Cell::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileTTL1Cell::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << whoami();
   if (s_cabling->getTileTBID()->is_tiletb(m_ID)) {
     text << "MBTS Id = " << s_cabling->getTileTBID()->to_string(m_ID);
   } else {
     text << " Id = " << s_cabling->getCaloLVL1_ID()->show_to_string(m_ID);
   }
   text << " tower ene = " << eneTower();
   text << " ave time = " << timeAve();
   text << " correction factor = " << corrFactor();
   text << " qual = " << qualTower();

   return text.str();
}

