/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CMAcablingdata.h"

#include "AthenaKernel/errorcheck.h"

using namespace RPC_CondCabling;

CMAcablingdata::CMAcablingdata(DBline& data, int type) : BaseObject(Logic, "CMA Cabling Data") {
    (++data)("{");
    do {
        CMAparameters::parseParams parser;
        parser.sectorType = type;
        if (get_data(data, parser)) {
            if (m_view == ViewType::Eta) { m_etaCMA.emplace_back(parser); }
        }
        data++;
    } while (!data("}"));
}

void CMAcablingdata::reset_data() { m_fail = true; }

bool CMAcablingdata::confirm_data(ViewType side, const CMAparameters::parseParams& parser) {
    unsigned int div = 247;
    std::string view = (side == ViewType::Phi) ? "phi" : "eta";

    std::ostringstream disp;

    bool have_low_Pt_input = (parser.lowPtNumCo != -1);
    bool have_high_Pt_input = (parser.highPtNumCo != -1);

    if (side == Eta) {
        if (parser.lowPtStartCo >= parser.lowPtStopCo && have_low_Pt_input) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "CMAcablingdata")
                 << "CMA cabling error in CONF data for Sector Type " << parser.sectorType << ", " << view
                 << " CMA at eta = " << parser.etaIndex << ",  phi = " << parser.phiIndex << std::endl
                 << " start position for low Pt cabling (" << parser.lowPtStartCo << ") is greater than stop position ("
                 << parser.lowPtStopCo;
            return false;
        }
        if (parser.lowPtStopCo - parser.lowPtStartCo + 1 != parser.lowPtNumCo && have_low_Pt_input) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "CMAcablingdata")
                 << "CMA cabling error in CONF data for Sector Type " << parser.sectorType << ", " << view
                 << " CMA at eta = " << parser.etaIndex << ",  phi = " << parser.phiIndex << std::endl
                 << " cabling inconsistence for low Pt <" << parser.lowPtNumCo << ", " << parser.lowPtStartCo << " " << (char)div << " "
                 << parser.lowPtStopCo << ">";
            return false;
        }

        if (parser.highPtStartCo >= parser.highPtStopCo && have_high_Pt_input) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "CMAcablingdata")
                 << "CMA cabling error in CONF data for Sector Type " << parser.sectorType << ", " << view
                 << " CMA at eta = " << parser.etaIndex << ",  phi = " << parser.phiIndex << std::endl
                 << " start position for high Pt cabling (" << parser.highPtStartCo << ") is greater than stop position ("
                 << parser.highPtStopCo;
            return false;
        }
        if (parser.highPtStopCo - parser.highPtStartCo + 1 != parser.highPtNumCo && have_high_Pt_input) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "CMAcablingdata")
                 << "CMA cabling error in CONF data for Sector Type " << parser.sectorType << ", " << view
                 << " CMA at eta = " << parser.etaIndex << ",  phi = " << parser.phiIndex << std::endl
                 << " cabling inconsistence for high Pt <" << parser.highPtNumCo << ", " << parser.highPtStartCo << " " << (char)div << " "
                 << parser.highPtStopCo << ">";
            return false;
        }
    }

    return true;
}

bool CMAcablingdata::get_data(DBline& data, CMAparameters::parseParams& parser) {
    reset_data();

    if (data("eta matrix") >> "eta" >> parser.etaIndex >> "phi" >> parser.phiIndex >> ":" >> "low Pt" >> parser.lowPtNumCo >> "," >>
        parser.lowPtStartCo >> "-" >> parser.lowPtStopCo >> "high Pt" >> parser.highPtNumCo >> "," >> parser.highPtStartCo >> "-" >>
        parser.highPtStopCo) {
        if (parser.lowPtNumCo == 0) {
            parser.lowPtNumCo = -1;
            parser.lowPtStartCo = -1;
            parser.lowPtStopCo = -1;
        }
        if (parser.highPtNumCo == 0) {
            parser.highPtNumCo = -1;
            parser.highPtStartCo = -1;
            parser.highPtStopCo = -1;
        }
        m_view = parser.view = ViewType::Eta;
        m_fail = false;
        if (!confirm_data(Eta, parser)) m_fail = true;
    }

    return !m_fail;
}

std::unique_ptr<EtaCMA> CMAcablingdata::give_eta_cma() {
    if (!m_etaCMA.empty()) {
        std::unique_ptr<EtaCMA> CMA = std::make_unique<EtaCMA>(m_etaCMA.front());
        m_etaCMA.pop_front();
        return CMA;
    }
    return nullptr;
}

void CMAcablingdata::Print(std::ostream& stream, bool detail) const {
    stream << "CMA cabling data ";

    stream << "It contains " << m_etaCMA.size();
    stream << " eta CMAs:" << std::endl;
    ETAlist::const_iterator ei;
    for (ei = m_etaCMA.begin(); ei != m_etaCMA.end(); ++ei) stream << ShowRequest<EtaCMA>(*ei, detail);
}
