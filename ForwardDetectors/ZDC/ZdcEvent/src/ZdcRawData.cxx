/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : ZdcRawData.cxx
//  Author   : Peter Steinberg
//  Created  : March 2009
//
//  DESCRIPTION:
//     
//
//  HISTORY:
//    20 March 2009: Created
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "ZdcEvent/ZdcRawData.h"
#include "ZdcConditions/ZdcCablingService.h"
#include "ZdcIdentifier/ZdcHardwareID.h"

#include <iostream>
#include <sstream>
#include <iomanip>

ZdcRawData::ZdcRawData( const Identifier& id )
  : m_id (id)
{
}

void ZdcRawData::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

ZdcRawData::operator std::string() const
{
    std::string result(whoami());
    //result += " Id = " + ZdcCablingService::getInstance()->getZdcID()->to_string(m_id);
    return result;    
}


/*
Identifier ZdcRawData::side_ID(void)   const {
    return ZdcCablingService::getInstance()->h2s_side_id(m_channel_hwid);
}

Identifier ZdcRawData::module_ID(void)   const {
    return ZdcCablingService::getInstance()->h2s_module_id(m_channel_hwid);
}

Identifier ZdcRawData::type_ID(void)   const {
    return ZdcCablingService::getInstance()->h2s_type_id(m_channel_hwid);
}

Identifier ZdcRawData::channel_ID(void)   const {
    return ZdcCablingService::getInstance()->h2s_channel_id(m_channel_hwid);
}

Identifier ZdcRawData::gain_ID(void)   const {
    return ZdcCablingService::getInstance()->h2s_gain_id(m_channel_hwid);
}

Identifier ZdcRawData::delay_ID(void)   const {
    return ZdcCablingService::getInstance()->h2s_delay_id(m_channel_hwid);
}
*/


void ZdcRawData::print_to_stream ( const std::vector<double>& val,
                                    const std::string & label,
                                    std::ostringstream & text)
{
    text << label;
    for (double v : val) {
      text << " " << v;
    }
}

void ZdcRawData::print_to_stream ( const std::vector<int>& val,
                                    const std::string & label,
                                    std::ostringstream & text)
{
    text << label;
    for (double v : val) {
      text << " " << v;
    }
}

