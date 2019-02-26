/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TGC_RDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

TGC_RDOAnalysis::TGC_RDOAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputKey("TGCRDO")
  , m_inputTruthKey("TGC_SDO")
  , m_tgcID(0)
  , m_tgcSubDetID(0)
  , m_tgcRodID(0)
  , m_tgcTrigType(0)
  , m_tgcBcID(0)
  , m_tgcL1ID(0)
  , m_tgcOrbit(0)
  , m_tgcVersion(0)
  , m_bcTag(0)
  , m_subDetID(0)
  , m_rodID(0)
  , m_sswID(0)
  , m_slbID(0)
  , m_bcID(0)
  , m_l1ID(0)
  , m_type(0)
  , m_slbType(0)
  , m_coinc(0)
  , m_bitPos(0)
  , m_track(0)
  , m_adj(0)
  , m_forward(0)
  , m_index(0)
  , m_pos(0)
  , m_delta(0)
  , m_segment(0)
  , m_subMat(0)
  , m_sector(0)
  , m_chip(0)
  , m_hipt(0)
  , m_hitID(0)
  , m_hsub(0)
  , m_strip(0)
  , m_inner(0)
  , m_cand3plus(0)
  , m_muplus(0)
  , m_thresh(0)
  , m_overlap(0)
  , m_veto(0)
  , m_roi(0)

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

  , m_h_tgcID(0)
  , m_h_tgcSubDetID(0)
  , m_h_tgcRodID(0)
  , m_h_tgcTrigType(0)
  , m_h_tgcBcID(0)
  , m_h_tgcL1ID(0)
  , m_h_bcTag(0)
  , m_h_subDetID(0)
  , m_h_rodID(0)
  , m_h_sswID(0)
  , m_h_slbID(0)
  , m_h_bcID(0)
  , m_h_l1ID(0)
  , m_h_type(0)
  , m_h_slbType(0)
  , m_h_bitPos(0)
  , m_h_track(0)
  , m_h_adj(0)
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
  , m_ntupleDirName("/TGC_RDOAnalysis/")
  , m_ntupleTreeName("TGC_RDOAna")
  , m_path("/TGC_RDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputKey", m_inputKey);
  declareProperty("InputTruthKey", m_inputTruthKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode TGC_RDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TGC_RDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputKey.initialize() );
  ATH_CHECK( m_inputTruthKey.initialize() );

  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "TGC_RDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    m_tree->Branch("tgcID", &m_tgcID);
    m_tree->Branch("tgcSubDetID", &m_tgcSubDetID);
    m_tree->Branch("tgcRodID", &m_tgcRodID);
    m_tree->Branch("tgcTrigType", &m_tgcTrigType);
    m_tree->Branch("tgcBcID", &m_tgcBcID);
    m_tree->Branch("tgcL1ID", &m_tgcL1ID);
    m_tree->Branch("tgcOrbit", &m_tgcOrbit);
    m_tree->Branch("tgcVersion", &m_tgcVersion);
    m_tree->Branch("bcTag", &m_bcTag);
    m_tree->Branch("subDetID", &m_subDetID);
    m_tree->Branch("rodID", &m_rodID);
    m_tree->Branch("sswID", &m_sswID);
    m_tree->Branch("slbID", &m_slbID);
    m_tree->Branch("bcID", &m_bcID);
    m_tree->Branch("l1ID", &m_l1ID);
    m_tree->Branch("type", &m_type);
    m_tree->Branch("slbType", &m_slbType);
    m_tree->Branch("coinc", &m_coinc);
    m_tree->Branch("bitPos", &m_bitPos);
    m_tree->Branch("track", &m_track);
    m_tree->Branch("adj", &m_adj);
    m_tree->Branch("forward", &m_forward);
    m_tree->Branch("index", &m_index);
    m_tree->Branch("pos", &m_pos);
    m_tree->Branch("delta", &m_delta);
    m_tree->Branch("segment", &m_segment);
    m_tree->Branch("subMat", &m_subMat);
    m_tree->Branch("sector", &m_sector);
    m_tree->Branch("chip", &m_chip);
    m_tree->Branch("hipt", &m_hipt);
    m_tree->Branch("hitID", &m_hitID);
    m_tree->Branch("hsub", &m_hsub);
    m_tree->Branch("strip", &m_strip);
    m_tree->Branch("inner", &m_inner);
    m_tree->Branch("cand3plus", &m_cand3plus);
    m_tree->Branch("muplus", &m_muplus);
    m_tree->Branch("thresh", &m_thresh);
    m_tree->Branch("overlap", &m_overlap);
    m_tree->Branch("veto", &m_veto);
    m_tree->Branch("roi", &m_roi);

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

  // HISTOGRAMS
  m_h_tgcID = new TH1F("h_tgcID", "TGC ID", 100, 0, 25);
  m_h_tgcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tgcID->GetName(), m_h_tgcID));

  m_h_tgcSubDetID = new TH1F("h_tgcSubDetID", "TGC sub-detector ID", 100, 0, 110);
  m_h_tgcSubDetID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tgcSubDetID->GetName(), m_h_tgcSubDetID));

  m_h_tgcRodID = new TH1F("h_tgcRodID", "TGC ROD ID", 100, 0, 15);
  m_h_tgcRodID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tgcRodID->GetName(), m_h_tgcRodID));

  m_h_tgcTrigType = new TH1F("h_tgcTrigType", "TGC trigger type", 100, 0, 2);
  m_h_tgcTrigType->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tgcTrigType->GetName(), m_h_tgcTrigType));

  m_h_tgcBcID = new TH1F("h_tgcBcID", "TGC BCID", 100, 0, 2);
  m_h_tgcBcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tgcBcID->GetName(), m_h_tgcBcID));

  m_h_tgcL1ID = new TH1F("h_tgcL1ID", "TGC L1ID", 100, 0, 2);
  m_h_tgcL1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_tgcL1ID->GetName(), m_h_tgcL1ID));

  m_h_bcTag = new TH1F("h_bcTag", "BC Tag", 100, 0, 5);
  m_h_bcTag->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_bcTag->GetName(), m_h_bcTag));

  m_h_subDetID = new TH1F("h_subDetID", "Sub-detector ID", 100, 0, 110);
  m_h_subDetID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_subDetID->GetName(), m_h_subDetID));

  m_h_rodID = new TH1F("h_rodID", "ROD ID", 100, 0, 15);
  m_h_rodID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rodID->GetName(), m_h_rodID));

  m_h_sswID = new TH1F("h_sswID", "SSW ID", 100, 0, 10);
  m_h_sswID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sswID->GetName(), m_h_sswID));

  m_h_slbID = new TH1F("h_slbID", "SLB ID", 100, 0, 30);
  m_h_slbID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_slbID->GetName(), m_h_slbID));

  m_h_bcID = new TH1F("h_bcID", "BCID", 100, 0, 10);
  m_h_bcID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_bcID->GetName(), m_h_bcID));

  m_h_l1ID = new TH1F("h_l1ID", "L1ID", 100, 0, 10);
  m_h_l1ID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_l1ID->GetName(), m_h_l1ID));

  m_h_type = new TH1F("h_type", "type (hit or coinc)", 100, 0, 10);
  m_h_type->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_type->GetName(), m_h_type));

  m_h_slbType = new TH1F("h_slbType", "SLB type", 100, 0, 10);
  m_h_slbType->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_slbType->GetName(), m_h_slbType));

  m_h_bitPos = new TH1F("h_bitPos", "Bitmap position", 100, 0, 220);
  m_h_bitPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_bitPos->GetName(), m_h_bitPos));

  m_h_track = new TH1F("h_track", "tracklet", 100, 0, 10);
  m_h_track->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_track->GetName(), m_h_track));

  m_h_adj = new TH1F("h_adj", "Adjacent", 100, 0, 2);
  m_h_adj->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_adj->GetName(), m_h_adj));

  m_h_sdoID = new TH1F("h_sdoID", "sdoID", 100, 0, 1e19);
  m_h_sdoID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoID->GetName(), m_h_sdoID));

  m_h_sdoWord = new TH1F("h_sdoWord", "sdoWord", 100, 0, 10);
  m_h_sdoWord->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_sdoWord->GetName(), m_h_sdoWord));

  m_h_xPos = new TH1F("h_xPos", "Global x-position (SDO)", 100, -15000, 15000);
  m_h_xPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_xPos->GetName(), m_h_xPos));

  m_h_yPos = new TH1F("h_yPos", "Global y-position (SDO)", 100, -15000, 15000);
  m_h_yPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_yPos->GetName(), m_h_yPos));

  m_h_zPos = new TH1F("h_zPos", "Global z-position (SDO)", 100, -15000, 15000);
  m_h_zPos->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_zPos->GetName(), m_h_zPos));

  m_h_barcode = new TH1F("h_barcode", "Barcode (SDO)", 100, 0, 2.2e9);
  m_h_barcode->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_barcode->GetName(), m_h_barcode));

  m_h_eventIndex = new TH1F("h_eventIndex", "Event index (SDO)", 100, 0, 1000);
  m_h_eventIndex->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_eventIndex->GetName(), m_h_eventIndex));

  m_h_radius = new TH1F("h_radius", "Radius (SDO)", 100, 0, 1);
  m_h_radius->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_radius->GetName(), m_h_radius));

  m_h_localZ = new TH1F("h_localZ", "Local z-position (SDO)", 100, -250, 250);
  m_h_localZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_localZ->GetName(), m_h_localZ));

  return StatusCode::SUCCESS;
}

StatusCode TGC_RDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In TGC_RDOAnalysis::execute()" );

  m_tgcID->clear();
  m_tgcSubDetID->clear();
  m_tgcRodID->clear();
  m_tgcTrigType->clear();
  m_tgcBcID->clear();
  m_tgcL1ID->clear();
  m_tgcOrbit->clear();
  m_tgcVersion->clear();
  m_bcTag->clear();
  m_subDetID->clear();
  m_rodID->clear();
  m_sswID->clear();
  m_slbID->clear();
  m_bcID->clear();
  m_l1ID->clear();
  m_type->clear();
  m_slbType->clear();
  m_coinc->clear();
  m_bitPos->clear();
  m_track->clear();
  m_adj->clear();
  m_forward->clear();
  m_index->clear();
  m_pos->clear();
  m_delta->clear();
  m_segment->clear();
  m_subMat->clear();
  m_sector->clear();
  m_chip->clear();
  m_hipt->clear();
  m_hitID->clear();
  m_hsub->clear();
  m_strip->clear();
  m_inner->clear();
  m_cand3plus->clear();
  m_muplus->clear();
  m_thresh->clear();
  m_overlap->clear();
  m_veto->clear();
  m_roi->clear();

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

  SG::ReadHandle<TgcRdoContainer> p_TGCcont (m_inputKey);
  if(p_TGCcont.isValid()) {
    TgcRdoContainer::const_iterator tgcCont_itr(p_TGCcont->begin());
    const TgcRdoContainer::const_iterator tgcCont_end(p_TGCcont->end());

    for ( ; tgcCont_itr != tgcCont_end; ++tgcCont_itr ) {
      const uint16_t tgcID((*tgcCont_itr)->identify());
      const uint16_t tgcSubDetID((*tgcCont_itr)->subDetectorId());
      const uint16_t tgcRodID((*tgcCont_itr)->rodId());
      const uint16_t tgcTrigType((*tgcCont_itr)->triggerType());
      const uint16_t tgcBcID((*tgcCont_itr)->bcId());
      const uint16_t tgcL1ID((*tgcCont_itr)->l1Id());
      // @todo variables to plot in the future
      // const TgcRdo::Errors& tgcErr = (*tgcCont_itr)->errors();
      // const TgcRdo::RodStatus& tgcRodStat = (*tgcCont_itr)->rodStatus();
      // const TgcRdo::LocalStatus& tgcLocalStat = (*tgcCont_itr)->localStatus();
      const uint32_t tgcOrbit((*tgcCont_itr)->orbit());
      const uint16_t tgcVersion((*tgcCont_itr)->version());

      m_tgcID->push_back(tgcID);
      m_tgcSubDetID->push_back(tgcSubDetID);
      m_tgcRodID->push_back(tgcRodID);
      m_tgcTrigType->push_back(tgcTrigType);
      m_tgcBcID->push_back(tgcBcID);
      m_tgcL1ID->push_back(tgcL1ID);
      m_tgcOrbit->push_back(tgcOrbit);
      m_tgcVersion->push_back(tgcVersion);

      m_h_tgcID->Fill(tgcID);
      m_h_tgcSubDetID->Fill(tgcSubDetID);
      m_h_tgcRodID->Fill(tgcRodID);
      m_h_tgcTrigType->Fill(tgcTrigType);
      m_h_tgcBcID->Fill(tgcBcID);
      m_h_tgcL1ID->Fill(tgcL1ID);

      const TgcRdo* p_TGCraw(*tgcCont_itr);
      TgcRdo::const_iterator tgc_itr(p_TGCraw->begin());
      const TgcRdo::const_iterator tgc_end(p_TGCraw->end());

      for ( ; tgc_itr != tgc_end; ++tgc_itr ) {
        const uint16_t bctag((*tgc_itr)->bcTag());
        const uint16_t subDetID((*tgc_itr)->subDetectorId());
        const uint16_t rodID((*tgc_itr)->rodId());
        const uint16_t sswID((*tgc_itr)->sswId());
        const uint16_t slbID((*tgc_itr)->slbId());
        const uint16_t bcID((*tgc_itr)->bcId());
        const uint16_t l1ID((*tgc_itr)->l1Id());
        TgcRawData::DataType type_var = (*tgc_itr)->type();
        TgcRawData::SlbType slbType_var = (*tgc_itr)->slbType();
        const bool coinc((*tgc_itr)->isCoincidence());
        const uint16_t bitPos((*tgc_itr)->bitpos());
        const uint16_t track((*tgc_itr)->tracklet());
        const bool adj((*tgc_itr)->isAdjacent());
        const bool forward((*tgc_itr)->isForward());
        const uint16_t ix((*tgc_itr)->index());
        const uint16_t pos((*tgc_itr)->position());
        const uint16_t del((*tgc_itr)->delta());
        const uint16_t seg((*tgc_itr)->segment());
        const uint16_t subMat((*tgc_itr)->subMatrix());
        const uint16_t sector((*tgc_itr)->sector());
        const uint16_t chip((*tgc_itr)->chip());
        const bool hipt((*tgc_itr)->isHipt());
        const uint16_t hitID((*tgc_itr)->hitId());
        const uint16_t hsub((*tgc_itr)->hsub());
        const bool strip((*tgc_itr)->isStrip());
        //const uint16_t inner((*tgc_itr)->inner());
        const bool cand3plus((*tgc_itr)->cand3plus());
        const bool muplus((*tgc_itr)->isMuplus());
        const bool thresh((*tgc_itr)->threshold());
        const bool overlap((*tgc_itr)->isOverlap());
        const bool veto((*tgc_itr)->isVeto());
        const uint16_t roi((*tgc_itr)->roi());

        m_bcTag->push_back(bctag);
        m_subDetID->push_back(subDetID);
        m_rodID->push_back(rodID);
        m_sswID->push_back(sswID);
        m_slbID->push_back(slbID);
        m_bcID->push_back(bcID);
        m_l1ID->push_back(l1ID);
        m_type->push_back(int(type_var));
        m_slbType->push_back(int(slbType_var));
        m_coinc->push_back(coinc);
        m_bitPos->push_back(bitPos);
        m_track->push_back(track);
        m_adj->push_back(adj);
        m_forward->push_back(forward);
        m_index->push_back(ix);
        m_pos->push_back(pos);
        m_delta->push_back(del);
        m_segment->push_back(seg);
        m_subMat->push_back(subMat);
        m_sector->push_back(sector);
        m_chip->push_back(chip);
        m_hipt->push_back(hipt);
        m_hitID->push_back(hitID);
        m_hsub->push_back(hsub);
        m_strip->push_back(strip);
        //m_inner->push_back(inner);
        m_cand3plus->push_back(cand3plus);
        m_muplus->push_back(muplus);
        m_thresh->push_back(thresh);
        m_overlap->push_back(overlap);
        m_veto->push_back(veto);
        m_roi->push_back(roi);

        m_h_bcTag->Fill(bctag);
        m_h_subDetID->Fill(subDetID);
        m_h_rodID->Fill(rodID);
        m_h_sswID->Fill(sswID);
        m_h_slbID->Fill(slbID);
        m_h_bcID->Fill(bcID);
        m_h_l1ID->Fill(l1ID);
        m_h_type->Fill(int(type_var));
        m_h_slbType->Fill(int(slbType_var));
        if (int(type_var) == 0) {
          m_h_bitPos->Fill(bitPos);
          m_h_track->Fill(track);
          m_h_adj->Fill(adj);
        }
      }
    }
  }

  // SimData
  SG::ReadHandle<MuonSimDataCollection> simDataMapTGC (m_inputTruthKey);
  if(simDataMapTGC.isValid()) {
    MuonSimDataCollection::const_iterator sdo_itr(simDataMapTGC->begin());
    const MuonSimDataCollection::const_iterator sdo_end(simDataMapTGC->end());

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
        m_radius->push_back(radius);
        m_localZ->push_back(localZ);

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

StatusCode TGC_RDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
