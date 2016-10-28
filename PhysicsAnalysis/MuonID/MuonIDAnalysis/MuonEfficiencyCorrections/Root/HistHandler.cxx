/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEfficiencyCorrections/HistHandler.h"

namespace CP {

    HistHandler_TH1F::HistHandler_TH1F(TH1F* h) :
                m_h(h),
                m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }
    HistHandler_TH2F::HistHandler_TH2F(TH2F* h) :
                m_h(h),
                m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                m_y_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }
    HistHandler_TH3F::HistHandler_TH3F(TH3F* h) :
                m_h(h),
                m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                m_y_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())),
                m_z_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetZaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }
    HistHandler_TH2Poly::HistHandler_TH2Poly(TH2Poly* h) :
                m_h(h),
                m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                m_y_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }

    HistHandler_TH1F::HistHandler_TH1F(const HistHandler_TH1F & other) :
                m_h(other.m_h == NULL ? 0 : dynamic_cast<TH1F*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str()))),
                m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }
    HistHandler_TH2F::HistHandler_TH2F(const HistHandler_TH2F & other) :
                m_h(other.m_h == NULL ? 0 : dynamic_cast<TH2F*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str()))),
                m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                m_y_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }
    HistHandler_TH3F::HistHandler_TH3F(const HistHandler_TH3F & other) :
                m_h(other.m_h == NULL ? 0 : dynamic_cast<TH3F*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str()))),
                m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                m_y_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())),
                m_z_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetZaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }
    HistHandler_TH2Poly::HistHandler_TH2Poly(const HistHandler_TH2Poly & other) :
                m_h(other.m_h == NULL ? 0 : dynamic_cast<TH2Poly*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str()))),
                m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                m_y_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())) {
        if (m_h) m_h->SetDirectory(0);
    }

    HistHandler_TH1F & HistHandler_TH1F::operator =(const HistHandler_TH1F & other) {
        if (this == &other) {
            return *this;
        }
        delete m_h;
        m_h = (other.m_h == NULL ? 0 : dynamic_cast<TH1F*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str())));
        if (m_h) m_h->SetDirectory(0);

        delete m_x_handler;
        m_x_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        return *this;
    }
    HistHandler_TH2F & HistHandler_TH2F::operator =(const HistHandler_TH2F & other) {
        if (this == &other) {
            return *this;
        }
        delete m_h;
        m_h = (other.m_h == NULL ? 0 : dynamic_cast<TH2F*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str())));
        if (m_h) m_h->SetDirectory(0);

        delete m_x_handler;
        delete m_y_handler;
        m_x_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        return *this;
    }
    HistHandler_TH3F & HistHandler_TH3F::operator =(const HistHandler_TH3F & other) {
        if (this == &other) {
            return *this;
        }
        delete m_h;
        m_h = (other.m_h == NULL ? 0 : dynamic_cast<TH3F*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str())));
        if (m_h) m_h->SetDirectory(0);

        delete m_x_handler;
        delete m_y_handler;
        delete m_z_handler;
        m_x_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        m_z_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetZaxis()));
        return *this;
    }
    HistHandler_TH2Poly & HistHandler_TH2Poly::operator =(const HistHandler_TH2Poly & other) {
        if (this == &other) {
            return *this;
        }
        delete m_h;
        m_h = (other.m_h == NULL ? 0 : dynamic_cast<TH2Poly*>(other.m_h->Clone((std::string("CloneOf") + other.m_h->GetName()).c_str())));
        if (m_h) m_h->SetDirectory(0);

        delete m_x_handler;
        delete m_y_handler;
        m_x_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        return *this;
    }

    HistHandler_TH1F::~HistHandler_TH1F() {
        delete m_h;
        delete m_x_handler;
    }
    HistHandler_TH2F::~HistHandler_TH2F() {
        delete m_h;
        delete m_x_handler;
        delete m_y_handler;
    }
    HistHandler_TH3F::~HistHandler_TH3F() {
        delete m_h;
        delete m_x_handler;
        delete m_y_handler;
        delete m_z_handler;

    }
    HistHandler_TH2Poly::~HistHandler_TH2Poly() {
        delete m_h;
        delete m_x_handler;
        delete m_y_handler;

    }

    int HistHandler_TH1F::NBins() {
        return m_h->GetNbinsX() + 2;
    }
    int HistHandler_TH2F::NBins() {
        return (m_h->GetNbinsX() + 2) * (m_h->GetNbinsY() + 2);
    }
    int HistHandler_TH3F::NBins() {
        return (m_h->GetNbinsX() + 2) * (m_h->GetNbinsY() + 2) * (m_h->GetNbinsZ() + 2);
    }
    int HistHandler_TH2Poly::NBins() {
        return m_h->GetNumberOfBins() + 1;
    }

    CorrectionCode HistHandler_TH1F::FindBin(const xAOD::Muon & muon, int & bin) {
        if (!m_h) return CorrectionCode::Error;
        float par = 0;
        CorrectionCode found = m_x_handler->GetBinningParameter(muon, par);
        if (found == CorrectionCode::Error) return found;
        else {
            bin = m_h->FindBin(par);
            if (bin < 1 || bin > m_h->GetNbinsX()) return CorrectionCode::OutOfValidityRange;
        }
        return CorrectionCode::Ok;
    }

    CorrectionCode HistHandler_TH2F::FindBin(const xAOD::Muon & muon, int & bin) {
        if (!m_h) return CorrectionCode::Error;
        float parx = 0;
        float pary = 0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon, parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon, pary);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error) return CorrectionCode::Error;
        else {
            int binx = m_h->GetXaxis()->FindBin(parx);
            int biny = m_h->GetYaxis()->FindBin(pary);
            if (binx < 1 || binx > m_h->GetNbinsX() || biny < 1 || biny > m_h->GetNbinsY()) {
                return CorrectionCode::OutOfValidityRange;
            }
            bin = m_h->GetBin(binx, biny);
        }
        return CorrectionCode::Ok;
    }

    CorrectionCode HistHandler_TH3F::FindBin(const xAOD::Muon & muon, int & bin) {
        if (!m_h) return CorrectionCode::Error;
        float parx = 0;
        float pary = 0;
        float parz = 0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon, parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon, pary);
        CorrectionCode foundz = m_z_handler->GetBinningParameter(muon, parz);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error || foundz == CorrectionCode::Error) return CorrectionCode::Error;
        else {
            int binx = m_h->GetXaxis()->FindBin(parx);
            int biny = m_h->GetYaxis()->FindBin(pary);
            int binz = m_h->GetZaxis()->FindBin(parz);
            if (binx < 1 || binx > m_h->GetNbinsX() || biny < 1 || biny > m_h->GetNbinsY() || binz < 1 || binz > m_h->GetNbinsZ()) {
                return CorrectionCode::OutOfValidityRange;
            }
            bin = m_h->GetBin(binx, biny, binz);
        }
        return CorrectionCode::Ok;
    }

    CorrectionCode HistHandler_TH2Poly::FindBin(const xAOD::Muon & muon, int & bin) {
        if (!m_h) return CorrectionCode::Error;
        float parx = 0;
        float pary = 0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon, parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon, pary);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error) return CorrectionCode::Error;
        else {
            bin = m_h->FindBin(parx, pary);
            if (bin < 0) {
                return CorrectionCode::OutOfValidityRange;
            }
        }
        return CorrectionCode::Ok;
    }

    std::string AxisHandler::EraseWhiteSpaces(std::string str) {
        str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
        if (str.find(" ") == 0) return EraseWhiteSpaces(str.substr(1, str.size()));
        if (str.size() > 0 && str.find(" ") == str.size() - 1) return EraseWhiteSpaces(str.substr(0, str.size() - 1));
        return str;
    }

    AxisHandler* AxisHandlerProvider::GetAxisHandler(TAxis* axisptr) {
        if (axisptr != NULL) {
            std::string axis = axisptr->GetTitle();
            axis = AxisHandler::EraseWhiteSpaces(axis);
            size_t Abs1 = axis.find("|");
            size_t Abs2(0);
            if (Abs1 != std::string::npos) Abs2 = axis.find("|", Abs1 + 1);
            bool AbsAxis = (Abs2 != std::string::npos) && (Abs2 != 0);
            if (axis.find("pt") != std::string::npos || axis.find("pT") != std::string::npos || axis.find("p_{T}") != std::string::npos) {
                return new PtAxisHandler;
            } else if (axis.find("etaphi") != std::string::npos) {
                return new FineEtaPhiAxisHandler;
            } else if (axis.find("region") != std::string::npos) {
                if (axis.find("signed") != std::string::npos || axis.find("Signed") != std::string::npos) {
                    return new SignedDetRegionAxisHandler;
                } else {
                    return new DetRegionAxisHandler;
                }
            } else if (axis.find("phi") != std::string::npos) {
                return new PhiAxisHandler;
            } else if (axis.find("q") != std::string::npos || axis.find("charge") != std::string::npos) {
                return new ChargeAxisHandler;
            } else if (axis.find("eta") != std::string::npos) {
                if (AbsAxis) return new AbsEtaAxisHandler;
                return new EtaAxisHandler;
            } else if (axis.find("dRJet") != std::string::npos) {
                return new dRJetAxisHandler;
            }

            Error("AxisHandlerProvider", "Can not interpret axis title %s", axis.c_str());
        } else {
            Error("AxisHandlerProvider", "NULL pointer passed");
        }
        return new UndefinedAxisHandler;
    }

} // namespace CP
