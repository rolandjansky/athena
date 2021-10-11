/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/WiredOR.h"

#include "AthenaKernel/errorcheck.h"
#include "RPC_CondCabling/SectorLogicSetup.h"

#include <iomanip>

using namespace RPC_CondCabling;

WiredOR::WiredOR(parseParams pars) : CablingObject{pars, "WOR"}, m_params{pars} {}

WiredOR::~WiredOR() = default;

bool WiredOR::connect(SectorLogicSetup& setup) {
    // Check if RPC chambers exist
    for (int i = m_params.start; i <= m_params.stop; ++i) {
        RPCchamber* rpc = setup.find_chamber(station(), i);

        if (rpc) {
            rpc->add_wor(this);
            m_RPCread.insert(RPClink::value_type(i, rpc));
        } else {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR") << no_connection_error("RPC", i);
            return false;
        }
    }
    return true;
}

int WiredOR::give_max_phi_strips() const {
    if (m_params.side != ViewType::Phi) return 0;
    int max{0};
    for (const auto& it : m_RPCread) { max = std::max(it.second->phi_strips(), max); }
    return max;
}

int WiredOR::give_max_eta_strips() const {
    if (m_params.side != ViewType::Eta) return 0;
    int max{0};
    for (const auto& it : m_RPCread) { max = std::max(max, it.second->eta_strips()); }
    return max;
}

const RPCchamber* WiredOR::connected_rpc(int ord) const {
    RPClink::const_iterator rpcs = m_RPCread.begin();
    while (rpcs != m_RPCread.end()) {
        if (!ord) return (*rpcs).second;
        --ord;
        ++rpcs;
    }
    return nullptr;
}

void WiredOR::add_cma(const CMAparameters* cma) { m_readoutCMAs.push_back(cma); }

void WiredOR::add_even_read_mul(ReadoutCh& mul) {
    for (int i = 0; i < give_max_phi_strips(); ++i) m_even_read_mul[i] += mul[i];
}

void WiredOR::add_odd_read_mul(ReadoutCh& mul) {
    for (int i = 0; i < give_max_phi_strips(); ++i) m_odd_read_mul[i] += mul[i];
}

bool WiredOR::setup(SectorLogicSetup& setup) {
    WiredOR* prev = setup.previousWOR(*this);
    if (prev && !(start() == prev->stop() + 1)) {
        REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR") << two_obj_error_message("boundary inconsistence", prev);
        return false;
    }

    if (!connect(setup)) return false;

    int ch = (m_params.side == Eta) ? give_max_eta_strips() : give_max_phi_strips();
    m_even_read_mul.resize(ch);
    m_odd_read_mul.resize(ch);
    for (int i = 0; i < ch; ++i) { m_even_read_mul.push_back(0); }
    for (int i = 0; i < ch; ++i) { m_odd_read_mul.push_back(0); }
    return true;
}

bool WiredOR::check() {
    WiredOR::CMAlist::const_iterator cma = m_readoutCMAs.begin();
    const CMAinput IO = (*cma)->whichCMAinput(station());

    int ch = (m_params.side == ViewType::Eta) ? give_max_eta_strips() : give_max_phi_strips();
    for (int i = 0; i < ch; ++i) {
        if (!m_even_read_mul[i]) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR")
                << error("==> No readout coverage for the full set of even PHI strip!");
            return false;
        }
        if (!m_odd_read_mul[i]) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR")
                << error("==> No readout coverage for the full set of odd PHI strip!");
            return false;
        }
        if (m_even_read_mul[i] > 1 && IO == Pivot) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR")
                << error("==> Pivot plane even PHI strips must be read only once!");
            return false;
        }
        if (m_odd_read_mul[i] > 1 && IO == Pivot) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR")
                << error("==> Pivot plane odd PHI strips must be read only once!");
            return false;
        }
        if (m_even_read_mul[i] > 2) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR") <<
                error("==> Confirm plane even PHI strips can be read only 3 times!");
            return false;
        }
        if (m_odd_read_mul[i] > 1 && IO == Pivot) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "WiredOR") <<
                error("==> Confirm plane odd PHI strips can be read only 3 times");
            return false;
        }
    }

    return true;
}

void WiredOR::Print(std::ostream& stream, bool detail) const {
    stream << " wired OR n. " << std::setw(2) << number();
    stream << "  (stat " << station() << ")";
    stream << "   connects RPC chamber n. " << std::setw(2) << start();
    stream << " to RPC chamber n. " << std::setw(2) << stop() << std::endl;

    if (detail) {
        stream << " It reads " << RPCacquired() << " RPC phi pannel:" << std::endl;
        RPClink::const_iterator rpc = m_RPCread.begin();
        while (rpc != m_RPCread.end()) {
            stream << *(*rpc).second;
            ++rpc;
        }

        stream << " It gives input to " << m_readoutCMAs.size() << " Phi CMA:" << std::endl;
        CMAlist::const_iterator cma = m_readoutCMAs.begin();
        while (cma != m_readoutCMAs.end()) {
            stream << *(*cma);
            ++cma;
        }
        int ch = give_max_phi_strips();
        stream << " Phi even sector readout multiplicity:" << std::endl;
        stream << "1   5    10   15   20   25   30   35   40   "
               << "45   50   55   60   65   70   75" << std::endl;
        stream << "|   |    |    |    |    |    |    |    |    "
               << "|    |    |    |    |    |    |" << std::endl;
        for (int i = 0; i < ch; ++i) stream << m_even_read_mul[i];
        stream << std::endl;
        stream << " Phi odd sector readout multiplicity:" << std::endl;
        stream << "1   5    10   15   20   25   30   35   40   "
               << "45   50   55   60   65   70   75" << std::endl;
        stream << "|   |    |    |    |    |    |    |    |    "
               << "|    |    |    |    |    |    |" << std::endl;
        for (int i = 0; i < ch; ++i) stream << m_odd_read_mul[i];
        stream << std::endl;
        stream << "========================================"
               << "=======================================" << std::endl;
    }
}

std::string WiredOR::two_obj_error_message(const std::string& msg, WiredOR* wor) {
    std::ostringstream disp;
    disp << error_header()
         << "  " << msg << " between " << name() << " n. " << number() << " and " << wor->name() << " n. " << wor->number() << std::endl
         << *this << *wor;
    return disp.str();
}

std::string WiredOR::error(const std::string& msg) {
    std::ostringstream disp;
    disp << error_header() << msg << std::endl << *this;
    return disp.str();
}

ViewType WiredOR::side() const { return m_params.side; }
int WiredOR::start() const { return m_params.start; }
int WiredOR::stop() const { return m_params.stop; }
const WiredOR::ReadoutCh& WiredOR::even_read_mul() const { return m_even_read_mul; }
const WiredOR::ReadoutCh& WiredOR::odd_read_mul() const { return m_odd_read_mul; }
const WiredOR::RPClink& WiredOR::RPCread() const { return m_RPCread; }
const WiredOR::CMAlist& WiredOR::readoutCMAs() const { return m_readoutCMAs; }
