/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MDT_RDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

MDT_RDOAnalysis::MDT_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputKey("MDTCSM")
  , m_inputTruthKey("MDT_SDO")
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

  , m_h_mdtID(0)
  , m_h_subID(0)
  , m_h_mrodID(0)
  , m_h_csmID(0)
  , m_h_tdcID(0)
  , m_h_chanID(0)
  , m_h_coarse(0)
  , m_h_fine(0)
  , m_h_width(0)
  , m_h_sdoID(0)
  , m_h_sdoWord(0)
  , m_h_xPos(0)
  , m_h_yPos(0)
  , m_h_zPos(0)
  , m_h_barcode(0)
  , m_h_eventIndex(0)
  , m_h_radius(0)
  , m_h_localZ(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/MDT_RDOAnalysis/")
  //, m_ntupleDirName("MDT_RDOAnalysis")
  , m_ntupleTreeName("MDT_RDOAna")
  , m_path("/MDT_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputKey", m_inputKey);
  declareProperty("InputTruthKey", m_inputTruthKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode MDT_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing MDT_RDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputKey.initialize() );
  ATH_CHECK( m_inputTruthKey.initialize() );

  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "MDT_RDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  //std::string fullNtupleName = "/" + m_ntupleFileName + "/" + m_ntupleDirName + "/" + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
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

  m_h_mdtID = new TH1F("h_mdtID", "MDT ID", 100, 0, 1e19);
  m_h_mdtID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_mdtID->GetName(), m_h_mdtID));

  m_h_subID = new TH1F("h_subID", "Sub-detector ID", 100, 0, 150);
  m_h_subID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_subID->GetName(), m_h_subID));

  m_h_mrodID = new TH1F("h_mrodID", "MROD ID", 100, 0, 150);
  m_h_mrodID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_mrodID->GetName(), m_h_mrodID));

  m_h_csmID = new TH1F("h_csmID", "CSM ID", 100, 0, 10);
  m_h_csmID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_csmID->GetName(), m_h_csmID));

  m_h_tdcID = new TH1F("h_tdcID", "TDC ID", 100, 0, 50);
  m_h_tdcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tdcID->GetName(), m_h_tdcID));

  m_h_chanID = new TH1F("h_chanID", "Channel ID", 100, 0, 50);
  m_h_chanID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_chanID->GetName(), m_h_chanID));

  m_h_coarse = new TH1F("h_coarse", "Drift time (coarse)", 100, 0, 100);
  m_h_coarse->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_coarse->GetName(), m_h_coarse));

  m_h_fine = new TH1F("h_fine", "Drift time (fine)", 100, 0, 50);
  m_h_fine->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_fine->GetName(), m_h_fine));

  m_h_width = new TH1F("h_width", "Width", 100, 0, 500);
  m_h_width->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_width->GetName(), m_h_width));

  m_h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 1e19);
  m_h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoID->GetName(), m_h_sdoID));

  m_h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 10);
  m_h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoWord->GetName(), m_h_sdoWord));

  m_h_xPos = new TH1F("h_xPos", "Global x-position (SDO)", 100, -13000, 13000);
  m_h_xPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_xPos->GetName(), m_h_xPos));

  m_h_yPos = new TH1F("h_yPos", "Global y-position (SDO)", 100, -15000, 15000);
  m_h_yPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_yPos->GetName(), m_h_yPos));

  m_h_zPos = new TH1F("h_zPos", "Global z-position (SDO)", 100, -22000, 22000);
  m_h_zPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_zPos->GetName(), m_h_zPos));

  m_h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e9);
  m_h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_barcode->GetName(), m_h_barcode));

  m_h_eventIndex = new TH1F("h_eventIndex", "Event index (SDO)", 100, 0, 1000);
  m_h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_eventIndex->GetName(), m_h_eventIndex));

  m_h_radius = new TH1F("h_radius", "Radius (SDO)", 100, -15, 15);
  m_h_radius->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_radius->GetName(), m_h_radius));

  m_h_localZ = new TH1F("h_localZ", "Local z-position (SDO)", 100, -3200, 3200);
  m_h_localZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_localZ->GetName(), m_h_localZ));

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

  SG::ReadHandle<MdtCsmContainer> p_MDTcont (m_inputKey);
  if(p_MDTcont.isValid()) {
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

      m_h_mdtID->Fill(mdtID_int);
      m_h_subID->Fill(subID);
      m_h_mrodID->Fill(mrodID);
      m_h_csmID->Fill(csmID);

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

        m_h_tdcID->Fill(tdcID);
        m_h_chanID->Fill(chanID);
        m_h_coarse->Fill(coarseTime);
        m_h_fine->Fill(fineTime);
        m_h_width->Fill(widthComb);

      }
    }
  }

  // SimData
  SG::ReadHandle<MuonSimDataCollection> simDataMapMDT (m_inputTruthKey);
  if(simDataMapMDT.isValid()) {
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

      m_h_sdoID->Fill(sdoID_int);
      m_h_sdoWord->Fill(sdoWord);
      m_h_xPos->Fill(xPos);
      m_h_yPos->Fill(yPos);
      m_h_zPos->Fill(zPos);

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

        m_h_barcode->Fill(bar);
        m_h_eventIndex->Fill(eventIx);
        m_h_radius->Fill(radius);
        m_h_localZ->Fill(localZ);

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
