/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/RPCchamber.h"

#include <iomanip>

#include "RPC_CondCabling/EtaCMA.h"
#include "RPC_CondCabling/SectorLogicSetup.h"
#include "RPC_CondCabling/WiredOR.h"

using namespace RPC_CondCabling;
/// Helper struct to reduce the number of arguments in the constructor
RPCchamber::RPCchamber(RPCchamber::chamberParameters params, IMessageSvc* msgSvc) : CablingObject{params, "RPC", msgSvc}, m_params{params} {
    for (int i = 0; i < m_params.etaStrips; ++i) m_eta_read_mul.push_back(0);
}

RPCchamber::~RPCchamber() = default;

int RPCchamber::eta_strips() const { return m_params.etaStrips; }
int RPCchamber::eta_connectors() const { return m_params.etaConnectors; }
int RPCchamber::eta_strip_global() const { return m_eta_strip_global; }
int RPCchamber::eta_conn_global() const { return m_eta_conn_global; }
int RPCchamber::phi_strips() const { return m_params.phiStrips; }

int RPCchamber::phi_connectors() const { return m_params.phiConnectors; }

int RPCchamber::ijk_etaReadout() const { return m_params.ijk_EtaReadOut; }
int RPCchamber::ijk_phiReadout() const { return m_params.ijk_PhiReadOut; }

std::string RPCchamber::chamber_name() const { return m_params.chamberName; }
std::string RPCchamber::stationName() const { return m_params.chamberName.substr(0, 3); }
int RPCchamber::stationEta() const { return m_params.stationEta; }
int RPCchamber::doubletR() const { return m_params.doubletR; }
int RPCchamber::doubletZ() const { return m_params.doubletZ; }
int RPCchamber::phiReadoutPannels() const { return m_params.phiReadOutPanels; }

int RPCchamber::strips_in_Eta_Conn() const { return m_params.stripsInEtaCon; }
int RPCchamber::strips_in_Phi_Conn() const { return m_params.stripsInPhiCon; }

const RPCchamber::ReadoutCh& RPCchamber::eta_read_mul() const { return m_eta_read_mul; }

const RPCchamber::CMAlist& RPCchamber::readoutCMAs() const { return m_readoutCMAs; }
const RPCchamber::WORlist& RPCchamber::readoutWORs() const { return m_readoutWORs; }

bool RPCchamber::setup(SectorLogicSetup& setup) {
    RPCchamber* rpc = setup.find_chamber(station(), 1);
    int ijk_eta = rpc->ijk_etaReadout();
    int ijk_phi = rpc->ijk_phiReadout();

    if (ijk_eta != m_params.ijk_EtaReadOut) {
        error("==> mismatch of ijk_etaReadout with respect to others RPC");
        return false;
    }

    if (ijk_phi != m_params.ijk_PhiReadOut) {
        error("==> mismatch of ijk_phiReadout with respect to others RPC");
        return false;
    }

    return true;
}

bool RPCchamber::check() {
    if (m_readoutCMAs.size() == 0) {
        error("==> No readout coverage for this chamber!");
        return false;
    }

    RPCchamber::CMAlist::const_iterator cma = m_readoutCMAs.begin();
    const CMAinput IO = (*cma)->whichCMAinput(station());

    int channels = m_params.etaStrips;
    for (int i = 0; i < channels; ++i) {
        if (!m_eta_read_mul[i]) {
            error("==> No readout coverage for the full set of ETA strip!");
            return false;
        }
        if (m_eta_read_mul[i] > 1 && IO == Pivot) {
            error("==> Pivot plane ETA strips must be read only once!");
            return false;
        }
        if (m_eta_read_mul[i] > 2) {
            error("==> Confirm plane ETA strips can be read only twice!");
            return false;
        }
    }

    if (m_readoutWORs.size() > 1) {
        error("==> Gives input to more than 1 Wired OR pannel!");
        return false;
    }
    return true;
}

void RPCchamber::error(const std::string& mess) const {
    error_header();
    DISP << mess << std::endl << *this;
    DISP_ERROR;
}

bool RPCchamber::local_strip(ViewType side, int strip_number, int& local_address) const {
    if (side == Eta) {
        if (strip_number >= m_params.etaStrips) return false;
        if (number()) {
            local_address = strip_number + 1;
        } else {
            int middle = m_params.etaStrips / 2;
            int address = strip_number - middle;
            local_address = (strip_number < middle) ? address : address + 1;
        }
        return true;
    }

    if (side == Phi) {
        if (strip_number >= m_params.phiStrips) return false;
        local_address = strip_number + 1;
    }

    return false;
}

bool RPCchamber::global_strip(ViewType side, HalfType h_barrel, int strip_number, int& global_address) const {
    int local_address;
    if (local_strip(side, strip_number, local_address)) {
        if (side == Eta) {
            global_address = local_address + m_eta_strip_global;
            if (number()) {
                if (h_barrel == Negative) global_address = -global_address;
            } else {
                if (h_barrel == Negative && global_address > 0) return false;
                if (h_barrel == Positive && global_address < 0) return false;
            }
            return true;
        }

        if (side == Phi) { return false; }
    }

    return false;
}

bool RPCchamber::local_connector(ViewType side, int strip_number, int& local_address, int& low_eta_strips, int& hi_eta_strips) const {
    int strip_address;
    if (local_strip(side, strip_number, strip_address)) {
        int address = abs(strip_address) - 1;
        int strip_sign = (abs(strip_address)) / strip_address;
        if (side == Eta) {
            local_address = (address / m_params.stripsInEtaCon + 1) * strip_sign;
            low_eta_strips = address % m_params.stripsInEtaCon;
            int left = m_params.stripsInEtaCon - low_eta_strips - 1;
            int residual_strips = residual(side, strip_number);
            hi_eta_strips = (left <= residual_strips) ? left : residual_strips;
            return true;
        }
        if (side == Phi) { return false; }
    }

    return false;
}

int RPCchamber::residual(ViewType side, int strip_number) const {
    if (side == Eta) {
        if (number()) {
            return (m_params.etaStrips - 1 - strip_number);
        } else {
            int total_strips = m_params.etaStrips / 2;
            if (strip_number < total_strips)
                return strip_number;
            else
                return m_params.etaStrips - strip_number - 1;
        }
    }
    if (side == Phi) { return (m_params.phiStrips - 1 - strip_number); }
    return 0;
}

bool RPCchamber::global_connector(ViewType side, HalfType h_barrel, int strip_number, int& global_address, int& left_strips,
                                  int& right_strips) const {
    int l_address;
    if (local_connector(side, strip_number, l_address, left_strips, right_strips)) {
        if (side == Eta) {
            global_address = l_address + m_eta_conn_global;
            if (number()) {
                if (h_barrel == Negative) global_address = -global_address;
            } else {
                if (h_barrel == Negative && global_address > 0) return false;
                if (h_barrel == Positive && global_address < 0) return false;
            }
            return true;
        }
        if (side == Phi) { return false; }
    }

    return false;
}

bool RPCchamber::Gstrip_2_Lnumber(ViewType side, int global_address, int& strip_number) const {
    if (side == Eta) {
        if (number()) {
            strip_number = abs(global_address) - m_eta_strip_global - 1;
            if (strip_number <= m_params.etaStrips - 1) return true;
        } else {
            strip_number = (global_address > 0) ? global_address + m_params.etaStrips / 2 - 1 : global_address + m_params.etaStrips / 2;
            if (strip_number <= m_params.etaStrips - 1 && strip_number >= 0) return true;
        }
    }
    if (side == Phi) { return false; }

    return false;
}

bool RPCchamber::Gconn_2_Lnumber(ViewType side, int global_address, int& local_address, int& strip_number) const {
    if (side == Eta) {
        if (number()) {
            local_address = abs(global_address) - m_eta_conn_global - 1;
            if (local_address <= m_params.etaConnectors - 1) {
                strip_number = local_address * m_params.stripsInEtaCon;
                return true;
            }
        } else {
            local_address =
                (global_address > 0) ? global_address + m_params.etaConnectors / 2 - 1 : global_address + m_params.etaConnectors / 2;
            if (local_address <= m_params.etaConnectors - 1 && local_address >= 0) {
                strip_number = local_address * m_params.stripsInEtaCon;
                return true;
            }
        }
    }

    if (side == Phi) { return false; }

    return false;
}

void RPCchamber::set_eta_st_global(int gl) { m_eta_strip_global = gl; }

void RPCchamber::set_eta_co_global(int gl) { m_eta_conn_global = gl; }

void RPCchamber::Print(std::ostream& stream, bool detail) const {
    stream << " chamber n. " << std::setw(2) << number() << " ";
    stream << chamber_name();
    if (chamber_name().length() == 5) stream << " ";
    stream << "  " << std::setw(2) << stationEta() << std::setw(2) << doubletR();
    stream << std::setw(2) << doubletZ() << "  ";
    stream << "  (stat " << station() << ")";
    stream << " :  eta -> " << std::setw(2) << eta_strips() << " ";
    stream << std::setw(2) << eta_connectors() << " ";
    stream << std::setw(2) << std::setfill('0') << ijk_etaReadout() << std::setfill(' ');
    stream << "    phi -> " << std::setw(2) << phi_strips() << " ";
    stream << std::setw(2) << phi_connectors() << " ";
    stream << std::setw(2) << std::setfill('0') << ijk_phiReadout() << std::setfill(' ');
    stream << std::endl;
    if (detail) {
        stream << " global strips offset -( " << m_eta_strip_global << " )-   global connectors offset -( " << m_eta_conn_global << " )-"
               << std::endl;
        stream << " It gives input to " << m_readoutCMAs.size() << " Eta CMA:" << std::endl;
        CMAlist::const_iterator cma = m_readoutCMAs.begin();
        while (cma != m_readoutCMAs.end()) {
            stream << *(*cma);
            ++cma;
        }
        stream << " Eta Readout multiplicity:" << std::endl;
        stream << "                          "
               << " 1   5    10   15   20   25   30   35   40" << std::endl;
        stream << "                          "
               << " |   |    |    |    |    |    |    |    |" << std::endl;
        stream << "                           ";
        for (int i = 0; i < eta_strips(); ++i) stream << m_eta_read_mul[i];
        stream << std::endl;
        stream << " It gives input to " << m_readoutWORs.size() << " WiredOR pannel:" << std::endl;
        RPCchamber::WORlist::const_iterator wor = m_readoutWORs.begin();
        while (wor != m_readoutWORs.end()) {
            stream << *(*wor);
            ++wor;
        }

        stream << "========================================"
               << "=======================================" << std::endl;
    }
}

void RPCchamber::add_cma(const EtaCMA* cma) { m_readoutCMAs.push_back(cma); }

void RPCchamber::add_wor(const WiredOR* wor) { m_readoutWORs.push_back(wor); }

void RPCchamber::add_eta_channel(int channel) { m_eta_read_mul[channel] += 1; }

std::string RPCchamber::extendedName(int sector) const {
    std::string side;
    if (sector <= 31)
        side = "C";
    else
        side = "A";

    std::string sd;

    switch (stationName()[2]) {
        case 'L': sd = (sector % 2) ? "HV" : "RO"; break;

        case 'E': sd = (sector % 2) ? "HV" : "RO"; break;

        case 'R': sd = (sector % 2) ? "HV" : "RO"; break;

        case 'M': sd = (sector % 2) ? "HV" : "RO"; break;

        case 'S': sd = (sector % 2) ? "RO" : "HV"; break;

        case 'F': sd = (sector % 2) ? "RO" : "HV"; break;

        case 'G': sd = (sector % 2) ? "RO" : "HV"; break;

        default: return "";
    }

    __osstream out;

    int physicsSector = (((sector + 1) % 32) / 2 + 1) % 16;
    if (!physicsSector) physicsSector = 16;

    if (stationName().c_str()[1] == 'O' && (stationName().c_str()[2] == 'F' || stationName().c_str()[2] == 'G')) {
        int etaIdx = (stationName().c_str()[2] == 'F') ? (stationEta() - 1) * 2 + 1 : (stationEta()) * 2;

        out << stationName() << etaIdx << side << std::setw(2) << std::setfill('0') << physicsSector << "-" << sd << "-" << doubletZ();
    } else {
        out << stationName() << stationEta() << side << std::setw(2) << std::setfill('0') << physicsSector << "-" << sd << "-"
            << doubletZ();
    }

    std::string outstr;
    outstr = out.str();

    return outstr;
}

bool RPCchamber::inversion(int sector) const {
    switch (stationName()[2]) {
        case 'L':
            if (sector % 2)
                return (sector <= 31) ? true : false;  // HV
            else
                return (sector <= 31) ? false : true;  // RO
            break;

        case 'E':
            if (sector % 2)
                return (sector <= 31) ? true : false;  // HV
            else
                return (sector <= 31) ? false : true;  // RO
            break;

        case 'R':
            if (sector % 2)
                return (sector <= 31) ? false : false;  // HV
            else
                return (sector <= 31) ? false : false;  // RO
            break;

        case 'M':
            if (sector % 2)
                return (sector <= 31) ? false : false;  // HV
            else
                return (sector <= 31) ? false : false;  // RO
            break;

        case 'S':
            if (sector % 2)
                return (sector <= 31) ? true : false;  // RO
            else
                return (sector <= 31) ? false : true;  // HV
            break;

        case 'F':
            if (stationName()[1] == 'O' && (sector == 25 || sector == 26 || sector == 57 || sector == 58)) {
                if (sector % 2)
                    return (sector <= 31) ? false : true;  // RO
                else
                    return (sector <= 31) ? true : false;  // HV
            }

            if (sector % 2)
                return (sector <= 31) ? true : false;  // RO
            else
                return (sector <= 31) ? false : true;  // HV
            break;

        case 'G':
            if (stationName()[1] == 'O' && stationName()[3] != '8' && (sector == 25 || sector == 26 || sector == 57 || sector == 58)) {
                if (sector % 2)
                    return (sector <= 31) ? false : true;  // RO
                else
                    return (sector <= 31) ? true : false;  // HV
            }

            if (sector % 2)
                return (sector <= 31) ? true : false;  // RO
            else
                return (sector <= 31) ? false : true;  // HV
            break;

        default: return false;
    }
}
