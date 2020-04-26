/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include <MuonEfficiencyCorrections/KinematicSystHandler.h>
namespace CP {

    float IKinematicSystHandler::Eta(const xAOD::Muon &mu) const {
        return mu.eta();
    }
    float IKinematicSystHandler::Pt(const xAOD::Muon &mu) const {
        return mu.pt();
    }
    float IKinematicSystHandler::PtGeV(const xAOD::Muon &mu) const {
        return mu.pt() / 1.e3;
    }
    float IKinematicSystHandler::AbsEta(const xAOD::Muon &mu) const {
        return std::abs(mu.eta());
    }
    
    IKinematicSystHandler::KinVariable IKinematicSystHandler::GetMuonVariableToUse(const std::string &name) {
        if (name == "pt") return &IKinematicSystHandler::Pt;
        if (name == "ptGeV") return &IKinematicSystHandler::PtGeV;
        if (name == "eta") return &IKinematicSystHandler::Eta;
        if (name == "AbsEta") return &IKinematicSystHandler::AbsEta;
        return nullptr;
    }

    //###############################################################
    //                  PtKinematicSystHandler
    //###############################################################                
    PtKinematicSystHandler::PtKinematicSystHandler(std::unique_ptr<HistHandler> pt_flatnesss, std::unique_ptr<HistHandler> energy_loss):
             m_flatness(),
             m_loss(),
             m_SystWeight(0){
        m_flatness.swap(pt_flatnesss);
        m_loss.swap(energy_loss);
    }
   
    CorrectionCode PtKinematicSystHandler::GetKineDependent(const xAOD::Muon& mu, float& eff) const {
        int bin_flat(-1), bin_loss(-1);
        float syst = 0;
        CorrectionCode cc_flat = m_flatness->FindBin(mu, bin_flat);
        CorrectionCode cc_eloss = mu.pt() > 200.e3 ? m_loss->FindBin(mu, bin_loss) : cc_flat;
      
        float eloss_syst = bin_loss < 1 ? 1.e6 : std::abs( m_loss->GetBinContent(bin_loss) * mu.pt()/1.0e6);
        /// We exceed the limits of the histogram
        if (cc_flat != CorrectionCode::Ok){
            /// The eloss is going to take over now
            if (cc_eloss == CorrectionCode::Ok){ 
                syst = eloss_syst;
                eff *= 1 + m_SystWeight * std::abs(syst);
                return cc_eloss;
             } else return cc_flat; 
        } else {        
            // The eloss -systematic is valid and smaller than the error from the flatness        
            float abs_error = std::abs( m_flatness->GetBinError(bin_flat));
            if (cc_eloss == CorrectionCode::Ok && mu.pt() > 200.e3 && (eloss_syst < abs_error || abs_error == 0 || mu.pt() > 500.e3)){
                syst = eloss_syst;
            // The flatness of the scale-factor is still more precise than the eloss. Assign this as an extra syst
            } else {            
                syst = m_flatness->GetBinContent(bin_flat); 
            }
        }       
        eff *= 1 + m_SystWeight * std::abs(syst);
        return cc_flat;
    }
            
    void PtKinematicSystHandler::SetSystematicWeight(float syst_weight) {
        m_SystWeight = syst_weight;
    }
            
    bool PtKinematicSystHandler::initialize(){
        return m_flatness.get() != nullptr && m_loss.get() != nullptr;
    }
    //###############################################################
    //                  TTVAClosureSysHandler
    //###############################################################
    TTVAClosureSysHandler::TTVAClosureSysHandler(std::unique_ptr<HistHandler> HistHandler):
        m_Handler(),
        m_SystWeight(0) {
      m_Handler.swap(HistHandler);
    }
    void TTVAClosureSysHandler::SetSystematicWeight( float SystWeight){m_SystWeight = SystWeight;}
    bool TTVAClosureSysHandler::initialize() { return m_Handler.get() != nullptr; }
    CorrectionCode TTVAClosureSysHandler::GetKineDependent(const xAOD::Muon&mu, float& Eff) const{
        int binsys = -1;
        CorrectionCode cc = m_Handler->FindBin(mu, binsys);
        if (cc != CorrectionCode::Ok) {
            return cc;
        }
        Eff *= (1. + m_SystWeight * std::abs(m_Handler->GetBinContent(binsys)));
        return CorrectionCode::Ok;   
    }
    //###############################################################
    //                  PrimodialPtSystematic
    //###############################################################
    PrimodialPtSystematic::PrimodialPtSystematic(std::unique_ptr<HistHandler> Handler) :
                    m_Handler(),
                    m_SystWeight(0) {
        m_Handler.swap(Handler);
    }
    CorrectionCode PrimodialPtSystematic::GetKineDependent(const xAOD::Muon &mu, float& Eff) const {
        // Account for catastrophic energy loss for  very high
        // pt's
        if (mu.pt() <= 200.e3) return CorrectionCode::Ok;

        int binsys = -1;
        CorrectionCode cc = m_Handler->FindBin(mu, binsys);
        if (cc != CorrectionCode::Ok) {
            return cc;
        }
        Eff *= (1. + m_SystWeight * std::abs(m_Handler->GetBinContent(binsys)) * mu.pt() / 1.0e6);
        return CorrectionCode::Ok;
    }
    void PrimodialPtSystematic::SetSystematicWeight(float SystWeight) {
        m_SystWeight = SystWeight;
    }
    bool PrimodialPtSystematic::initialize() {
        return m_Handler.get() != nullptr;
    }
    //###############################################################
    //                  BadMuonVetoSystHandler
    //###############################################################
    BadMuonVetoSystHandler::BadMuonVetoSystHandler(TDirectory* InDir_3Stations, TDirectory* InDir_2Stations) :
                    m_syst3Stations(),
                    m_syst2Stations(),
                    m_uncertVar(nullptr),
                    m_polySelVar(nullptr),
                    m_SystWeight(0.) {
        fillMap(InDir_3Stations, m_syst3Stations);
        if (InDir_2Stations && InDir_2Stations!= InDir_3Stations) {
            fillMap(InDir_2Stations,m_syst2Stations);            
        }
                        
    }
    void BadMuonVetoSystHandler::fillMap(TDirectory* InDir, std::map<Ranges, std::unique_ptr<TF1>>& systPolynomials){
        if (!InDir) {
            Error("BadMuonSysVetoHandler()", "No TDirectory is given");
            return;
        }
        TIter next(InDir->GetListOfKeys());
        while (TObject* obj = next()) {
            std::string ObjName = obj->GetName();
            float lowRange(-1.e20), highRange(1.e20);
            if (ObjName.find("TF1") == std::string::npos) {
                continue;
            }
            TF1* TF = nullptr;
            InDir->GetObject(ObjName.c_str(), TF);
            if (!TF) {
                continue;
            }
            //Elimnate the TF1_
            getNextProperty(ObjName);
            if (!m_uncertVar) {m_uncertVar = GetMuonVariableToUse(getNextProperty(ObjName));}
            /// Otherwise we need to get rid of the string
            else {getNextProperty(ObjName);}        
            if (!m_polySelVar) {m_polySelVar = GetMuonVariableToUse(getNextProperty(ObjName));}
            else {getNextProperty(ObjName);}
           
            std::string LowRange_str = getNextProperty(ObjName);
            std::string HighRange_str = getNextProperty(ObjName);
            if (!LowRange_str.empty()) {
                lowRange = atof(LowRange_str.c_str()) / std::pow(10, LowRange_str.size() -1);
            }
            if (!HighRange_str.empty()) {
                highRange = atof(HighRange_str.c_str()) / std::pow(10, LowRange_str.size() -1);
            }
            systPolynomials.insert(std::pair<Ranges, std::unique_ptr<TF1>>(Ranges(lowRange, highRange), std::unique_ptr<TF1>(TF)));
        }

    }
    CorrectionCode BadMuonVetoSystHandler::GetKineDependent(const xAOD::Muon &mu, float& Eff) const {
        if (m_SystWeight == 0.) {           
            return CorrectionCode::Ok;
        }
        TF1* Poly = nullptr;
        // we know that Eff=(1+relative sys error), since SF==1
        float RelHighPtSys = 0.;
        if (mu.pt() >= 100.e3) {
            CorrectionCode cc = findAppropiatePolynomial(mu, Poly);
            if (cc != CorrectionCode::Ok) {
                return cc;
            }            
            RelHighPtSys = Poly->Eval((this->*m_uncertVar)(mu));           

        } else {
            //Apply flat 0.5% systematic
            RelHighPtSys = 0.005;
        }
        Eff = 1 + m_SystWeight * std::sqrt(std::pow(Eff - 1,2) + RelHighPtSys * RelHighPtSys);

        return CorrectionCode::Ok;
    }
    void BadMuonVetoSystHandler::SetSystematicWeight(float SystWeight) {
        m_SystWeight = SystWeight;
    }

    bool BadMuonVetoSystHandler::initialize() {
        if (m_syst3Stations.empty()) {
            Error("BadMuonVetoSystHandler::initialize()", "No polynomials");
            return false;
        }
        if (!m_uncertVar || !m_polySelVar) {
            return false;
        }
        return true;
    }
    BadMuonVetoSystHandler::~BadMuonVetoSystHandler() {
    }

    CP::CorrectionCode BadMuonVetoSystHandler::findAppropiatePolynomial(const xAOD::Muon& mu, TF1* &Poly) const {
        if (!m_polySelVar) {
            Error("BadMuonVetoSystHandler()", "Something went wrong with the initialization");
            return CorrectionCode::Error;
        }
        uint8_t nprecisionLayers=0;      
        if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers)){
            Error("BadMuonSysVetoHandler()", "Precisionlayers unkown");
            return CorrectionCode::Error;
        } else if (nprecisionLayers >= 3) {
            for (const auto& BinnedPoly : m_syst3Stations) {
                if (BinnedPoly.first.first <= (this->*m_polySelVar)(mu) && (this->*m_polySelVar)(mu) < BinnedPoly.first.second) {
                    Poly = BinnedPoly.second.get();
                    return CorrectionCode::Ok;
                }
            }
        } else if (nprecisionLayers == 2) {
            for (const auto& BinnedPoly : m_syst2Stations) {
                if (BinnedPoly.first.first <= (this->*m_polySelVar)(mu) && (this->*m_polySelVar)(mu) < BinnedPoly.first.second) {
                    Poly = BinnedPoly.second.get();
                    return CorrectionCode::Ok;
                }
            }
        }       
        return CP::CorrectionCode::OutOfValidityRange;
    }
    std::string BadMuonVetoSystHandler::getNextProperty(std::string &sstr) const {
        //Elimnate the beginning underscores
        while (sstr.find("_") == 0) {
            sstr.substr(1, sstr.size());
        }
        size_t pos = sstr.find("_");
        std::string Property = pos != std::string::npos ? sstr.substr(0, pos) : sstr;
        if (pos != std::string::npos) {
            sstr = sstr.substr(Property.size() + 1, sstr.size());
        }
        return Property;
    }

} /* namespace CP */
