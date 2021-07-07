/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/WiredORdata.h"

#include <iomanip>

using namespace RPC_CondCabling;

WiredORdata::WiredORdata(DBline& data, int type, IMessageSvc* msgSvc) : BaseObject{Logic, "Wired OR Data", msgSvc} {
    reset_data();
    if (!(data("station") >> m_station)) return;

    m_fail = false;

    (++data)("{");
    do {
        WiredOR::parseParams parse_params;
        parse_params.sectorType = type;
        parse_params.station = m_station;
        parse_params.number = -1;
        if (get_data(data, parse_params)) { m_wor.emplace_back(parse_params, msgSvc); }
        data++;
    } while (!data("}"));
}
WiredORdata::~WiredORdata() = default;
void WiredORdata::reset_data() { m_fail = true; }

bool WiredORdata::confirm_boundary(WiredOR::parseParams& parse_params) const {
    if (parse_params.start > parse_params.stop) {
        __osstream display;

        display << "WORdata error in configuration for Sector Type " << parse_params.sectorType << ", station " << parse_params.station
                << ", WOR number " << parse_params.number << std::endl
                << " start RPC chamber (" << parse_params.start << ") is greater than "
                << "stop RPC chamber (" << parse_params.stop << ")" << std::endl;
        display_error(display);
        return false;
    }
    return true;
}

bool WiredORdata::get_data(DBline& data, WiredOR::parseParams& parse_params) {
    reset_data();
    if (data("hard wired phi chamber") >> parse_params.number >> parse_params.start >> "-" >> parse_params.stop) {
        m_fail = false;
        if (!confirm_boundary(parse_params)) m_fail = true;
    }

    return !m_fail;
}

std::unique_ptr<WiredOR> WiredORdata::give_wor(void) {
    if (m_wor.size()) {
        std::unique_ptr<WiredOR> Wor = std::make_unique<WiredOR>(m_wor.front());
        m_wor.pop_front();
        return Wor;
    }
    return nullptr;
}

void WiredORdata::Print(std::ostream& stream, bool detail) const {
    stream << "Wired OR data of station n. " << m_station;
    // stream << " belonging to sector type " << parse_params.sectorType << std::endl;
    stream << "It contains " << m_wor.size();
    stream << " Wired ORs:" << std::endl;
    for (const WiredOR& it : m_wor) stream << ShowRequest<WiredOR>(it, detail);
}

std::ostream& operator<<(std::ostream& stream, WiredORdata& data) {
    data.Print(stream, false);
    return stream;
}
