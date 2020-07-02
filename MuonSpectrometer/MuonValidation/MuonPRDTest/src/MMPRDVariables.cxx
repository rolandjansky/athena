/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MMPRDVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepData.h"

#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "MMRDOVariables.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include <TString.h> // for Form

#include "TTree.h"


StatusCode MMPRDVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillNSWMMPRDVariables()");
  ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

  CHECK( this->clearVariables() );

  const Muon::MMPrepDataContainer *nsw_MMPrepDataContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_MMPrepDataContainer, m_ContainerName.c_str()) );

  if(nsw_MMPrepDataContainer->size()==0) ATH_MSG_WARNING(" MM PRD Container empty ");
  
  const Muon::MM_RawDataContainer* rdo_container = nullptr;
  CHECK( m_evtStore->retrieve(rdo_container, "MMRDO") ); // retrieve  RDOs to get information per strip since PRDs only contain ids of strips

  if(rdo_container->size()==0) ATH_MSG_WARNING(" RDO Container empty ");

  for(const Muon::MMPrepDataCollection* coll : *nsw_MMPrepDataContainer) {

    const Muon::MM_RawDataCollection* rdo_coll = rdo_container->indexFindPtr(coll->identifyHash());
    if(rdo_coll==0){
      ATH_MSG_ERROR("Did not find rdo collection " << coll->identifyHash());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Found rdo_coll at "<<rdo_coll);
    for (unsigned int item=0; item<coll->size(); item++) {
      const Muon::MMPrepData* prd = coll->at(item);
      Identifier Id = prd->identify();

      std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
      int stationEta       = m_MmIdHelper->stationEta(Id);
      int stationPhi       = m_MmIdHelper->stationPhi(Id);
      int multiplet        = m_MmIdHelper->multilayer(Id);
      int gas_gap          = m_MmIdHelper->gasGap(Id);
      int channel          = m_MmIdHelper->channel(Id);


      ATH_MSG_DEBUG(     "MicroMegas PRD Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

     ATH_MSG_DEBUG(  " MM channel " << channel << " prd->time() " << prd->time() );

      m_NSWMM_prd_stationName->push_back(stName);
      m_NSWMM_prd_stationEta->push_back(stationEta);
      m_NSWMM_prd_stationPhi->push_back(stationPhi);
      m_NSWMM_prd_multiplet->push_back(multiplet);
      m_NSWMM_prd_gas_gap->push_back(gas_gap);
      m_NSWMM_prd_channel->push_back(channel);
      m_NSWMM_prd_time->push_back(prd->time());

      const MuonGM::MMReadoutElement* det = prd->detectorElement();
      if (!det) throw std::runtime_error(Form("File: %s, Line: %d\nMMPRDVariables::fillVariables() - no associated detectorElement", __FILE__, __LINE__));
      Amg::Vector3D pos    = prd->globalPosition();
      const Amg::MatrixX & cov = prd->localCovariance();
      Amg::Vector2D loc_pos(0., 0.);
      det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      ATH_MSG_DEBUG(     "MicroMegas PRD local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                      << ", ex=" << std::setw(6) << std::setprecision(2) << cov(0,0)
                                                      << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] );

      m_NSWMM_prd_globalPosX->push_back(pos.x());
      m_NSWMM_prd_globalPosY->push_back(pos.y());
      m_NSWMM_prd_globalPosZ->push_back(pos.z());

      m_NSWMM_prd_localPosX->push_back(loc_pos[0]);
      m_NSWMM_prd_localPosY->push_back(loc_pos[1]);
      m_NSWMM_prd_covMatrix_1_1->push_back(cov(0,0));

      m_NSWMM_prd_rdos_charge->push_back(std::vector<int>());      
      m_NSWMM_prd_rdos_time->push_back(std::vector<int>());      
      m_NSWMM_prd_rdos_channel->push_back(std::vector<int>());      
      m_NSWMM_prd_nRdos->push_back((prd->rdoList()).size());

      for(const Identifier &id_rdo:prd->rdoList()){
        const Muon::MM_RawData* rdo=nullptr;
        for(auto it :*rdo_coll){if(it->identify() == id_rdo) rdo=it;}
        if(rdo==0){
          ATH_MSG_ERROR("Did not find rdo for identifier in rdo list");
          return StatusCode::FAILURE;
        }
        m_NSWMM_prd_rdos_charge->back().push_back(rdo->charge());
        m_NSWMM_prd_rdos_time->back().push_back(rdo->time());
        m_NSWMM_prd_rdos_channel->back().push_back(m_MmIdHelper->channel(id_rdo));
        
      }

      m_NSWMM_prd_uTPCAngle->push_back(prd->angle());  
      m_nsw_prd_uTPCChiSqProb->push_back(prd->chisqProb());





      m_NSWMM_nPRDs++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_NSWMM_nPRDs << " MicroMegas PRD's");
  return StatusCode::SUCCESS;
}

void MMPRDVariables::deleteVariables()
{
  ATH_MSG_DEBUG("begin of deleteVariables()");
  delete m_NSWMM_prd_stationName;
  delete m_NSWMM_prd_stationEta;
  delete m_NSWMM_prd_stationPhi;
  delete m_NSWMM_prd_multiplet;
  delete m_NSWMM_prd_gas_gap;
  delete m_NSWMM_prd_channel;
  delete m_NSWMM_prd_time;

  delete m_NSWMM_prd_globalPosX;
  delete m_NSWMM_prd_globalPosY;
  delete m_NSWMM_prd_globalPosZ;

  delete m_NSWMM_prd_localPosX;
  delete m_NSWMM_prd_localPosY;
  delete m_NSWMM_prd_covMatrix_1_1;

  delete m_NSWMM_prd_rdos_charge;
  delete m_NSWMM_prd_rdos_time;
  delete m_NSWMM_prd_rdos_channel;
  delete m_NSWMM_prd_nRdos;

  delete m_NSWMM_prd_uTPCAngle;  
  delete m_nsw_prd_uTPCChiSqProb;



  m_NSWMM_nPRDs    = 0;
  
  m_NSWMM_prd_stationName= nullptr;
  m_NSWMM_prd_stationEta = nullptr;
  m_NSWMM_prd_stationPhi = nullptr;
  m_NSWMM_prd_multiplet  = nullptr;
  m_NSWMM_prd_gas_gap    = nullptr;
  m_NSWMM_prd_channel    = nullptr;
  m_NSWMM_prd_time       = nullptr;

  m_NSWMM_prd_globalPosX = nullptr;
  m_NSWMM_prd_globalPosY = nullptr;
  m_NSWMM_prd_globalPosZ = nullptr;

  m_NSWMM_prd_localPosX  = nullptr;
  m_NSWMM_prd_localPosY  = nullptr;
  m_NSWMM_prd_covMatrix_1_1 = nullptr;

  m_NSWMM_prd_rdos_charge=nullptr;
  m_NSWMM_prd_rdos_time=nullptr;
  m_NSWMM_prd_rdos_channel=nullptr;
  m_NSWMM_prd_nRdos=nullptr;

  m_NSWMM_prd_uTPCAngle=nullptr;
  m_nsw_prd_uTPCChiSqProb=nullptr;

  ATH_MSG_DEBUG("end of deleteVariables()");

  return;
}


StatusCode MMPRDVariables::clearVariables()
{
  ATH_MSG_DEBUG("beginninng of clearVariables()");
  m_NSWMM_nPRDs = 0;
  m_NSWMM_prd_stationName->clear();
  m_NSWMM_prd_stationEta->clear();
  m_NSWMM_prd_stationPhi->clear();
  m_NSWMM_prd_multiplet->clear();
  m_NSWMM_prd_gas_gap->clear();
  m_NSWMM_prd_channel->clear();
  m_NSWMM_prd_time->clear();

  m_NSWMM_prd_globalPosX->clear();
  m_NSWMM_prd_globalPosY->clear();
  m_NSWMM_prd_globalPosZ->clear();

  m_NSWMM_prd_localPosX->clear();
  m_NSWMM_prd_localPosY->clear();
  m_NSWMM_prd_covMatrix_1_1->clear();

  m_NSWMM_prd_rdos_charge->clear();
  m_NSWMM_prd_rdos_time->clear();
  m_NSWMM_prd_rdos_channel->clear();
  m_NSWMM_prd_nRdos->clear();
  m_NSWMM_prd_uTPCAngle->clear();  
  m_nsw_prd_uTPCChiSqProb->clear();


  ATH_MSG_DEBUG("end of clearVariables()");

  return StatusCode::SUCCESS;
}


StatusCode MMPRDVariables::initializeVariables()
{
  ATH_MSG_DEBUG("begin initializeVariables()");
  m_NSWMM_nPRDs = 0;
  m_NSWMM_prd_stationName = new std::vector<std::string>();
  m_NSWMM_prd_stationEta = new std::vector<int>();
  m_NSWMM_prd_stationPhi = new std::vector<int>();
  m_NSWMM_prd_multiplet  = new std::vector<int>();
  m_NSWMM_prd_gas_gap    = new std::vector<int>();
  m_NSWMM_prd_channel    = new std::vector<int>();
  m_NSWMM_prd_time       = new std::vector<int>();

  m_NSWMM_prd_globalPosX = new std::vector<double>();
  m_NSWMM_prd_globalPosY = new std::vector<double>();
  m_NSWMM_prd_globalPosZ = new std::vector<double>();

  m_NSWMM_prd_localPosX = new std::vector<double>();
  m_NSWMM_prd_localPosY = new std::vector<double>();
  m_NSWMM_prd_covMatrix_1_1 = new std::vector<double>();

  m_NSWMM_prd_rdos_charge = new std::vector<std::vector<int>>();
  m_NSWMM_prd_rdos_time = new std::vector<std::vector<int>>();
  m_NSWMM_prd_rdos_channel = new std::vector<std::vector<int>>();
  m_NSWMM_prd_nRdos = new std::vector<int>();

  m_NSWMM_prd_uTPCAngle   = new std::vector<double>();
  m_nsw_prd_uTPCChiSqProb = new std::vector<double>();


  ATH_MSG_DEBUG("did init of vars initializeVariables()");

  if(m_tree) {
    m_tree->Branch("PRD_MM", &m_NSWMM_nPRDs, "PRDs_MM_n/i");
    m_tree->Branch("PRD_MM_stationName",   &m_NSWMM_prd_stationName);
    m_tree->Branch("PRD_MM_stationEta",    &m_NSWMM_prd_stationEta); 
    m_tree->Branch("PRD_MM_stationPhi",    &m_NSWMM_prd_stationPhi);
    m_tree->Branch("PRD_MM_multiplet",     &m_NSWMM_prd_multiplet);
    m_tree->Branch("PRD_MM_gas_gap",       &m_NSWMM_prd_gas_gap);
    m_tree->Branch("PRD_MM_channel",       &m_NSWMM_prd_channel);
    m_tree->Branch("PRD_MM_time",          &m_NSWMM_prd_time);

    m_tree->Branch("PRD_MM_globalPosX",    &m_NSWMM_prd_globalPosX);
    m_tree->Branch("PRD_MM_globalPosY",    &m_NSWMM_prd_globalPosY);
    m_tree->Branch("PRD_MM_globalPosZ",    &m_NSWMM_prd_globalPosZ);

    m_tree->Branch("PRD_MM_localPosX",     &m_NSWMM_prd_localPosX);
    m_tree->Branch("PRD_MM_localPosY",     &m_NSWMM_prd_localPosY);
    m_tree->Branch("PRD_MM_covMatrix_1_1", &m_NSWMM_prd_covMatrix_1_1);


    m_tree->Branch("PRD_MM_rdos_charge", &m_NSWMM_prd_rdos_charge);
    m_tree->Branch("PRD_MM_rdos_time", &m_NSWMM_prd_rdos_time);
    m_tree->Branch("PRD_MM_rdos_channel", &m_NSWMM_prd_rdos_channel);
    m_tree->Branch("PRD_MM_nRdos", &m_NSWMM_prd_nRdos);

    m_tree->Branch("PRD_MM_uTPCAngle",&m_NSWMM_prd_uTPCAngle);
    m_tree->Branch("PRD_MM_uTPCChiSqProb",&m_nsw_prd_uTPCChiSqProb);

  
    ATH_MSG_DEBUG("did init of tree initializeVariables()");

  }

  ATH_MSG_DEBUG("after init of tree initializeVariables()");

  return StatusCode::SUCCESS;
}

