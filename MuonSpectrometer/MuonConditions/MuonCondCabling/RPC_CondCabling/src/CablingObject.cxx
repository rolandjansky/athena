/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CablingObject.h"

#include <iomanip>

CablingObject::CablingObject(const CablingObject::cablingParameters& params, const std::string& obj_name, IMessageSvc* msgSvc) :
    BaseObject(Logic, obj_name, msgSvc), m_number{params.number}, m_station{params.station}, m_sector_type{params.sectorType} {}

int CablingObject::number() const { return m_number; }
int CablingObject::station() const { return m_station; }
int CablingObject::sector_type() const { return m_sector_type; }

std::ostream& operator<<(std::ostream& stream, const CablingObject& obj) {
    stream << obj.name() << " number " << std::setw(3) << obj.number() << "  associated to sector type " << std::setw(3)
           << obj.sector_type() << " in station n. " << obj.station() << std::endl;
    return stream;
}

void CablingObject::error_header() const {
    __osstream disp;
    disp << "Error in Sector Type " << m_sector_type;
    if (m_station > 0)
        disp << ", station " << m_station << ":" << std::endl;
    else
        disp << ":" << std::endl;

    display_error(disp);
}

void CablingObject::no_connection_error(const std::string& conn_name, int num) {
    error_header();
    DISP << name() << " n. " << number() << " is supposed to receive input from " << conn_name << " n. " << num << " which doesn't exist!"
         << std::endl;
    DISP_ERROR;
}
