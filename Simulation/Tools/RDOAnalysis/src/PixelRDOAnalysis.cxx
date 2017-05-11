/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelRDOAnalysis.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

PixelRDOAnalysis::PixelRDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_pixelID(nullptr)
  , m_rdoID(0)
  , m_rdoWord(0)
  , m_barrelEndcap(0)
  , m_layerDisk(0)
  , m_phiModule(0)
  , m_etaModule(0)
  , m_phiIndex(0)
  , m_etaIndex(0)
  , m_ToT(0)
  , m_BCID(0)
  , m_LVL1A(0)
  , m_LVL1ID(0)
  , m_sdoID(0)
  , m_sdoWord(0)
  , m_barrelEndcap_sdo(0)
  , m_layerDisk_sdo(0)
  , m_phiModule_sdo(0)
  , m_etaModule_sdo(0)
  , m_phiIndex_sdo(0)
  , m_etaIndex_sdo(0)
  , m_noise(0)
  , m_belowThresh(0)
  , m_disabled(0)
  , m_badTOT(0)
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
  , h_phiIndex(0)
  , h_etaIndex(0)
  , h_ToT(0)
  , h_BCID(0)
  , h_LVL1A(0)
  , h_LVL1ID(0)
  , h_brlLayer(0)
  , h_brlPhiMod(0)
  , h_brlEtaMod(0)
  , h_brlPhiIndex(0)
  , h_brlEtaIndex(0)
  , h_brlToT(0)
  , h_brlBCID(0)
  , h_brlLVL1A(0)
  , h_brlLVL1ID(0)
  , h_ecDisk(0)
  , h_ecPhiMod(0)
  , h_ecEtaMod(0)
  , h_ecPhiIndex(0)
  , h_ecEtaIndex(0)
  , h_ecToT(0)
  , h_ecBCID(0)
  , h_ecLVL1A(0)
  , h_ecLVL1ID(0)
    
  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/PixelRDOAnalysis/")
  , m_ntupleTreeName("PixelRDOAna")
  , m_path("/PixelRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode PixelRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing PixelRDOAnalysis" );

  // Grab PixelID helper
  ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "PixelRDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    // PIXEL RDO
    m_tree->Branch("rdoID", &m_rdoID);
    m_tree->Branch("rdoWord", &m_rdoWord);
    m_tree->Branch("barrelEndcap", &m_barrelEndcap);
    m_tree->Branch("layerDisk", &m_layerDisk);
    m_tree->Branch("phiModule", &m_phiModule);
    m_tree->Branch("etaModule", &m_etaModule);
    m_tree->Branch("phiIndex", &m_phiIndex);
    m_tree->Branch("etaIndex", &m_etaIndex);
    m_tree->Branch("ToT", &m_ToT); // time over threshold value (0-255)
    m_tree->Branch("BCID", &m_BCID); // beam crossing ID
    m_tree->Branch("LVL1A", &m_LVL1A); // Level1 accept (0-15)
    m_tree->Branch("LVL1ID", &m_LVL1ID); // ATLAS LVL1 (0-255)
    // PIXEL SDO DEPOSITS
    m_tree->Branch("sdoID", &m_sdoID);
    m_tree->Branch("sdoWord", &m_sdoWord);
    m_tree->Branch("barrelEndcap_sdo", &m_barrelEndcap_sdo);
    m_tree->Branch("layerDisk_sdo", &m_layerDisk_sdo);
    m_tree->Branch("phiModule_sdo", &m_phiModule_sdo);
    m_tree->Branch("etaModule_sdo", &m_etaModule_sdo);
    m_tree->Branch("phiIndex_sdo", &m_phiIndex_sdo);
    m_tree->Branch("etaIndex_sdo", &m_etaIndex_sdo);
    m_tree->Branch("noise", &m_noise);
    m_tree->Branch("belowThresh", &m_belowThresh);
    m_tree->Branch("disabled", &m_disabled);
    m_tree->Branch("badTOT", &m_badTOT);
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
  h_rdoID = new TH1F("h_rdoID", "rdoID", 100, 0, 5e17);
  h_rdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoID->GetName(), h_rdoID));

  h_rdoWord = new TH1F("h_rdoWord", "rdoWord", 100, 0, 350);
  h_rdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoWord->GetName(), h_rdoWord));
  
  h_barrelEndcap = new TH1F("h_barrelEndcap", "Barrel or Endcap", 100, -5, 5);
  h_barrelEndcap->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barrelEndcap->GetName(), h_barrelEndcap));

  h_layerDisk = new TH1F("h_layerDisk", "Barrel layer or Endcap disk", 100, 0, 3);
  h_layerDisk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_layerDisk->GetName(), h_layerDisk));

  h_phiModule = new TH1F("h_phiModule", "Phi module", 100, 0, 75);
  h_phiModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiModule->GetName(), h_phiModule));

  h_etaModule = new TH1F("h_etaModule", "Eta module", 100, 0, 15);
  h_etaModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_etaModule->GetName(), h_etaModule));

  h_phiIndex = new TH1F("h_phiIndex", "Phi index", 100, 0, 350);
  h_phiIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiIndex->GetName(), h_phiIndex));

  h_etaIndex = new TH1F("h_etaIndex", "Eta index", 100, 0, 225);
  h_etaIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_etaIndex->GetName(), h_etaIndex));

  h_ToT = new TH1F("h_ToT", "ToT", 100, 0, 250);
  h_ToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ToT->GetName(), h_ToT));
  
  h_BCID = new TH1F("h_BCID", "BCID", 100, -1.5, 1.5);
  h_BCID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_BCID->GetName(), h_BCID));
  
  h_LVL1A = new TH1F("h_LVL1A", "LVL1A", 100, -1.5, 1.5);
  h_LVL1A->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_LVL1A->GetName(), h_LVL1A));
  
  h_LVL1ID = new TH1F("h_LVL1ID", "LVL1ID", 100, -1.5, 1.5);
  h_LVL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_LVL1ID->GetName(), h_LVL1ID));

  h_brlLayer = new TH1F("h_brlLayer", "Barrel layer", 100, 0, 3);
  h_brlLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlLayer->GetName(), h_brlLayer));
  
  h_brlPhiMod = new TH1F("h_brlPhiMod", "Barrel phi module", 100, 0, 80);
  h_brlPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlPhiMod->GetName(), h_brlPhiMod));

  h_brlEtaMod = new TH1F("h_brlEtaMod", "Barrel eta module", 100, 0, 15);
  h_brlEtaMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlEtaMod->GetName(), h_brlEtaMod));

  h_brlPhiIndex = new TH1F("h_brlPhiIndex", "Barrel phi index", 100, 0, 350);
  h_brlPhiIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlPhiIndex->GetName(), h_brlPhiIndex));

  h_brlEtaIndex = new TH1F("h_brlEtaIndex", "Barrel eta index", 100, 0, 225);
  h_brlEtaIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlEtaIndex->GetName(), h_brlEtaIndex));

  h_brlToT = new TH1F("h_brlToT", "Barrel ToT", 100, 0, 250);
  h_brlToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlToT->GetName(), h_brlToT));
  
  h_brlBCID = new TH1F("h_brlBCID", "Barrel BCID", 100, -1.5, 1.5);
  h_brlBCID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlBCID->GetName(), h_brlBCID));
  
  h_brlLVL1A = new TH1F("h_brlLVL1A", "Barrel LVL1A", 100, -1.5, 1.5);
  h_brlLVL1A->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlLVL1A->GetName(), h_brlLVL1A));
  
  h_brlLVL1ID = new TH1F("h_brlLVL1ID", "Barrel LVL1ID", 100, -1.5, 1.5);
  h_brlLVL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlLVL1ID->GetName(), h_brlLVL1ID));

  h_ecDisk = new TH1F("h_ecDisk", "Endcap disk", 100, 0, 3);
  h_ecDisk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecDisk->GetName(), h_ecDisk));
  
  h_ecPhiMod = new TH1F("h_ecPhiMod", "Endcap phi module", 100, 0, 80);
  h_ecPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecPhiMod->GetName(), h_ecPhiMod));

  h_ecEtaMod = new TH1F("h_ecEtaMod", "Endcap eta module", 100, 0, 15);
  h_ecEtaMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecEtaMod->GetName(), h_ecEtaMod));

  h_ecPhiIndex = new TH1F("h_ecPhiIndex", "Endcap phi index", 100, 0, 350);
  h_ecPhiIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecPhiIndex->GetName(), h_ecPhiIndex));

  h_ecEtaIndex = new TH1F("h_ecEtaIndex", "Endcap eta index", 100, 0, 225);
  h_ecEtaIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecEtaIndex->GetName(), h_ecEtaIndex));

  h_ecToT = new TH1F("h_ecToT", "EndcapToT", 100, 0, 250);
  h_ecToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecToT->GetName(), h_ecToT));
  
  h_ecBCID = new TH1F("h_ecBCID", "Endcap BCID", 100, -1.5, 1.5);
  h_ecBCID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecBCID->GetName(), h_ecBCID));
  
  h_ecLVL1A = new TH1F("h_ecLVL1A", "Endcap LVL1A", 100, -1.5, 1.5);
  h_ecLVL1A->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecLVL1A->GetName(), h_ecLVL1A));
  
  h_ecLVL1ID = new TH1F("h_ecLVL1ID", "Endcap LVL1ID", 100, -1.5, 1.5);
  h_ecLVL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecLVL1ID->GetName(), h_ecLVL1ID));

  h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 5e17);
  h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoID->GetName(), h_sdoID));

  h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 350);
  h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoWord->GetName(), h_sdoWord));
  
  h_barrelEndcap_sdo = new TH1F("h_barrelEndcap_sdo", "Barrel or Endcap (SDO)", 100, -5, 5);
  h_barrelEndcap_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barrelEndcap_sdo->GetName(), h_barrelEndcap_sdo));

  h_layerDisk_sdo = new TH1F("h_layerDisk_sdo", "Barrel layer or Endcap disk (SDO)", 100, 0, 3);
  h_layerDisk_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_layerDisk_sdo->GetName(), h_layerDisk_sdo));

  h_phiModule_sdo = new TH1F("h_phiModule_sdo", "Phi module (SDO)", 100, 0, 75);
  h_phiModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiModule_sdo->GetName(), h_phiModule_sdo));

  h_etaModule_sdo = new TH1F("h_etaModule_sdo", "Eta module (SDO)", 100, 0, 15);
  h_etaModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_etaModule_sdo->GetName(), h_etaModule_sdo));

  h_phiIndex_sdo = new TH1F("h_phiIndex_sdo", "Phi index (SDO)", 100, 0, 350);
  h_phiIndex_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiIndex_sdo->GetName(), h_phiIndex_sdo));

  h_etaIndex_sdo = new TH1F("h_etaIndex_sdo", "Eta index (SDO)", 100, 0, 350);
  h_etaIndex_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_etaIndex_sdo->GetName(), h_etaIndex_sdo));

  h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e5);
  h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barcode->GetName(), h_barcode));

  h_eventIndex = new TH1F("h_eventIndex", "Event Index (SDO)", 100, 0, 2);
  h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_eventIndex->GetName(), h_eventIndex));

  h_charge = new TH1F("h_charge", "Charge (SDO)", 100, 0, 1e7);
  h_charge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_charge->GetName(), h_charge));

  
  return StatusCode::SUCCESS;
}

StatusCode PixelRDOAnalysis::execute() {
  ATH_MSG_DEBUG(" In PixelRDOAnalysis::execute()" );

  m_rdoID->clear();
  m_rdoWord->clear();
  m_barrelEndcap->clear();
  m_layerDisk->clear();
  m_phiModule->clear();
  m_etaModule->clear();
  m_phiIndex->clear();
  m_etaIndex->clear();
  m_ToT->clear();
  m_BCID->clear();
  m_LVL1A->clear();
  m_LVL1ID->clear();
  m_sdoID->clear();
  m_sdoWord->clear();
  m_barrelEndcap_sdo->clear();
  m_layerDisk_sdo->clear();
  m_phiModule_sdo->clear();
  m_etaModule_sdo->clear();
  m_phiIndex_sdo->clear();
  m_etaIndex_sdo->clear();
  m_noise->clear();
  m_belowThresh->clear();
  m_disabled->clear();
  m_badTOT->clear();
  m_barcode->clear();
  m_eventIndex->clear();
  m_charge->clear();
  m_barcode_vec->clear();
  m_eventIndex_vec->clear();
  m_charge_vec->clear();
  
  // Raw Data
  const PixelRDO_Container* p_pixelRDO_cont;
  if (evtStore()->retrieve(p_pixelRDO_cont, "PixelRDOs") == StatusCode::SUCCESS) {
    // loop over RDO container
    PixelRDO_Container::const_iterator rdoCont_itr(p_pixelRDO_cont->begin());
    const PixelRDO_Container::const_iterator rdoCont_end(p_pixelRDO_cont->end());
    for ( ; rdoCont_itr != rdoCont_end; ++rdoCont_itr ) {
      const PixelRDO_Collection* p_pixelRDO_coll(*rdoCont_itr);
      PixelRDO_Collection::const_iterator rdo_itr(p_pixelRDO_coll->begin());
      const PixelRDO_Collection::const_iterator rdo_end(p_pixelRDO_coll->end());
      for ( ; rdo_itr != rdo_end; ++rdo_itr ) {
	const Identifier rdoID((*rdo_itr)->identify());
	const int pixBrlEc(m_pixelID->barrel_ec(rdoID));
	const unsigned int rdoWord((*rdo_itr)->getWord());
	const int pixLayerDisk(m_pixelID->layer_disk(rdoID));
        const int pixPhiMod(m_pixelID->phi_module(rdoID));
	const int pixEtaMod(m_pixelID->eta_module(rdoID));
	const int pixPhiIx(m_pixelID->phi_index(rdoID));
	const int pixEtaIx(m_pixelID->eta_index(rdoID));
	const int pixToT((*rdo_itr)->getToT());
	const int pixBCID((*rdo_itr)->getBCID());
	const int pixLVL1A((*rdo_itr)->getLVL1A());
	const int pixLVL1ID((*rdo_itr)->getLVL1ID());

	const unsigned long long rdoID_int = rdoID.get_compact();
	m_rdoID->push_back(rdoID_int);
	m_rdoWord->push_back(rdoWord);
	m_barrelEndcap->push_back(pixBrlEc);
	m_layerDisk->push_back(pixLayerDisk);
	m_phiModule->push_back(pixPhiMod);
	m_etaModule->push_back(pixEtaMod);
	m_phiIndex->push_back(pixPhiIx);
	m_etaIndex->push_back(pixEtaIx);
	m_ToT->push_back(pixToT);
	m_BCID->push_back(pixBCID);
	m_LVL1A->push_back(pixLVL1A);
	m_LVL1ID->push_back(pixLVL1ID);

	h_rdoID->Fill(rdoID_int);
	h_rdoWord->Fill(rdoWord);
	h_barrelEndcap->Fill(pixBrlEc);
	h_layerDisk->Fill(pixLayerDisk);
	h_phiModule->Fill(pixPhiMod);
	h_etaModule->Fill(pixEtaMod);
	h_phiIndex->Fill(pixPhiIx);
	h_etaIndex->Fill(pixEtaIx);
	h_ToT->Fill(pixToT);
	h_BCID->Fill(pixBCID);
	h_LVL1A->Fill(pixLVL1A);
	h_LVL1ID->Fill(pixLVL1ID);

	if (pixBrlEc == 0) {
	  h_brlLayer->Fill(pixLayerDisk);
	  h_brlPhiMod->Fill(pixPhiMod);
	  h_brlEtaMod->Fill(pixEtaMod);
	  h_brlPhiIndex->Fill(pixPhiIx);
	  h_brlEtaIndex->Fill(pixEtaIx);
	  h_brlToT->Fill(pixToT);
	  h_brlBCID->Fill(pixBCID);
	  h_brlLVL1A->Fill(pixLVL1A);
	  h_brlLVL1ID->Fill(pixLVL1ID);
	}
	else if (abs(pixBrlEc) == 4) {
	  h_ecDisk->Fill(pixLayerDisk);
	  h_ecPhiMod->Fill(pixPhiMod);
	  h_ecEtaMod->Fill(pixEtaMod);
	  h_ecPhiIndex->Fill(pixPhiIx);
	  h_ecEtaIndex->Fill(pixEtaIx);
	  h_ecToT->Fill(pixToT);
	  h_ecBCID->Fill(pixBCID);
	  h_ecLVL1A->Fill(pixLVL1A);
	  h_ecLVL1ID->Fill(pixLVL1ID);
	}
      }
    }
  }

  // Sim Data
  const InDetSimDataCollection* simDataMapPixel(nullptr);
  if (evtStore()->retrieve(simDataMapPixel, "PixelSDO_Map") == StatusCode::SUCCESS) {
    // loop over SDO container
    InDetSimDataCollection::const_iterator sdo_itr(simDataMapPixel->begin());
    const InDetSimDataCollection::const_iterator sdo_end(simDataMapPixel->end());

    std::vector<int> barcode_vec;
    std::vector<int> eventIndex_vec;
    std::vector<float> charge_vec;
    for ( ; sdo_itr != sdo_end; ++sdo_itr ) {
      const Identifier sdoID((*sdo_itr).first);
      const InDetSimData& sdo((*sdo_itr).second);
      const unsigned long long sdoID_int = sdoID.get_compact();
      const int sdoWord(sdo.word());
      const int pixBrlEc_sdo(m_pixelID->barrel_ec(sdoID));
      const int pixLayerDisk_sdo(m_pixelID->layer_disk(sdoID));
      const int pixPhiMod_sdo(m_pixelID->phi_module(sdoID));
      const int pixEtaMod_sdo(m_pixelID->eta_module(sdoID));
      const int pixPhiIx_sdo(m_pixelID->phi_index(sdoID));
      const int pixEtaIx_sdo(m_pixelID->eta_index(sdoID));
      const bool noise(PixelSimHelper::isNoise(sdo));
      const bool belowThresh(PixelSimHelper::isBelowThreshold(sdo));
      const bool disabled(PixelSimHelper::isDisabled(sdo));
      const bool badTOT(PixelSimHelper::hasBadTOT(sdo));
      
      m_sdoID->push_back(sdoID_int);
      m_sdoWord->push_back(sdoWord);
      m_barrelEndcap_sdo->push_back(pixBrlEc_sdo);
      m_layerDisk_sdo->push_back(pixLayerDisk_sdo);
      m_phiModule_sdo->push_back(pixPhiMod_sdo);
      m_etaModule_sdo->push_back(pixEtaMod_sdo);
      m_phiIndex_sdo->push_back(pixPhiIx_sdo);
      m_etaIndex_sdo->push_back(pixEtaIx_sdo);
      m_noise->push_back(noise);
      m_belowThresh->push_back(belowThresh);
      m_disabled->push_back(disabled);
      m_badTOT->push_back(badTOT);

      h_sdoID->Fill(sdoID_int);
      h_sdoWord->Fill(sdoWord);
      h_barrelEndcap_sdo->Fill(pixBrlEc_sdo);
      h_layerDisk_sdo->Fill(pixLayerDisk_sdo);
      h_phiModule_sdo->Fill(pixPhiMod_sdo);
      h_etaModule_sdo->Fill(pixEtaMod_sdo);
      h_phiIndex_sdo->Fill(pixPhiIx_sdo);
      h_etaIndex_sdo->Fill(pixEtaIx_sdo);

      // loop over deposits
      const std::vector<InDetSimData::Deposit>& deposits = sdo.getdeposits();
      std::vector<InDetSimData::Deposit>::const_iterator dep_itr(deposits.begin());
      const std::vector<InDetSimData::Deposit>::const_iterator dep_end(deposits.end());
      for ( ; dep_itr != dep_end; ++dep_itr ) {
      	const HepMcParticleLink& particleLink = (*dep_itr).first;
	const int bar(particleLink.barcode());
	const int eventIx(particleLink.eventIndex());
	const int charge((*dep_itr).second);
	
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

StatusCode PixelRDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
