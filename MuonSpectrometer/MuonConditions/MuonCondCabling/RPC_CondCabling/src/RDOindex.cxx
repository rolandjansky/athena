/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/RDOindex.h"

#include <iomanip>

#include "MuonCablingTools/RPCdecoder.h"

RDOindex::RDOindex(unsigned int PAD, unsigned int code) : m_PADid{static_cast<unsigned short int>(PAD)}, m_lvl1_code{code} {
    set_indexes();
}

RDOindex::RDOindex(unsigned int PAD, unsigned int code, std::string Name, int sEta, int sPhi, int dR, int dZ, int dP) :
    m_PADid(static_cast<unsigned short int>(PAD)),
    m_lvl1_code{code},
    m_stationEta{sEta},
    m_stationPhi{sPhi},
    m_doubletR{dR},
    m_doubletZ{dZ},
    m_doubletPhi{dP} {
#ifndef LVL1_STANDALONE
    if (s_rpcIdHelper) m_stationName = s_rpcIdHelper->stationNameIndex(Name);
#endif
    set_indexes();
}

void RDOindex::set_indexes() {
    RPCdecoder decode(m_lvl1_code);
    if (decode) {
        unsigned int sector = decode.logic_sector();
        m_RXid = (sector % 32) % 2;
        m_SLid = sector % 32;
        m_side = (sector < 32) ? 0x66 : 0x65;
        m_RODid = (sector % 32) / 2;
        m_ROBid = (sector % 32) / 2;
        m_status = true;
    }
}
RDOindex::operator bool() const { return m_status; }
bool RDOindex::operator!() const { return !m_status; }

void RDOindex::set_hash(unsigned int h) { m_hash = h; }

#ifndef LVL1_STANDALONE

const RpcIdHelper* RDOindex::s_rpcIdHelper = nullptr;

void RDOindex::setRpcIdHelper(const RpcIdHelper* helper) { s_rpcIdHelper = helper; }

void RDOindex::offline_indexes(int& name, int& eta, int& phi, int& doublet_r, int& doublet_z, int& doublet_phi, int& gas_gap,
                               int& measures_phi, int& strip) const {
    if (m_status) {
        if (m_stationPhi) {
            name = m_stationName;
            eta = m_stationEta;
            phi = m_stationPhi;
            doublet_r = m_doubletR;
            doublet_z = m_doubletZ;
            doublet_phi = m_doubletPhi;
            gas_gap = 1;
            measures_phi = 0;
            strip = 1;
        }
    }
}

void RDOindex::pad_identifier(Identifier& id) const {
    if (m_status) {
        int name = 0;
        int eta = 0;
        int phi = 0;
        int doublet_r = 0;
        int doublet_z = 0;
        int doublet_phi = 0;

        if (m_stationPhi) {
            name = m_stationName;
            eta = m_stationEta;
            phi = m_stationPhi;
            doublet_r = m_doubletR;
            doublet_z = m_doubletZ;
            doublet_phi = m_doubletPhi;
        }

        if (s_rpcIdHelper != 0) id = s_rpcIdHelper->padID(name, eta, phi, doublet_r, doublet_z, doublet_phi);
    }
}

#endif

std::ostream& operator<<(std::ostream& stream, const RDOindex& rdo) {
    std::stringstream tmp_stream;
#ifndef LVL1_STANDALONE

    int name;
    int eta;
    int phi;
    int doublet_r;
    int doublet_z;
    int doublet_phi;
    int gas_gap;
    int measures_phi;
    int strip;

    rdo.offline_indexes(name, eta, phi, doublet_r, doublet_z, doublet_phi, gas_gap, measures_phi, strip);

    tmp_stream << "RPC PAD /" << std::hex << std::showbase << rdo.side() << "/" << rdo.SLid() << "/" << rdo.PADid()
               << "   mapped on offline Id /" << std::dec << name << "/" << eta << "/" << phi << "/" << doublet_r << "/" << doublet_z << "/"
               << doublet_phi << "/" << gas_gap << "/" << measures_phi << "/" << strip << " .... hashId = " << rdo.hash() << std::endl;

#else
    std::string side = (rdo.side() == 0x66) ? "Negative" : "Positive";
    tmp_stream << "/side=" << side << "/ROB=ROD=" << setw(2) << rdo.ROBid() << "/SL=RX=" << setw(2) << rdo.SLid() << "/PAD=" << setw(2)
               << rdo.PADid() << "/" << std::endl;
#endif

    stream << tmp_stream.str();

    return stream;
}
