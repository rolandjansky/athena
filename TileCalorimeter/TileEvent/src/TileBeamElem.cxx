/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileBeamElem.cxx
//  Author   : AS
//  Created  : Oct, 2003
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//    29Oct03: Created for testbeam data coming from Beam ROD
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileBeamElem.h"

#include <iostream>
#include <sstream>
#include <iomanip>

TileBeamElem::TileBeamElem( const Identifier& id,
                            const std::vector<uint32_t>& digits )

  : TileRawData( id )
  , m_digits ( digits )
{
}

TileBeamElem::TileBeamElem( const HWIdentifier& HWid,
                            const std::vector<uint32_t>& digits )

  : TileRawData( HWid )
  , m_digits ( digits )
{
}

TileBeamElem::TileBeamElem( const HWIdentifier& HWid,
                            std::vector<uint32_t>&& digits )

  : TileRawData( HWid )
  , m_digits ( std::move(digits) )
{
}

TileBeamElem::TileBeamElem( const HWIdentifier& HWid,
                            uint32_t digit )

  : TileRawData( HWid )
  , m_digits ()
{
  m_digits.push_back(digit);
}

void TileBeamElem::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileBeamElem::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << (std::string) (*(TileRawData *)this);

    TileRawData::print_to_stream(m_digits," Val:",text);

    return text.str();
}
