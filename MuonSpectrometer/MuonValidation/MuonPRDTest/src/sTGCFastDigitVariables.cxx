/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCFastDigitVariables.h"
#include "AthenaKernel/errorcheck.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

#include "TTree.h"


StatusCode sTGCFastDigitVariables::fillVariables()
{
  ATH_MSG_INFO("do fillNSWsTGCFastDigitVariables()");
  CHECK( this->clearVariables() );

  const MuonSimDataCollection* nsw_sTgcSdoContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_sTgcSdoContainer, "STGC_SDO") );

  const Muon::sTgcPrepDataContainer *nsw_sTgcPrepDataContainer = nullptr; 
  CHECK( m_evtStore->retrieve(nsw_sTgcPrepDataContainer, m_ContainerName.c_str()) );

  if(nsw_sTgcPrepDataContainer->size()==0) ATH_MSG_WARNING(" sTgc Fast DigitContainer empty ");

  for(auto it : *nsw_sTgcPrepDataContainer ) {
    const Muon::sTgcPrepDataCollection* coll = it;
    
    for (unsigned int item=0; item<coll->size(); item++) {
      const Muon::sTgcPrepData* fast_digit = coll->at(item);
      Identifier Id = fast_digit->identify();

      std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
      int stationEta       = m_sTgcIdHelper->stationEta(Id);
      int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
      int multiplet        = m_sTgcIdHelper->multilayer(Id);
      int gas_gap          = m_sTgcIdHelper->gasGap(Id);
      int channel_type     = m_sTgcIdHelper->channelType(Id);
      int channel          = m_sTgcIdHelper->channel(Id);

      ATH_MSG_DEBUG(     "sTGC Fast Digit Offline id:  Station Name [" << stName << "]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " Type ["         << channel_type    << "]"
                      << " ChNr ["         << channel         << "]" );

      m_NSWsTGC_fdg_stationName->push_back(stName);
      m_NSWsTGC_fdg_stationEta->push_back(stationEta);
      m_NSWsTGC_fdg_stationPhi->push_back(stationPhi);
      m_NSWsTGC_fdg_multiplet->push_back(multiplet);
      m_NSWsTGC_fdg_gas_gap->push_back(gas_gap);
      m_NSWsTGC_fdg_channel_type->push_back(channel_type);
      m_NSWsTGC_fdg_channel->push_back(channel);

      const MuonGM::sTgcReadoutElement* det = fast_digit->detectorElement();
      Amg::Vector3D pos = fast_digit->globalPosition();
      Amg::Vector2D loc_pos(0., 0.);
       det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      double err_x = fast_digit->localCovariance()(0,0);
      double err_y = ( fast_digit->localCovariance().rows()==2)? fast_digit->localCovariance()(1,1) : 0.;

      ATH_MSG_DEBUG(     "sTgc Fast Digit local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                << ", ex=" << std::setw(6) << std::setprecision(2) << err_x
                                                << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] 
                                                << ", ey=" << std::setw(6) << std::setprecision(2) << err_y );

      m_NSWsTGC_fdg_globalPosX->push_back(pos.x());
      m_NSWsTGC_fdg_globalPosY->push_back(pos.y());
      m_NSWsTGC_fdg_globalPosZ->push_back(pos.z());

      m_NSWsTGC_fdg_localPosX->push_back(loc_pos[0]);
      m_NSWsTGC_fdg_localPosY->push_back(loc_pos[1]);
      m_NSWsTGC_fdg_covMatrix_1_1->push_back(err_x);
      m_NSWsTGC_fdg_covMatrix_2_2->push_back(err_y);


      if (nsw_sTgcSdoContainer) {
        const MuonSimData stgc_sdo = (nsw_sTgcSdoContainer->find(Id))->second;
        std::vector<MuonSimData::Deposit> deposits;
        stgc_sdo.deposits(deposits);

        int    truth_barcode       = deposits[0].first.barcode();
        double truth_hitOnSurfaceX = deposits[0].second.firstEntry(); 
        double truth_hitOnSurfaceY = deposits[0].second.secondEntry();

        ATH_MSG_DEBUG("sTGC Fast Digit, truth barcode=" << truth_barcode);
        ATH_MSG_DEBUG("sTGC Fast Digit, truth hitOnSurfaceX=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceX
                                   << ", truth hitOnSurfaceY=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceY);

        m_NSWsTGC_fdg_truth_barcode->push_back( truth_barcode );
        m_NSWsTGC_fdg_truth_hitOnSurfaceX->push_back( truth_hitOnSurfaceX );
        m_NSWsTGC_fdg_truth_hitOnSurfaceY->push_back( truth_hitOnSurfaceY );
      }


      m_NSWsTGC_nFastDigits++;
    }
  }

  ATH_MSG_INFO("processed " << m_NSWsTGC_nFastDigits << " sTGC fast digitis");

  return StatusCode::SUCCESS;
}

StatusCode sTGCFastDigitVariables::deleteVariables()
{
  delete m_NSWsTGC_fdg_stationName;
  delete m_NSWsTGC_fdg_stationEta;
  delete m_NSWsTGC_fdg_stationPhi;
  delete m_NSWsTGC_fdg_multiplet;
  delete m_NSWsTGC_fdg_gas_gap;
  delete m_NSWsTGC_fdg_channel_type;
  delete m_NSWsTGC_fdg_channel;

  delete m_NSWsTGC_fdg_globalPosX;
  delete m_NSWsTGC_fdg_globalPosY;
  delete m_NSWsTGC_fdg_globalPosZ;

  delete m_NSWsTGC_fdg_localPosX;
  delete m_NSWsTGC_fdg_localPosY;
  delete m_NSWsTGC_fdg_covMatrix_1_1;
  delete m_NSWsTGC_fdg_covMatrix_2_2;

  delete m_NSWsTGC_fdg_truth_barcode;
  delete m_NSWsTGC_fdg_truth_hitOnSurfaceX;
  delete m_NSWsTGC_fdg_truth_hitOnSurfaceY;


  m_NSWsTGC_nFastDigits = 0;
  m_NSWsTGC_fdg_stationName   = nullptr;
  m_NSWsTGC_fdg_stationEta    = nullptr;
  m_NSWsTGC_fdg_stationPhi    = nullptr;
  m_NSWsTGC_fdg_multiplet     = nullptr;
  m_NSWsTGC_fdg_gas_gap       = nullptr;
  m_NSWsTGC_fdg_channel_type  = nullptr;
  m_NSWsTGC_fdg_channel       = nullptr;

  m_NSWsTGC_fdg_globalPosX    = nullptr;
  m_NSWsTGC_fdg_globalPosY    = nullptr;
  m_NSWsTGC_fdg_globalPosZ    = nullptr;

  m_NSWsTGC_fdg_localPosX     = nullptr;
  m_NSWsTGC_fdg_localPosY     = nullptr;
  m_NSWsTGC_fdg_covMatrix_1_1 = nullptr;
  m_NSWsTGC_fdg_covMatrix_2_2 = nullptr;

  m_NSWsTGC_fdg_truth_barcode       = nullptr;
  m_NSWsTGC_fdg_truth_hitOnSurfaceX = nullptr;
  m_NSWsTGC_fdg_truth_hitOnSurfaceY = nullptr;

  return StatusCode::SUCCESS;
}


StatusCode sTGCFastDigitVariables::clearVariables()
{

  m_NSWsTGC_nFastDigits = 0;
  m_NSWsTGC_fdg_stationName->clear();
  m_NSWsTGC_fdg_stationEta ->clear();
  m_NSWsTGC_fdg_stationPhi ->clear();
  m_NSWsTGC_fdg_multiplet  ->clear();
  m_NSWsTGC_fdg_gas_gap    ->clear();
  m_NSWsTGC_fdg_channel_type->clear();
  m_NSWsTGC_fdg_channel    ->clear();

  m_NSWsTGC_fdg_globalPosX ->clear();
  m_NSWsTGC_fdg_globalPosY ->clear();
  m_NSWsTGC_fdg_globalPosZ ->clear();

  m_NSWsTGC_fdg_localPosX  ->clear();
  m_NSWsTGC_fdg_localPosY  ->clear();
  m_NSWsTGC_fdg_covMatrix_1_1 ->clear();
  m_NSWsTGC_fdg_covMatrix_2_2 ->clear();

  m_NSWsTGC_fdg_truth_barcode->clear();
  m_NSWsTGC_fdg_truth_hitOnSurfaceX->clear();
  m_NSWsTGC_fdg_truth_hitOnSurfaceY->clear();

  return StatusCode::SUCCESS;
}


StatusCode sTGCFastDigitVariables::initializeVariables()
{
  m_NSWsTGC_nFastDigits = 0;
  m_NSWsTGC_fdg_stationName   = new std::vector<std::string>;
  m_NSWsTGC_fdg_stationEta    = new std::vector<int>;
  m_NSWsTGC_fdg_stationPhi    = new std::vector<int>;
  m_NSWsTGC_fdg_multiplet     = new std::vector<int>;
  m_NSWsTGC_fdg_gas_gap       = new std::vector<int>;
  m_NSWsTGC_fdg_channel_type  = new std::vector<int>;
  m_NSWsTGC_fdg_channel       = new std::vector<int>;

  m_NSWsTGC_fdg_globalPosX    = new std::vector<double>;
  m_NSWsTGC_fdg_globalPosY    = new std::vector<double>;
  m_NSWsTGC_fdg_globalPosZ    = new std::vector<double>;

  m_NSWsTGC_fdg_localPosX     = new std::vector<double>;
  m_NSWsTGC_fdg_localPosY     = new std::vector<double>;
  m_NSWsTGC_fdg_covMatrix_1_1 = new std::vector<double>;
  m_NSWsTGC_fdg_covMatrix_2_2 = new std::vector<double>;

  m_NSWsTGC_fdg_truth_barcode       = new std::vector<int>;
  m_NSWsTGC_fdg_truth_hitOnSurfaceX = new std::vector<double>;
  m_NSWsTGC_fdg_truth_hitOnSurfaceY = new std::vector<double>;

  if(m_tree) {
    m_tree->Branch("FastDigit_sTGC", &m_NSWsTGC_nFastDigits, "FastDigits_sTGC_n/i");
    m_tree->Branch("FastDigit_sTGC_stationName", &m_NSWsTGC_fdg_stationName);
    m_tree->Branch("FastDigit_sTGC_stationEta",  &m_NSWsTGC_fdg_stationEta);
    m_tree->Branch("FastDigit_sTGC_stationPhi",  &m_NSWsTGC_fdg_stationPhi);
    m_tree->Branch("FastDigit_sTGC_multiplet",   &m_NSWsTGC_fdg_multiplet);
    m_tree->Branch("FastDigit_sTGC_gas_gap",     &m_NSWsTGC_fdg_gas_gap);
    m_tree->Branch("FastDigit_sTGC_channel_type",&m_NSWsTGC_fdg_channel_type);
    m_tree->Branch("FastDigit_sTGC_channel",     &m_NSWsTGC_fdg_channel);

    m_tree->Branch("FastDigit_sTGC_globalPosX",  &m_NSWsTGC_fdg_globalPosX);
    m_tree->Branch("FastDigit_sTGC_globalPosY",  &m_NSWsTGC_fdg_globalPosY);
    m_tree->Branch("FastDigit_sTGC_globalPosZ",  &m_NSWsTGC_fdg_globalPosZ);

    m_tree->Branch("FastDigit_sTGC_localPosX",   &m_NSWsTGC_fdg_localPosX);
    m_tree->Branch("FastDigit_sTGC_localPosY",   &m_NSWsTGC_fdg_localPosY);
    m_tree->Branch("FastDigit_sTGC_covMatrix_1_1", &m_NSWsTGC_fdg_covMatrix_1_1);
    m_tree->Branch("FastDigit_sTGC_covMatrix_2_2", &m_NSWsTGC_fdg_covMatrix_2_2);


    m_tree->Branch("FastDigit_sTGC_truth_barcode", &m_NSWsTGC_fdg_truth_barcode);
    m_tree->Branch("FastDigit_sTGC_truth_hitOnSurfaceX", &m_NSWsTGC_fdg_truth_hitOnSurfaceX);
    m_tree->Branch("FastDigit_sTGC_truth_hitOnSurfaceY", &m_NSWsTGC_fdg_truth_hitOnSurfaceY);
  }

  return StatusCode::SUCCESS;
}

