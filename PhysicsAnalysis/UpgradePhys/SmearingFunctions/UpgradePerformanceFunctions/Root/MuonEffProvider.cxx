/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "UpgradePerformanceFunctions/MuonEffProvider.h"

MuonEffProvider::MuonEffProvider(const std::string& name):
  AsgMessaging(name),
  m_useMuonHighEta(true),
  m_reco_MESF(nullptr)
{}

bool MuonEffProvider::initialize(const std::string & effi_dir, const std::string &  effi_file, bool useHighEtaTagger) {
    m_useMuonHighEta = useHighEtaTagger;
    m_auxMuoncont.reset( new xAOD::MuonContainer() );
    m_auxMuonaux.reset( new xAOD::MuonAuxContainer() );
    m_auxMuoncont->setStore(m_auxMuonaux.get());
    m_auxMuon = new xAOD::Muon();
    m_auxMuoncont->push_back(m_auxMuon);
    m_auxMuon->setCharge(1); // provide charge invariant efficiencies, can pick arbitrary value...
    m_auxMuon->setAuthor(xAOD::Muon::MuidCo); // pretend only CB muons, to make the tool pick up the default histo we provide
    m_auxMuon->setMuonType(xAOD::Muon::Combined);

    m_dummy_eventInfoCont.reset( new xAOD::EventInfoContainer() );
    m_dummy_eventInfoAux.reset( new xAOD::EventInfoAuxContainer() );
    m_dummy_eventInfoCont->setStore(m_dummy_eventInfoAux.get());
    m_dummy_eventInfo = new xAOD::EventInfo();
    m_dummy_eventInfoCont->push_back(m_dummy_eventInfo);

    m_dummy_eventInfo->setEventTypeBitmask(true); // set to simulation

    SG::AuxElement::Decorator<unsigned int> dec_rnd("RandomRunNumber");
    dec_rnd(*m_dummy_eventInfo) = 999999;

    CP::MuonEfficiencyScaleFactors* MySfTool = new CP::MuonEfficiencyScaleFactors("MuonCustomTotalSF" + effi_file);
    ATH_CHECK( MySfTool->setProperty("WorkingPoint", "Upgrade"));
    ATH_CHECK( MySfTool->setProperty("CustomInputFolder", effi_dir.c_str()));
    ATH_CHECK( MySfTool->setProperty("CustomFileCaloTag", effi_file.c_str()));
    ATH_CHECK( MySfTool->setProperty("CustomFileCombined", effi_file.c_str()));
    ATH_CHECK( MySfTool->setProperty("CustomFileLowPt", effi_file.c_str()));
    ATH_CHECK( MySfTool->setProperty("CustomFileLowPtCalo", effi_file.c_str()));
    ATH_CHECK( MySfTool->setProperty("CustomFileHighEta", effi_file.c_str()));
    ATH_CHECK( MySfTool->initialize());
    m_reco_MESF = ToolHandle<CP::IMuonEfficiencyScaleFactors>(MySfTool->name());
    return true;
}

float MuonEffProvider::getEfficiency (double pt, double eta, double phi) {
    float aux_eff = 0;
    if (pt < 2500) return 0; // no muons below 2.5 GeV supported
    if (fabs(eta) > 2.7) {
        // High-Eta-Tagger: 95% flat once we are above p = 10 GeV
        if (!m_useMuonHighEta || (pt * cosh(eta) < 10000)) return 0;
        else return 0.95;
    }
    else {
        updateMuon(pt < 14e6 ?  pt : 13.9e6 , eta, phi);
        if (m_reco_MESF->getMCEfficiency(*m_auxMuon, aux_eff, m_dummy_eventInfo) != CP::CorrectionCode::Ok) {
            ATH_MSG_WARNING("had a problem retrieving the efficiency for a muon with pt "<<pt<<", eta "<< eta<<", phi --> returning Eff==0");
            return 0;
        }
        return aux_eff;
    }
}

// update the internal dummy xAOD event
inline void MuonEffProvider::updateMuon(double pt, double eta, double phi) {
    m_auxMuon->setP4(pt, eta, phi);
}
