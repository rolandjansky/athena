/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigit.cxx
//  Author   : Chicago( Merritt)
//  Created  : May, 2002
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//    16May02: Created for DC1 and HLT tests.
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileDigits.h"

#include <iostream>
#include <sstream>
#include <iomanip>

TileDigits::TileDigits( const Identifier& id,
                        const std::vector<double>& digits )

  : TileRawData( id )
{

  m_digits.reserve(digits.size());

  std::vector<double>::const_iterator it1=digits.begin();
  std::vector<double>::const_iterator it2=digits.end();  

  float dig;
  for ( ; it1!=it2; ++it1) {
    dig = (*it1);
    m_digits.push_back(dig);
  }

}

TileDigits::TileDigits( const HWIdentifier& HWid,
                        const std::vector<double>& digits )

  : TileRawData( HWid )
{

  m_digits.reserve(digits.size());

  std::vector<double>::const_iterator it1=digits.begin();
  std::vector<double>::const_iterator it2=digits.end();
  
  float dig;
  for ( ; it1!=it2; ++it1) {
    dig = (*it1);
    m_digits.push_back(dig);
  }
 
}

TileDigits::TileDigits( const HWIdentifier& HWid,
                        const std::vector<short>& digits )

  : TileRawData( HWid )
{
  m_digits.reserve(digits.size());

  std::vector<short>::const_iterator it1=digits.begin();
  std::vector<short>::const_iterator it2=digits.end();

  float dig;
  for ( ; it1!=it2; ++it1) {
    dig = (*it1);
    m_digits.push_back(dig);
  }  
}

TileDigits::TileDigits( const Identifier& id,
                        const std::vector<float>& digits )

  : TileRawData( id )
  , m_digits ( digits )
{
}
  
TileDigits::TileDigits( const HWIdentifier& HWid,
                        const std::vector<float>& digits )
  
  : TileRawData( HWid )
  , m_digits ( digits )
{
}


TileDigits::TileDigits( const HWIdentifier& HWid,
                        std::vector<float>&& digits )
  
  : TileRawData( HWid )
    , m_digits ( std::move(digits) )
{
}


void TileDigits::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileDigits::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << TileRawData::operator std::string();

    text << std::setiosflags( std::ios::fixed );
    text << std::setiosflags( std::ios::showpoint );
    text << std::setprecision(2);

    TileRawData::print_to_stream(m_digits," Val:",text);

    return text.str();
}
