/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEfficiencyCorrections/HistHandler.h"

namespace CP{
    
    HistHandler_TH1F::HistHandler_TH1F(TH1F* h): m_h(h),
        m_x_handler(AxisHandlerProvider::GetAxisHandler(h->GetXaxis()->GetTitle())){
    }
    HistHandler_TH2F::HistHandler_TH2F(TH2F* h): m_h(h),
        m_x_handler(AxisHandlerProvider::GetAxisHandler(h->GetXaxis()->GetTitle())),
        m_y_handler(AxisHandlerProvider::GetAxisHandler(h->GetYaxis()->GetTitle())){
    }
    HistHandler_TH3F::HistHandler_TH3F(TH3F* h): m_h(h),
        m_x_handler(AxisHandlerProvider::GetAxisHandler(h->GetXaxis()->GetTitle())),
        m_y_handler(AxisHandlerProvider::GetAxisHandler(h->GetYaxis()->GetTitle())),
        m_z_handler(AxisHandlerProvider::GetAxisHandler(h->GetZaxis()->GetTitle())){
    }
    HistHandler_TH2Poly::HistHandler_TH2Poly(TH2Poly* h): m_h(h),
        m_x_handler(AxisHandlerProvider::GetAxisHandler(h->GetXaxis()->GetTitle())),
        m_y_handler(AxisHandlerProvider::GetAxisHandler(h->GetYaxis()->GetTitle())){
    }
    
    
    HistHandler_TH1F::~HistHandler_TH1F(){
        delete m_h;
        delete m_x_handler;
    }
    HistHandler_TH2F::~HistHandler_TH2F(){
        delete m_h;
        delete m_x_handler;
        delete m_y_handler;
    }
    HistHandler_TH3F::~HistHandler_TH3F(){
        delete m_h;
        delete m_x_handler;
        delete m_y_handler;
        delete m_z_handler;
    }
    HistHandler_TH2Poly::~HistHandler_TH2Poly(){
        delete m_h;
        delete m_x_handler;
        delete m_y_handler;
    }
    
    int HistHandler_TH1F::NBins(){
        return m_h->GetNbinsX()+2;
    }
    int HistHandler_TH2F::NBins(){
        return (m_h->GetNbinsX()+2)*(m_h->GetNbinsY()+2);
    }
    int HistHandler_TH3F::NBins(){
        return (m_h->GetNbinsX()+2)*(m_h->GetNbinsY()+2)*(m_h->GetNbinsZ()+2);
    }
    int HistHandler_TH2Poly::NBins(){
        return m_h->GetNumberOfBins()+1;
    }
    
    
    CorrectionCode HistHandler_TH1F::FindBin (const xAOD::Muon & muon, int & bin){
        float par =  0;
        CorrectionCode found = m_x_handler->GetBinningParameter(muon,par);
        if (found == CorrectionCode::Error) return found;
        else {
            bin = m_h->FindBin(par);
            if (bin < 1 || bin > m_h->GetNbinsX()) return CorrectionCode::OutOfValidityRange;
        }
        return CorrectionCode::Ok;
    }
    
    CorrectionCode HistHandler_TH2F::FindBin (const xAOD::Muon & muon, int & bin){
        float parx =  0;
        float pary =  0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon,parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon,pary);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error) 
            return CorrectionCode::Error;
        else {
            int binx = m_h->GetXaxis()->FindBin(parx);
            int biny = m_h->GetYaxis()->FindBin(pary);
            if (binx < 1 || binx > m_h->GetNbinsX()
               || biny < 1 || biny > m_h->GetNbinsY()
            ) {
                return CorrectionCode::OutOfValidityRange;
            }
            bin = m_h->GetBin(binx,biny);
        }
        return CorrectionCode::Ok;
    }
    
    CorrectionCode HistHandler_TH3F::FindBin (const xAOD::Muon & muon, int & bin){
        float parx =  0;
        float pary =  0;
        float parz =  0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon,parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon,pary);
        CorrectionCode foundz = m_z_handler->GetBinningParameter(muon,parz);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error
            || foundz == CorrectionCode::Error
        ) return CorrectionCode::Error;
        else {
            int binx = m_h->GetXaxis()->FindBin(parx);
            int biny = m_h->GetYaxis()->FindBin(pary);
            int binz = m_h->GetZaxis()->FindBin(parz);
            if (binx < 1 || binx > m_h->GetNbinsX()
               || biny < 1 || biny > m_h->GetNbinsY()
               || binz < 1 || binz > m_h->GetNbinsZ()
            ) {
                return CorrectionCode::OutOfValidityRange;
            }
            bin = m_h->GetBin(binx,biny,binz);
        }
        return CorrectionCode::Ok;
    }
    
    CorrectionCode HistHandler_TH2Poly::FindBin (const xAOD::Muon & muon, int & bin){
        float parx =  0;
        float pary =  0;
        CorrectionCode foundx = m_x_handler->GetBinningParameter(muon,parx);
        CorrectionCode foundy = m_y_handler->GetBinningParameter(muon,pary);
        if (foundx == CorrectionCode::Error || foundy == CorrectionCode::Error) 
            return CorrectionCode::Error;
        else {
            bin = m_h->FindBin(parx,pary);
            if (bin < 0 ) {
                return CorrectionCode::OutOfValidityRange;
            }
        }
        return CorrectionCode::Ok;
    }
    
    AxisHandler* AxisHandlerProvider::GetAxisHandler (std::string axis){
         if (axis.find("pt") != std::string::npos || axis.find("pT") != std::string::npos || axis.find("p_{T}") != std::string::npos ){
            return new PtAxisHandler;
        }
        else if (axis.find("etaphi") != std::string::npos){
            return new FineEtaPhiAxisHandler;
        }
        else if (axis.find("phi") != std::string::npos){
            return new PhiAxisHandler;
        }
        else if (axis.find("q") != std::string::npos || axis.find("charge") != std::string::npos){
            return new ChargeAxisHandler;
        }
        else if (axis.find("eta") != std::string::npos){
            return new EtaAxisHandler;
        }
        Error("AxisHandlerProvider","Can not interpret axis title %s",axis.c_str());
        return new UndefinedAxisHandler;
    }
    
} // namespace CP
