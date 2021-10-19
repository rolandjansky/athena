/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/RPCchamberdata.h"

#include "AthenaKernel/errorcheck.h"

#include <iomanip>

using namespace RPC_CondCabling;

RPCchamberdata::RPCchamberdata(DBline& data, int type) : BaseObject(Logic, "RPC Chamber Data") {
    int chams{0}, stripsInEtaCon{0}, stripsInPhiCon{0};

    reset_data();

    m_fail = false;
    if (!(data("station") >> m_station)) return;
    if (!(data("made of") >> chams >> "chamber. Strips in connectors:")) return;
    if (!(data("eta") >> stripsInEtaCon)) return;
    if (!(data("phi") >> stripsInPhiCon)) return;

    (++data)("{");
    do {
        RPCchamber::chamberParameters params{};
        params.sectorType = type;
        params.station = m_station;
        params.stripsInEtaCon = stripsInEtaCon;
        params.stripsInPhiCon = stripsInPhiCon;

        if (get_data(data, params)) { m_rpc.emplace_back(params); }
        data++;
    } while (!data("}"));
}

void RPCchamberdata::reset_data() { m_fail = true; }

bool RPCchamberdata::confirm_connectors(ViewType side, RPCchamber::chamberParameters& params) {
    int strips = (side == ViewType::Phi) ? params.phiStrips : params.etaStrips;

    if (side == ViewType::Phi)
        params.phiConnectors = strips / params.stripsInPhiCon;
    else
        params.etaConnectors = strips / params.stripsInEtaCon;

    int connectors = (side == ViewType::Phi) ? params.phiConnectors : params.etaConnectors;
    int strips_in_conn = (side == ViewType::Phi) ? params.stripsInPhiCon : params.stripsInEtaCon;
    float str = (float)strips / (float)connectors;
    std::string view = (side == ViewType::Phi) ? "phi" : "eta";

    std::ostringstream disp;

    if (str > strips_in_conn) {
        REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "RPCchamberdata")
             << "RPCdata error in configuration for Sector Type " << params.sectorType << ", station " << params.station << ", RPC number "
             << params.number << std::endl
             << " " << view << " strips into connectors must be less than " << strips_in_conn << " (instead are " << std::setprecision(2)
             << str << ")";
        return false;
    }
    if (params.number == 0 && ((params.etaStrips % 2) || params.etaConnectors % 2)) {
        REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "RPCchamberdata")
             << "RPCdata error in configuration for Sector Type " << params.sectorType << ", station " << params.station << ", RPC number "
             << params.number << std::endl
             << " " << view << " strips and/or connectors must be "
             << "multiple of 2 "
             << " (eta_strips " << params.etaStrips << ", eta_conn " << params.etaConnectors << ")";
        return false;
    }
    if (strips_in_conn * connectors != strips) {
        REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "RPCchamberdata")
             << "RPCdata error in configuration for Sector Type " << params.sectorType << ", station " << params.station << ", RPC number "
             << params.number << std::endl
             << " strips into " << view << " connectors are " << std::setprecision(2) << str << " instead of " << strips_in_conn;
        return false;
    }
    return true;
}

bool RPCchamberdata::confirm_ijk(ViewType side, RPCchamber::chamberParameters& params) {
    int ijk = (side == ViewType::Phi) ? params.ijk_PhiReadOut : params.ijk_EtaReadOut;
    std::string view = (side == ViewType::Phi) ? "phi" : "eta";

    std::ostringstream disp;

    if (ijk != 1 && ijk != 10) {
        REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "RPCchamberdata")
             << "RPCdata error in configuration for Sector Type " << params.sectorType << ", station " << params.station << ", RPC number "
             << params.number << std::endl
             << " " << view << " ijk readout must be 01 or 10; "
             << " on the contrary it is " << std::setw(2) << std::setfill('0') << ijk << std::setfill(' ');
        return false;
    }

    return true;
}

bool RPCchamberdata::get_data(DBline& data, RPCchamber::chamberParameters& params) {
    reset_data();
    if (data("cham") >> params.number >> params.chamberName >> params.stationEta >> params.doubletR >> params.doubletZ >>
        params.phiReadOutPanels >> "Eview" >> params.etaStrips >> params.ijk_EtaReadOut >> "Pview" >> params.phiStrips >>
        params.ijk_PhiReadOut) {
        m_fail = false;
        if (!confirm_connectors(ViewType::Eta, params) || !confirm_connectors(ViewType::Phi, params)) m_fail = true;
        if (!confirm_ijk(ViewType::Eta, params) || !confirm_ijk(ViewType::Phi, params)) m_fail = true;
    }

    return !m_fail;
}

std::unique_ptr<RPCchamber> RPCchamberdata::give_rpc() {
    if (!m_rpc.empty()) {
        std::unique_ptr<RPCchamber> cham = std::make_unique<RPCchamber>(m_rpc.front());
        m_rpc.pop_front();
        return cham;
    }
    return nullptr;
}

void RPCchamberdata::Print(std::ostream& stream, bool detail) const {
    // stream << "RPC data of station n. " << params.station;
    // stream << " belonging to sector type " << params.sectorType << std::endl;
    stream << "It contains " << m_rpc.size();
    stream << " RPC chambers:" << std::endl;
    std::list<RPCchamber>::const_iterator it;
    for (it = m_rpc.begin(); it != m_rpc.end(); ++it) stream << ShowRequest<RPCchamber>(*it, detail);
}
