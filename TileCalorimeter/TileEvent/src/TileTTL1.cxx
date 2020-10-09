/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigit.cxx
//  Author   : Chicago( Merritt)
//  Created  : February 2003
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

#include "TileEvent/TileTTL1.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/CaloLVL1_ID.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// get cabling
namespace {
const TileCablingService * const s_cabling = TileCablingService::getInstance();
}

TileTTL1::TileTTL1( const Identifier& id,
                    const std::vector<float>& digits ) :
  m_ID (id),
  m_TTL1digits (digits)
  { }


TileTTL1::TileTTL1( const Identifier& id,
                    std::vector<float>&& digits ) :
  m_ID (id),
  m_TTL1digits (std::move(digits))
  { }


TileTTL1::TileTTL1( const Identifier& id,
		    const std::vector<double>& digits ) :
  m_ID (id)
 {

  m_TTL1digits.reserve(digits.size());

  std::vector<double>::const_iterator it1=digits.begin();
  std::vector<double>::const_iterator it2=digits.end();  

  float dig;
  for ( ; it1!=it2; ++it1) {
    dig = (*it1);
    m_TTL1digits.push_back(dig);
  }
 }

/* return Identifier */
const Identifier & TileTTL1::TTL1_ID() const {
  return m_ID;
}

/* return number of time slices (dim of vector)*/
short TileTTL1::nsamples() const {
  return m_TTL1digits.size();
}


/* return reference to the vector of digits (float, not int) */
const std::vector<float>& TileTTL1::fsamples() const {
  return m_TTL1digits;
}


/* return vector of digits (double, not int) */
const std::vector<double> TileTTL1::samples() const {
     std::vector<double> dTTL1digits(m_TTL1digits.size());
     for (unsigned int i=0; i<m_TTL1digits.size(); ++i) dTTL1digits[i] = m_TTL1digits[i];
      return dTTL1digits;
}

void TileTTL1::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileTTL1::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << whoami();

    if (s_cabling->getTileTBID()->is_tiletb(m_ID)) {
      text << "MBTS Id = " << s_cabling->getTileTBID()->to_string(m_ID);
    } else {
      text << " Id = " << s_cabling->getCaloLVL1_ID()->show_to_string(m_ID);
    }

    text << " Val:";
    text << std::setiosflags( std::ios::fixed );
    text << std::setiosflags( std::ios::showpoint );
    text << std::setprecision(2);

    std::vector<float>::const_iterator it1=m_TTL1digits.begin();
    std::vector<float>::const_iterator it2=m_TTL1digits.end();

    for ( ; it1!=it2; ++it1) text << " " << (*it1);

    return text.str();
}

