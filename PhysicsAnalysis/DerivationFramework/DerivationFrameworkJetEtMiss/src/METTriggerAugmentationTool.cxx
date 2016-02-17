/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "METTriggerAugmentationTool.h"
#include <xAODTrigger/EnergySumRoI.h>
#include <xAODTrigger/JetRoIContainer.h>
#include <xAODTrigger/JetRoI.h>
#include <PathResolver/PathResolver.h>
#include <vector>
#include <string>

#include <TFile.h>
#include <TH2.h>

namespace DerivationFramework {

  METTriggerAugmentationTool::METTriggerAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p), m_SGPrefix(""), m_LUTFileName("LUT_ZHnub_mu80_BCID0_sigHT500_int.root")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("SGPrefix", m_SGPrefix);
    declareProperty("LUTFileName", m_LUTFileName);
  }

  StatusCode METTriggerAugmentationTool::initialize() 
  {
    std::string LUTFileLoc = PathResolver::find_file("LUT_ZHnub_mu80_BCID0_sigHT500_int.root", "DATAPATH");
    ATH_MSG_INFO ("Opening LUT file at " << LUTFileLoc);
    m_LUTFile = new TFile(LUTFileLoc.c_str(), "READ");
    if (m_LUTFile->IsZombie()) return StatusCode::FAILURE;
    m_LUT = (TH2F*)m_LUTFile->Get("LUT");
    if (!m_LUT) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }

  StatusCode METTriggerAugmentationTool::finalize()
  {
    m_LUTFile->Close();
    return StatusCode::SUCCESS;
  }



  StatusCode METTriggerAugmentationTool::addBranches() const
  {
    // Set up the outputs
    std::unique_ptr<std::vector<float>> KFEx(new std::vector<float>());
    std::unique_ptr<std::vector<float>> KFEy(new std::vector<float>());

    // Retrieve the containers
    const xAOD::EnergySumRoI* L1XE_c = evtStore()->retrieve<const xAOD::EnergySumRoI>("LVL1EnergySumRoI");
    if (!L1XE_c) {
      ATH_MSG_ERROR ("Couldn't retrieve EnergySumRoI with key LVL1EnergySumRoI");
      return StatusCode::FAILURE;
    }

    const xAOD::JetRoIContainer* L1Jet_c = evtStore()->retrieve<const xAOD::JetRoIContainer>("LVL1JetRoIs");
    if (!L1Jet_c) {
      ATH_MSG_ERROR ("Couldn't retrieve JetRoIContainer with key LVL1JetRoIs");
      return StatusCode::FAILURE;
    }


    float KF_XE_x = L1XE_c->energyX();
    float KF_XE_y = L1XE_c->energyY();

    for (const xAOD::JetRoI* L1Jet : *L1Jet_c){
      Int_t etaBin = m_LUT->GetXaxis()->FindBin(abs(L1Jet->eta()));
      Int_t ptBin  = m_LUT->GetYaxis()->FindBin(log2(L1Jet->et8x8() * 0.001));
      if(ptBin==0) ptBin++;
      Double_t weight = m_LUT->GetBinContent(etaBin,ptBin);

      KF_XE_x -= L1Jet->et8x8() * cos(L1Jet->phi()) * weight;
      KF_XE_y -= L1Jet->et8x8() * sin(L1Jet->phi()) * weight;
    }


    KFEx->push_back(KF_XE_x);
    KFEy->push_back(KF_XE_y);

    // Write output to SG for access by downstream algs
    if (evtStore()->contains<std::vector<float>> ((m_SGPrefix+"KFEx").c_str()) ||
        evtStore()->contains<std::vector<float>> ((m_SGPrefix+"KFEy").c_str())){
      ATH_MSG_ERROR("Tool is attempting to write StoreGate keys which already exist. Please use different keys");
      return StatusCode::FAILURE;
    } else {
      CHECK(evtStore()->record(std::move(KFEx), (m_SGPrefix+"KFEx").c_str()));
      CHECK(evtStore()->record(std::move(KFEy), (m_SGPrefix+"KFEy").c_str()));
    }

    return StatusCode::SUCCESS;
  }
}
