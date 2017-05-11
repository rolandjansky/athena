/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MDT_RDOAnalysis.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

MDT_RDOAnalysis::MDT_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_mdtID(0)
  , m_mdtIDhash(0)
  , m_subID(0)
  , m_mrodID(0)
  , m_csmID(0)
  , m_tdcID(0)
  , m_chanID(0)
  , m_coarse(0)
  , m_fine(0)
  , m_width(0)
  , m_leading(0)
  , m_isMasked(0)
  , m_dataWords(0)
  , m_sdoID(0)
  , m_sdoWord(0)
  , m_xPos(0)
  , m_yPos(0)
  , m_zPos(0)
  , m_barcode(0)
  , m_eventIndex(0)
  , m_radius(0)
  , m_localZ(0)
  , m_barcode_vec(0)
  , m_eventIndex_vec(0)
  , m_radius_vec(0)
  , m_localZ_vec(0)

  , h_mdtID(0)
  , h_subID(0)
  , h_mrodID(0)
  , h_csmID(0)
  , h_tdcID(0)
  , h_chanID(0)
  , h_coarse(0)
  , h_fine(0)
  , h_width(0)
  , h_sdoID(0)
  , h_sdoWord(0)
  , h_xPos(0)
  , h_yPos(0)
  , h_zPos(0)
  , h_barcode(0)
  , h_eventIndex(0)
  , h_radius(0)
  , h_localZ(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/MDT_RDOAnalysis/")
    //, m_ntupleDirName("MDT_RDOAnalysis")
  , m_ntupleTreeName("MDT_RDOAna")
  , m_path("/MDT_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode MDT_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing MDT_RDOAnalysis" );

  CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "MDT_RDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  //std::string fullNtupleName = "/" + m_ntupleFileName + "/" + m_ntupleDirName + "/" + m_ntupleTreeName;
  CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    m_tree->Branch("mdtID", &m_mdtID);
    m_tree->Branch("mdtIDhash", &m_mdtIDhash);
    m_tree->Branch("subID", &m_subID);
    m_tree->Branch("mrodID", &m_mrodID);
    m_tree->Branch("csmID", &m_csmID);
    m_tree->Branch("tdcID", &m_tdcID);
    m_tree->Branch("chanID", &m_chanID);
    m_tree->Branch("coarse", &m_coarse);
    m_tree->Branch("fine", &m_fine);
    m_tree->Branch("width", &m_width);
    m_tree->Branch("leading", &m_leading);
    m_tree->Branch("isMasked", &m_isMasked);
    m_tree->Branch("dataWords", &m_dataWords);
    m_tree->Branch("sdoID", &m_sdoID);
    m_tree->Branch("sdoWord", &m_sdoWord);
    m_tree->Branch("xPos", &m_xPos);
    m_tree->Branch("yPos", &m_yPos);
    m_tree->Branch("zPos", &m_zPos);
    m_tree->Branch("barcode", &m_barcode);
    m_tree->Branch("eventIndex", &m_eventIndex);
    m_tree->Branch("radius", &m_radius);
    m_tree->Branch("localZ", &m_localZ);
    m_tree->Branch("barcode_vec", &m_barcode_vec);
    m_tree->Branch("eventIndex_vec", &m_eventIndex_vec);
    m_tree->Branch("radius_vec", &m_radius_vec);
    m_tree->Branch("localZ_vec", &m_localZ_vec);
  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  h_mdtID = new TH1F("h_mdtID", "MDT ID", 100, 0, 1e19);
  h_mdtID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_mdtID->GetName(), h_mdtID));
      
  h_subID = new TH1F("h_subID", "Sub-detector ID", 100, 0, 150);
  h_subID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_subID->GetName(), h_subID));

  h_mrodID = new TH1F("h_mrodID", "MROD ID", 100, 0, 150);
  h_mrodID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_mrodID->GetName(), h_mrodID));

  h_csmID = new TH1F("h_csmID", "CSM ID", 100, 0, 10);
  h_csmID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_csmID->GetName(), h_csmID));
  
  h_tdcID = new TH1F("h_tdcID", "TDC ID", 100, 0, 50);
  h_tdcID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_tdcID->GetName(), h_tdcID));
	      
  h_chanID = new TH1F("h_chanID", "Channel ID", 100, 0, 50);
  h_chanID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_chanID->GetName(), h_chanID));
    
  h_coarse = new TH1F("h_coarse", "Drift time (coarse)", 100, 0, 100);
  h_coarse->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_coarse->GetName(), h_coarse));
  
  h_fine = new TH1F("h_fine", "Drift time (fine)", 100, 0, 50);
  h_fine->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_fine->GetName(), h_fine));
    
  h_width = new TH1F("h_width", "Width", 100, 0, 500);
  h_width->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_width->GetName(), h_width));

  h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 1e19);
  h_sdoID->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_sdoID->GetName(), h_sdoID));

  h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 10);
  h_sdoWord->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_sdoWord->GetName(), h_sdoWord));

  h_xPos = new TH1F("h_xPos", "Global x-position (SDO)", 100, -13000, 13000);
  h_xPos->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_xPos->GetName(), h_xPos));

  h_yPos = new TH1F("h_yPos", "Global y-position (SDO)", 100, -15000, 15000);
  h_yPos->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_yPos->GetName(), h_yPos));

  h_zPos = new TH1F("h_zPos", "Global z-position (SDO)", 100, -22000, 22000);
  h_zPos->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_zPos->GetName(), h_zPos));

  h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e9);
  h_barcode->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_barcode->GetName(), h_barcode));

  h_eventIndex = new TH1F("h_eventIndex", "Event index (SDO)", 100, 0, 1000);
  h_eventIndex->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_eventIndex->GetName(), h_eventIndex));

  h_radius = new TH1F("h_radius", "Radius (SDO)", 100, -15, 15);
  h_radius->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_radius->GetName(), h_radius));

  h_localZ = new TH1F("h_localZ", "Local z-position (SDO)", 100, -3200, 3200);
  h_localZ->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_localZ->GetName(), h_localZ));

  return StatusCode::SUCCESS;
}

StatusCode MDT_RDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In MDT_RDOAnalysis::execute()" );

  m_mdtID->clear();
  m_mdtIDhash->clear();
  m_subID->clear();
  m_mrodID->clear();
  m_csmID->clear();
  m_tdcID->clear();
  m_chanID->clear();
  m_coarse->clear();
  m_fine->clear();
  m_width->clear();
  m_leading->clear();
  m_isMasked->clear();
  m_dataWords->clear();
  m_sdoID->clear();
  m_sdoWord->clear();
  m_xPos->clear();
  m_yPos->clear();
  m_zPos->clear();
  m_barcode->clear();
  m_eventIndex->clear();
  m_radius->clear();
  m_localZ->clear();
  m_barcode_vec->clear();
  m_eventIndex_vec->clear();
  m_radius_vec->clear();
  m_localZ_vec->clear();

  const MdtCsmContainer* p_MDTcont;
  if (evtStore()->retrieve(p_MDTcont, "MDTCSM") == StatusCode::SUCCESS) {
    MdtCsmContainer::const_iterator mdtCont_itr(p_MDTcont->begin());
    const MdtCsmContainer::const_iterator mdtCont_end(p_MDTcont->end());

    for ( ; mdtCont_itr != mdtCont_end; ++mdtCont_itr ) {
      const Identifier mdtID((*mdtCont_itr)->identify());
      const IdentifierHash mdtIDhash((*mdtCont_itr)->identifyHash());
      const uint16_t subID((*mdtCont_itr)->SubDetId());
      const uint16_t mrodID((*mdtCont_itr)->MrodId());
      const uint16_t csmID((*mdtCont_itr)->CsmId());
      
      unsigned long long mdtID_int = mdtID.get_compact();
      unsigned int mdtIDhash_int = mdtIDhash.value();
      m_mdtID->push_back(mdtID_int);
      m_mdtIDhash->push_back(mdtIDhash_int);
      m_subID->push_back(subID);
      m_mrodID->push_back(mrodID);
      m_csmID->push_back(csmID);

      h_mdtID->Fill(mdtID_int);
      h_subID->Fill(subID);
      h_mrodID->Fill(mrodID);
      h_csmID->Fill(csmID);
      
      const MdtCsm* p_mdtAmt(*mdtCont_itr);
      MdtCsm::const_iterator mdt_itr(p_mdtAmt->begin());
      const MdtCsm::const_iterator mdt_end(p_mdtAmt->end());
      for ( ; mdt_itr != mdt_end; ++mdt_itr ) {
	
	const uint16_t tdcID((*mdt_itr)->tdcId());
	const uint16_t chanID((*mdt_itr)->channelId());
	const uint16_t coarseTime((*mdt_itr)->coarse());
	const uint16_t fineTime((*mdt_itr)->fine());
	const uint16_t widthComb((*mdt_itr)->width());
	const bool lead((*mdt_itr)->leading());
	const bool masked((*mdt_itr)->isMasked());
	const std::vector<uint32_t>* dataWords_vec = (*mdt_itr)->dataWords();
		
	m_tdcID->push_back(tdcID);
	m_chanID->push_back(chanID);
	m_coarse->push_back(coarseTime);
	m_fine->push_back(fineTime);
	m_width->push_back(widthComb);
	m_leading->push_back(lead);
	m_isMasked->push_back(masked);	
	for (std::vector<uint32_t>::size_type i = 0; i != dataWords_vec->size(); ++i) {
	  m_dataWords->push_back(dataWords_vec->at(i));
	}

	h_tdcID->Fill(tdcID);
	h_chanID->Fill(chanID);
	h_coarse->Fill(coarseTime);
	h_fine->Fill(fineTime);
	h_width->Fill(widthComb);
	
      }
    }
  }

  // SimData
  const MuonSimDataCollection* simDataMapMDT(NULL);
  if (evtStore()->retrieve(simDataMapMDT, "MDT_SDO") == StatusCode::SUCCESS) {
    MuonSimDataCollection::const_iterator sdo_itr(simDataMapMDT->begin());
    const MuonSimDataCollection::const_iterator sdo_end(simDataMapMDT->end());

    std::vector<int> barcode_vec;
    std::vector<int> eventIndex_vec;
    std::vector<double> radius_vec;
    std::vector<float> localZ_vec;
    for ( ; sdo_itr != sdo_end; ++sdo_itr ) {
      const Identifier sdoID((*sdo_itr).first);
      const MuonSimData& sdo((*sdo_itr).second);
      const unsigned long long sdoID_int = sdoID.get_compact();
      const int sdoWord(sdo.word());
      const Amg::Vector3D gPos(sdo.globalPosition());
      const float xPos(gPos.x());
      const float yPos(gPos.y());
      const float zPos(gPos.z());
      
      m_sdoID->push_back(sdoID_int);
      m_sdoWord->push_back(sdoWord);
      m_xPos->push_back(xPos);
      m_yPos->push_back(yPos);
      m_zPos->push_back(zPos);

      h_sdoID->Fill(sdoID_int);
      h_sdoWord->Fill(sdoWord);
      h_xPos->Fill(xPos);
      h_yPos->Fill(yPos);
      h_zPos->Fill(zPos);

      // loop over deposits
      const std::vector<MuonSimData::Deposit>& deposits = sdo.getdeposits();
      std::vector<MuonSimData::Deposit>::const_iterator dep_itr(deposits.begin());
      const std::vector<MuonSimData::Deposit>::const_iterator dep_end(deposits.end());
      for ( ; dep_itr != dep_end; ++dep_itr ) {	
	const HepMcParticleLink& particleLink = (*dep_itr).first;
	const int bar(particleLink.barcode());
	const int eventIx(particleLink.eventIndex());
	const MuonMCData& data = (*dep_itr).second;
	const double radius(data.firstEntry());
	const float localZ(data.secondEntry());
	
	m_barcode->push_back(bar);
	m_eventIndex->push_back(eventIx);
	m_radius->push_back(radius); // radius?
	m_localZ->push_back(localZ); // local z-pos?

	h_barcode->Fill(bar);
	h_eventIndex->Fill(eventIx);
	h_radius->Fill(radius);
	h_localZ->Fill(localZ);
	
	barcode_vec.push_back(bar);
	eventIndex_vec.push_back(eventIx);
	radius_vec.push_back(radius);
	localZ_vec.push_back(localZ);
      }
      m_barcode_vec->push_back(barcode_vec);
      m_eventIndex_vec->push_back(eventIndex_vec);
      m_radius_vec->push_back(radius_vec);
      m_localZ_vec->push_back(localZ_vec);
      barcode_vec.clear();
      eventIndex_vec.clear();
      radius_vec.clear();
      localZ_vec.clear();
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode MDT_RDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
