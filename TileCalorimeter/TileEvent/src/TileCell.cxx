/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileCell.cxx
//  Author   : Ed Frank, Ambreesh Gupta
//  Created  : Jan, 2001
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//     12June02  Completely changed by Zhifang Wu
//
//  BUGS:
//    
//
//*****************************************************************************

#include "TileEvent/TileCell.h"
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
TileCell::TileCell( )
  : CaloCell()
  , m_eneDiff(0.0)
  , m_timeDiff(0.0)
{
   // DO: Nothing, Default  
}

TileCell::TileCell(const CaloDetDescrElement* const & caloDDE,
                   float energy, float time, uint16_t quality,
                   uint16_t provenance, CaloGain::CaloGain gain)
  : CaloCell(caloDDE,energy,time,quality,provenance,gain)
  , m_eneDiff(energy)
  , m_timeDiff(0.0)
{ 
}

TileCell::TileCell(const CaloDetDescrElement* const & caloDDE,
                   const Identifier & cell_ID, 
                   float energy, float time, uint16_t quality,
                   uint16_t provenance, CaloGain::CaloGain gain)
  : CaloCell(caloDDE,cell_ID,energy,time,quality,provenance,gain)
  , m_eneDiff(energy)
  , m_timeDiff(0.0)
{ 
}

TileCell::TileCell(const CaloDetDescrElement* const & caloDDE,
                   float ene1, float ene2, float time1, float time2,
                   int qual1, int qual2, int qbit1, int qbit2, 
                   int gain1, int gain2)
  : CaloCell(caloDDE,ene1+ene2,(time1+time2)/2.0, 0, 0,
             (CaloGain::CaloGain) ( 0xFFFFFFF0 | (static_cast<unsigned int>(gain2) << 2) | (gain1 & 3) ))
  , m_eneDiff(ene1-ene2)
  , m_timeDiff((time1-time2)/2.)
{ 
  m_tileQual[0] = std::min(255,abs(qual1));
  // cppcheck-suppress objectIndex
  m_tileQual[1] = std::min(255,abs(qual2));
  // cppcheck-suppress objectIndex
  m_tileQual[2] = (qbit1 & 0xFF);
  // cppcheck-suppress objectIndex
  m_tileQual[3] = (qbit2 & 0xFF);
}

TileCell::TileCell(const TileCell *cell)
  : CaloCell(cell->caloDDE(),cell->ID(),cell->energy(),cell->time(),cell->quality(),cell->provenance(),cell->gain())
  , m_eneDiff(cell->eneDiff())
  , m_timeDiff(cell->timeDiff())
{
}

TileCell::TileCell(const CaloDetDescrElement* const & caloDDE,
                   const Identifier & cell_ID, 
                   float energy, float time, uint16_t quality,
                   uint16_t provenance, CaloGain::CaloGain gain,
                   float eneDiff, float timeDiff)
  : CaloCell(caloDDE,cell_ID,energy,time,quality,provenance,gain)
  , m_eneDiff(eneDiff)
  , m_timeDiff(timeDiff)
{ 
}

TileCell::~TileCell()
{ }

// clone this cell
CaloCell* TileCell::clone() const
{
  return new TileCell(this->caloDDE(),
		      this->ID(),
		      this->energy(),
		      this->time(),
		      this->quality(),
		      this->provenance(),
		      this->gain(),
		      this->eneDiff(), 
		      this->timeDiff());
}


//=========================
// Set attributes
//=========================
void TileCell::setEnergy(float ene)
{ 
   m_energy  = ene;

   // cells with single PMT should always have zero energy in second PMT
   if (m_gain == CaloGain::TILEONELOW || m_gain == CaloGain::TILEONEHIGH) {
     m_eneDiff = ene;
   } else {
     m_eneDiff = 0.0;
   }
}
void TileCell::setEnergy(float e1, float e2)
{
   m_eneDiff = e1-e2;
   m_energy  = e1+e2;
}
void TileCell::setEnergy(float e1, float e2, int gain1, int gain2)
{ 
  setEnergy_nonvirt(e1, e2, gain1, gain2);
}

void TileCell::addEnergy(float ene)
{ 
   m_energy  += ene;
}
void TileCell::scaleEnergy(float scale)
{ 
   m_energy  *= scale;
   m_eneDiff *= scale;
}
void TileCell::addEnergy(float e, int pmt, int gain)
{ 
  if (pmt > 0) { // second pmt
    m_eneDiff -= e;
    m_gain = (CaloGain::CaloGain)( (m_gain & 0xFFFFFFF3) | ((gain & 3) << 2)  );
  } else { // first PMT
    m_eneDiff += e;
    m_gain = (CaloGain::CaloGain)( (m_gain & 0xFFFFFFFC) | ( gain & 3 ) );
   }  
  m_energy += e;
}

void TileCell::setTime(float t)
{ 
  setTime_nonvirt(t);
}
void TileCell::setTime(float t, int pmt) // works only for second PMT in a cell
{ 
  if (pmt > 0) { // second pmt in a cell with index "1",
    m_time = (m_time + t)/2.;
    m_timeDiff = m_time - t;
  } else { // second in a cell pmt with index "0"
    m_time = (t + m_time)/2.;
    m_timeDiff = t - m_time;
  }  
}

void TileCell::setQuality(unsigned char qual, unsigned char qbit, int pmt)
{ 
  setQuality_nonvirt(qual, qbit, pmt);
}

void TileCell::setQuality(uint16_t quality)
{ 
  CaloCell::setQuality(quality);
}

void TileCell::setQuality(double quality)
{ 
  CaloCell::setQuality(quality);
}

int TileCell::gain1(void) const
{ 
  int gain = m_gain & 3;
  
  return ( ( gain < 2) ? gain : CaloGain::INVALIDGAIN);
}

int TileCell::gain2(void) const
{ 
  int gain = (m_gain >> 2) & 3;

  return ( ( gain < 2) ? gain : CaloGain::INVALIDGAIN);
}


//=========================
// Supporting functions
//=========================

void TileCell::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileCell::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << whoami();
    text << " Id = " << s_cabling->getTileID()->to_string(subcalo_hash(),-2); // two levels above adc_id
    text << " ener = " << energy();
    text << " gain = " << gain();
    text << " time = " << time();
    text << " qual = " << quality();
    text << " prov = " << provenance();
    text << " E1 = " << ene1();
    text << " G1 = " << gain1();
    text << " T1 = " << time1();
    text << " Q1 = " << (int)qual1() << " " << (int)qbit1();
    text << " E2 = " << ene2();
    text << " G2 = " << gain2();
    text << " T2 = " << time2();
    text << " Q2 = " << (int)qual2() << " " << (int)qbit2();
    text << " eta = " << eta();
    text << " phi = " << phi();
    text << " r = " << caloDDE()->r();

    return text.str();
}

