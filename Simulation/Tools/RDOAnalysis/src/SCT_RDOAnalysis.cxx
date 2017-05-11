/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SCT_RDOAnalysis.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

SCT_RDOAnalysis::SCT_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_sctID(nullptr)
  , m_rdoID(0)
  , m_rdoWord(0)
  , m_barrelEndcap(0)
  , m_layerDisk(0)
  , m_phiModule(0)
  , m_etaModule(0)
  , m_side(0)
  , m_strip(0)
  , m_groupSize(0)
  , m_sdoID(0)
  , m_sdoWord(0)
  , m_barrelEndcap_sdo(0)
  , m_layerDisk_sdo(0)
  , m_phiModule_sdo(0)
  , m_etaModule_sdo(0)
  , m_side_sdo(0)
  , m_strip_sdo(0)
  , m_noise(0)
  , m_belowThresh(0)
  , m_disabled(0)
  , m_barcode(0)
  , m_eventIndex(0)
  , m_charge(0)
  , m_barcode_vec(0)
  , m_eventIndex_vec(0)
  , m_charge_vec(0)

  , h_rdoID(0)
  , h_rdoWord(0)
  , h_barrelEndcap(0)
  , h_layerDisk(0)
  , h_phiModule(0)
  , h_etaModule(0)
  , h_side(0)
  , h_strip(0)
  , h_groupSize(0)
  , h_phi_v_eta(0)
  , h_brlLayer(0)
  , h_brlPhiMod(0)
  , h_brlEtaMod(0)
  , h_brlSide(0)
  , h_brlStrip(0)
  , h_brlGroupSize(0)
  , h_brl_phi_v_eta(0)
  , h_ecDisk(0)
  , h_ecPhiMod(0)
  , h_ecEtaMod(0)
  , h_ecSide(0)
  , h_ecStrip(0)
  , h_ecGroupSize(0)
  , h_ec_phi_v_eta(0)
  , h_sdoID(0)
  , h_sdoWord(0)
  , h_barrelEndcap_sdo(0)
  , h_layerDisk_sdo(0)
  , h_phiModule_sdo(0)
  , h_etaModule_sdo(0)
  , h_side_sdo(0)
  , h_strip_sdo(0)
  , h_barcode(0)
  , h_eventIndex(0)
  , h_charge(0)
  , h_phi_v_eta_sdo(0)
    
  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/SCT_RDOAnalysis/")
  , m_ntupleTreeName("SCT_RDOAna")
  , m_path("/SCT_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode SCT_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing SCT_RDOAnalysis" );

  // Grab SCT_ID helper
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
    return StatusCode::FAILURE;
  }

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "SCT_RDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    // SCT RDO
    m_tree->Branch("rdoID", &m_rdoID);
    m_tree->Branch("rdoWord", &m_rdoWord);
    m_tree->Branch("barrelEndcap", &m_barrelEndcap);
    m_tree->Branch("layerDisk", &m_layerDisk);
    m_tree->Branch("phiModule", &m_phiModule);
    m_tree->Branch("etaModule", &m_etaModule);
    m_tree->Branch("side", &m_side);
    m_tree->Branch("strip", &m_strip);
    m_tree->Branch("groupSize", &m_groupSize);
    // SCT SDO deposits
    m_tree->Branch("sdoID", &m_sdoID);
    m_tree->Branch("sdoWord", &m_sdoWord);
    m_tree->Branch("barrelEndcap_sdo", &m_barrelEndcap_sdo);
    m_tree->Branch("layerDisk_sdo", &m_layerDisk_sdo);
    m_tree->Branch("phiModule_sdo", &m_phiModule_sdo);
    m_tree->Branch("etaModule_sdo", &m_etaModule_sdo);
    m_tree->Branch("side_sdo", &m_side_sdo);
    m_tree->Branch("strip_sdo", &m_strip_sdo);
    m_tree->Branch("noise", &m_noise);
    m_tree->Branch("belowThresh", &m_belowThresh);
    m_tree->Branch("disabled", &m_disabled);
    m_tree->Branch("barcode", &m_barcode);
    m_tree->Branch("eventIndex", &m_eventIndex);
    m_tree->Branch("charge", &m_charge);
    m_tree->Branch("barcode_vec", &m_barcode_vec);
    m_tree->Branch("eventIndex_vec", &m_eventIndex_vec);
    m_tree->Branch("charge_vec", &m_charge_vec);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  // HISTOGRAMS
  h_rdoID = new TH1F("h_rdoID", "rdoID", 100, 0, 1e18);
  h_rdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoID->GetName(), h_rdoID));

  h_rdoWord = new TH1F("h_rdoWord", "rdoWord", 100, 0, 1e7);
  h_rdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoWord->GetName(), h_rdoWord));
  
  h_barrelEndcap = new TH1F("h_barrelEndcap", "Barrel or Endcap", 100, -3, 3);
  h_barrelEndcap->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barrelEndcap->GetName(), h_barrelEndcap));

  h_layerDisk = new TH1F("h_layerDisk", "Barrel layer or Endcap disk", 100, 0, 10);
  h_layerDisk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_layerDisk->GetName(), h_layerDisk));

  h_phiModule = new TH1F("h_phiModule", "Phi module", 100, 0, 60);
  h_phiModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiModule->GetName(), h_phiModule));

  h_etaModule = new TH1F("h_etaModule", "Eta module", 100, -7, 7);
  h_etaModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_etaModule->GetName(), h_etaModule));

  h_side = new TH1F("h_side", "Side", 100, 0, 1.5);
  h_side->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_side->GetName(), h_side));

  h_strip = new TH1F("h_strip", "Strip", 100, 0, 800);
  h_strip->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strip->GetName(), h_strip));

  h_groupSize = new TH1F("h_groupSize", "Group size", 100, 0, 150);
  h_groupSize->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_groupSize->GetName(), h_groupSize));

  h_phi_v_eta = new TH2F("h_phi_v_eta", "Phi module vs eta module", 100, -7, 7, 100, 0, 60);
  h_phi_v_eta->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phi_v_eta->GetName(), h_phi_v_eta));

  h_brlLayer = new TH1F("h_brlLayer", "Barrel layer", 100, 0, 10);
  h_brlLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlLayer->GetName(), h_brlLayer));
  
  h_brlPhiMod = new TH1F("h_brlPhiMod", "Barrel phi module", 100, 0, 60);
  h_brlPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlPhiMod->GetName(), h_brlPhiMod));

  h_brlEtaMod = new TH1F("h_brlEtaMod", "Barrel eta module", 100, -7, 7);
  h_brlEtaMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlEtaMod->GetName(), h_brlEtaMod));

  h_brlSide = new TH1F("h_brlSide", "Barrel side", 100, 0, 1.5);
  h_brlSide->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlSide->GetName(), h_brlSide));

  h_brlStrip = new TH1F("h_brlStrip", "Barrel strip", 100, 0, 800);
  h_brlStrip->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlStrip->GetName(), h_brlStrip));

  h_brlGroupSize = new TH1F("h_brlGroupSize", "Barrel group size", 100, 0, 150);
  h_brlGroupSize->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlGroupSize->GetName(), h_brlGroupSize));

  h_brl_phi_v_eta = new TH2F("h_brl_phi_v_eta", "Barrel phi module vs eta module", 100, -7, 7, 100, 0, 60);
  h_brl_phi_v_eta->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brl_phi_v_eta->GetName(), h_brl_phi_v_eta));

  h_ecDisk = new TH1F("h_ecDisk", "Endcap disk", 100, 0, 10);
  h_ecDisk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecDisk->GetName(), h_ecDisk));
  
  h_ecPhiMod = new TH1F("h_ecPhiMod", "Endcap phi module", 100, 0, 60);
  h_ecPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecPhiMod->GetName(), h_ecPhiMod));

  h_ecEtaMod = new TH1F("h_ecEtaMod", "Endcap eta module", 100, -7, 7);
  h_ecEtaMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecEtaMod->GetName(), h_ecEtaMod));

  h_ecSide = new TH1F("h_ecSide", "Endcap side", 100, 0, 1.5);
  h_ecSide->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecSide->GetName(), h_ecSide));

  h_ecStrip = new TH1F("h_ecStrip", "Endcap strip", 100, 0, 800);
  h_ecStrip->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecStrip->GetName(), h_ecStrip));

  h_ecGroupSize = new TH1F("h_ecGroupSize", "Endcap group size", 100, 0, 150);
  h_ecGroupSize->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecGroupSize->GetName(), h_ecGroupSize));

  h_ec_phi_v_eta = new TH2F("h_ec_phi_v_eta", "Endcap phi module vs eta module", 100, -7.5, 7.5, 100, 0, 60);
  h_ec_phi_v_eta->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ec_phi_v_eta->GetName(), h_ec_phi_v_eta));

  h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 1e18);
  h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoID->GetName(), h_sdoID));

  h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 1e7);
  h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoWord->GetName(), h_sdoWord));
  
  h_barrelEndcap_sdo = new TH1F("h_barrelEndcap_sdo", "Barrel or Endcap (SDO)", 100, -3, 3);
  h_barrelEndcap_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barrelEndcap_sdo->GetName(), h_barrelEndcap_sdo));

  h_layerDisk_sdo = new TH1F("h_layerDisk_sdo", "Barrel layer or Endcap disk (SDO)", 100, 0, 10);
  h_layerDisk_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_layerDisk_sdo->GetName(), h_layerDisk_sdo));

  h_phiModule_sdo = new TH1F("h_phiModule_sdo", "Phi module (SDO)", 100, 0, 60);
  h_phiModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiModule_sdo->GetName(), h_phiModule_sdo));

  h_etaModule_sdo = new TH1F("h_etaModule_sdo", "Eta module (SDO)", 100, -7, 7);
  h_etaModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_etaModule_sdo->GetName(), h_etaModule_sdo));

  h_side_sdo = new TH1F("h_side_sdo", "Side (SDO)", 100, 0, 1.5);
  h_side_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_side_sdo->GetName(), h_side_sdo));

  h_strip_sdo = new TH1F("h_strip_sdo", "Strip (SDO)", 100, 0, 800);
  h_strip_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strip_sdo->GetName(), h_strip_sdo));

  h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e5);
  h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barcode->GetName(), h_barcode));

  h_eventIndex = new TH1F("h_eventIndex", "Event index (SDO)", 100, 0, 10);
  h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_eventIndex->GetName(), h_eventIndex));

  h_charge = new TH1F("h_charge", "Charge (SDO)", 100, 0, 6e6);
  h_charge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_charge->GetName(), h_charge));

  h_phi_v_eta_sdo = new TH2F("h_phi_v_eta_sdo", "Phi module vs eta module (SDO)", 100, -7, 7, 100, 0, 60);
  h_phi_v_eta_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phi_v_eta_sdo->GetName(), h_phi_v_eta_sdo));

  return StatusCode::SUCCESS;
}

StatusCode SCT_RDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In SCT_RDOAnalysis::execute()" );

  m_rdoID->clear();
  m_rdoWord->clear();
  m_barrelEndcap->clear();
  m_layerDisk->clear();
  m_phiModule->clear();
  m_etaModule->clear();
  m_side->clear();
  m_strip->clear();
  m_groupSize->clear();
  m_sdoID->clear();
  m_sdoWord->clear();
  m_barrelEndcap_sdo->clear();
  m_layerDisk_sdo->clear();
  m_phiModule_sdo->clear();
  m_etaModule_sdo->clear();
  m_side_sdo->clear();
  m_strip_sdo->clear();
  m_noise->clear();
  m_belowThresh->clear();
  m_disabled->clear();
  m_barcode->clear();
  m_eventIndex->clear();
  m_charge->clear();
  m_barcode_vec->clear();
  m_eventIndex_vec->clear();
  m_charge_vec->clear();

  // RawData
  const SCT_RDO_Container* p_SCT_RDO_cont;
  if (evtStore()->retrieve(p_SCT_RDO_cont, "SCT_RDOs") == StatusCode::SUCCESS) {   
    // loop over RDO container
    SCT_RDO_Container::const_iterator rdoCont_itr(p_SCT_RDO_cont->begin());
    const SCT_RDO_Container::const_iterator rdoCont_end(p_SCT_RDO_cont->end());

    for ( ; rdoCont_itr != rdoCont_end; ++rdoCont_itr ) {
      const SCT_RDO_Collection* p_SCT_RDO_coll(*rdoCont_itr);
      SCT_RDO_Collection::const_iterator rdo_itr(p_SCT_RDO_coll->begin());
      const SCT_RDO_Collection::const_iterator rdo_end(p_SCT_RDO_coll->end());
      
      for ( ; rdo_itr != rdo_end; ++rdo_itr ) {
	const Identifier rdoID((*rdo_itr)->identify());
	const unsigned int rdoWord((*rdo_itr)->getWord());
	const int sctBrlEc(m_sctID->barrel_ec(rdoID));
	const int sctLayerDisk(m_sctID->layer_disk(rdoID));
	const int sctPhiMod(m_sctID->phi_module(rdoID));
	const int sctEtaMod(m_sctID->eta_module(rdoID));
	const int sctSide(m_sctID->side(rdoID));
	const int sctStrip(m_sctID->strip(rdoID));
	const int sctGroupSize((*rdo_itr)->getGroupSize());

	const unsigned long long rdoID_int = rdoID.get_compact();
	m_rdoID->push_back(rdoID_int);
	m_rdoWord->push_back(rdoWord);
	m_barrelEndcap->push_back(sctBrlEc);
	m_layerDisk->push_back(sctLayerDisk);
	m_phiModule->push_back(sctPhiMod);
	m_etaModule->push_back(sctEtaMod);
	m_side->push_back(sctSide);
	m_strip->push_back(sctStrip);
	m_groupSize->push_back(sctGroupSize);

	h_rdoID->Fill(rdoID_int);
	h_rdoWord->Fill(rdoWord);
	h_barrelEndcap->Fill(sctBrlEc);
	h_layerDisk->Fill(sctLayerDisk);
	h_phiModule->Fill(sctPhiMod);
	h_etaModule->Fill(sctEtaMod);
	h_side->Fill(sctSide);
	h_strip->Fill(sctStrip);
	h_groupSize->Fill(sctGroupSize);
	h_phi_v_eta->Fill(sctEtaMod, sctPhiMod);

	if (sctBrlEc == 0) {
	  h_brlLayer->Fill(sctLayerDisk);
	  h_brlPhiMod->Fill(sctPhiMod);
	  h_brlEtaMod->Fill(sctEtaMod);
	  h_brlSide->Fill(sctSide);
	  h_brlStrip->Fill(sctStrip);
	  h_brlGroupSize->Fill(sctGroupSize);
	  h_brl_phi_v_eta->Fill(sctEtaMod, sctPhiMod);
	}
	else if (abs(sctBrlEc) == 2) {
	  h_ecDisk->Fill(sctLayerDisk);
	  h_ecPhiMod->Fill(sctPhiMod);
	  h_ecEtaMod->Fill(sctEtaMod);
	  h_ecSide->Fill(sctSide);
	  h_ecStrip->Fill(sctStrip);
	  h_ecGroupSize->Fill(sctGroupSize);
	  h_ec_phi_v_eta->Fill(sctEtaMod, sctPhiMod);
	}
      }
    }
  }

  // SimData
  const InDetSimDataCollection* simDataMapSCT(nullptr);
  if (evtStore()->retrieve(simDataMapSCT, "SCT_SDO_Map") == StatusCode::SUCCESS) {
    // loop over SDO container
    InDetSimDataCollection::const_iterator sdo_itr(simDataMapSCT->begin());
    const InDetSimDataCollection::const_iterator sdo_end(simDataMapSCT->end());

    std::vector<int> barcode_vec;
    std::vector<int> eventIndex_vec;
    std::vector<float> charge_vec;
    for ( ; sdo_itr != sdo_end; ++sdo_itr ) {
      const Identifier sdoID((*sdo_itr).first);
      const InDetSimData& sdo((*sdo_itr).second);
      const unsigned long long sdoID_int = sdoID.get_compact();
      const int sdoWord(sdo.word());
      const int sctBrlEc_sdo(m_sctID->barrel_ec(sdoID));
      const int sctLayerDisk_sdo(m_sctID->layer_disk(sdoID));
      const int sctPhiMod_sdo(m_sctID->phi_module(sdoID));
      const int sctEtaMod_sdo(m_sctID->eta_module(sdoID));
      const int sctSide_sdo(m_sctID->side(sdoID));
      const int sctStrip_sdo(m_sctID->strip(sdoID));
      const bool noise(SCT_SimHelper::isNoise(sdo));
      const bool belowThresh(SCT_SimHelper::isBelowThreshold(sdo));
      const bool disabled(SCT_SimHelper::isDisabled(sdo));
      
      m_sdoID->push_back(sdoID_int);
      m_sdoWord->push_back(sdoWord);
      m_barrelEndcap_sdo->push_back(sctBrlEc_sdo);
      m_layerDisk_sdo->push_back(sctLayerDisk_sdo);
      m_phiModule_sdo->push_back(sctPhiMod_sdo);
      m_etaModule_sdo->push_back(sctEtaMod_sdo);
      m_side_sdo->push_back(sctSide_sdo);
      m_strip_sdo->push_back(sctStrip_sdo);
      m_noise->push_back(noise);
      m_belowThresh->push_back(belowThresh);
      m_disabled->push_back(disabled);

      h_sdoID->Fill(sdoID_int);
      h_sdoWord->Fill(sdoWord);
      h_barrelEndcap_sdo->Fill(sctBrlEc_sdo);
      h_layerDisk_sdo->Fill(sctLayerDisk_sdo);
      h_phiModule_sdo->Fill(sctPhiMod_sdo);
      h_etaModule_sdo->Fill(sctEtaMod_sdo);
      h_side_sdo->Fill(sctSide_sdo);
      h_strip_sdo->Fill(sctStrip_sdo);
      h_phi_v_eta_sdo->Fill(sctEtaMod_sdo, sctPhiMod_sdo);

      // loop over deposits
      // InDetSimData::Deposit typedef for std::pair<HepMCParticleLink,float>
      const std::vector<InDetSimData::Deposit>& deposits = sdo.getdeposits();
      std::vector<InDetSimData::Deposit>::const_iterator dep_itr(deposits.begin());
      const std::vector<InDetSimData::Deposit>::const_iterator dep_end(deposits.end());
      
      for ( ; dep_itr != dep_end; ++dep_itr ) {
	const HepMcParticleLink& particleLink = (*dep_itr).first;
	const int bar(particleLink.barcode());
	const int eventIx(particleLink.eventIndex());
	const float charge((*dep_itr).second);
	
	m_barcode->push_back(bar);
	m_eventIndex->push_back(eventIx);
	m_charge->push_back(charge);

	h_barcode->Fill(bar);
	h_eventIndex->Fill(eventIx);
	h_charge->Fill(charge);
	
	barcode_vec.push_back(bar);
	eventIndex_vec.push_back(eventIx);
	charge_vec.push_back(charge);
      }
      m_barcode_vec->push_back(barcode_vec);
      m_eventIndex_vec->push_back(eventIndex_vec);
      m_charge_vec->push_back(charge_vec);
      barcode_vec.clear();
      eventIndex_vec.clear();
      charge_vec.clear();
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }
  
  return StatusCode::SUCCESS;
}

StatusCode SCT_RDOAnalysis::finalize() {

  return StatusCode::SUCCESS;
  
}
