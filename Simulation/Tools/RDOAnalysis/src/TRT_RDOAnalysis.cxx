/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_RDOAnalysis.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

TRT_RDOAnalysis::TRT_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_trtID(nullptr)
  , m_rdoID(0)
  , m_rdoWord(0)
  , m_barrelEndcap(0)
  , m_phiModule(0)
  , m_layerWheel(0)
  , m_strawLayer(0)
  , m_straw(0)
  , m_highLevel(0)
  , m_timeOverThreshold(0)
  , m_driftTimeBin(0)
  , m_trailEdge(0)
  , m_firstBin(0)
  , m_lastBin(0)
  , m_sdoID(0)
  , m_sdoWord(0)
  , m_barrelEndcap_sdo(0)
  , m_phiModule_sdo(0)
  , m_layerWheel_sdo(0)
  , m_strawLayer_sdo(0)
  , m_straw_sdo(0)
  , m_aboveThresh(0)
  , m_deadChan(0)
  , m_RODdata(0)
  , m_validStraw(0)
  , m_hit(0)
  , m_barcode(0)
  , m_eventIndex(0)
  , m_charge(0)
  , m_barcode_vec(0)
  , m_eventIndex_vec(0)
  , m_charge_vec(0)
 
  , h_rdoID(0)
  , h_rdoWord(0)
  , h_barrelEndcap(0)
  , h_phiModule(0)
  , h_layerWheel(0)
  , h_strawLayer(0)
  , h_straw(0)
  , h_ToT(0)
  , h_ToT_HL(0)
  , h_driftTimeBin(0)
  , h_trailEdge(0)
  , h_brlPhiMod(0)
  , h_brlLayer(0)
  , h_brlStrawLayer(0)
  , h_brlStraw(0)
  , h_brlToT(0)
  , h_brlToT_HL(0)
  , h_brlDriftTimeBin(0)
  , h_brlTrailEdge(0)
  , h_ecPhiMod(0)
  , h_ecWheel(0)
  , h_ecStrawLayer(0)
  , h_ecStraw(0)
  , h_ecToT(0)
  , h_ecToT_HL(0)
  , h_ecDriftTimeBin(0)
  , h_ecTrailEdge(0)
  , h_sdoID(0)
  , h_sdoWord(0)
  , h_barrelEndcap_sdo(0)
  , h_phiModule_sdo(0)
  , h_layerWheel_sdo(0)
  , h_strawLayer_sdo(0)
  , h_straw_sdo(0)
  , h_barcode(0)
  , h_eventIndex(0)
  , h_charge(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/TRT_RDOAnalysis/")
  , m_ntupleTreeName("TRT_RDOAna")
  , m_path("/TRT_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode TRT_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TRT_RDOAnalysis" );

  // Grab TRT_ID helper
  ATH_CHECK(detStore()->retrieve(m_trtID, "TRT_ID"));

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  // ntuple branches
  m_tree = new TTree(TString(m_ntupleTreeName), "TRT_RDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));
  if (m_tree) {
    // TRT RDO
    m_tree->Branch("rdoID", &m_rdoID);
    m_tree->Branch("rdoWord", &m_rdoWord);
    m_tree->Branch("barrelEndcap", &m_barrelEndcap);
    m_tree->Branch("phiModule", &m_phiModule);
    m_tree->Branch("layerWheel", &m_layerWheel);
    m_tree->Branch("strawLayer", &m_strawLayer);
    m_tree->Branch("straw", &m_straw);	  
    m_tree->Branch("highLevel", &m_highLevel);
    m_tree->Branch("timeOverThreshold", &m_timeOverThreshold);
    m_tree->Branch("driftTimeBin", &m_driftTimeBin);
    m_tree->Branch("trailEdge", &m_trailEdge);
    m_tree->Branch("firstBin", &m_firstBin);
    m_tree->Branch("lastBin", &m_lastBin);
    // TRT SDO deposits
    m_tree->Branch("sdoID", &m_sdoID);
    m_tree->Branch("sdoWord", &m_sdoWord);
    m_tree->Branch("barrelEndcap_sdo", &m_barrelEndcap_sdo);
    m_tree->Branch("phiModule_sdo", &m_phiModule_sdo);
    m_tree->Branch("layerWheel_sdo", &m_layerWheel_sdo);
    m_tree->Branch("strawLayer_sdo", &m_strawLayer_sdo);
    m_tree->Branch("straw_sdo", &m_straw_sdo);
    m_tree->Branch("aboveThresh", &m_aboveThresh);
    m_tree->Branch("deadChan", &m_deadChan);
    m_tree->Branch("RODdata", &m_RODdata);
    m_tree->Branch("validStraw", &m_validStraw);
    m_tree->Branch("hit", &m_hit);
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
  h_rdoID = new TH1F("h_rdoID", "rdoID", 100, 0, 2e18);
  h_rdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoID->GetName(), h_rdoID));

  h_rdoWord = new TH1F("h_rdoWord", "rdoWord", 100, 0, 1.5e8);
  h_rdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoWord->GetName(), h_rdoWord));
  
  h_barrelEndcap = new TH1F("h_barrelEndcap", "Barrel or Endcap", 100, -3.5, 3.5);
  h_barrelEndcap->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barrelEndcap->GetName(), h_barrelEndcap));

  h_phiModule = new TH1F("h_phiModule", "Phi module", 100, 0, 35);
  h_phiModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiModule->GetName(), h_phiModule));

  h_layerWheel = new TH1F("h_layerWheel", "Barrel layer or Endcap wheel", 100, 0, 20);
  h_layerWheel->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_layerWheel->GetName(), h_layerWheel));

  h_strawLayer = new TH1F("h_strawLayer", "Straw layer", 100, 0, 35);
  h_strawLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strawLayer->GetName(), h_strawLayer));

  h_straw = new TH1F("h_straw", "Straw", 100, 0, 1000);
  h_straw->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_straw->GetName(), h_straw));
   
  h_ToT = new TH1F("h_ToT", "Time over Threshold", 100, 0, 100);
  h_ToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ToT->GetName(), h_ToT));
  
  h_ToT_HL = new TH1F("h_ToT_HL", "Time over Threshold (highLevel)", 100, 0, 100);
  h_ToT_HL->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ToT_HL->GetName(), h_ToT_HL));
  
  h_driftTimeBin = new TH1F("h_driftTimeBin", "Drift Time Bin", 100, 0, 30);
  h_driftTimeBin->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_driftTimeBin->GetName(), h_driftTimeBin));

  h_trailEdge = new TH1F("h_trailEdge", "Trailing Edge", 100, 0, 30);
  h_trailEdge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_trailEdge->GetName(), h_trailEdge));

  h_brlPhiMod = new TH1F("h_brlPhiMod", "Barrel phi module", 100, 0, 35);
  h_brlPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlPhiMod->GetName(), h_brlPhiMod));

  h_brlLayer = new TH1F("h_brlLayer", "Barrel layer", 100, 0, 20);
  h_brlLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlLayer->GetName(), h_brlLayer));

  h_brlStrawLayer = new TH1F("h_brlStrawLayer", "Barrel straw layer", 100, 0, 35);
  h_brlStrawLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlStrawLayer->GetName(), h_brlStrawLayer));

  h_brlStraw = new TH1F("h_brlStraw", "Barrel straw", 100, 0, 1000);
  h_brlStraw->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlStraw->GetName(), h_brlStraw));
   
  h_brlToT = new TH1F("h_brlToT", "Barrel Time over Threshold", 100, 0, 100);
  h_brlToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlToT->GetName(), h_brlToT));
  
  h_brlToT_HL = new TH1F("h_brlToT_HL", "Barrel Time over Threshold (highLevel)", 100, 0, 100);
  h_brlToT_HL->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlToT_HL->GetName(), h_brlToT_HL));
  
  h_brlDriftTimeBin = new TH1F("h_brlDriftTimeBin", "Barrel Drift Time Bin", 100, 0, 30);
  h_brlDriftTimeBin->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlDriftTimeBin->GetName(), h_brlDriftTimeBin));

  h_brlTrailEdge = new TH1F("h_brlTrailEdge", "Barrel Trailing Edge", 100, 0, 30);
  h_brlTrailEdge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_brlTrailEdge->GetName(), h_brlTrailEdge));

  h_ecPhiMod = new TH1F("h_ecPhiMod", "Endcap phi module", 100, 0, 35);
  h_ecPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecPhiMod->GetName(), h_ecPhiMod));

  h_ecWheel = new TH1F("h_ecWheel", "Endcap wheel", 100, 0, 20);
  h_ecWheel->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecWheel->GetName(), h_ecWheel));

  h_ecStrawLayer = new TH1F("h_ecStrawLayer", "Endcap straw layer", 100, 0, 35);
  h_ecStrawLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecStrawLayer->GetName(), h_ecStrawLayer));

  h_ecStraw = new TH1F("h_ecStraw", "Endcap straw", 100, 0, 1000);
  h_ecStraw->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecStraw->GetName(), h_ecStraw));
   
  h_ecToT = new TH1F("h_ecToT", "Endcap Time over Threshold", 100, 0, 100);
  h_ecToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecToT->GetName(), h_ecToT));
  
  h_ecToT_HL = new TH1F("h_ecToT_HL", "Endcap Time over Threshold (highLevel)", 100, 0, 100);
  h_ecToT_HL->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecToT_HL->GetName(), h_ecToT_HL));
  
  h_ecDriftTimeBin = new TH1F("h_ecDriftTimeBin", "Endcap Drift Time Bin", 100, 0, 30);
  h_ecDriftTimeBin->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecDriftTimeBin->GetName(), h_ecDriftTimeBin));

  h_ecTrailEdge = new TH1F("h_ecTrailEdge", "Endcap Trailing Edge", 100, 0, 30);
  h_ecTrailEdge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_ecTrailEdge->GetName(), h_ecTrailEdge));

  h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 2.5e18);
  h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoID->GetName(), h_sdoID));

  h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 10);
  h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_sdoWord->GetName(), h_sdoWord));
  
  h_barrelEndcap_sdo = new TH1F("h_barrelEndcap_sdo", "Barrel or Endcap (sdo)", 100, -3.5, 3.5);
  h_barrelEndcap_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barrelEndcap_sdo->GetName(), h_barrelEndcap_sdo));

  h_phiModule_sdo = new TH1F("h_phiModule_sdo", "Phi module (sdo)", 100, 0, 35);
  h_phiModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_phiModule_sdo->GetName(), h_phiModule_sdo));

  h_layerWheel_sdo = new TH1F("h_layerWheel_sdo", "Barrel layer or Endcap wheel (sdo)", 100, 0, 20);
  h_layerWheel_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_layerWheel_sdo->GetName(), h_layerWheel_sdo));

  h_strawLayer_sdo = new TH1F("h_strawLayer_sdo", "Straw layer (sdo)", 100, 0, 35);
  h_strawLayer_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_strawLayer_sdo->GetName(), h_strawLayer_sdo));

  h_straw_sdo = new TH1F("h_straw_sdo", "Straw (sdo)", 100, 0, 1000);
  h_straw_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_straw_sdo->GetName(), h_straw_sdo));

  h_barcode = new TH1F("h_barcode", "Barcode (sdo)", 100, 0, 2.2e5);
  h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_barcode->GetName(), h_barcode));

  h_eventIndex = new TH1F("h_eventIndex", "Event index (sdo)", 100, 0, 10);
  h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_eventIndex->GetName(), h_eventIndex));

  h_charge = new TH1F("h_charge", "Charge (sdo)", 100, 0, 500);
  h_charge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_charge->GetName(), h_charge));

  return StatusCode::SUCCESS;
}

StatusCode TRT_RDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In TRT_RDOAnalysis::execute()" );

  m_rdoID->clear();
  m_rdoWord->clear();
  m_barrelEndcap->clear();
  m_phiModule->clear();
  m_layerWheel->clear();
  m_strawLayer->clear();
  m_straw->clear();
  m_highLevel->clear();
  m_timeOverThreshold->clear();
  m_driftTimeBin->clear();
  m_trailEdge->clear();
  m_firstBin->clear();
  m_lastBin->clear();
  m_sdoID->clear();
  m_sdoWord->clear();
  m_barrelEndcap_sdo->clear();
  m_phiModule_sdo->clear();
  m_layerWheel_sdo->clear();
  m_strawLayer_sdo->clear();
  m_straw_sdo->clear();
  m_aboveThresh->clear();
  m_deadChan->clear();
  m_RODdata->clear();
  m_validStraw->clear();
  m_hit->clear();
  m_barcode->clear();
  m_eventIndex->clear();
  m_charge->clear();
  m_barcode_vec->clear();
  m_eventIndex_vec->clear();
  m_charge_vec->clear();

  // RawData
  const TRT_RDO_Container* p_TRT_RDO_cont;
  if (evtStore()->retrieve(p_TRT_RDO_cont, "TRT_RDOs") == StatusCode::SUCCESS) {  
    // loop over RDO container
    TRT_RDO_Container::const_iterator rdoCont_itr(p_TRT_RDO_cont->begin());
    const TRT_RDO_Container::const_iterator rdoCont_end(p_TRT_RDO_cont->end());
    
    for ( ; rdoCont_itr != rdoCont_end; ++rdoCont_itr ) {
      const TRT_RDO_Collection* p_TRT_RDO_coll(*rdoCont_itr);
      TRT_RDO_Collection::const_iterator rdo_itr(p_TRT_RDO_coll->begin());
      const TRT_RDO_Collection::const_iterator rdo_end(p_TRT_RDO_coll->end());
      
      for ( ; rdo_itr != rdo_end; ++rdo_itr ) {
	const Identifier rdoID((*rdo_itr)->identify());
	const unsigned int rdoWord((*rdo_itr)->getWord());
	const int trtBrlEc(m_trtID->barrel_ec(rdoID));
	const int trtPhiMod(m_trtID->phi_module(rdoID));
	const int trtLayerWheel(m_trtID->layer_or_wheel(rdoID));
	const int trtStrawLayer(m_trtID->straw_layer(rdoID));
	const int trtStraw(m_trtID->straw(rdoID));
        const bool trtHL((*rdo_itr)->highLevel());
	const double trtToT((*rdo_itr)->timeOverThreshold());
	const int trtDriftTimeBin((*rdo_itr)->driftTimeBin());
	const int trtTrailEdge = dynamic_cast<const TRT_LoLumRawData*>(*rdo_itr)->trailingEdge();
	const bool trtFirstBin = dynamic_cast<const TRT_LoLumRawData*>(*rdo_itr)->firstBinHigh();
	const bool trtLastBin = dynamic_cast<const TRT_LoLumRawData*>(*rdo_itr)->lastBinHigh();

	const unsigned long long rdoID_int = rdoID.get_compact();
	m_rdoID->push_back(rdoID_int);
	m_rdoWord->push_back(rdoWord);
	m_barrelEndcap->push_back(trtBrlEc);
	m_phiModule->push_back(trtPhiMod);
	m_layerWheel->push_back(trtLayerWheel);
	m_strawLayer->push_back(trtStrawLayer);
	m_straw->push_back(trtStraw);
	m_highLevel->push_back(trtHL);
	m_timeOverThreshold->push_back(trtToT);
	m_driftTimeBin->push_back(trtDriftTimeBin);
	m_trailEdge->push_back(trtTrailEdge);
	m_firstBin->push_back(trtFirstBin);
	m_lastBin->push_back(trtLastBin);

	h_rdoID->Fill(rdoID_int);
	h_rdoWord->Fill(rdoWord);
	h_barrelEndcap->Fill(trtBrlEc);
	h_phiModule->Fill(trtPhiMod);
	h_layerWheel->Fill(trtLayerWheel);
	h_strawLayer->Fill(trtStrawLayer);
	h_straw->Fill(trtStraw);
	h_ToT->Fill(trtToT);
	if (trtHL) {
	  h_ToT_HL->Fill(trtToT);
	}
	h_driftTimeBin->Fill(trtDriftTimeBin);
	h_trailEdge->Fill(trtTrailEdge);

	if (abs(trtBrlEc) == 1) {
	  h_brlPhiMod->Fill(trtPhiMod);
	  h_brlLayer->Fill(trtLayerWheel);
	  h_brlStrawLayer->Fill(trtStrawLayer);
	  h_brlStraw->Fill(trtStraw);
	  h_brlToT->Fill(trtToT);
	  if (trtHL) {
	    h_brlToT_HL->Fill(trtToT);
	  }
	  h_brlDriftTimeBin->Fill(trtDriftTimeBin);
	  h_brlTrailEdge->Fill(trtTrailEdge);
	}
	else if (abs(trtBrlEc) == 2) {
	  h_ecPhiMod->Fill(trtPhiMod);
	  h_ecWheel->Fill(trtLayerWheel);
	  h_ecStrawLayer->Fill(trtStrawLayer);
	  h_ecStraw->Fill(trtStraw);
	  h_ecToT->Fill(trtToT);
	  if (trtHL) {
	    h_ecToT_HL->Fill(trtToT);
	  }
	  h_ecDriftTimeBin->Fill(trtDriftTimeBin);
	  h_ecTrailEdge->Fill(trtTrailEdge);
	}
      }
    }
  }

  // SimData
  const InDetSimDataCollection* simDataMapTRT(nullptr);
  if (evtStore()->retrieve(simDataMapTRT, "TRT_SDO_Map") == StatusCode::SUCCESS) {
    // loop over SDO container
    InDetSimDataCollection::const_iterator sdo_itr(simDataMapTRT->begin());
    const InDetSimDataCollection::const_iterator sdo_end(simDataMapTRT->end());

    std::vector<int> barcode_vec;
    std::vector<int> eventIndex_vec;
    std::vector<float> charge_vec;
    for ( ; sdo_itr != sdo_end; ++sdo_itr ) {
      const Identifier sdoID((*sdo_itr).first);
      const InDetSimData& sdo((*sdo_itr).second);
      
      const unsigned long long sdoID_int = sdoID.get_compact();
      const int sdoWord(sdo.word());
      const int trtBrlEc_sdo(m_trtID->barrel_ec(sdoID));
      const int trtPhiMod_sdo(m_trtID->phi_module(sdoID));
      const int trtLayerWheel_sdo(m_trtID->layer_or_wheel(sdoID));
      const int trtStrawLayer_sdo(m_trtID->straw_layer(sdoID));
      const int trtStraw_sdo(m_trtID->straw(sdoID));
      const bool aboveThresh(TRT_SimHelper::isAboveTR_Thresh(sdo));
      const bool deadChan(TRT_SimHelper::isDeadChannel(sdo));
      const bool rodData(TRT_SimHelper::isRodData(sdo));
      const bool validStraw(TRT_SimHelper::isValidStraw(sdo));
      const bool hit(TRT_SimHelper::HasHit(sdo));

      m_sdoID->push_back(sdoID_int);
      m_sdoWord->push_back(sdoWord);
      m_barrelEndcap_sdo->push_back(trtBrlEc_sdo);
      m_phiModule_sdo->push_back(trtPhiMod_sdo);
      m_layerWheel_sdo->push_back(trtLayerWheel_sdo);
      m_strawLayer_sdo->push_back(trtStrawLayer_sdo);
      m_straw_sdo->push_back(trtStraw_sdo);
      m_aboveThresh->push_back(aboveThresh);
      m_deadChan->push_back(deadChan);
      m_RODdata->push_back(rodData);
      m_validStraw->push_back(validStraw);
      m_hit->push_back(hit);

      h_sdoID->Fill(sdoID_int);
      h_sdoWord->Fill(sdoWord);
      h_barrelEndcap_sdo->Fill(trtBrlEc_sdo);
      h_phiModule_sdo->Fill(trtPhiMod_sdo);
      h_layerWheel_sdo->Fill(trtLayerWheel_sdo);
      h_strawLayer_sdo->Fill(trtStrawLayer_sdo);
      h_straw_sdo->Fill(trtStraw_sdo);

      // loop over deposits
      // InDetSimData::Deposit typedef for std::pair<HepMcParticleLink, float>
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

StatusCode TRT_RDOAnalysis::finalize() {

  return StatusCode::SUCCESS;
  
}
