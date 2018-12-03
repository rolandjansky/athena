/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SCT_FastRDOAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

SCT_FastRDOAnalysis::SCT_FastRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputKey("SCT_Clusters")
  , m_hitsTimeBin3(0)
  , m_siCol(0)
  , m_siRow(0)
  , m_siPhiR(0)
  , m_siZ(0)
  , m_siPos_x(0)
  , m_siPos_y(0)
  , m_siPos_z(0)
  , m_siGangPix(0)
  , m_siDetID(0)
  , m_siDetPix(0)
  , m_siDetSCT(0)
  , m_siDetBrl(0)
  , m_siDetEc(0)
  , m_siDetBlay(0)
  , m_siDetInPixLay(0)
  , m_siDetNtInPixLay(0)
  , m_siDetDBM(0)
  , m_siDetHitDepthDir(0)
  , m_siDetHitPhiDir(0)
  , m_siDetHitEtaDir(0)
  , m_siDetMinR(0)
  , m_siDetMaxR(0)
  , m_siDetMinZ(0)
  , m_siDetMaxZ(0)
  , m_siDetMinPhi(0)
  , m_siDetMaxPhi(0)
  , m_siDetWidth(0)
  , m_siDetMinWidth(0)
  , m_siDetMaxWidth(0)
  , m_siDetLength(0)
  , m_siDetThick(0)
  , m_siDetEtaPitch(0)
  , m_siDetPhiPitch(0)
  , m_clusID(0)
  , m_locpos_x(0)
  , m_locpos_y(0)
  , m_rdoID_prd(0)

  , m_h_hitsTimeBin3(0)
  , m_h_siCol(0)
  , m_h_siRow(0)
  , m_h_siPhiR(0)
  , m_h_siZ(0)
  , m_h_siPos_x(0)
  , m_h_siPos_y(0)
  , m_h_siPos_z(0)
  , m_h_siGangPix(0)
  , m_h_siDetID(0)
  , m_h_siDetPix(0)
  , m_h_siDetSCT(0)
  , m_h_siDetBrl(0)
  , m_h_siDetEc(0)
  , m_h_siDetBlay(0)
  , m_h_siDetInPixLay(0)
  , m_h_siDetNtInPixLay(0)
  , m_h_siDetDBM(0)
  , m_h_siDetHitDepthDir(0)
  , m_h_siDetHitPhiDir(0)
  , m_h_siDetHitEtaDir(0)
  , m_h_siDetMinR(0)
  , m_h_siDetMaxR(0)
  , m_h_siDetMinZ(0)
  , m_h_siDetMaxZ(0)
  , m_h_siDetMinPhi(0)
  , m_h_siDetMaxPhi(0)
  , m_h_siDetWidth(0)
  , m_h_siDetMinWidth(0)
  , m_h_siDetMaxWidth(0)
  , m_h_siDetLength(0)
  , m_h_siDetThick(0)
  , m_h_siDetEtaPitch(0)
  , m_h_siDetPhiPitch(0)
  , m_h_clusID(0)
  , m_h_locpos_x(0)
  , m_h_locpos_y(0)
  , m_h_rdoID_prd(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/SCT_FastRDOAnalysis/")
  , m_ntupleTreeName("/SCT_FastRDOAna")
  , m_path("/SCT_FastRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("InputKey", m_inputKey);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode SCT_FastRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing SCT_FastRDOAnalysis" );

  // This will check that the properties were initialized
  // properly by job configuration.
  ATH_CHECK( m_inputKey.initialize() );

  // Grab Ntuple and histogramming service for tree
  ATH_CHECK(m_thistSvc.retrieve());

  m_tree = new TTree(TString(m_ntupleTreeName), "SCT_FastRDOAna");
  std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
  ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));
  if (m_tree) {
    // TREE BRANCHES
    // SCT_Cluster
    m_tree->Branch("hitsTimeBin3", &m_hitsTimeBin3);
    // SiCluster
    m_tree->Branch("siCol", &m_siCol);
    m_tree->Branch("siRow", &m_siRow);
    m_tree->Branch("siPhiR", &m_siPhiR);
    m_tree->Branch("siZ", &m_siZ);
    m_tree->Branch("siPos_x", &m_siPos_x);
    m_tree->Branch("siPos_y", &m_siPos_y);
    m_tree->Branch("siPos_z", &m_siPos_z);
    m_tree->Branch("siGangPix", &m_siGangPix);
    m_tree->Branch("siDetID", &m_siDetID);
    m_tree->Branch("siDetPix", &m_siDetPix);
    m_tree->Branch("siDetSCT", &m_siDetSCT);
    m_tree->Branch("siDetBrl", &m_siDetBrl);
    m_tree->Branch("siDetEc", &m_siDetEc);
    m_tree->Branch("siDetBlay", &m_siDetBlay);
    m_tree->Branch("siDetInPixLay", &m_siDetInPixLay);
    m_tree->Branch("siDetNtInPixLay", &m_siDetNtInPixLay);
    m_tree->Branch("siDetDBM", &m_siDetDBM);
    m_tree->Branch("siDetHitDepthDir", &m_siDetHitDepthDir);
    m_tree->Branch("siDetHitPhiDir", &m_siDetHitPhiDir);
    m_tree->Branch("siDetHitEtaDir", &m_siDetHitEtaDir);
    m_tree->Branch("siDetMinR", &m_siDetMinR);
    m_tree->Branch("siDetMaxR", &m_siDetMaxR);
    m_tree->Branch("siDetMinZ", &m_siDetMinZ);
    m_tree->Branch("siDetMaxZ", &m_siDetMaxZ);
    m_tree->Branch("siDetMinPhi", &m_siDetMinPhi);
    m_tree->Branch("siDetMaxPhi", &m_siDetMaxPhi);
    m_tree->Branch("siDetWidth", &m_siDetWidth);
    m_tree->Branch("siDetMinWidth", &m_siDetMinWidth);
    m_tree->Branch("siDetMaxWidth", &m_siDetMaxWidth);
    m_tree->Branch("siDetLength", &m_siDetLength);
    m_tree->Branch("siDetThick", &m_siDetThick);
    m_tree->Branch("siDetEtaPitch", &m_siDetEtaPitch);
    m_tree->Branch("siDetPhiPitch", &m_siDetPhiPitch);
    // Trk::PrepRawData
    m_tree->Branch("clusID", &m_clusID);
    m_tree->Branch("locpos_x", &m_locpos_x);
    m_tree->Branch("locpos_y", &m_locpos_y);
    m_tree->Branch("rdoID_prd", &m_rdoID_prd);

  }
  else {
    ATH_MSG_ERROR("No tree found!");
  }

  // HISTOGRAMS
  m_h_hitsTimeBin3 = new TH1F("h_hitsTimeBin3", "hitsTimeBine3", 100, 0, 2);
  m_h_hitsTimeBin3->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_hitsTimeBin3->GetName(), m_h_hitsTimeBin3));

  m_h_siCol = new TH1F("h_siCol", "siCol", 100, 0, 25);
  m_h_siCol->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siCol->GetName(), m_h_siCol));

  m_h_siRow = new TH1F("h_siRow", "siRow", 100, 0, 2);
  m_h_siRow->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siRow->GetName(), m_h_siRow));

  m_h_siPhiR = new TH1F("h_siPhiR", "siPhiR", 100, 0, 3.5);
  m_h_siPhiR->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siPhiR->GetName(), m_h_siPhiR));

  m_h_siZ = new TH1F("h_siZ", "siZ", 100, 0, 130);
  m_h_siZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siZ->GetName(), m_h_siZ));

  m_h_siPos_x = new TH1F("h_siPos_x", "siPos_x", 100, -600, 600);
  m_h_siPos_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siPos_x->GetName(), m_h_siPos_x));

  m_h_siPos_y = new TH1F("h_siPos_y", "siPos_y", 100, -600, 600);
  m_h_siPos_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siPos_y->GetName(), m_h_siPos_y));

  m_h_siPos_z = new TH1F("h_siPos_z", "siPos_z", 100, -3000, 3000);
  m_h_siPos_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siPos_z->GetName(), m_h_siPos_z));

  m_h_siGangPix = new TH1F("h_siGangPix", "siGangPix", 100, 0, 2);
  m_h_siGangPix->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siGangPix->GetName(), m_h_siGangPix));

  m_h_siDetID = new TH1F("h_siDetID", "siDetID", 100, 0, 1e18);
  m_h_siDetID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetID->GetName(), m_h_siDetID));

  m_h_siDetPix = new TH1F("h_siDetPix", "siDetPix", 100, 0, 2);
  m_h_siDetPix->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetPix->GetName(), m_h_siDetPix));

  m_h_siDetSCT = new TH1F("h_siDetSCT", "siDetSCT", 100, 0, 2);
  m_h_siDetSCT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetSCT->GetName(), m_h_siDetSCT));

  m_h_siDetBrl = new TH1F("h_siDetBrl", "siDetBrl", 100, 0, 2);
  m_h_siDetBrl->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetBrl->GetName(), m_h_siDetBrl));

  m_h_siDetEc = new TH1F("h_siDetEc", "siDetEc", 100, 0, 2);
  m_h_siDetEc->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetEc->GetName(), m_h_siDetEc));

  m_h_siDetBlay = new TH1F("h_siDetBlay", "siDetBlay", 100, 0, 2);
  m_h_siDetBlay->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetBlay->GetName(), m_h_siDetBlay));

  m_h_siDetInPixLay = new TH1F("h_siDetInPixLay", "siDetInPixLay", 100, 0, 2);
  m_h_siDetInPixLay->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetInPixLay->GetName(), m_h_siDetInPixLay));

  m_h_siDetNtInPixLay = new TH1F("h_siDetNtInPixLay", "siDetNtInPixLay", 100, 0, 2);
  m_h_siDetNtInPixLay->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetNtInPixLay->GetName(), m_h_siDetNtInPixLay));

  m_h_siDetDBM = new TH1F("h_siDetDBM", "siDetDBM", 100, 0, 2);
  m_h_siDetDBM->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetDBM->GetName(), m_h_siDetDBM));

  m_h_siDetHitDepthDir = new TH1F("h_siDetHitDepthDir", "siDetHitDepthDir", 100, -2, 2);
  m_h_siDetHitDepthDir->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetHitDepthDir->GetName(), m_h_siDetHitDepthDir));

  m_h_siDetHitPhiDir = new TH1F("h_siDetHitPhiDir", "siDetHitPhiDir", 100, -2, 2);
  m_h_siDetHitPhiDir->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetHitPhiDir->GetName(), m_h_siDetHitPhiDir));

  m_h_siDetHitEtaDir = new TH1F("h_siDetHitEtaDir", "siDetHitEtaDir", 100, -2, 2);
  m_h_siDetHitEtaDir->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetHitEtaDir->GetName(), m_h_siDetHitEtaDir));

  m_h_siDetMinR = new TH1F("h_siDetMinR", "siDetMinR", 100, 0, 600);
  m_h_siDetMinR->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMinR->GetName(), m_h_siDetMinR));

  m_h_siDetMaxR = new TH1F("h_siDetMaxR", "siDetMaxR", 100, 0, 600);
  m_h_siDetMaxR->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMaxR->GetName(), m_h_siDetMaxR));

  m_h_siDetMinZ = new TH1F("h_siDetMinZ", "siDetMinZ", 100, -3000, 3000);
  m_h_siDetMinZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMinZ->GetName(), m_h_siDetMinZ));

  m_h_siDetMaxZ = new TH1F("h_siDetMaxZ", "siDetMaxZ", 100, -3000, 3000);
  m_h_siDetMaxZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMaxZ->GetName(), m_h_siDetMaxZ));

  m_h_siDetMinPhi = new TH1F("h_siDetMinPhi", "siDetMinPhi", 100, -3.5, 3.5);
  m_h_siDetMinPhi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMinPhi->GetName(), m_h_siDetMinPhi));

  m_h_siDetMaxPhi = new TH1F("h_siDetMaxPhi", "siDetMaxPhi", 100, 3.5, 3.5);
  m_h_siDetMaxPhi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMaxPhi->GetName(), m_h_siDetMaxPhi));

  m_h_siDetWidth = new TH1F("h_siDetWidth", "siDetWidth", 100, 0, 75);
  m_h_siDetWidth->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetWidth->GetName(), m_h_siDetWidth));

  m_h_siDetMinWidth = new TH1F("h_siDetMinWidth", "siDetMinWidth", 100, 0, 75);
  m_h_siDetMinWidth->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMinWidth->GetName(), m_h_siDetMinWidth));

  m_h_siDetMaxWidth = new TH1F("h_siDetMaxWidth", "siDetMaxWidth", 100, 0, 75);
  m_h_siDetMaxWidth->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetMaxWidth->GetName(), m_h_siDetMaxWidth));

  m_h_siDetLength = new TH1F("h_siDetLength", "siDetLength", 100, 0, 150);
  m_h_siDetLength->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetLength->GetName(), m_h_siDetLength));

  m_h_siDetThick = new TH1F("h_siDetThick", "siDetThick", 100, 0, 1);
  m_h_siDetThick->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetThick->GetName(), m_h_siDetThick));

  m_h_siDetEtaPitch = new TH1F("h_siDetEtaPitch", "siDetEtaPitch", 100, 0, 150);
  m_h_siDetEtaPitch->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetEtaPitch->GetName(), m_h_siDetEtaPitch));

  m_h_siDetPhiPitch = new TH1F("h_siDetPhiPitch", "siDetPhiPitch", 100, 0, 0.1);
  m_h_siDetPhiPitch->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_siDetPhiPitch->GetName(), m_h_siDetPhiPitch));

  m_h_clusID = new TH1F("h_clusID", "clusID", 100, 0, 1e18);
  m_h_clusID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_clusID->GetName(), m_h_clusID));

  m_h_locpos_x = new TH1F("h_locpos_x", "locpos_x", 100, -40, 40);
  m_h_locpos_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_locpos_x->GetName(), m_h_locpos_x));

  m_h_locpos_y = new TH1F("h_locpos_y", "locpos_y", 100, -1, 1);
  m_h_locpos_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_locpos_y->GetName(), m_h_locpos_y));

  m_h_rdoID_prd = new TH1F("h_rdoID_prd", "rdoID_prd", 100, 0, 1e18);
  m_h_rdoID_prd->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + m_h_rdoID_prd->GetName(), m_h_rdoID_prd));

  return StatusCode::SUCCESS;
}

StatusCode SCT_FastRDOAnalysis::execute() {
  ATH_MSG_DEBUG( "In SCT_FastRDOAnalysis::execute()" );

  // clear branches
  m_hitsTimeBin3->clear();

  m_siCol->clear();
  m_siRow->clear();
  m_siPhiR->clear();
  m_siZ->clear();
  m_siPos_x->clear();
  m_siPos_y->clear();
  m_siPos_z->clear();
  m_siGangPix->clear();
  m_siDetID->clear();
  m_siDetPix->clear();
  m_siDetSCT->clear();
  m_siDetBrl->clear();
  m_siDetEc->clear();
  m_siDetBlay->clear();
  m_siDetInPixLay->clear();
  m_siDetNtInPixLay->clear();
  m_siDetDBM->clear();
  m_siDetHitDepthDir->clear();
  m_siDetHitPhiDir->clear();
  m_siDetHitEtaDir->clear();
  m_siDetMinR->clear();
  m_siDetMaxR->clear();
  m_siDetMinZ->clear();
  m_siDetMaxZ->clear();
  m_siDetMinPhi->clear();
  m_siDetMaxPhi->clear();
  m_siDetWidth->clear();
  m_siDetMinWidth->clear();
  m_siDetMaxWidth->clear();
  m_siDetLength->clear();
  m_siDetThick->clear();
  m_siDetEtaPitch->clear();
  m_siDetPhiPitch->clear();

  m_clusID->clear();
  m_locpos_x->clear();
  m_locpos_y->clear();
  m_rdoID_prd->clear();

  // get containers -- fill branches + histos
  SG::ReadHandle<InDet::SCT_ClusterContainer> p_sctClus_cont (m_inputKey);
  if(p_sctClus_cont.isValid()) {
    // loop over cluster container
    InDet::SCT_ClusterContainer::const_iterator clusCont_itr(p_sctClus_cont->begin());
    const InDet::SCT_ClusterContainer::const_iterator clusCont_end(p_sctClus_cont->end());
    for ( ; clusCont_itr != clusCont_end; ++clusCont_itr ) {
      const InDet::SCT_ClusterCollection* p_sctClus_coll(*clusCont_itr);
      InDet::SCT_ClusterCollection::const_iterator clus_itr(p_sctClus_coll->begin());
      const InDet::SCT_ClusterCollection::const_iterator clus_end(p_sctClus_coll->end());
      for ( ; clus_itr != clus_end; ++clus_itr ) {
        // SCT_Cluster
        const uint16_t hitsTimeBin3((*clus_itr)->hitsInThirdTimeBin());
        m_hitsTimeBin3->push_back(hitsTimeBin3);

        m_h_hitsTimeBin3->Fill(hitsTimeBin3);

        // SiCluster
        const InDet::SiWidth& siWidth((*clus_itr)->SiCluster::width());
        const Amg::Vector2D& siColRow(siWidth.colRow());
        const float siCol(siColRow.x());
        const float siRow(siColRow.y());
        const Amg::Vector2D& siPhiRZ(siWidth.widthPhiRZ());
        const float siPhiR(siPhiRZ.x());
        const float siZ(siPhiRZ.y());

        const Amg::Vector3D& siPos((*clus_itr)->SiCluster::globalPosition());
        const float siPos_x(siPos.x());
        const float siPos_y(siPos.y());
        const float siPos_z(siPos.z());

        const bool siGangPix((*clus_itr)->SiCluster::gangedPixel());

        const InDetDD::SiDetectorElement* siDetEl((*clus_itr)->SiCluster::detectorElement());
        const Identifier siDetID((*siDetEl).identify());
        //const AtlasDetectorID* siDetAtlasID((*siDetEl).getIdHelper());
        const bool siDetPix((*siDetEl).isPixel());
        const bool siDetSCT((*siDetEl).isSCT());
        const bool siDetBrl((*siDetEl).isBarrel());
        const bool siDetEc((*siDetEl).isEndcap());
        const bool siDetBlay((*siDetEl).isBlayer());
        const bool siDetInPixLay((*siDetEl).isInnermostPixelLayer());
        const bool siDetNtInPixLay((*siDetEl).isNextToInnermostPixelLayer());
        const bool siDetDBM((*siDetEl).isDBM());
        const double siDetHitDepthDir((*siDetEl).hitDepthDirection());
        const double siDetHitPhiDir((*siDetEl).hitPhiDirection());
        const double siDetHitEtaDir((*siDetEl).hitEtaDirection());
        const double siDetMinR((*siDetEl).rMin());
        const double siDetMaxR((*siDetEl).rMax());
        const double siDetMinZ((*siDetEl).zMin());
        const double siDetMaxZ((*siDetEl).zMax());
        const double siDetMinPhi((*siDetEl).phiMin());
        const double siDetMaxPhi((*siDetEl).phiMax());
        const double siDetWidth((*siDetEl).width());
        const double siDetMinWidth((*siDetEl).minWidth());
        const double siDetMaxWidth((*siDetEl).maxWidth());
        const double siDetLength((*siDetEl).length());
        const double siDetThick((*siDetEl).thickness());
        const double siDetEtaPitch((*siDetEl).etaPitch());
        const double siDetPhiPitch((*siDetEl).phiPitch());

        const unsigned long long siDetID_int = siDetID.get_compact();

        m_siCol->push_back(siCol);
        m_siRow->push_back(siRow);
        m_siPhiR->push_back(siPhiR);
        m_siZ->push_back(siZ);
        m_siPos_x->push_back(siPos_x);
        m_siPos_y->push_back(siPos_y);
        m_siPos_z->push_back(siPos_z);
        m_siGangPix->push_back(siGangPix);
        m_siDetID->push_back(siDetID_int);
        m_siDetPix->push_back(siDetPix);
        m_siDetSCT->push_back(siDetSCT);
        m_siDetBrl->push_back(siDetBrl);
        m_siDetEc->push_back(siDetEc);
        m_siDetBlay->push_back(siDetBlay);
        m_siDetInPixLay->push_back(siDetInPixLay);
        m_siDetNtInPixLay->push_back(siDetNtInPixLay);
        m_siDetDBM->push_back(siDetDBM);
        m_siDetHitDepthDir->push_back(siDetHitDepthDir);
        m_siDetHitPhiDir->push_back(siDetHitPhiDir);
        m_siDetHitEtaDir->push_back(siDetHitEtaDir);
        m_siDetMinR->push_back(siDetMinR);
        m_siDetMaxR->push_back(siDetMaxR);
        m_siDetMinZ->push_back(siDetMinZ);
        m_siDetMaxZ->push_back(siDetMaxZ);
        m_siDetMinPhi->push_back(siDetMinPhi);
        m_siDetMaxPhi->push_back(siDetMaxPhi);
        m_siDetWidth->push_back(siDetWidth);
        m_siDetMinWidth->push_back(siDetMinWidth);
        m_siDetMaxWidth->push_back(siDetMaxWidth);
        m_siDetLength->push_back(siDetLength);
        m_siDetThick->push_back(siDetThick);
        m_siDetEtaPitch->push_back(siDetEtaPitch);
        m_siDetPhiPitch->push_back(siDetPhiPitch);

        m_h_siCol->Fill(siCol);
        m_h_siRow->Fill(siRow);
        m_h_siPhiR->Fill(siPhiR);
        m_h_siZ->Fill(siZ);
        m_h_siPos_x->Fill(siPos_x);
        m_h_siPos_y->Fill(siPos_y);
        m_h_siPos_z->Fill(siPos_z);
        m_h_siGangPix->Fill(siGangPix);
        m_h_siDetID->Fill(siDetID_int);
        m_h_siDetPix->Fill(siDetPix);
        m_h_siDetSCT->Fill(siDetSCT);
        m_h_siDetBrl->Fill(siDetBrl);
        m_h_siDetEc->Fill(siDetEc);
        m_h_siDetBlay->Fill(siDetBlay);
        m_h_siDetInPixLay->Fill(siDetInPixLay);
        m_h_siDetNtInPixLay->Fill(siDetNtInPixLay);
        m_h_siDetDBM->Fill(siDetDBM);
        m_h_siDetHitDepthDir->Fill(siDetHitDepthDir);
        m_h_siDetHitPhiDir->Fill(siDetHitPhiDir);
        m_h_siDetHitEtaDir->Fill(siDetHitEtaDir);
        m_h_siDetMinR->Fill(siDetMinR);
        m_h_siDetMaxR->Fill(siDetMaxR);
        m_h_siDetMinZ->Fill(siDetMinZ);
        m_h_siDetMaxZ->Fill(siDetMaxZ);
        m_h_siDetMinPhi->Fill(siDetMinPhi);
        m_h_siDetMaxPhi->Fill(siDetMaxPhi);
        m_h_siDetWidth->Fill(siDetWidth);
        m_h_siDetMinWidth->Fill(siDetMinWidth);
        m_h_siDetMaxWidth->Fill(siDetMaxWidth);
        m_h_siDetLength->Fill(siDetLength);
        m_h_siDetThick->Fill(siDetThick);
        m_h_siDetEtaPitch->Fill(siDetEtaPitch);
        m_h_siDetPhiPitch->Fill(siDetPhiPitch);

        // Trk::PrepRawData
        const Identifier clusID((*clus_itr)->PrepRawData::identify());
        const unsigned long long clusID_int = clusID.get_compact();

        const Amg::Vector2D& locPos((*clus_itr)->PrepRawData::localPosition());
        float locpos_x(locPos.x());
        float locpos_y(locPos.y());

        const std::vector<Identifier>& rdoID_v((*clus_itr)->PrepRawData::rdoList());

        m_clusID->push_back(clusID_int);
        m_locpos_x->push_back(locpos_x);
        m_locpos_y->push_back(locpos_y);
        for (size_t k = 0; k != rdoID_v.size(); ++k) {
          const unsigned long long rdoID_prd_int = rdoID_v.at(k).get_compact();
          m_rdoID_prd->push_back(rdoID_prd_int);
          m_h_rdoID_prd->Fill(rdoID_prd_int);
        }

        m_h_clusID->Fill(clusID_int);
        m_h_locpos_x->Fill(locpos_x);
        m_h_locpos_y->Fill(locpos_y);
      }
    }
  }

  if (m_tree) {
    m_tree->Fill();
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_FastRDOAnalysis::finalize() {
  return StatusCode::SUCCESS;
}
