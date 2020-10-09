/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileSimData.cxx
//  Author   : Alexandre Solodkov
//  Created  : Feb, 2003
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//    02Feb03: Created
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileSimData.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/TileID.h"

#include <iostream>

// get cabling
namespace {
const TileCablingService * const s_cabling = TileCablingService::getInstance();
}

TileSimData::TileSimData( const Identifier& id ) : m_pmt_id( id )
{
}

TileSimData::TileSimData( const HWIdentifier& HWid )
  : m_pmt_id (s_cabling->h2s_pmt_id(HWid))
{
}

void TileSimData::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileSimData::operator std::string() const
{
    std::string result(whoami());
    result += " Id = " + s_cabling->getTileID()->to_string(identify(),-1); // pmt_id => -1 level from adc_id
    return result;    
}

HWIdentifier TileSimData::pmt_HWID(void)   const {
    return s_cabling->s2h_channel_id(m_pmt_id);
}

Identifier TileSimData::tt_ID(void)   const {
    return s_cabling->pmt2tt_id(m_pmt_id);
}

Identifier TileSimData::mt_ID(void)   const {
    return s_cabling->pmt2mt_id(m_pmt_id);
}

int TileSimData::frag_ID(void)   const {
    return s_cabling->frag(pmt_HWID());
}

