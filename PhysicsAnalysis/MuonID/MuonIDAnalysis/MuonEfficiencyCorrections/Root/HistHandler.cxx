/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/HistHandler.h"
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace CP {

    //###########################################################################################################
    //                                                   AxisHandlerProvider
    //###########################################################################################################
    AxisHandler * AxisHandlerProvider::GetAxisHandler(const TAxis * axisptr) {
        if (axisptr != NULL) {
            std::string axis = axisptr->GetTitle();
            axis = AxisHandlerProvider::EraseWhiteSpaces(axis);
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
    std::string AxisHandlerProvider::EraseWhiteSpaces(std::string str) {
        str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
        if (str.find(" ") == 0) return EraseWhiteSpaces(str.substr(1, str.size()));
        if (str.size() > 0 && str.find(" ") == str.size() - 1) return EraseWhiteSpaces(str.substr(0, str.size() - 1));
        return str;
    }

    //###########################################################################################################
    //                                                   HistHandler
    //###########################################################################################################
    HistHandler::HistHandler(TH1* Hist) :
                    m_H(Hist) {
        if (m_H) m_H->SetDirectory(0);
    }

    void HistHandler::Copy(const HistHandler & other) {
        if (this == &other) {
            return;
        }
        if (m_H) {
            delete m_H;
        }
        if (other.m_H) {
            m_H = dynamic_cast<TH1*>(other.m_H->Clone(Form("CloneOf_%s", m_H->GetName())));
        }
    }
    HistHandler::HistHandler(const HistHandler & other) :
                    m_H(NULL) {
        Copy(other);
    }
    HistHandler::~HistHandler() {
        if (m_H) {
            delete m_H;
        }
    }
    double HistHandler::GetBinContent(int bin) const {
        if (!m_H) {
            return DBL_MAX;
        }
        return m_H->GetBinContent(bin);
    }
    void HistHandler::SetBinContent(int bin, float val) const {
        if (m_H) {
            m_H->SetBinContent(bin, val);
        }
    }
    double HistHandler::GetBinError(int bin) const {
        if (!m_H) {
            return DBL_MAX;
        }
        return m_H->GetBinError(bin);
    }
    void HistHandler::SetBinError(int bin, float val) const {
        if (m_H) {
            m_H->SetBinError(bin, val);
        }
    }

    TH1* HistHandler::GetHist() const {
        return m_H;
    }

    //###########################################################################################################
    //                                                   HistHandler_TH1F
    //###########################################################################################################
    HistHandler_TH1F::HistHandler_TH1F(TH1F* h) :
                    HistHandler(h),
                    m_x_handler(h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())) {
    }

    HistHandler_TH1F::HistHandler_TH1F(const HistHandler_TH1F & other) :
                    HistHandler(other),
                    m_x_handler(other.GetHist() == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetXaxis())) {

    }
    HistHandler_TH1F & HistHandler_TH1F::operator =(const HistHandler_TH1F & other) {
        if (this == &other) {
            return *this;
        }
        Copy(other);
        if (m_x_handler) {
            delete m_x_handler;
        }
        m_x_handler = (other.GetHist() == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetXaxis()));
        return *this;
    }
    HistHandler_TH1F::~HistHandler_TH1F() {
        if (m_x_handler) {
            delete m_x_handler;
        }
    }

    int HistHandler_TH1F::NBins() const {
        return GetHist()->GetNbinsX() + 2;
    }
    CorrectionCode HistHandler_TH1F::FindBin(const xAOD::Muon & muon, int & bin) const {
        if (!GetHist()) {
            bin = -1;
            return CorrectionCode::Error;
        }
        float par = 0;
        CorrectionCode found = m_x_handler->GetBinningParameter(muon, par);
        if (found == CorrectionCode::Error) {
            return found;
        } else {
            bin = GetHist()->FindBin(par);
            if (bin < 1 || bin > GetHist()->GetNbinsX()) {
                return CorrectionCode::OutOfValidityRange;
            }
        }
        return CorrectionCode::Ok;
    }

    //###########################################################################################################
    //                                                   HistHandler_TH2F
    //###########################################################################################################
    HistHandler_TH2F::HistHandler_TH2F(TH2F * h) :
                    HistHandler(h),
                    m_h(h),
                    m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                    m_y_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())) {

    }
    int HistHandler_TH2F::NBins() const {
        return (GetHist()->GetNbinsX() + 2) * (GetHist()->GetNbinsY() + 2);
    }
    CorrectionCode HistHandler_TH2F::FindBin(const xAOD::Muon & muon, int & bin) const {
        if (!GetHist()) return CorrectionCode::Error;
        float parx = 0;
        float pary = 0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon, parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon, pary);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error) {
            return CorrectionCode::Error;
        } else {
            int binx = m_h->GetXaxis()->FindBin(parx);
            int biny = m_h->GetYaxis()->FindBin(pary);
            if (binx < 1 || binx > m_h->GetNbinsX() || biny < 1 || biny > m_h->GetNbinsY()) {
                return CorrectionCode::OutOfValidityRange;
            }
            bin = m_h->GetBin(binx, biny);
        }
        return CorrectionCode::Ok;
    }
    HistHandler_TH2F & HistHandler_TH2F::operator =(const HistHandler_TH2F & other) {
        if (this == &other) {
            return *this;
        }

        if (m_x_handler) {
            delete m_x_handler;
        }
        if (m_y_handler) {
            delete m_y_handler;
        }
        m_x_handler = (other.GetHist() == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetXaxis()));
        m_y_handler = (other.GetHist() == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetYaxis()));
        return *this;
    }
    HistHandler_TH2F::~HistHandler_TH2F() {
        if (m_x_handler) delete m_x_handler;
        if (m_y_handler) delete m_y_handler;
    }
    HistHandler_TH2F::HistHandler_TH2F(const HistHandler_TH2F & other) :
                    HistHandler(other),
                    m_h(other.m_h),
                    m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                    m_y_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())) {
    }

    //###########################################################################################################
    //                                                   HistHandler_TH3F
    //###########################################################################################################

    HistHandler_TH3F::HistHandler_TH3F(TH3F * h) :
                    HistHandler(h),
                    m_h(h),
                    m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                    m_y_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())),
                    m_z_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetZaxis())) {

    }
    HistHandler_TH3F::HistHandler_TH3F(const HistHandler_TH3F & other) :
                    HistHandler(other),
                    m_h(other.m_h),
                    m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                    m_y_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())),
                    m_z_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetZaxis())) {

    }

    HistHandler_TH3F::~HistHandler_TH3F() {
        if (m_x_handler) {
            delete m_x_handler;
        }
        if (m_y_handler) {
            delete m_y_handler;
        }
        if (m_z_handler) {
            delete m_z_handler;
        }
    }
    HistHandler_TH3F & HistHandler_TH3F::operator =(const HistHandler_TH3F & other) {
        if (this == &other) {
            return *this;
        }
        Copy(other);
        if (m_x_handler) {
            delete m_x_handler;
        }
        if (m_y_handler) {
            delete m_y_handler;
        }
        if (m_z_handler) {
            delete m_z_handler;
        }
        m_x_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        m_z_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetZaxis()));
        return *this;
    }
    int HistHandler_TH3F::NBins() const {
        return (GetHist()->GetNbinsX() + 2) * (GetHist()->GetNbinsY() + 2) * (GetHist()->GetNbinsZ() + 2);
    }

    CorrectionCode HistHandler_TH3F::FindBin(const xAOD::Muon & muon, int & bin) const {
        if (!GetHist()) return CorrectionCode::Error;
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

    //###########################################################################################################
    //                                                   HistHandler_TH2Poly
    //###########################################################################################################

    HistHandler_TH2Poly::HistHandler_TH2Poly(TH2Poly * h) :
                    HistHandler(h),
                    m_h(h),
                    m_x_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                    m_y_handler(m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())) {

    }

    HistHandler_TH2Poly::HistHandler_TH2Poly(const HistHandler_TH2Poly & other) :
                    HistHandler(other),
                    m_h(other.m_h),
                    m_x_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                    m_y_handler(other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())) {

    }

    HistHandler_TH2Poly & HistHandler_TH2Poly::operator =(const HistHandler_TH2Poly & other) {
        if (this == &other) {
            return *this;
        }
        Copy(other);
        if (m_x_handler) {
            delete m_x_handler;
        }
        if (m_y_handler) {
            delete m_y_handler;
        }
        m_x_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = (other.m_h == NULL ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        return *this;
    }

    HistHandler_TH2Poly::~HistHandler_TH2Poly() {
        if (m_x_handler) {
            delete m_x_handler;
        }
        if (m_y_handler) {
            delete m_y_handler;
        }

    }
    int HistHandler_TH2Poly::NBins() const {
        return m_h->GetNumberOfBins() + 1;
    }
    CorrectionCode HistHandler_TH2Poly::FindBin(const xAOD::Muon & muon, int & bin) const {
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

} // namespace CP
