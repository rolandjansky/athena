/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "METTriggerAugmentationTool.h"
#include <xAODTrigger/EnergySumRoI.h>
#include <xAODTrigger/EnergySumRoIAuxInfo.h>
#include <xAODTrigger/JetRoIContainer.h>
#include <xAODTrigger/JetRoI.h>
#include <PathResolver/PathResolver.h>
#include <vector>
#include <string>

#include <TFile.h>

namespace DerivationFramework {

  METTriggerAugmentationTool::METTriggerAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("OutputName", m_outputName = "LVL1EnergySumRoI_KF");
    declareProperty("LUTFile", m_LUTFileName = "LUT_data15.root");
    declareProperty("L1METName", m_L1METName = "LVL1EnergySumRoI");
    declareProperty("L1JetName", m_L1JetName = "LVL1JetRoIs");
  }

  StatusCode METTriggerAugmentationTool::initialize() 
  {
    std::string fullLUTFileName = PathResolver::find_file(m_LUTFileName, "DATAPATH");

    if (fullLUTFileName.empty() ) {
      ATH_MSG_ERROR( "File " << m_LUTFileName << " not found!" );
      return StatusCode::FAILURE;
    }

    TFile* lutFile = TFile::Open(fullLUTFileName.c_str() );
    if (lutFile->IsZombie() ) return StatusCode::FAILURE;

    TH2* lutFromFile(0);
    lutFile->GetObject("LUT", lutFromFile);
    if (!lutFromFile) {
      ATH_MSG_ERROR( "LUT file doesn't contain a 'LUT' object!" );
      return StatusCode::FAILURE;
    }

    m_LUT.reset(dynamic_cast<TH2*>(lutFromFile->Clone() ) );
    m_LUT->SetDirectory(0);
    lutFile->Close();
    return StatusCode::SUCCESS;
  }

  StatusCode METTriggerAugmentationTool::finalize()
  {
    return StatusCode::SUCCESS;
  }



  StatusCode METTriggerAugmentationTool::addBranches() const
  {
    ATH_MSG_DEBUG(" In L1KF_METMaker::makeKFMET()" );

    // if the output has already been written we don't need to do anything
    if (evtStore()->contains<xAOD::EnergySumRoI>(m_outputName) ) return StatusCode::SUCCESS;

    const xAOD::EnergySumRoI* originalL1(0);
    ATH_CHECK( evtStore()->retrieve(originalL1, m_L1METName) );

    const xAOD::JetRoIContainer* l1Jets(0);
    ATH_CHECK( evtStore()->retrieve(l1Jets, m_L1JetName) );

    xAOD::EnergySumRoI* l1_kf = new xAOD::EnergySumRoI();
    xAOD::EnergySumRoIAuxInfo* l1_kfAux = new xAOD::EnergySumRoIAuxInfo();

    ATH_MSG_DEBUG( "Setting the store" );
    // set the store
    l1_kf->setStore(l1_kfAux);

    ATH_MSG_DEBUG( "Making deep copy" );
    // copy across the info
    *l1_kf = *originalL1; 

    ATH_MSG_DEBUG( "Building KF MET" );
    float KFMETx = l1_kf->exMiss();
    float KFMETy = l1_kf->eyMiss();
    float KFSumEt = l1_kf->energyT();

    for (const auto jet : *l1Jets) {
      int etaBin = m_LUT->GetXaxis()->FindBin( fabs( jet->eta() ) );
      int ptBin = m_LUT->GetYaxis()->FindBin( log2( jet->et8x8() / 1000.) );
      if (ptBin==0) ptBin=1;
      float KFweight = m_LUT->GetBinContent(etaBin, ptBin);
      float jetContribution = jet->et8x8() * KFweight;

      KFMETx -= jetContribution * cos( jet->phi() );
      KFMETy -= jetContribution * sin( jet->phi() );
      KFSumEt += jetContribution;
    }
    l1_kf->setEnergyX(-KFMETx);
    l1_kf->setEnergyY(-KFMETy);
    l1_kf->setEnergyT(KFSumEt);

    ATH_MSG_DEBUG( "Built KF MET" );
    ATH_CHECK( evtStore()->record(l1_kf, m_outputName) );
    ATH_CHECK( evtStore()->record(l1_kfAux, m_outputName+"Aux.") );
    return StatusCode::SUCCESS;
  }
}
