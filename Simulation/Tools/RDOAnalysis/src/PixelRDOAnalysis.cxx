/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelRDOAnalysis.h"
#include "StoreGate/ReadHandle.h"
#include "GeneratorObjects/McEventCollection.h"


#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

PixelRDOAnalysis::PixelRDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputKey("PixelRDOs")
  , m_inputTruthKey("PixelSDO_Map")
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
  , m_h_rdoID(0)
  , m_h_rdoWord(0)
  , m_h_barrelEndcap(0)
  , m_h_layerDisk(0)
  , m_h_phiModule(0)
  , m_h_etaModule(0)
  , m_h_phiIndex(0)
  , m_h_etaIndex(0)
  , m_h_ToT(0)
  , m_h_BCID(0)
  , m_h_LVL1A(0)
  , m_h_LVL1ID(0)
  , m_h_brlLayer(0)
  , m_h_brlPhiMod(0)
  , m_h_brlEtaMod(0)
  , m_h_brlPhiIndex(0)
  , m_h_brlEtaIndex(0)
  , m_h_brlToT(0)
  , m_h_brlBCID(0)
  , m_h_brlLVL1A(0)
  , m_h_brlLVL1ID(0)
  , m_h_ecDisk(0)
  , m_h_ecPhiMod(0)
  , m_h_ecEtaMod(0)
  , m_h_ecPhiIndex(0)
  , m_h_ecEtaIndex(0)
  , m_h_ecToT(0)
  , m_h_ecBCID(0)
  , m_h_ecLVL1A(0)
  , m_h_ecLVL1ID(0)
  , m_h_TruthMatchedRDOs(nullptr)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/PixelRDOAnalysis/")
  , m_ntupleTreeName("PixelRDOAna")
  , m_path("/PixelRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputKey", m_inputKey);
  declareProperty("InputTruthKey", m_inputTruthKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode PixelRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing PixelRDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputKey.initialize() );
  ATH_CHECK( m_inputTruthKey.initialize() );

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
  m_h_rdoID = new TH1F("h_rdoID", "rdoID", 100, 0, 5e17);
  m_h_rdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rdoID->GetName(), m_h_rdoID));

  m_h_rdoWord = new TH1F("h_rdoWord", "rdoWord", 100, 0, 350);
  m_h_rdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rdoWord->GetName(), m_h_rdoWord));

  m_h_barrelEndcap = new TH1F("h_barrelEndcap", "Barrel or Endcap", 100, -5, 5);
  m_h_barrelEndcap->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_barrelEndcap->GetName(), m_h_barrelEndcap));

  m_h_layerDisk = new TH1F("h_layerDisk", "Barrel layer or Endcap disk", 100, 0, 3);
  m_h_layerDisk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_layerDisk->GetName(), m_h_layerDisk));

  m_h_phiModule = new TH1F("h_phiModule", "Phi module", 100, 0, 75);
  m_h_phiModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_phiModule->GetName(), m_h_phiModule));

  m_h_etaModule = new TH1F("h_etaModule", "Eta module", 100, 0, 15);
  m_h_etaModule->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_etaModule->GetName(), m_h_etaModule));

  m_h_phiIndex = new TH1F("h_phiIndex", "Phi index", 100, 0, 350);
  m_h_phiIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_phiIndex->GetName(), m_h_phiIndex));

  m_h_etaIndex = new TH1F("h_etaIndex", "Eta index", 100, 0, 225);
  m_h_etaIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_etaIndex->GetName(), m_h_etaIndex));

  m_h_ToT = new TH1F("h_ToT", "ToT", 100, 0, 250);
  m_h_ToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ToT->GetName(), m_h_ToT));

  m_h_BCID = new TH1F("h_BCID", "BCID", 100, -1.5, 1.5);
  m_h_BCID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_BCID->GetName(), m_h_BCID));

  m_h_LVL1A = new TH1F("h_LVL1A", "LVL1A", 100, -1.5, 1.5);
  m_h_LVL1A->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_LVL1A->GetName(), m_h_LVL1A));

  m_h_LVL1ID = new TH1F("h_LVL1ID", "LVL1ID", 100, -1.5, 1.5);
  m_h_LVL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_LVL1ID->GetName(), m_h_LVL1ID));

  m_h_brlLayer = new TH1F("h_brlLayer", "Barrel layer", 100, 0, 3);
  m_h_brlLayer->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlLayer->GetName(), m_h_brlLayer));

  m_h_brlPhiMod = new TH1F("h_brlPhiMod", "Barrel phi module", 100, 0, 80);
  m_h_brlPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlPhiMod->GetName(), m_h_brlPhiMod));

  m_h_brlEtaMod = new TH1F("h_brlEtaMod", "Barrel eta module", 100, 0, 15);
  m_h_brlEtaMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlEtaMod->GetName(), m_h_brlEtaMod));

  m_h_brlPhiIndex = new TH1F("h_brlPhiIndex", "Barrel phi index", 100, 0, 350);
  m_h_brlPhiIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlPhiIndex->GetName(), m_h_brlPhiIndex));

  m_h_brlEtaIndex = new TH1F("h_brlEtaIndex", "Barrel eta index", 100, 0, 225);
  m_h_brlEtaIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlEtaIndex->GetName(), m_h_brlEtaIndex));

  m_h_brlToT = new TH1F("h_brlToT", "Barrel ToT", 100, 0, 250);
  m_h_brlToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlToT->GetName(), m_h_brlToT));

  m_h_brlBCID = new TH1F("h_brlBCID", "Barrel BCID", 100, -1.5, 1.5);
  m_h_brlBCID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlBCID->GetName(), m_h_brlBCID));

  m_h_brlLVL1A = new TH1F("h_brlLVL1A", "Barrel LVL1A", 100, -1.5, 1.5);
  m_h_brlLVL1A->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlLVL1A->GetName(), m_h_brlLVL1A));

  m_h_brlLVL1ID = new TH1F("h_brlLVL1ID", "Barrel LVL1ID", 100, -1.5, 1.5);
  m_h_brlLVL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_brlLVL1ID->GetName(), m_h_brlLVL1ID));

  m_h_ecDisk = new TH1F("h_ecDisk", "Endcap disk", 100, 0, 3);
  m_h_ecDisk->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecDisk->GetName(), m_h_ecDisk));

  m_h_ecPhiMod = new TH1F("h_ecPhiMod", "Endcap phi module", 100, 0, 80);
  m_h_ecPhiMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecPhiMod->GetName(), m_h_ecPhiMod));

  m_h_ecEtaMod = new TH1F("h_ecEtaMod", "Endcap eta module", 100, 0, 15);
  m_h_ecEtaMod->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecEtaMod->GetName(), m_h_ecEtaMod));

  m_h_ecPhiIndex = new TH1F("h_ecPhiIndex", "Endcap phi index", 100, 0, 350);
  m_h_ecPhiIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecPhiIndex->GetName(), m_h_ecPhiIndex));

  m_h_ecEtaIndex = new TH1F("h_ecEtaIndex", "Endcap eta index", 100, 0, 225);
  m_h_ecEtaIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecEtaIndex->GetName(), m_h_ecEtaIndex));

  m_h_ecToT = new TH1F("h_ecToT", "EndcapToT", 100, 0, 250);
  m_h_ecToT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecToT->GetName(), m_h_ecToT));

  m_h_ecBCID = new TH1F("h_ecBCID", "Endcap BCID", 100, -1.5, 1.5);
  m_h_ecBCID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecBCID->GetName(), m_h_ecBCID));

  m_h_ecLVL1A = new TH1F("h_ecLVL1A", "Endcap LVL1A", 100, -1.5, 1.5);
  m_h_ecLVL1A->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecLVL1A->GetName(), m_h_ecLVL1A));

  m_h_ecLVL1ID = new TH1F("h_ecLVL1ID", "Endcap LVL1ID", 100, -1.5, 1.5);
  m_h_ecLVL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_ecLVL1ID->GetName(), m_h_ecLVL1ID));

  m_h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 5e17);
  m_h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoID->GetName(), m_h_sdoID));

  m_h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 350);
  m_h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoWord->GetName(), m_h_sdoWord));

  m_h_barrelEndcap_sdo = new TH1F("h_barrelEndcap_sdo", "Barrel or Endcap (SDO)", 100, -5, 5);
  m_h_barrelEndcap_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_barrelEndcap_sdo->GetName(), m_h_barrelEndcap_sdo));

  m_h_layerDisk_sdo = new TH1F("h_layerDisk_sdo", "Barrel layer or Endcap disk (SDO)", 100, 0, 3);
  m_h_layerDisk_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_layerDisk_sdo->GetName(), m_h_layerDisk_sdo));

  m_h_phiModule_sdo = new TH1F("h_phiModule_sdo", "Phi module (SDO)", 100, 0, 75);
  m_h_phiModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_phiModule_sdo->GetName(), m_h_phiModule_sdo));

  m_h_etaModule_sdo = new TH1F("h_etaModule_sdo", "Eta module (SDO)", 100, 0, 15);
  m_h_etaModule_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_etaModule_sdo->GetName(), m_h_etaModule_sdo));

  m_h_phiIndex_sdo = new TH1F("h_phiIndex_sdo", "Phi index (SDO)", 100, 0, 350);
  m_h_phiIndex_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_phiIndex_sdo->GetName(), m_h_phiIndex_sdo));

  m_h_etaIndex_sdo = new TH1F("h_etaIndex_sdo", "Eta index (SDO)", 100, 0, 350);
  m_h_etaIndex_sdo->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_etaIndex_sdo->GetName(), m_h_etaIndex_sdo));

  m_h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e5);
  m_h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_barcode->GetName(), m_h_barcode));

  m_h_eventIndex = new TH1F("h_eventIndex", "Event Index (SDO)", 100, 0, 2);
  m_h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_eventIndex->GetName(), m_h_eventIndex));

  m_h_charge = new TH1F("h_charge", "Charge (SDO)", 100, 0, 1e7);
  m_h_charge->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_charge->GetName(), m_h_charge));

  m_h_TruthMatchedRDOs = new TH1F("h_TruthMatchedPixelRDOs", "h_TruthMatchedPixelRDOs", 4, 1, 5);
  TString truthMatchBinLables[4] = { "All RDOs", "Truth Matched", "HS Matched", "Unmatched" };
  for(unsigned int ibin = 1; ibin < 5; ibin++) {
    m_h_TruthMatchedRDOs->GetXaxis()->SetBinLabel(ibin, truthMatchBinLables[ibin-1]);
  }
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_TruthMatchedRDOs->GetName(), m_h_TruthMatchedRDOs));


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
  SG::ReadHandle<PixelRDO_Container> p_pixelRDO_cont (m_inputKey);
  //Adding SimMap and McEvent here for added truthMatching checks
  SG::ReadHandle<InDetSimDataCollection> simDataMapPixel (m_inputTruthKey);
  SG::ReadHandle<McEventCollection> mcEventCollection("TruthEvent");
  bool doTruthMatching = true;
  const HepMC::GenEvent* hardScatterEvent(nullptr);

  if (mcEventCollection->size()==0){
    ATH_MSG_WARNING("Failed to retrieve a nonzero sized truth event collection, disabling truthMatching");
    doTruthMatching = false;
  }
  if(doTruthMatching) hardScatterEvent = mcEventCollection->at(0);

  if(p_pixelRDO_cont.isValid()) {
    // loop over RDO container
    PixelRDO_Container::const_iterator rdoCont_itr(p_pixelRDO_cont->begin());
    const PixelRDO_Container::const_iterator rdoCont_end(p_pixelRDO_cont->end());
    for ( ; rdoCont_itr != rdoCont_end; ++rdoCont_itr ) {
      const PixelRDO_Collection* p_pixelRDO_coll(*rdoCont_itr);
      PixelRDO_Collection::const_iterator rdo_itr(p_pixelRDO_coll->begin());
      const PixelRDO_Collection::const_iterator rdo_end(p_pixelRDO_coll->end());
      for ( ; rdo_itr != rdo_end; ++rdo_itr ) {
        if(doTruthMatching){
          m_h_TruthMatchedRDOs->Fill(1.5);
          bool findMatch = false; 
          if(simDataMapPixel.isValid()){
            InDetSimDataCollection::const_iterator iter = (*simDataMapPixel).find((*rdo_itr)->identify());
        
            if ( iter != (*simDataMapPixel).end() ) {
              const InDetSimData& sdo = iter->second;
              const std::vector< InDetSimData::Deposit >& deposits = sdo.getdeposits();
              std::vector< InDetSimData::Deposit >::const_iterator nextdeposit = deposits.begin();
              std::vector< InDetSimData::Deposit >::const_iterator lastdeposit = deposits.end();
              for( ; nextdeposit!=lastdeposit; ++nextdeposit) {
	              const HepMcParticleLink& particleLink = nextdeposit->first;
                if(particleLink.isValid() && !findMatch){
                  const HepMC::GenParticle *genPart(particleLink.cptr());
                  if(genPart->parent_event() == hardScatterEvent) m_h_TruthMatchedRDOs->Fill(3.5);
                  m_h_TruthMatchedRDOs->Fill(2.5);
                  findMatch = true;
                }
              }
            }
          }
          if(!findMatch) m_h_TruthMatchedRDOs->Fill(4.5);
        }
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

        m_h_rdoID->Fill(rdoID_int);
        m_h_rdoWord->Fill(rdoWord);
        m_h_barrelEndcap->Fill(pixBrlEc);
        m_h_layerDisk->Fill(pixLayerDisk);
        m_h_phiModule->Fill(pixPhiMod);
        m_h_etaModule->Fill(pixEtaMod);
        m_h_phiIndex->Fill(pixPhiIx);
        m_h_etaIndex->Fill(pixEtaIx);
        m_h_ToT->Fill(pixToT);
        m_h_BCID->Fill(pixBCID);
        m_h_LVL1A->Fill(pixLVL1A);
        m_h_LVL1ID->Fill(pixLVL1ID);

        if (pixBrlEc == 0) {
          m_h_brlLayer->Fill(pixLayerDisk);
          m_h_brlPhiMod->Fill(pixPhiMod);
          m_h_brlEtaMod->Fill(pixEtaMod);
          m_h_brlPhiIndex->Fill(pixPhiIx);
          m_h_brlEtaIndex->Fill(pixEtaIx);
          m_h_brlToT->Fill(pixToT);
          m_h_brlBCID->Fill(pixBCID);
          m_h_brlLVL1A->Fill(pixLVL1A);
          m_h_brlLVL1ID->Fill(pixLVL1ID);
        }
        else if (abs(pixBrlEc) == 4) {
          m_h_ecDisk->Fill(pixLayerDisk);
          m_h_ecPhiMod->Fill(pixPhiMod);
          m_h_ecEtaMod->Fill(pixEtaMod);
          m_h_ecPhiIndex->Fill(pixPhiIx);
          m_h_ecEtaIndex->Fill(pixEtaIx);
          m_h_ecToT->Fill(pixToT);
          m_h_ecBCID->Fill(pixBCID);
          m_h_ecLVL1A->Fill(pixLVL1A);
          m_h_ecLVL1ID->Fill(pixLVL1ID);
        }
      }
    }
  }

  // Sim Data
  if(simDataMapPixel.isValid()) {
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

      m_h_sdoID->Fill(sdoID_int);
      m_h_sdoWord->Fill(sdoWord);
      m_h_barrelEndcap_sdo->Fill(pixBrlEc_sdo);
      m_h_layerDisk_sdo->Fill(pixLayerDisk_sdo);
      m_h_phiModule_sdo->Fill(pixPhiMod_sdo);
      m_h_etaModule_sdo->Fill(pixEtaMod_sdo);
      m_h_phiIndex_sdo->Fill(pixPhiIx_sdo);
      m_h_etaIndex_sdo->Fill(pixEtaIx_sdo);

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

        m_h_barcode->Fill(bar);
        m_h_eventIndex->Fill(eventIx);
        m_h_charge->Fill(charge);

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
