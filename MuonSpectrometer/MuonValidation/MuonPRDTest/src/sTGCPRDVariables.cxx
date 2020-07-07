/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCPRDVariables.h"
#include "AthenaKernel/errorcheck.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

#include "TTree.h"
#include <TString.h> // for Form

StatusCode sTGCPRDVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillNSWsTGCPRDVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);
  CHECK( this->clearVariables() );

  const Muon::sTgcPrepDataContainer *nsw_sTgcPrepDataContainer = nullptr; 
  CHECK( m_evtStore->retrieve(nsw_sTgcPrepDataContainer, m_ContainerName.c_str()) );

  if(nsw_sTgcPrepDataContainer->size()==0) ATH_MSG_WARNING(" sTgc PRD Container empty ");

  for(const Muon::sTgcPrepDataCollection* coll : *nsw_sTgcPrepDataContainer ) {
    
    for (unsigned int item=0; item<coll->size(); item++) {
      const Muon::sTgcPrepData* prd = coll->at(item);
      Identifier Id = prd->identify();

      std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
      int stationEta       = m_sTgcIdHelper->stationEta(Id);
      int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
      int multiplet        = m_sTgcIdHelper->multilayer(Id);
      int gas_gap          = m_sTgcIdHelper->gasGap(Id);
      int channel_type     = m_sTgcIdHelper->channelType(Id);
      int channel          = m_sTgcIdHelper->channel(Id);
      int charge           = prd->charge();
      uint16_t bcTag      = prd->getBcBitMap();

      ATH_MSG_DEBUG(     "sTGC PRD Offline id:  Station Name [" << stName << "]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " Type ["         << channel_type    << "]"
                      << " ChNr ["         << channel         << "]" );

      m_NSWsTGC_prd_stationName->push_back(stName);
      m_NSWsTGC_prd_stationEta->push_back(stationEta);
      m_NSWsTGC_prd_stationPhi->push_back(stationPhi);
      m_NSWsTGC_prd_multiplet->push_back(multiplet);
      m_NSWsTGC_prd_gas_gap->push_back(gas_gap);
      m_NSWsTGC_prd_channel_type->push_back(channel_type);
      m_NSWsTGC_prd_channel->push_back(channel);
      m_NSWsTGC_prd_charge->push_back(charge);
      m_NSWsTGC_prd_bcTag->push_back(bcTag);

      const MuonGM::sTgcReadoutElement* det = prd->detectorElement();
      if (!det) throw std::runtime_error(Form("File: %s, Line: %d\nsTGCPRDVariables::fillVariables() - no associated detectorElement", __FILE__, __LINE__));
      Amg::Vector3D pos = prd->globalPosition();
      Amg::Vector2D loc_pos(0., 0.);
       det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      double err_x = prd->localCovariance()(0,0);
      double err_y = ( prd->localCovariance().rows()==2)? prd->localCovariance()(1,1) : 0.;

      ATH_MSG_DEBUG(     "sTgc PRD local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                << ", ex=" << std::setw(6) << std::setprecision(2) << err_x
                                                << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] 
                                                << ", ey=" << std::setw(6) << std::setprecision(2) << err_y );

      m_NSWsTGC_prd_globalPosX->push_back(pos.x());
      m_NSWsTGC_prd_globalPosY->push_back(pos.y());
      m_NSWsTGC_prd_globalPosZ->push_back(pos.z());

      m_NSWsTGC_prd_localPosX->push_back(loc_pos[0]);
      m_NSWsTGC_prd_localPosY->push_back(loc_pos[1]);
      m_NSWsTGC_prd_covMatrix_1_1->push_back(err_x);
      m_NSWsTGC_prd_covMatrix_2_2->push_back(err_y);

      m_NSWsTGC_nPRDs++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_NSWsTGC_nPRDs << " sTGC PRD's");

  return StatusCode::SUCCESS;
}

void sTGCPRDVariables::deleteVariables()
{
  delete m_NSWsTGC_prd_stationName;
  delete m_NSWsTGC_prd_stationEta;
  delete m_NSWsTGC_prd_stationPhi;
  delete m_NSWsTGC_prd_multiplet;
  delete m_NSWsTGC_prd_gas_gap;
  delete m_NSWsTGC_prd_channel_type;
  delete m_NSWsTGC_prd_channel;
  delete m_NSWsTGC_prd_charge;
  delete m_NSWsTGC_prd_bcTag;

  delete m_NSWsTGC_prd_globalPosX;
  delete m_NSWsTGC_prd_globalPosY;
  delete m_NSWsTGC_prd_globalPosZ;

  delete m_NSWsTGC_prd_localPosX;
  delete m_NSWsTGC_prd_localPosY;
  delete m_NSWsTGC_prd_covMatrix_1_1;
  delete m_NSWsTGC_prd_covMatrix_2_2;

  m_NSWsTGC_nPRDs = 0;
  m_NSWsTGC_prd_stationName   = nullptr;
  m_NSWsTGC_prd_stationEta    = nullptr;
  m_NSWsTGC_prd_stationPhi    = nullptr;
  m_NSWsTGC_prd_multiplet     = nullptr;
  m_NSWsTGC_prd_gas_gap       = nullptr;
  m_NSWsTGC_prd_channel_type  = nullptr;
  m_NSWsTGC_prd_channel       = nullptr;
  m_NSWsTGC_prd_charge       = nullptr;
  m_NSWsTGC_prd_bcTag       = nullptr;

  m_NSWsTGC_prd_globalPosX    = nullptr;
  m_NSWsTGC_prd_globalPosY    = nullptr;
  m_NSWsTGC_prd_globalPosZ    = nullptr;

  m_NSWsTGC_prd_localPosX     = nullptr;
  m_NSWsTGC_prd_localPosY     = nullptr;
  m_NSWsTGC_prd_covMatrix_1_1 = nullptr;
  m_NSWsTGC_prd_covMatrix_2_2 = nullptr;

  return;
}


StatusCode sTGCPRDVariables::clearVariables()
{

  m_NSWsTGC_nPRDs = 0;
  m_NSWsTGC_prd_stationName->clear();
  m_NSWsTGC_prd_stationEta ->clear();
  m_NSWsTGC_prd_stationPhi ->clear();
  m_NSWsTGC_prd_multiplet  ->clear();
  m_NSWsTGC_prd_gas_gap    ->clear();
  m_NSWsTGC_prd_channel_type->clear();
  m_NSWsTGC_prd_channel    ->clear();
  m_NSWsTGC_prd_charge    ->clear();
  m_NSWsTGC_prd_bcTag    ->clear();

  m_NSWsTGC_prd_globalPosX ->clear();
  m_NSWsTGC_prd_globalPosY ->clear();
  m_NSWsTGC_prd_globalPosZ ->clear();

  m_NSWsTGC_prd_localPosX  ->clear();
  m_NSWsTGC_prd_localPosY  ->clear();
  m_NSWsTGC_prd_covMatrix_1_1 ->clear();
  m_NSWsTGC_prd_covMatrix_2_2 ->clear();

  return StatusCode::SUCCESS;
}


StatusCode sTGCPRDVariables::initializeVariables()
{
  m_NSWsTGC_nPRDs = 0;
  m_NSWsTGC_prd_stationName   = new std::vector<std::string>;
  m_NSWsTGC_prd_stationEta    = new std::vector<int>;
  m_NSWsTGC_prd_stationPhi    = new std::vector<int>;
  m_NSWsTGC_prd_multiplet     = new std::vector<int>;
  m_NSWsTGC_prd_gas_gap       = new std::vector<int>;
  m_NSWsTGC_prd_channel_type  = new std::vector<int>;
  m_NSWsTGC_prd_channel       = new std::vector<int>;
  m_NSWsTGC_prd_charge       = new std::vector<int>;
  m_NSWsTGC_prd_bcTag       = new std::vector<uint16_t>;

  m_NSWsTGC_prd_globalPosX    = new std::vector<double>;
  m_NSWsTGC_prd_globalPosY    = new std::vector<double>;
  m_NSWsTGC_prd_globalPosZ    = new std::vector<double>;

  m_NSWsTGC_prd_localPosX     = new std::vector<double>;
  m_NSWsTGC_prd_localPosY     = new std::vector<double>;
  m_NSWsTGC_prd_covMatrix_1_1 = new std::vector<double>;
  m_NSWsTGC_prd_covMatrix_2_2 = new std::vector<double>;

  if(m_tree) {
    m_tree->Branch("PRD_sTGC", &m_NSWsTGC_nPRDs, "PRDs_sTGC_n/i");
    m_tree->Branch("PRD_sTGC_stationName", &m_NSWsTGC_prd_stationName);
    m_tree->Branch("PRD_sTGC_stationEta",  &m_NSWsTGC_prd_stationEta);
    m_tree->Branch("PRD_sTGC_stationPhi",  &m_NSWsTGC_prd_stationPhi);
    m_tree->Branch("PRD_sTGC_multiplet",   &m_NSWsTGC_prd_multiplet);
    m_tree->Branch("PRD_sTGC_gas_gap",     &m_NSWsTGC_prd_gas_gap);
    m_tree->Branch("PRD_sTGC_channel_type",&m_NSWsTGC_prd_channel_type);
    m_tree->Branch("PRD_sTGC_channel",     &m_NSWsTGC_prd_channel);
    m_tree->Branch("PRD_sTGC_charge",     &m_NSWsTGC_prd_charge);
    m_tree->Branch("PRD_sTGC_bcTag",     &m_NSWsTGC_prd_bcTag);

    m_tree->Branch("PRD_sTGC_globalPosX",  &m_NSWsTGC_prd_globalPosX);
    m_tree->Branch("PRD_sTGC_globalPosY",  &m_NSWsTGC_prd_globalPosY);
    m_tree->Branch("PRD_sTGC_globalPosZ",  &m_NSWsTGC_prd_globalPosZ);

    m_tree->Branch("PRD_sTGC_localPosX",   &m_NSWsTGC_prd_localPosX);
    m_tree->Branch("PRD_sTGC_localPosY",   &m_NSWsTGC_prd_localPosY);
    m_tree->Branch("PRD_sTGC_covMatrix_1_1", &m_NSWsTGC_prd_covMatrix_1_1);
    m_tree->Branch("PRD_sTGC_covMatrix_2_2", &m_NSWsTGC_prd_covMatrix_2_2);

  }

  return StatusCode::SUCCESS;
}

