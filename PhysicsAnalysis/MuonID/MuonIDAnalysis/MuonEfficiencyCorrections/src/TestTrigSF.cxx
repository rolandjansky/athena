/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// ROOT
#include "TH1.h"
#include "TH2.h"

// xAOD
#include "xAODMuon/MuonContainer.h"
#include "AthContainers/ConstDataVector.h"

// Local
#include "TestTrigSF.h"

using namespace Trig;

//=============================================================================
TestTrigSF::TestTrigSF(const std::string& name,
                ISvcLocator* pSvcLocator) :
                AthAlgorithm(name, pSvcLocator),
                m_histSvc("THistSvc/THistSvc", name),
                m_countEvent(0),
                m_barrel_sf(0),
                m_endcap_sf(0),
                m_barrel_sf_err_stat_up(0),
                m_barrel_sf_err_stat_dw(0),
                m_endcap_sf_err_stat_up(0),
                m_endcap_sf_err_stat_dw(0),
                m_barrel_sf_err_syst_up(0),
                m_barrel_sf_err_syst_dw(0),
                m_endcap_sf_err_syst_up(0),
                m_endcap_sf_err_syst_dw(0)
{
    declareProperty("outputStream", m_outputStream);
    declareProperty("inputContainerName", m_inputContainerName);
    declareProperty("triggersName", m_triggerName = "HLT_mu14");

    declareProperty("trigEff", m_trigEff);
}

//=============================================================================
StatusCode TestTrigSF::initialize()
{
    //
    // Initialize tools and services
    //
    CHECK(m_histSvc.retrieve());
    CHECK(m_trigEff.retrieve());

    //
    // Start total
    //
    m_timerTotal.Start();

    double *barrel_phi = new double[7];
    barrel_phi[0] = -2.9453;
    barrel_phi[1] = -2.1598;
    barrel_phi[2] = -0.9817;
    barrel_phi[3] = -0.1963;
    barrel_phi[4] = 0.9817;
    barrel_phi[5] = 2.1598;
    barrel_phi[6] = 3.3380;

    double *barrel_eta = new double[7];
    barrel_eta[0] = -1.05;
    barrel_eta[1] = -0.908;
    barrel_eta[2] = -0.476;
    barrel_eta[3] = 0.0;
    barrel_eta[4] = 0.476;
    barrel_eta[5] = 0.908;
    barrel_eta[6] = 1.05;

    double *endcap_eta = new double[12];
    endcap_eta[0] = -2.5;
    endcap_eta[1] = -2.4;
    endcap_eta[2] = -1.918;
    endcap_eta[3] = -1.348;
    endcap_eta[4] = -1.1479;
    endcap_eta[5] = -1.05;
    endcap_eta[6] = 1.05;
    endcap_eta[7] = 1.1479;
    endcap_eta[8] = 1.348;
    endcap_eta[9] = 1.918;
    endcap_eta[10] = 2.4;
    endcap_eta[11] = 2.5;

    m_barrel_sf = new TH2D("barrel_sf", "", 6, barrel_eta, 6, barrel_phi);
    m_endcap_sf = new TH2D("endcap_sf", "", 11, endcap_eta, 12, -3.403392, 2.879793);

    RegHist(m_barrel_sf, "");
    RegHist(m_endcap_sf, "");

    m_barrel_sf_err_stat_up = new TH2D("barrel_sf_err_stat_up", "", 6, barrel_eta, 6, barrel_phi);
    m_barrel_sf_err_stat_dw = new TH2D("barrel_sf_err_stat_dw", "", 6, barrel_eta, 6, barrel_phi);

    m_endcap_sf_err_stat_up = new TH2D("endcap_sf_err_stat_up", "", 11, endcap_eta, 12, -3.403392, 2.879793);
    m_endcap_sf_err_stat_dw = new TH2D("endcap_sf_err_stat_dw", "", 11, endcap_eta, 12, -3.403392, 2.879793);

    m_barrel_sf_err_syst_up = new TH2D("barrel_sf_err_syst_up", "", 6, barrel_eta, 6, barrel_phi);
    m_barrel_sf_err_syst_dw = new TH2D("barrel_sf_err_syst_dw", "", 6, barrel_eta, 6, barrel_phi);

    m_endcap_sf_err_syst_up = new TH2D("endcap_sf_err_syst_up", "", 11, endcap_eta, 12, -3.403392, 2.879793);
    m_endcap_sf_err_syst_dw = new TH2D("endcap_sf_err_syst_dw", "", 11, endcap_eta, 12, -3.403392, 2.879793);

    RegHist(m_barrel_sf_err_stat_up, "");
    RegHist(m_barrel_sf_err_stat_dw, "");

    RegHist(m_endcap_sf_err_stat_up, "");
    RegHist(m_endcap_sf_err_stat_dw, "");

    RegHist(m_barrel_sf_err_syst_up, "");
    RegHist(m_barrel_sf_err_syst_dw, "");

    RegHist(m_endcap_sf_err_syst_up, "");
    RegHist(m_endcap_sf_err_syst_dw, "");

    for (CP::SystematicVariation syst : m_trigEff->affectingSystematics()) {
        msg(MSG::INFO) << "Recommended systematic: " << syst.name() << endmsg;
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TestTrigSF::execute()
{
    //
    // Process current event
    //
    msg() << MSG::DEBUG << "execute() - begin..." << endmsg;

    m_countEvent++;

    const xAOD::MuonContainer* xaodContainer = 0;

    if (evtStore()->retrieve(xaodContainer, m_inputContainerName).isFailure() || !xaodContainer) {
        msg() << MSG::ERROR << "Failed to get:" << m_inputContainerName << endmsg;
        return StatusCode::FAILURE;
    }

    msg(MSG::DEBUG) << "Size of MuonContainer: " << xaodContainer->size() << endmsg;

    //-----------------------------------------------------------------------------
    // Process muons
    //
    for (const xAOD::Muon *ptr : *xaodContainer) {

        msg(MSG::DEBUG) << "   pT= " << ptr->pt() << endmsg;

        CheckSF(ptr);
    }

    msg(MSG::DEBUG) << "execute() - processed " << endmsg;
    return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TestTrigSF::finalize()
{
    //
    // Finalize output
    //
    msg(MSG::INFO) << " processed " << m_countEvent << " event(s)" << endmsg;

    return StatusCode::SUCCESS;
}

//=============================================================================
void TestTrigSF::CheckSF(const xAOD::Muon *ptr)
                {
    //-----------------------------------------------------------------------------
    // Muon trigger SF
    //
    ConstDataVector < xAOD::MuonContainer > trigger_SF_muon(SG::VIEW_ELEMENTS);
    trigger_SF_muon.push_back(ptr);

    double triggerSF = 0.0;

    CP::CorrectionCode ct = m_trigEff->getTriggerScaleFactor(*trigger_SF_muon.asDataVector(), triggerSF, m_triggerName);

    if (ct.code() == 0) {
        msg(MSG::WARNING) << "Failed to get trigger SF" << endmsg;
        return;
    }

    TH2 *h = m_endcap_sf;
    TH2 *h_stat_up = m_endcap_sf_err_stat_up;
    TH2 *h_stat_dw = m_endcap_sf_err_stat_dw;
    TH2 *h_syst_up = m_endcap_sf_err_syst_up;
    TH2 *h_syst_dw = m_endcap_sf_err_syst_dw;

    std::string key = "endcap";

    if (std::fabs(ptr->eta()) < 1.05) {
        h = m_barrel_sf;
        h_stat_up = m_barrel_sf_err_stat_up;
        h_stat_dw = m_barrel_sf_err_stat_dw;
        h_syst_up = m_barrel_sf_err_syst_up;
        h_syst_dw = m_barrel_sf_err_syst_dw;

        key = "barrel";
    }

    if (!h) {
        msg(MSG::WARNING) << "Missing hist" << endmsg;
        return;
    }

    FillHist(h, ptr, triggerSF);

    msg(MSG::INFO) << m_triggerName << " eta=" << ptr->eta() << " phi=" << ptr->phi() << " SF value=" << triggerSF << endmsg;

    //
    // Chck systematic
    //
    CheckSyst(ptr, "MUON_EFF_TrigStatUncertainty", +1, h_stat_up);
    CheckSyst(ptr, "MUON_EFF_TrigStatUncertainty", -1, h_stat_dw);

    CheckSyst(ptr, "MUON_EFF_TrigSystUncertainty", +1, h_syst_up);
    CheckSyst(ptr, "MUON_EFF_TrigSystUncertainty", -1, h_syst_dw);
}

//=============================================================================
void TestTrigSF::CheckSyst(const xAOD::Muon *ptr, const std::string &syst_name, int step, TH2 *h)
                {
    CP::SystematicVariation syst(syst_name, step);

    const bool isaff = m_trigEff->isAffectedBySystematic(syst);

    if (!isaff) {
        msg(MSG::WARNING) << syst_name << " step=" << step << ": isAffected=" << isaff << endmsg;
        return;
    }

    CP::SystematicSet syst_set;
    syst_set.insert(syst);

    if (m_trigEff->applySystematicVariation(syst_set) != CP::SystematicCode::Ok) {
        msg(MSG::WARNING) << "Failed to apply systematic: " << syst_name << endmsg;
        return;
    }

    ConstDataVector < xAOD::MuonContainer > trigger_SF_muon(SG::VIEW_ELEMENTS);
    trigger_SF_muon.push_back(ptr);

    double triggerSF = 0.0;

    if (m_trigEff->getTriggerScaleFactor(*trigger_SF_muon.asDataVector(), triggerSF, m_triggerName) != CP::CorrectionCode::Ok) {
        msg(MSG::WARNING) << "Failed to get trigger SF" << endmsg;
        return;
    }

    FillHist(h, ptr, triggerSF);

    msg(MSG::INFO) << "Systematic=" << syst_name << " step=" << step << " hist=" << h->GetName() << " SF value=" << triggerSF << endmsg;

    if (m_trigEff->applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok) {
        msg(MSG::WARNING) << "Failed to apply default after systematic: " << syst_name << endmsg;
        return;
    }
}

//=============================================================================
void TestTrigSF::FillHist(TH2 *h, const xAOD::Muon *ptr, double val)
                {
    const int bin = h->FindBin(ptr->eta(), ptr->phi());

    const double hcont = h->GetBinContent(bin);

    msg(MSG::DEBUG) << " hist=" << h->GetName() << " bin=" << bin << " eta=" << ptr->eta() << " phi=" << ptr->phi() << endmsg;

    if (hcont > 0.0) {

        if (std::fabs(hcont - val) > 0.0001) {
            msg(MSG::WARNING) << " hist=" << h->GetName() << "   content=" << hcont << " != val=" << val << endmsg;
        }
    }
    else {
        h->SetBinContent(bin, val);
    }
}

//=============================================================================
void TestTrigSF::RegHist(TH1 *h, const std::string &key)
                {
    //
    // Register histogram
    //
    std::string hist_key = "/" + m_outputStream + "/" + key;
    if (key.empty() && h) {
        hist_key = "/" + m_outputStream + "/" + std::string(h->GetName());
    }

    if (m_histSvc->regHist(hist_key, h).isFailure()) {
        msg() << MSG::WARNING << "Could not register histogram: " << hist_key << endmsg;
    }
    else {
        msg() << MSG::INFO << "Registered histogram: " << hist_key << endmsg;
    }
}

