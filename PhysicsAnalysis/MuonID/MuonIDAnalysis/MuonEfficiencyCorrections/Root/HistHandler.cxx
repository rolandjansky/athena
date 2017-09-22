/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/HistHandler.h"
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstring>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>

#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

namespace CP {

    //###########################################################################################################
    //                                                   AxisHandlerProvider
    //###########################################################################################################
    AxisHandler * AxisHandlerProvider::GetAxisHandler(const TAxis * axisptr) {
        if (axisptr != nullptr) {
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
            Error("AxisHandlerProvider", "nullptr pointer passed");
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
        if (m_H.get() != nullptr) m_H->SetDirectory(nullptr);
    }

    void HistHandler::Copy(const HistHandler & other) {
        if (this == &other) {
            return;
        }
        if (other.m_H) {
            m_H = std::unique_ptr < TH1 > (dynamic_cast<TH1*>(other.m_H->Clone(Form("CloneOf_%s", m_H->GetName()))));
        }
    }
    HistHandler::HistHandler(const HistHandler & other) :
                m_H() {
        Copy(other);
    }
    HistHandler::~HistHandler() {
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
        return m_H.get();
    }

    //###########################################################################################################
    //                                                   HistHandler_TH1
    //###########################################################################################################
    HistHandler_TH1::HistHandler_TH1(TH1* h) :
                HistHandler(h),
                m_x_handler(h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())) {
    }

    HistHandler_TH1::HistHandler_TH1(const HistHandler_TH1 & other) :
                HistHandler(other),
                m_x_handler(other.GetHist() == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetXaxis())) {

    }
    HistHandler_TH1 & HistHandler_TH1::operator =(const HistHandler_TH1 & other) {
        if (this == &other) {
            return *this;
        }
        Copy(other);
        m_x_handler = AxisHandler_Ptr(other.GetHist() == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetXaxis()));
        return *this;
    }
    HistHandler_TH1::~HistHandler_TH1() {

    }
    int HistHandler_TH1::NBins() const {
        return GetHist()->GetNbinsX() + 2;
    }
    std::string HistHandler_TH1::GetBinName(unsigned int bin) const {
        TAxis* xAx = GetHist()->GetXaxis();
        return Form("%s_%.2f-%.2f", xAx->GetTitle(), xAx->GetBinLowEdge(bin), xAx->GetBinUpEdge(bin));
    }

    CorrectionCode HistHandler_TH1::FindBin(const xAOD::Muon & muon, int & bin) const {
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
    //                                                   HistHandler_TH2
    //###########################################################################################################
    HistHandler_TH2::HistHandler_TH2(TH2 * h) :
                HistHandler(h),
                m_h(h),
                m_x_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                m_y_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())) {

    }
    int HistHandler_TH2::NBins() const {
        return (GetHist()->GetNbinsX() + 2) * (GetHist()->GetNbinsY() + 2);
    }
    CorrectionCode HistHandler_TH2::FindBin(const xAOD::Muon & muon, int & bin) const {
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
    HistHandler_TH2 & HistHandler_TH2::operator =(const HistHandler_TH2 & other) {
        if (this == &other) {
            return *this;
        }
        m_x_handler = AxisHandler_Ptr(other.GetHist() == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetXaxis()));
        m_y_handler = AxisHandler_Ptr(other.GetHist() == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.GetHist()->GetYaxis()));
        return *this;
    }
    HistHandler_TH2::~HistHandler_TH2() {
    }
    HistHandler_TH2::HistHandler_TH2(const HistHandler_TH2 & other) :
                HistHandler(other),
                m_h(other.m_h),
                m_x_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                m_y_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())) {
    }
    std::string HistHandler_TH2::GetBinName(unsigned int bin) const {
        int x(0), y(0), z(0);
        GetHist()->GetBinXYZ(bin, x, y, z);
        TAxis* xAx = GetHist()->GetXaxis();
        TAxis* yAx = GetHist()->GetYaxis();
        return Form("%s_%.2f-%.2f--%s_%.2f-%.2f",
        //xAxis
                xAx->GetTitle(), xAx->GetBinLowEdge(x), xAx->GetBinUpEdge(x),
                //yAxis
                yAx->GetTitle(), yAx->GetBinLowEdge(y), yAx->GetBinUpEdge(y)

                );
    }

    //###########################################################################################################
    //                                                   HistHandler_TH3
    //###########################################################################################################

    HistHandler_TH3::HistHandler_TH3(TH3 * h) :
                HistHandler(h),
                m_h(h),
                m_x_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                m_y_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())),
                m_z_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetZaxis())) {

    }
    HistHandler_TH3::HistHandler_TH3(const HistHandler_TH3 & other) :
                HistHandler(other),
                m_h(other.m_h),
                m_x_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                m_y_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())),
                m_z_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetZaxis())) {

    }

    HistHandler_TH3::~HistHandler_TH3() {
    }
    HistHandler_TH3 & HistHandler_TH3::operator =(const HistHandler_TH3 & other) {
        if (this == &other) {
            return *this;
        }
        Copy(other);
        m_x_handler = AxisHandler_Ptr(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = AxisHandler_Ptr(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        m_z_handler = AxisHandler_Ptr(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetZaxis()));
        return *this;
    }
    int HistHandler_TH3::NBins() const {
        return (GetHist()->GetNbinsX() + 2) * (GetHist()->GetNbinsY() + 2) * (GetHist()->GetNbinsZ() + 2);
    }

    CorrectionCode HistHandler_TH3::FindBin(const xAOD::Muon & muon, int & bin) const {
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

    std::string HistHandler_TH3::GetBinName(unsigned int bin) const {
        int x(0), y(0), z(0);
        GetHist()->GetBinXYZ(bin, x, y, z);
        TAxis* xAx = GetHist()->GetXaxis();
        TAxis* yAx = GetHist()->GetYaxis();
        TAxis* zAx = GetHist()->GetZaxis();
        return Form("%s_%.2f-%.2f--%s_%.2f-%.2f--%s_%.2f-%.2f",
        //xAxis
                xAx->GetTitle(), xAx->GetBinLowEdge(x), xAx->GetBinUpEdge(x),
                //yAxis
                yAx->GetTitle(), yAx->GetBinLowEdge(y), yAx->GetBinUpEdge(y),
                //zAxis
                zAx->GetTitle(), zAx->GetBinLowEdge(z), zAx->GetBinUpEdge(z));
    }

    //###########################################################################################################
    //                                                   HistHandler_TH2Poly
    //###########################################################################################################

    HistHandler_TH2Poly::HistHandler_TH2Poly(TH2Poly * h) :
                HistHandler(h),
                m_h(h),
                m_x_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetXaxis())),
                m_y_handler(m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(h->GetYaxis())) {

    }

    HistHandler_TH2Poly::HistHandler_TH2Poly(const HistHandler_TH2Poly & other) :
                HistHandler(other),
                m_h(other.m_h),
                m_x_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis())),
                m_y_handler(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis())) {

    }

    HistHandler_TH2Poly & HistHandler_TH2Poly::operator =(const HistHandler_TH2Poly & other) {
        if (this == &other) {
            return *this;
        }
        Copy(other);
        m_x_handler = AxisHandler_Ptr(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetXaxis()));
        m_y_handler = AxisHandler_Ptr(other.m_h == nullptr ? 0 : AxisHandlerProvider::GetAxisHandler(other.m_h->GetYaxis()));
        return *this;
    }

    HistHandler_TH2Poly::~HistHandler_TH2Poly() {
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
    std::string HistHandler_TH2Poly::GetBinName(unsigned int bin) const {
        int x(0), y(0), z(0);
        GetHist()->GetBinXYZ(bin, x, y, z);
        TAxis* xAx = GetHist()->GetXaxis();
        TAxis* yAx = GetHist()->GetYaxis();
        return Form("%s_%.2f-%.2f--%s_%.2f-%.2f",
        //xAxis
                xAx->GetTitle(), xAx->GetBinLowEdge(x), xAx->GetBinUpEdge(x),
                //yAxis
                yAx->GetTitle(), yAx->GetBinLowEdge(y), yAx->GetBinUpEdge(y));
    }
} // namespace CP
