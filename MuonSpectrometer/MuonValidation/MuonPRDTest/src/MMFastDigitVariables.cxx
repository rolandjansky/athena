/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MMFastDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepData.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "TTree.h"


StatusCode MMFastDigitVariables::fillVariables()
{
  ATH_MSG_INFO(" do fillNSWMMFastDigitVariables()");

  CHECK( this->clearVariables() );

  const MuonSimDataCollection* nsw_MmSdoContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_MmSdoContainer, "MM_SDO") );

  const Muon::MMPrepDataContainer *nsw_MMPrepDataContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_MMPrepDataContainer, m_ContainerName.c_str()) );

  if(nsw_MMPrepDataContainer->size()==0) ATH_MSG_WARNING(" MM Fast DigitContainer empty ");

  for(auto it : *nsw_MMPrepDataContainer) {
    const Muon::MMPrepDataCollection* coll = it;
    
    for (unsigned int item=0; item<coll->size(); item++) {
      const Muon::MMPrepData* fast_digit = coll->at(item);
      Identifier Id = fast_digit->identify();

      std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
      int stationEta       = m_MmIdHelper->stationEta(Id);
      int stationPhi       = m_MmIdHelper->stationPhi(Id);
      int multiplet        = m_MmIdHelper->multilayer(Id);
      int gas_gap          = m_MmIdHelper->gasGap(Id);
      int channel          = m_MmIdHelper->channel(Id);


      ATH_MSG_DEBUG(     "MicroMegas Fast Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

      m_NSWMM_fdg_stationName->push_back(stName);
      m_NSWMM_fdg_stationEta->push_back(stationEta);
      m_NSWMM_fdg_stationPhi->push_back(stationPhi);
      m_NSWMM_fdg_multiplet->push_back(multiplet);
      m_NSWMM_fdg_gas_gap->push_back(gas_gap);
      m_NSWMM_fdg_channel->push_back(channel);

      const MuonGM::MMReadoutElement* det = fast_digit->detectorElement();
      Amg::Vector3D pos    = fast_digit->globalPosition();
      const Amg::MatrixX & cov = fast_digit->localCovariance();
      Amg::Vector2D loc_pos(0., 0.);
      det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      ATH_MSG_DEBUG(     "MicroMegas Fast Digit local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                      << ", ex=" << std::setw(6) << std::setprecision(2) << cov(0,0)
                                                      << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] );

      m_NSWMM_fdg_globalPosX->push_back(pos.x());
      m_NSWMM_fdg_globalPosY->push_back(pos.y());
      m_NSWMM_fdg_globalPosZ->push_back(pos.z());

      m_NSWMM_fdg_localPosX->push_back(loc_pos[0]);
      m_NSWMM_fdg_localPosY->push_back(loc_pos[1]);
      m_NSWMM_fdg_covMatrix_1_1->push_back(cov(0,0));

      if (nsw_MmSdoContainer) {
        const MuonSimData mm_sdo = (nsw_MmSdoContainer->find(Id))->second;
        std::vector<MuonSimData::Deposit> deposits;
        mm_sdo.deposits(deposits);

        int    truth_barcode       = deposits[0].first.barcode();
        double truth_hitOnSurfaceX = deposits[0].second.firstEntry(); 
        double truth_hitOnSurfaceY = deposits[0].second.secondEntry();

        ATH_MSG_DEBUG("MicroMegas Fast Digit, truth barcode=" << truth_barcode);
        ATH_MSG_DEBUG("MicroMegas Fast Digit, truth hitOnSurfaceX=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceX
                                   << ", truth hitOnSurfaceY=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceY);

        m_NSWMM_fdg_truth_barcode->push_back( truth_barcode );
        m_NSWMM_fdg_truth_hitOnSurfaceX->push_back( truth_hitOnSurfaceX );
        m_NSWMM_fdg_truth_hitOnSurfaceY->push_back( truth_hitOnSurfaceY );
      }

      m_NSWMM_nFastDigits++;
    }
  }

  ATH_MSG_INFO("processed " << m_NSWMM_nFastDigits << " MicroMegas fast digitis");
  return StatusCode::SUCCESS;
}

StatusCode MMFastDigitVariables::deleteVariables()
{
  delete m_NSWMM_fdg_stationName;
  delete m_NSWMM_fdg_stationEta;
  delete m_NSWMM_fdg_stationPhi;
  delete m_NSWMM_fdg_multiplet;
  delete m_NSWMM_fdg_gas_gap;
  delete m_NSWMM_fdg_channel;

  delete m_NSWMM_fdg_globalPosX;
  delete m_NSWMM_fdg_globalPosY;
  delete m_NSWMM_fdg_globalPosZ;

  delete m_NSWMM_fdg_localPosX;
  delete m_NSWMM_fdg_localPosY;
  delete m_NSWMM_fdg_covMatrix_1_1;

  delete m_NSWMM_fdg_truth_barcode;
  delete m_NSWMM_fdg_truth_hitOnSurfaceX;
  delete m_NSWMM_fdg_truth_hitOnSurfaceY;

  m_NSWMM_nFastDigits    = 0;
  m_NSWMM_fdg_stationName= nullptr;
  m_NSWMM_fdg_stationEta = nullptr;
  m_NSWMM_fdg_stationPhi = nullptr;
  m_NSWMM_fdg_multiplet  = nullptr;
  m_NSWMM_fdg_gas_gap    = nullptr;
  m_NSWMM_fdg_channel    = nullptr;

  m_NSWMM_fdg_globalPosX = nullptr;
  m_NSWMM_fdg_globalPosY = nullptr;
  m_NSWMM_fdg_globalPosZ = nullptr;

  m_NSWMM_fdg_localPosX  = nullptr;
  m_NSWMM_fdg_localPosY  = nullptr;
  m_NSWMM_fdg_covMatrix_1_1 = nullptr;

  m_NSWMM_fdg_truth_barcode       = nullptr;
  m_NSWMM_fdg_truth_hitOnSurfaceX = nullptr; 
  m_NSWMM_fdg_truth_hitOnSurfaceY = nullptr;

  return StatusCode::SUCCESS;
}


StatusCode MMFastDigitVariables::clearVariables()
{
  m_NSWMM_nFastDigits = 0;
  m_NSWMM_fdg_stationName->clear();
  m_NSWMM_fdg_stationEta->clear();
  m_NSWMM_fdg_stationPhi->clear();
  m_NSWMM_fdg_multiplet->clear();
  m_NSWMM_fdg_gas_gap->clear();
  m_NSWMM_fdg_channel->clear();

  m_NSWMM_fdg_globalPosX->clear();
  m_NSWMM_fdg_globalPosY->clear();
  m_NSWMM_fdg_globalPosZ->clear();

  m_NSWMM_fdg_localPosX->clear();
  m_NSWMM_fdg_localPosY->clear();
  m_NSWMM_fdg_covMatrix_1_1->clear();

  m_NSWMM_fdg_truth_barcode->clear();
  m_NSWMM_fdg_truth_hitOnSurfaceX->clear();
  m_NSWMM_fdg_truth_hitOnSurfaceY->clear();

  return StatusCode::SUCCESS;
}


StatusCode MMFastDigitVariables::initializeVariables()
{
  m_NSWMM_nFastDigits = 0;
  m_NSWMM_fdg_stationName = new std::vector<std::string>();
  m_NSWMM_fdg_stationEta = new std::vector<int>();
  m_NSWMM_fdg_stationPhi = new std::vector<int>();
  m_NSWMM_fdg_multiplet  = new std::vector<int>();
  m_NSWMM_fdg_gas_gap    = new std::vector<int>();
  m_NSWMM_fdg_channel    = new std::vector<int>();

  m_NSWMM_fdg_globalPosX = new std::vector<double>();
  m_NSWMM_fdg_globalPosY = new std::vector<double>();
  m_NSWMM_fdg_globalPosZ = new std::vector<double>();

  m_NSWMM_fdg_localPosX = new std::vector<double>();
  m_NSWMM_fdg_localPosY = new std::vector<double>();
  m_NSWMM_fdg_covMatrix_1_1 = new std::vector<double>();

  m_NSWMM_fdg_truth_barcode        = new std::vector<int>();
  m_NSWMM_fdg_truth_hitOnSurfaceX  = new std::vector<double>();
  m_NSWMM_fdg_truth_hitOnSurfaceY  = new std::vector<double>();

  if(m_tree) {
    m_tree->Branch("FastDigit_MM", &m_NSWMM_nFastDigits, "FastDigits_MM_n/i");
    m_tree->Branch("FastDigit_MM_stationName",   &m_NSWMM_fdg_stationName);
    m_tree->Branch("FastDigit_MM_stationEta",    &m_NSWMM_fdg_stationEta); 
    m_tree->Branch("FastDigit_MM_stationPhi",    &m_NSWMM_fdg_stationPhi);
    m_tree->Branch("FastDigit_MM_multiplet",     &m_NSWMM_fdg_multiplet);
    m_tree->Branch("FastDigit_MM_gas_gap",       &m_NSWMM_fdg_gas_gap);
    m_tree->Branch("FastDigit_MM_channel",       &m_NSWMM_fdg_channel);

    m_tree->Branch("FastDigit_MM_globalPosX",    &m_NSWMM_fdg_globalPosX);
    m_tree->Branch("FastDigit_MM_globalPosY",    &m_NSWMM_fdg_globalPosY);
    m_tree->Branch("FastDigit_MM_globalPosZ",    &m_NSWMM_fdg_globalPosZ);

    m_tree->Branch("FastDigit_MM_localPosX",     &m_NSWMM_fdg_localPosX);
    m_tree->Branch("FastDigit_MM_localPosY",     &m_NSWMM_fdg_localPosY);
    m_tree->Branch("FastDigit_MM_covMatrix_1_1", &m_NSWMM_fdg_covMatrix_1_1);

    m_tree->Branch("FastDigit_MM_truth_barcode",       &m_NSWMM_fdg_truth_barcode);
    m_tree->Branch("FastDigit_MM_truth_hitOnSurfaceX", &m_NSWMM_fdg_truth_hitOnSurfaceX);
    m_tree->Branch("FastDigit_MM_truth_hitOnSurfaceY", &m_NSWMM_fdg_truth_hitOnSurfaceY);
  }

  return StatusCode::SUCCESS;
}

