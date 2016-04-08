/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawData.cxx
//  Author   : Alexandre Solodkov
//  Created  : Oct, 2002
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//    02Oct02: Created
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileRawData.h"
#include "TileConditions/TileCablingService.h"

#include "TileIdentifier/TileHWID.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// get cabling
TileCablingService * s_cabling = TileCablingService::getInstance();

TileRawData::TileRawData( const Identifier& id )
{
    m_adc_hwid = s_cabling->s2h_adc_id(id);
}

void TileRawData::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileRawData::operator std::string() const
{
    std::string result(whoami());
    result += " Id = " + s_cabling->getTileHWID()->to_string(m_adc_hwid);
    return result;    
}

Identifier TileRawData::adc_ID(void)   const {
    return s_cabling->h2s_adc_id(m_adc_hwid);
}

Identifier TileRawData::pmt_ID(void)   const {
    return s_cabling->h2s_pmt_id(m_adc_hwid);
}

Identifier TileRawData::cell_ID(void)   const {
    return s_cabling->h2s_cell_id(m_adc_hwid);
}

Identifier TileRawData::cell_ID_index(int & index, int & pmt)   const {
    return s_cabling->h2s_cell_id_index(m_adc_hwid, index, pmt);
}

Identifier TileRawData::tt_ID(void)   const {
    return s_cabling->pmt2tt_id(adc_ID());
}

Identifier TileRawData::mt_ID(void)   const {
    return s_cabling->pmt2mt_id(adc_ID());
}

int TileRawData::frag_ID(void)   const {
    return s_cabling->frag(m_adc_hwid);
}

void TileRawData::print_to_stream ( const std::vector<float> val,
                                    const std::string & label,
                                    std::ostringstream & text)
{
    text << label;

    std::vector<float>::const_iterator it1=val.begin();
    std::vector<float>::const_iterator it2=val.end();

    for ( ; it1!=it2; ++it1) text << " " << (*it1);
}


void TileRawData::print_to_stream ( const std::vector<double> val,
                                    const std::string & label,
                                    std::ostringstream & text)
{
    text << label;

    std::vector<double>::const_iterator it1=val.begin();
    std::vector<double>::const_iterator it2=val.end();

    for ( ; it1!=it2; ++it1) text << " " << (*it1);
}

void TileRawData::print_to_stream ( const std::vector<uint32_t> val,
                                    const std::string & label,
                                    std::ostringstream & text)
{
    text << label;

    std::vector<uint32_t>::const_iterator it1=val.begin();
    std::vector<uint32_t>::const_iterator it2=val.end();

    for ( ; it1!=it2; ++it1) text << " " << (*it1);
}

