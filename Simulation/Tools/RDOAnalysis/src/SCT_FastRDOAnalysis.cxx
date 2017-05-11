/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SCT_FastRDOAnalysis.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

SCT_FastRDOAnalysis::SCT_FastRDOAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
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

  , h_hitsTimeBin3(0)
  , h_siCol(0)
  , h_siRow(0)
  , h_siPhiR(0)
  , h_siZ(0)
  , h_siPos_x(0)
  , h_siPos_y(0)
  , h_siPos_z(0)
  , h_siGangPix(0)
  , h_siDetID(0)
  , h_siDetPix(0)
  , h_siDetSCT(0)
  , h_siDetBrl(0)
  , h_siDetEc(0)
  , h_siDetBlay(0)
  , h_siDetInPixLay(0)
  , h_siDetNtInPixLay(0)
  , h_siDetDBM(0)
  , h_siDetHitDepthDir(0)
  , h_siDetHitPhiDir(0)
  , h_siDetHitEtaDir(0)
  , h_siDetMinR(0)
  , h_siDetMaxR(0)
  , h_siDetMinZ(0)
  , h_siDetMaxZ(0)
  , h_siDetMinPhi(0)
  , h_siDetMaxPhi(0)
  , h_siDetWidth(0)
  , h_siDetMinWidth(0)
  , h_siDetMaxWidth(0)
  , h_siDetLength(0)
  , h_siDetThick(0)
  , h_siDetEtaPitch(0)
  , h_siDetPhiPitch(0)
  , h_clusID(0)
  , h_locpos_x(0)
  , h_locpos_y(0)
  , h_rdoID_prd(0)

  , m_tree(0)
  , m_ntupleFileName("/ntuples/file1")
  , m_ntupleDirName("/SCT_FastRDOAnalysis/")
  , m_ntupleTreeName("/SCT_FastRDOAna")
  , m_path("/SCT_FastRDOAnalysis/")
  , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("HistPath", m_path);
}

StatusCode SCT_FastRDOAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing SCT_FastRDOAnalysis" );

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
  h_hitsTimeBin3 = new TH1F("h_hitsTimeBin3", "hitsTimeBine3", 100, 0, 2);
  h_hitsTimeBin3->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_hitsTimeBin3->GetName(), h_hitsTimeBin3));
  
  h_siCol = new TH1F("h_siCol", "siCol", 100, 0, 25);
  h_siCol->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siCol->GetName(), h_siCol));

  h_siRow = new TH1F("h_siRow", "siRow", 100, 0, 2);
  h_siRow->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siRow->GetName(), h_siRow));

  h_siPhiR = new TH1F("h_siPhiR", "siPhiR", 100, 0, 3.5);
  h_siPhiR->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siPhiR->GetName(), h_siPhiR));

  h_siZ = new TH1F("h_siZ", "siZ", 100, 0, 130);
  h_siZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siZ->GetName(), h_siZ));

  h_siPos_x = new TH1F("h_siPos_x", "siPos_x", 100, -600, 600);
  h_siPos_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siPos_x->GetName(), h_siPos_x));
  
  h_siPos_y = new TH1F("h_siPos_y", "siPos_y", 100, -600, 600);
  h_siPos_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siPos_y->GetName(), h_siPos_y));
  
  h_siPos_z = new TH1F("h_siPos_z", "siPos_z", 100, -3000, 3000);
  h_siPos_z->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siPos_z->GetName(), h_siPos_z));

  h_siGangPix = new TH1F("h_siGangPix", "siGangPix", 100, 0, 2);
  h_siGangPix->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siGangPix->GetName(), h_siGangPix));

  h_siDetID = new TH1F("h_siDetID", "siDetID", 100, 0, 1e18);
  h_siDetID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetID->GetName(), h_siDetID));

  h_siDetPix = new TH1F("h_siDetPix", "siDetPix", 100, 0, 2);
  h_siDetPix->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetPix->GetName(), h_siDetPix));

  h_siDetSCT = new TH1F("h_siDetSCT", "siDetSCT", 100, 0, 2);
  h_siDetSCT->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetSCT->GetName(), h_siDetSCT));

  h_siDetBrl = new TH1F("h_siDetBrl", "siDetBrl", 100, 0, 2);
  h_siDetBrl->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetBrl->GetName(), h_siDetBrl));

  h_siDetEc = new TH1F("h_siDetEc", "siDetEc", 100, 0, 2);
  h_siDetEc->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetEc->GetName(), h_siDetEc));

  h_siDetBlay = new TH1F("h_siDetBlay", "siDetBlay", 100, 0, 2);
  h_siDetBlay->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetBlay->GetName(), h_siDetBlay));

  h_siDetInPixLay = new TH1F("h_siDetInPixLay", "siDetInPixLay", 100, 0, 2);
  h_siDetInPixLay->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetInPixLay->GetName(), h_siDetInPixLay));

  h_siDetNtInPixLay = new TH1F("h_siDetNtInPixLay", "siDetNtInPixLay", 100, 0, 2);
  h_siDetNtInPixLay->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetNtInPixLay->GetName(), h_siDetNtInPixLay));

  h_siDetDBM = new TH1F("h_siDetDBM", "siDetDBM", 100, 0, 2);
  h_siDetDBM->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetDBM->GetName(), h_siDetDBM));

  h_siDetHitDepthDir = new TH1F("h_siDetHitDepthDir", "siDetHitDepthDir", 100, -2, 2);
  h_siDetHitDepthDir->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetHitDepthDir->GetName(), h_siDetHitDepthDir));

  h_siDetHitPhiDir = new TH1F("h_siDetHitPhiDir", "siDetHitPhiDir", 100, -2, 2);
  h_siDetHitPhiDir->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetHitPhiDir->GetName(), h_siDetHitPhiDir));

  h_siDetHitEtaDir = new TH1F("h_siDetHitEtaDir", "siDetHitEtaDir", 100, -2, 2);
  h_siDetHitEtaDir->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetHitEtaDir->GetName(), h_siDetHitEtaDir));

  h_siDetMinR = new TH1F("h_siDetMinR", "siDetMinR", 100, 0, 600);
  h_siDetMinR->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMinR->GetName(), h_siDetMinR));

  h_siDetMaxR = new TH1F("h_siDetMaxR", "siDetMaxR", 100, 0, 600);
  h_siDetMaxR->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMaxR->GetName(), h_siDetMaxR));

  h_siDetMinZ = new TH1F("h_siDetMinZ", "siDetMinZ", 100, -3000, 3000);
  h_siDetMinZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMinZ->GetName(), h_siDetMinZ));

  h_siDetMaxZ = new TH1F("h_siDetMaxZ", "siDetMaxZ", 100, -3000, 3000);
  h_siDetMaxZ->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMaxZ->GetName(), h_siDetMaxZ));

  h_siDetMinPhi = new TH1F("h_siDetMinPhi", "siDetMinPhi", 100, -3.5, 3.5);
  h_siDetMinPhi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMinPhi->GetName(), h_siDetMinPhi));

  h_siDetMaxPhi = new TH1F("h_siDetMaxPhi", "siDetMaxPhi", 100, 3.5, 3.5);
  h_siDetMaxPhi->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMaxPhi->GetName(), h_siDetMaxPhi));

  h_siDetWidth = new TH1F("h_siDetWidth", "siDetWidth", 100, 0, 75);
  h_siDetWidth->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetWidth->GetName(), h_siDetWidth));

  h_siDetMinWidth = new TH1F("h_siDetMinWidth", "siDetMinWidth", 100, 0, 75);
  h_siDetMinWidth->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMinWidth->GetName(), h_siDetMinWidth));

  h_siDetMaxWidth = new TH1F("h_siDetMaxWidth", "siDetMaxWidth", 100, 0, 75);
  h_siDetMaxWidth->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetMaxWidth->GetName(), h_siDetMaxWidth));

  h_siDetLength = new TH1F("h_siDetLength", "siDetLength", 100, 0, 150);
  h_siDetLength->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetLength->GetName(), h_siDetLength));

  h_siDetThick = new TH1F("h_siDetThick", "siDetThick", 100, 0, 1);
  h_siDetThick->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetThick->GetName(), h_siDetThick));

  h_siDetEtaPitch = new TH1F("h_siDetEtaPitch", "siDetEtaPitch", 100, 0, 150);
  h_siDetEtaPitch->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetEtaPitch->GetName(), h_siDetEtaPitch));

  h_siDetPhiPitch = new TH1F("h_siDetPhiPitch", "siDetPhiPitch", 100, 0, 0.1);
  h_siDetPhiPitch->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_siDetPhiPitch->GetName(), h_siDetPhiPitch));

  h_clusID = new TH1F("h_clusID", "clusID", 100, 0, 1e18);
  h_clusID->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_clusID->GetName(), h_clusID));

  h_locpos_x = new TH1F("h_locpos_x", "locpos_x", 100, -40, 40);
  h_locpos_x->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_locpos_x->GetName(), h_locpos_x));

  h_locpos_y = new TH1F("h_locpos_y", "locpos_y", 100, -1, 1);
  h_locpos_y->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_locpos_y->GetName(), h_locpos_y));

  h_rdoID_prd = new TH1F("h_rdoID_prd", "rdoID_prd", 100, 0, 1e18);
  h_rdoID_prd->StatOverflows();
  ATH_CHECK(m_thistSvc->regHist(m_path + h_rdoID_prd->GetName(), h_rdoID_prd));

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
  const InDet::SCT_ClusterContainer* p_sctClus_cont;
  if (evtStore()->retrieve(p_sctClus_cont, "SCT_Clusters") == StatusCode::SUCCESS) {
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

	h_hitsTimeBin3->Fill(hitsTimeBin3);

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

	h_siCol->Fill(siCol);
	h_siRow->Fill(siRow);
	h_siPhiR->Fill(siPhiR);
	h_siZ->Fill(siZ);
	h_siPos_x->Fill(siPos_x);
	h_siPos_y->Fill(siPos_y);
	h_siPos_z->Fill(siPos_z);
	h_siGangPix->Fill(siGangPix);
	h_siDetID->Fill(siDetID_int);
	h_siDetPix->Fill(siDetPix);
	h_siDetSCT->Fill(siDetSCT);
	h_siDetBrl->Fill(siDetBrl);
	h_siDetEc->Fill(siDetEc);
	h_siDetBlay->Fill(siDetBlay);
	h_siDetInPixLay->Fill(siDetInPixLay);
	h_siDetNtInPixLay->Fill(siDetNtInPixLay);
	h_siDetDBM->Fill(siDetDBM);
	h_siDetHitDepthDir->Fill(siDetHitDepthDir);
	h_siDetHitPhiDir->Fill(siDetHitPhiDir);
	h_siDetHitEtaDir->Fill(siDetHitEtaDir);
	h_siDetMinR->Fill(siDetMinR);
	h_siDetMaxR->Fill(siDetMaxR);
	h_siDetMinZ->Fill(siDetMinZ);
	h_siDetMaxZ->Fill(siDetMaxZ);
	h_siDetMinPhi->Fill(siDetMinPhi);
	h_siDetMaxPhi->Fill(siDetMaxPhi);
	h_siDetWidth->Fill(siDetWidth);
	h_siDetMinWidth->Fill(siDetMinWidth);
	h_siDetMaxWidth->Fill(siDetMaxWidth);
	h_siDetLength->Fill(siDetLength);
	h_siDetThick->Fill(siDetThick);
	h_siDetEtaPitch->Fill(siDetEtaPitch);
	h_siDetPhiPitch->Fill(siDetPhiPitch);
	
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
	  h_rdoID_prd->Fill(rdoID_prd_int);
	}

	h_clusID->Fill(clusID_int);
	h_locpos_x->Fill(locpos_x);
	h_locpos_y->Fill(locpos_y);
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
