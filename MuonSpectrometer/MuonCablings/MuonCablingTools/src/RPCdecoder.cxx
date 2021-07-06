/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/RPCdecoder.h"

#include <string>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

///////////////////////////////////////////////////////////////////////////////

const int RPCdecoder::s_value_boundaries[4][2] = {
    {1, 2},   // Min-Max of strip_type value
    {0, 63},  // Min-Max of logic_sector value
    {1, 3},   // Min-Max of station value
    {0, 1}    // Min_max of rpc_layer value
};
const char RPCdecoder::s_value_descriptions[4][15] = {{"strip_type"}, {"logic_sector"}, {"lvl1_station"}, {"rpc_layer"}};

///////////////////////////////////////////////////////////////////////////////
RPCdecoder::RPCdecoder() { this->reset(); }

RPCdecoder::RPCdecoder(unsigned int code) {
    this->reset();
    this->set_indexes(code);
}

RPCdecoder::RPCdecoder(int strip_type, int logic_sector, int lvl1_station, int rpc_layer, int rpc_z_index, int strip_number) {
    this->reset();
    this->set_code(strip_type, logic_sector, lvl1_station, rpc_layer, rpc_z_index, strip_number);
}

RPCdecoder::RPCdecoder(ViewType side, int logic_sector, int lvl1_station, int rpc_layer, int rpc_z_index, int strip_number) {
    this->reset();
    int strip_type = this->side2type(side);
    this->set_code(strip_type, logic_sector, lvl1_station, rpc_layer, rpc_z_index, strip_number);
}

RPCdecoder::RPCdecoder(const RPCdecoder& deco) {
    this->reset();
    this->set_indexes(deco.code());
}

///////////////////////////////////////////////////////////////////////////////

RPCdecoder& RPCdecoder::operator=(const RPCdecoder& deco) {
    this->reset();
    this->set_indexes(deco.code());
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
void RPCdecoder::reset() {
    this->reset_status();
    this->reset_data();
}

void RPCdecoder::reset_data() {
    m_code = 0;
    m_strip_type = -1;
    m_logic_sector = -1;
    m_lvl1_station = -1;
    m_rpc_layer = -1;
    m_rpc_z_index = -1;
    m_strip_number = -1;
}

void RPCdecoder::reset_status() { m_fail = false; }

void RPCdecoder::fault_decoding(int value, value_type type) {
    std::ostringstream ostream;
    ostream << "fault_decoding() - RPC decoder error: received " << s_value_descriptions[type] << " = " << value
            << "  (min = " << s_value_boundaries[type][0] << ", max = " << s_value_boundaries[type][1] << ")";
#ifdef LVL1_STANDALONE
    MsgStream log(Athena::getMessageSvc(), "RPCdecoder");
    log << MSG::INFO << ostream << endmsg;
#endif
    m_fail = true;
}

bool RPCdecoder::OK(int value, value_type type) const {
    if (value >= s_value_boundaries[type][0] && value <= s_value_boundaries[type][1]) return true;
    return false;
}

void RPCdecoder::set_code(int strip_type, int logic_sector, int lvl1_station, int rpc_layer, int rpc_z_index, int strip_number) {
    if (!OK(strip_type, Strip_Type)) fault_decoding(strip_type, Strip_Type);
    if (!OK(logic_sector, Logic_Sector)) fault_decoding(logic_sector, Logic_Sector);
    if (!OK(lvl1_station, LVL1_Station)) fault_decoding(lvl1_station, LVL1_Station);
    if (!OK(rpc_layer, RPC_Layer)) fault_decoding(rpc_layer, RPC_Layer);

    if (m_fail) {
        this->reset_data();
        return;
    }

    m_code = strip_type * 100000000 + logic_sector * 1000000 + lvl1_station * 100000 + rpc_layer * 10000 + rpc_z_index * 100 + strip_number;

    m_strip_type = strip_type;
    m_logic_sector = logic_sector;
    m_lvl1_station = lvl1_station;
    m_rpc_layer = rpc_layer;
    m_rpc_z_index = rpc_z_index;
    m_strip_number = strip_number;
}

void RPCdecoder::set_indexes(unsigned int code) {
    m_strip_number = code % 100;
    m_rpc_z_index = (code / 100) % 100;
    m_rpc_layer = (code / 10000) % 10;
    m_lvl1_station = (code / 100000) % 10;
    m_logic_sector = (code / 1000000) % 100;
    m_strip_type = code / 100000000;

    if (!OK(m_strip_type, Strip_Type)) fault_decoding(m_strip_type, Strip_Type);
    if (!OK(m_logic_sector, Logic_Sector)) fault_decoding(m_logic_sector, Logic_Sector);
    if (!OK(m_lvl1_station, LVL1_Station)) fault_decoding(m_lvl1_station, LVL1_Station);
    if (!OK(m_rpc_layer, RPC_Layer)) fault_decoding(m_rpc_layer, RPC_Layer);

    if (m_fail) {
        this->reset_data();
        return;
    }

    m_code = code;
}

///////////////////////////////////////////////////////////////////////////////

RPCdecoder& RPCdecoder::operator()(unsigned int code) {
    this->reset();
    this->set_indexes(code);
    return *this;
}

RPCdecoder& RPCdecoder::operator()(int strip_type, int logic_sector, int lvl1_station, int rpc_layer, int rpc_z_index, int strip_number) {
    this->reset();
    this->set_code(strip_type, logic_sector, lvl1_station, rpc_layer, rpc_z_index, strip_number);
    return *this;
}

RPCdecoder& RPCdecoder::operator()(ViewType side, int logic_sector, int lvl1_station, int rpc_layer, int rpc_z_index, int strip_number) {
    this->reset();
    int strip_type = side2type(side);
    this->set_code(strip_type, logic_sector, lvl1_station, rpc_layer, rpc_z_index, strip_number);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

ViewType RPCdecoder::view(void) const {
    if (m_strip_type == -1) return NoView;
    return (m_strip_type == 1) ? Phi : Eta;
}

HalfType RPCdecoder::half_barrel(void) const {
    if (m_logic_sector == -1) return NoHalf;
    return (m_logic_sector < 32) ? Negative : Positive;
}

int RPCdecoder::side2type(ViewType side) const {
    if (side == NoView) return -1;
    return (side == Eta) ? 2 : 1;
}

void RPCdecoder::Print(std::ostream& stream, bool detail) const {
    unsigned int div = 247;
    std::string half = (half_barrel() == Negative) ? "Negative" : "Positive";

    stream << "RPC strip hash code " << code() << ":" << std::endl;
    if (detail) {
        stream << "   half barrel            = " << half << std::endl;
        stream << "   type (1=Phi/2=Eta)     = " << strip_type() << std::endl;
        stream << "   logic sector (0" << (char)div << "63)    = " << logic_sector() << std::endl;
        stream << "   level-1 station (1" << (char)div << "n)  = " << lvl1_station() << std::endl;
        stream << "   RPC layer (0/1)        = " << rpc_layer() << std::endl;
        stream << "   RPC z index (0" << (char)div << "n)      = " << rpc_z_index() << std::endl;
        stream << "   RPC strip number (0" << (char)div << "n) = " << strip_number() << std::endl;
        stream << "   strip cabling code     = " << cabling_code() << std::endl;
    }
}
