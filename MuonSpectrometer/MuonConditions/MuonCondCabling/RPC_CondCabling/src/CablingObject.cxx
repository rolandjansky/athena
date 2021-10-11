/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CablingObject.h"

#include <iomanip>

CablingObject::CablingObject(const CablingObject::cablingParameters& params, const std::string& obj_name) :
    BaseObject(Logic, obj_name), m_number{params.number}, m_station{params.station}, m_sector_type{params.sectorType} {}

int CablingObject::number() const { return m_number; }
int CablingObject::station() const { return m_station; }
int CablingObject::sector_type() const { return m_sector_type; }

std::ostream& operator<<(std::ostream& stream, const CablingObject& obj) {
    stream << obj.name() << " number " << std::setw(3) << obj.number() << "  associated to sector type " << std::setw(3)
           << obj.sector_type() << " in station n. " << obj.station() << std::endl;
    return stream;
}

std::string CablingObject::error_header() const {
    std::ostringstream disp;
    disp << "Error in Sector Type " << m_sector_type;
    if (m_station > 0) disp << ", station " << m_station;
    disp << ":" << std::endl;
    return disp.str();
}

std::string CablingObject::no_connection_error(const std::string& conn_name, int num) const {
    std::ostringstream disp;
    disp << error_header() << name() << " n. " << number() << " is supposed to receive input from " << conn_name << " n. " << num << " which doesn't exist!";
    return disp.str();
}
