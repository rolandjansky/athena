/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/MuonRoiChainTestAlg.h"

#include "TrigMuonCoinHierarchy/IMuonRoiChainSvc.h"

#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH2I.h"

namespace Trigger { 
  MuonRoiChainTestAlg::MuonRoiChainTestAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator), 
      m_muonRoiChainSvc(0), 
      m_thistSvc(0), 
      m_h_ROI_RDO(0), 
      m_h_RDO_ROI(0), 
      m_h_RDO_TgcCoinData(0), 
      m_h_TgcCoinData_RDO(0)
  {
  }

  StatusCode MuonRoiChainTestAlg::initialize() {   
    ATH_MSG_INFO("initialize()");

    // Retrieve Trigger::MuonRoiChainSvc 
    StatusCode sc = service("Trigger::MuonRoiChainSvc", m_muonRoiChainSvc);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Could not find MuonRoiChainSvc");
      return sc;
    }
    
    // Retrieve THistSvc
    sc = service("THistSvc", m_thistSvc);
    if(!sc.isSuccess() || !m_thistSvc) {
      ATH_MSG_FATAL("Could not find THistSvc");
      return StatusCode::FAILURE;
    }

    m_h_ROI_RDO = new TH2I("h_ROI_RDO", 
			   "Muon_ROI (LVL1_ROI) #rightarrow MuCTPI_RDO dataWord (MUCTPI_RDO)", 
			   6, -0.5, 5.5, 2, -0.5, 1.5);  
    m_h_ROI_RDO->GetXaxis()->SetBinLabel(1, "C Forward");
    m_h_ROI_RDO->GetXaxis()->SetBinLabel(2, "C Endcap");
    m_h_ROI_RDO->GetXaxis()->SetBinLabel(3, "C Barrel");
    m_h_ROI_RDO->GetXaxis()->SetBinLabel(4, "A Barrel");
    m_h_ROI_RDO->GetXaxis()->SetBinLabel(5, "A Endcap");
    m_h_ROI_RDO->GetXaxis()->SetBinLabel(6, "A Forward");
    m_h_ROI_RDO->GetYaxis()->SetBinLabel(1, "Not found");
    m_h_ROI_RDO->GetYaxis()->SetBinLabel(2, "Found");
    if(m_thistSvc->regHist("/MuonRoiChainTestAlg/h_ROI_RDO", m_h_ROI_RDO).isFailure()) { 
      return StatusCode::FAILURE;
    }
    
    m_h_RDO_ROI = new TH2I("h_RDO_ROI", 
			   "MuCTPI_RDO dataWord (MUCTPI_RDO) #rightarrow Muon_ROI (LVL1_ROI)", 
			   6, -0.5, 5.5, 3, -0.5, 2.5);  
    m_h_RDO_ROI->GetXaxis()->SetBinLabel(1, "C Forward");
    m_h_RDO_ROI->GetXaxis()->SetBinLabel(2, "C Endcap");
    m_h_RDO_ROI->GetXaxis()->SetBinLabel(3, "C Barrel");
    m_h_RDO_ROI->GetXaxis()->SetBinLabel(4, "A Barrel");
    m_h_RDO_ROI->GetXaxis()->SetBinLabel(5, "A Endcap");
    m_h_RDO_ROI->GetXaxis()->SetBinLabel(6, "A Forward");
    m_h_RDO_ROI->GetYaxis()->SetBinLabel(1, "Not found");
    m_h_RDO_ROI->GetYaxis()->SetBinLabel(2, "Not sent");
    m_h_RDO_ROI->GetYaxis()->SetBinLabel(3, "Found");
    if(m_thistSvc->regHist("/MuonRoiChainTestAlg/h_RDO_ROI", m_h_RDO_ROI).isFailure()) { 
      return StatusCode::FAILURE;
    }
    
    m_h_RDO_TgcCoinData = new TH2I("h_RDO_TgcCoinData", 
				   "MuCTPI_RDO dataWord (MUCTPI_RDO) #rightarrow TgcCoinData (TrigT1CoinDataCollection)", 
				   6, -0.5, 5.5, 2, -0.5, 1.5); 
    m_h_RDO_TgcCoinData->GetXaxis()->SetBinLabel(1, "C Forward");
    m_h_RDO_TgcCoinData->GetXaxis()->SetBinLabel(2, "C Endcap boundary");
    m_h_RDO_TgcCoinData->GetXaxis()->SetBinLabel(3, "C Endcap not boundary");
    m_h_RDO_TgcCoinData->GetXaxis()->SetBinLabel(4, "A Endcap not boundary");
    m_h_RDO_TgcCoinData->GetXaxis()->SetBinLabel(5, "A Endcap boundary");
    m_h_RDO_TgcCoinData->GetXaxis()->SetBinLabel(6, "A Forward");
    m_h_RDO_TgcCoinData->GetYaxis()->SetBinLabel(1, "Not found");
    m_h_RDO_TgcCoinData->GetYaxis()->SetBinLabel(2, "Found");
    if(m_thistSvc->regHist("/MuonRoiChainTestAlg/h_RDO_TgcCoinData", m_h_RDO_TgcCoinData).isFailure()) { 
      return StatusCode::FAILURE;
    }
    
    m_h_TgcCoinData_RDO = new TH2I("h_TgcCoinData_RDO", 
				   "TgcCoinData (TrigT1CoinDataCollection) #rightarrow MuCTPI_RDO dataWord (MUCTPI_RDO)", 
				   4, -0.5, 3.5, 2, -0.5, 1.5); 
    m_h_TgcCoinData_RDO->GetXaxis()->SetBinLabel(1, "C Forward");
    m_h_TgcCoinData_RDO->GetXaxis()->SetBinLabel(2, "C Endcap");
    m_h_TgcCoinData_RDO->GetXaxis()->SetBinLabel(3, "A Endcap");
    m_h_TgcCoinData_RDO->GetXaxis()->SetBinLabel(4, "A Forward");
    m_h_TgcCoinData_RDO->GetYaxis()->SetBinLabel(1, "Not found");
    m_h_TgcCoinData_RDO->GetYaxis()->SetBinLabel(2, "Found");
    if(m_thistSvc->regHist("/MuonRoiChainTestAlg/h_TgcCoinData_RDO", m_h_TgcCoinData_RDO).isFailure()) { 
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainTestAlg::execute() {   
    ATH_MSG_DEBUG("execute()");
    
    // Get all the MuonRoiChains in this event
    std::vector<const MuonRoiChain*> chainVector;
    StatusCode sc = m_muonRoiChainSvc->getChains(&chainVector);
    if(sc.isFailure()) { 
      ATH_MSG_FATAL("getChains() failed in execute().");
      return StatusCode::FAILURE;
    }
    unsigned int size = chainVector.size();
    ATH_MSG_DEBUG("The number of MuonRoiChains in this event is " << size);
    for(unsigned int ic=0; ic<size; ic++) {
      const MuonRoiChain *chain = chainVector.at(ic);
      if(!chain) {
	ATH_MSG_WARNING("Obtained MuonRoiChain pointer is null. This should never happen."); 
	continue;
      }

      Muon_ROI::RoISource source = chain->getSource();
      Muon_ROI::Hemisphere hemisphere = chain->getHemisphere();

      int region = -1;
      if(hemisphere==Muon_ROI::Negative) {
	if(source==Muon_ROI::Forward)      region = 0;
	else if(source==Muon_ROI::Endcap)  region = 1;
	else if(source==Muon_ROI::Barrel)  region = 2;
      } else if(hemisphere==Muon_ROI::Positive) {
        if(source==Muon_ROI::Barrel)       region = 3;
        else if(source==Muon_ROI::Endcap)  region = 4;
	else if(source==Muon_ROI::Forward) region = 5;
      }

      const Muon_ROI* roi = chain->getMuon_ROI();
      uint32_t rdoDataWord = chain->getMuCTPI_RDO_dataWord();
      const Muon::TgcCoinData* sl = chain->getTgcCoinData();
      
      if(roi) {
	m_h_ROI_RDO->Fill(region, static_cast<bool>(rdoDataWord)); 
      } 

      if(rdoDataWord) {
	MuCTPI_DataWord_Decoder muctpiDecoder(rdoDataWord);
	
	double val_RDO_ROI = 0.;
	if(roi) val_RDO_ROI = 2.;
	else if(!muctpiDecoder.getSentRoi()) val_RDO_ROI = 1.;
	m_h_RDO_ROI->Fill(region, val_RDO_ROI);

	bool isInEndcapChamberBoundary = chain->isInEndcapChamberBoundary();
	int region_RDO_TgcCoinData = -1;
	if(region==0) {
	                                 region_RDO_TgcCoinData = 0; // C Forward 
	} else if(region==1) {
	  if(isInEndcapChamberBoundary)  region_RDO_TgcCoinData = 1; // C Endcap boundary 
	  else                           region_RDO_TgcCoinData = 2; // C Endcap not boundary 
	} else if(region==4) {
	  if(!isInEndcapChamberBoundary) region_RDO_TgcCoinData = 3; // A Endcap not boundary 
	  else                           region_RDO_TgcCoinData = 4; // A Endcap boundary 
	} else if(region==5) {
	                                 region_RDO_TgcCoinData = 5; // A Forward 
	}
	if(source!=Muon_ROI::Barrel) m_h_RDO_TgcCoinData->Fill(region_RDO_TgcCoinData, static_cast<bool>(sl));
      }

      if(sl) {
	m_h_TgcCoinData_RDO->Fill(region-(hemisphere==Muon_ROI::Positive ? 2 : 0), static_cast<bool>(rdoDataWord));
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainTestAlg::finalize() {   
    ATH_MSG_INFO("finalize()");

    return StatusCode::SUCCESS;
  }

} // end of namespace Trigger
