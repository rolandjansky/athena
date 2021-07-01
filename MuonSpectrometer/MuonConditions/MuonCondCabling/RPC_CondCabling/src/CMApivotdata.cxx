/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CMApivotdata.h"

using namespace RPC_CondCabling;

CMApivotdata::CMApivotdata(DBline& data, int type, const std::string& layout, IMessageSvc* msgSvc) : BaseObject(Logic, "CMA Data", msgSvc) {
    m_layout = layout;
    (++data)("{");
    do {
        CMAparameters::parseParams parser;
        parser.sectorType = type;
        parser.station = 0;
        if (get_data(data, parser)) {
            if (m_view == ViewType::Eta) {
                m_etaCMA.emplace_back(parser, msgSvc);
            } else if (m_view == ViewType::Phi) {
                if (parser.coverage == EvenSectors) {
                    m_evenphiCMA.emplace_back(parser, msgSvc);
                } else if (parser.coverage == OddSectors) {
                    m_oddphiCMA.emplace_back(parser, msgSvc);
                }
            }
        }
        data++;
    } while (!data("}"));
}
void CMApivotdata::reset_data() {
    m_view = ViewType::NoView;
    m_covtag.clear();
    m_fail = true;
}

bool CMApivotdata::confirm_data(ViewType side, CMAparameters::parseParams& parser) {
    int start = parser.pivotStartChan * 100 + parser.pivotStartStation;
    int stop = parser.pivotStopChan * 100 + parser.pivotStopStation;
    std::string view = (side == Phi) ? "phi" : "eta";

    __osstream disp;

    if (!CMAidentity::coverage(m_covtag, parser.coverage)) {
        disp << "CMA cabling error into configuration for Sector Type " << parser.sectorType << ", " << m_covtag << " " << view
             << " CMA number " << parser.number << std::endl
             << " coverage tag ==> " << m_covtag << " <== is not recognized!" << std::endl;
        display_error(disp);
        return false;
    }

    if ((side == ViewType::Eta) & start) {
        if (start >= stop) {
            disp << "CMA cabling error into configuration for Sector Type " << parser.sectorType << ", " << view << " CMA number "
                 << parser.number << std::endl
                 << " start position (" << parser.pivotStartChan << ":" << parser.pivotStartStation << ") is greater than stop position ("
                 << parser.pivotStopChan << ":" << parser.pivotStopStation << ")" << std::endl;
            display_error(disp);
            return false;
        }
    }
    if (side == ViewType::Phi && start) {
        if (start >= stop && parser.coverage == EvenSectors) {
            disp << "CMA cabling error into configuration for Sector Type " << parser.sectorType << ", " << m_covtag << " " << view
                 << " CMA number " << parser.number << std::endl
                 << " start position (" << parser.pivotStartChan << ":" << parser.pivotStartStation << ") is greater than stop position ("
                 << parser.pivotStopChan << ":" << parser.pivotStopStation << ")" << std::endl;
            display_error(disp);
            return false;
        }
        if (start <= stop && parser.coverage == OddSectors) {
            disp << "CMA cabling error into configuration for Sector Type " << parser.sectorType << ", " << m_covtag << " " << view
                 << " CMA number " << parser.number << std::endl
                 << " start position (" << parser.pivotStartChan << ":" << parser.pivotStartStation << ") is lower than stop position ("
                 << parser.pivotStopChan << ":" << parser.pivotStopStation << ")" << std::endl;
            display_error(disp);
            return false;
        }
        if (parser.pivotStartChan != parser.pivotStopChan) {
            disp << "CMA cabling error into configuration for Sector Type " << parser.sectorType << ", " << m_covtag << " " << view
                 << " CMA number " << parser.number << std::endl
                 << " phi CMA Pivot connected to more than 1 wired or" << std::endl;
            display_error(disp);
            return false;
        }
    }
    return true;
}

bool CMApivotdata::get_data(DBline& data, CMAparameters::parseParams& parser) {
    reset_data();

    if (data("eta matrix") >> parser.number >> ":" >> "eta" >> parser.etaIndex >> "phi" >> parser.phiIndex >> "PAD" >> parser.padIndex >>
        "Idx" >> parser.IxxIndex >> parser.pivotStation >> parser.lowPtStation >> parser.highPtStation >> parser.pivotStartChan >> ":" >>
        parser.pivotStartStation >> "-" >> parser.pivotStopChan >> ":" >> parser.pivotStopStation) {
        m_view = parser.view = ViewType::Eta;
        m_fail = false;
        if (!confirm_data(ViewType::Eta, parser)) m_fail = true;
    }

    if (data("phi matrix") >> m_covtag >> parser.number >> ":" >> "eta" >> parser.etaIndex >> "phi" >> parser.phiIndex >> "PAD" >>
        parser.padIndex >> "Idx" >> parser.IxxIndex >> parser.pivotStation >> parser.lowPtStation >> parser.highPtStation >>
        parser.pivotStartChan >> ":" >> parser.pivotStartStation >> "-" >> parser.pivotStopChan >> ":" >> parser.pivotStopStation) {
        m_view = parser.view = ViewType::Phi;
        m_fail = false;
        if (!confirm_data(ViewType::Phi, parser)) m_fail = true;
    }

    return !m_fail;
}

std::unique_ptr<EtaCMA> CMApivotdata::give_eta_cma() {
    if (m_etaCMA.size()) {
        std::unique_ptr<EtaCMA> CMA = std::make_unique<EtaCMA>(m_etaCMA.front());
        m_etaCMA.pop_front();
        return CMA;
    }
    return nullptr;
}

std::unique_ptr<EvenPhiCMA> CMApivotdata::give_evenphi_cma() {
    if (m_evenphiCMA.size()) {
        std::unique_ptr<EvenPhiCMA> CMA = std::make_unique<EvenPhiCMA>(m_evenphiCMA.front());
        m_evenphiCMA.pop_front();
        return CMA;
    }
    return nullptr;
}

std::unique_ptr<OddPhiCMA> CMApivotdata::give_oddphi_cma() {
    if (m_oddphiCMA.size()) {
        std::unique_ptr<OddPhiCMA> CMA = std::make_unique<OddPhiCMA>(m_oddphiCMA.front());
        m_oddphiCMA.pop_front();
        return CMA;
    }
    return nullptr;
}

void CMApivotdata::Print(std::ostream& stream, bool detail) const {
    stream << "CMA pivot segmentation";

    stream << "It contains " << m_etaCMA.size();
    stream << " eta CMAs:" << std::endl;
    ETAlist::const_iterator ei;
    for (ei = m_etaCMA.begin(); ei != m_etaCMA.end(); ++ei) stream << ShowRequest<EtaCMA>(*ei, detail);

    stream << "It contains " << m_evenphiCMA.size();
    stream << "even phi CMAs:" << std::endl;
    EvenPHIlist::const_iterator ev;
    for (ev = m_evenphiCMA.begin(); ev != m_evenphiCMA.end(); ++ev) stream << ShowRequest<EvenPhiCMA>(*ev, detail);

    stream << "It contains " << m_oddphiCMA.size();
    stream << "odd phi CMAs:" << std::endl;
    OddPHIlist::const_iterator od;
    for (od = m_oddphiCMA.begin(); od != m_oddphiCMA.end(); ++od) stream << ShowRequest<OddPhiCMA>(*od, detail);
}
