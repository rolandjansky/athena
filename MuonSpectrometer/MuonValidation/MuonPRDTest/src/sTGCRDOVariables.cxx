/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCRDOVariables.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonRDO/STGC_RawDataContainer.h"

#include "TTree.h"

using namespace Muon;

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode sTGCRDOVariables::fillVariables()
{
  ATH_MSG_DEBUG("do fillNSWsTGCRDOVariables()");

  // clear variables
  ATH_CHECK( this->clearVariables() );

  ATH_MSG_DEBUG("Retrieve RDO container with name " << m_ContainerName);
  // get the rdo (a container corresponds to a multilayer of a module)
  const STGC_RawDataContainer* rdo_container = nullptr;
  ATH_CHECK( m_evtStore->retrieve(rdo_container, m_ContainerName.c_str()) );

  if(rdo_container->size()==0) ATH_MSG_WARNING(" sTGC RDO Container empty ");
  
  // iteration on all containers, i.e. all multilayers of all modules
  for(const STGC_RawDataCollection* coll : *rdo_container) {

    // a digit collection is instanciated for each container, i.e. holds all digits of a multilayer
    // loop on all digits inside a collection, i.e. multilayer
    for (unsigned int item=0; item<coll->size(); item++) {

      // get specific digit and identify it
      const STGC_RawData* rdo = coll->at(item);
      Identifier Id = rdo->identify();

      std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
      int stationEta       = m_sTgcIdHelper->stationEta(Id);
      int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
      int multiplet        = m_sTgcIdHelper->multilayer(Id);
      int gas_gap          = m_sTgcIdHelper->gasGap(Id);
      int channel          = m_sTgcIdHelper->channel(Id);
      int channel_type     = m_sTgcIdHelper->channelType(Id);

      ATH_MSG_DEBUG(     "small TGC RDO Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]"
                      << " ChType ["       << channel_type    << "]" );

      // module details down to the level of channel which is closest to the Geant4 hit
      // to be stored in the ntuple
      m_NSWsTGC_rdo_stationName->push_back(stName);
      m_NSWsTGC_rdo_stationEta->push_back(stationEta);
      m_NSWsTGC_rdo_stationPhi->push_back(stationPhi);
      m_NSWsTGC_rdo_multiplet->push_back(multiplet);
      m_NSWsTGC_rdo_gas_gap->push_back(gas_gap);
      m_NSWsTGC_rdo_channel->push_back(channel);
      m_NSWsTGC_rdo_channel_type->push_back(channel_type);
      m_NSWsTGC_rdo_time->push_back(rdo->time());
      m_NSWsTGC_rdo_charge->push_back(rdo->charge());
      m_NSWsTGC_rdo_bcTag->push_back(rdo->bcTag());
      m_NSWsTGC_rdo_isDead->push_back(rdo->isDead());

      // get the readout element class where the RDO is recorded
      int isSmall = stName[2] == 'S';
      const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet );

      Amg::Vector2D localStripPos(0.,0.);
      if ( rdoEl->stripPosition(Id,localStripPos) )  {
        m_NSWsTGC_rdo_localPosX->push_back(localStripPos.x());
        m_NSWsTGC_rdo_localPosY->push_back(localStripPos.y());
        ATH_MSG_DEBUG("sTGC RDO: local pos.:  x=" << localStripPos[0] << ",  y=" << localStripPos[1]);
      } else { 
        ATH_MSG_WARNING("sTGC RDO: local Strip position not defined"); 
      }

       // asking the detector element to transform this local to the global position
      Amg::Vector3D globalStripPos(0., 0., 0.);
      rdoEl->surface(Id).localToGlobal(localStripPos,Amg::Vector3D(0.,0.,0.),globalStripPos);
      m_NSWsTGC_rdo_globalPosX->push_back(globalStripPos.x());
      m_NSWsTGC_rdo_globalPosY->push_back(globalStripPos.y());
      m_NSWsTGC_rdo_globalPosZ->push_back(globalStripPos.z());

      // rdo counter for the ntuple
      m_NSWsTGC_nrdo++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_NSWsTGC_nrdo << " sTGC RDOs");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode sTGCRDOVariables::clearVariables()
{
  m_NSWsTGC_nrdo = 0;

  // information of the module down to the channel closest to the initial G4 hit
  // size of vector is m_NSWsTGC_rdo
  m_NSWsTGC_rdo_stationName->clear();
  m_NSWsTGC_rdo_stationEta->clear();
  m_NSWsTGC_rdo_stationPhi->clear();
  m_NSWsTGC_rdo_multiplet->clear();
  m_NSWsTGC_rdo_gas_gap->clear();
  m_NSWsTGC_rdo_channel->clear();
  m_NSWsTGC_rdo_channel_type->clear();
  m_NSWsTGC_rdo_time->clear();
  m_NSWsTGC_rdo_charge->clear();
  m_NSWsTGC_rdo_bcTag->clear();
  m_NSWsTGC_rdo_isDead->clear();

  m_NSWsTGC_rdo_globalPosX->clear();
  m_NSWsTGC_rdo_globalPosY->clear();
  m_NSWsTGC_rdo_globalPosZ->clear();

  m_NSWsTGC_rdo_localPosX->clear();
  m_NSWsTGC_rdo_localPosY->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode sTGCRDOVariables::initializeVariables()
{

  m_NSWsTGC_nrdo = 0;
  m_NSWsTGC_rdo_stationName = new std::vector<std::string>();
  m_NSWsTGC_rdo_stationEta  = new std::vector<int>();
  m_NSWsTGC_rdo_stationPhi  = new std::vector<int>();
  m_NSWsTGC_rdo_multiplet   = new std::vector<int>();
  m_NSWsTGC_rdo_gas_gap     = new std::vector<int>();
  m_NSWsTGC_rdo_channel     = new std::vector<int>();
  m_NSWsTGC_rdo_channel_type= new std::vector<int>();
  m_NSWsTGC_rdo_time        = new std::vector<double>();
  m_NSWsTGC_rdo_charge      = new std::vector<double>();
  m_NSWsTGC_rdo_bcTag       = new std::vector<uint16_t>();
  m_NSWsTGC_rdo_isDead      = new std::vector<bool>();

  m_NSWsTGC_rdo_localPosX   = new std::vector<double>();
  m_NSWsTGC_rdo_localPosY   = new std::vector<double>();

  m_NSWsTGC_rdo_globalPosX   = new std::vector<double>();
  m_NSWsTGC_rdo_globalPosY   = new std::vector<double>();
  m_NSWsTGC_rdo_globalPosZ   = new std::vector<double>();

  if(m_tree) {
    m_tree->Branch("RDO_sTGC_n",            &m_NSWsTGC_nrdo);
    m_tree->Branch("RDO_sTGC_stationName",  &m_NSWsTGC_rdo_stationName);
    m_tree->Branch("RDO_sTGC_stationEta",   &m_NSWsTGC_rdo_stationEta);
    m_tree->Branch("RDO_sTGC_stationPhi",   &m_NSWsTGC_rdo_stationPhi);
    m_tree->Branch("RDO_sTGC_multiplet",    &m_NSWsTGC_rdo_multiplet);
    m_tree->Branch("RDO_sTGC_gas_gap",      &m_NSWsTGC_rdo_gas_gap);
    m_tree->Branch("RDO_sTGC_channel",      &m_NSWsTGC_rdo_channel);
    m_tree->Branch("RDO_sTGC_channel_type", &m_NSWsTGC_rdo_channel_type);
    m_tree->Branch("RDO_sTGC_time",         &m_NSWsTGC_rdo_time);
    m_tree->Branch("RDO_sTGC_charge",       &m_NSWsTGC_rdo_charge);
    m_tree->Branch("RDO_sTGC_bcTag",        &m_NSWsTGC_rdo_bcTag);
    m_tree->Branch("RDO_sTGC_isDead",       &m_NSWsTGC_rdo_isDead);

    m_tree->Branch("RDO_sTGC_localPosX",   &m_NSWsTGC_rdo_localPosX);
    m_tree->Branch("RDO_sTGC_localPosY",   &m_NSWsTGC_rdo_localPosY);

    m_tree->Branch("RDO_sTGC_globalPosX",  &m_NSWsTGC_rdo_globalPosX);
    m_tree->Branch("RDO_sTGC_globalPosY",  &m_NSWsTGC_rdo_globalPosY);
    m_tree->Branch("RDO_sTGC_globalPosZ",  &m_NSWsTGC_rdo_globalPosZ);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
void sTGCRDOVariables::deleteVariables()
{
  delete m_NSWsTGC_rdo_stationName;
  delete m_NSWsTGC_rdo_stationEta;
  delete m_NSWsTGC_rdo_stationPhi;
  delete m_NSWsTGC_rdo_multiplet;
  delete m_NSWsTGC_rdo_gas_gap;
  delete m_NSWsTGC_rdo_channel;
  delete m_NSWsTGC_rdo_channel_type;
  delete m_NSWsTGC_rdo_time;
  delete m_NSWsTGC_rdo_charge;
  delete m_NSWsTGC_rdo_bcTag;
  delete m_NSWsTGC_rdo_isDead;
  delete m_NSWsTGC_rdo_localPosX;
  delete m_NSWsTGC_rdo_localPosY;
  delete m_NSWsTGC_rdo_globalPosX;
  delete m_NSWsTGC_rdo_globalPosY;
  delete m_NSWsTGC_rdo_globalPosZ;

  m_NSWsTGC_nrdo = 0;
  m_NSWsTGC_rdo_stationName = nullptr;
  m_NSWsTGC_rdo_stationEta = nullptr;
  m_NSWsTGC_rdo_stationPhi = nullptr;
  m_NSWsTGC_rdo_multiplet = nullptr;
  m_NSWsTGC_rdo_gas_gap = nullptr;
  m_NSWsTGC_rdo_channel = nullptr;
  m_NSWsTGC_rdo_channel_type = nullptr;
  m_NSWsTGC_rdo_time = nullptr;
  m_NSWsTGC_rdo_charge = nullptr;
  m_NSWsTGC_rdo_bcTag = nullptr;
  m_NSWsTGC_rdo_isDead = nullptr;
  m_NSWsTGC_rdo_localPosX = nullptr;
  m_NSWsTGC_rdo_localPosY = nullptr;
  m_NSWsTGC_rdo_globalPosX = nullptr;
  m_NSWsTGC_rdo_globalPosY = nullptr;
  m_NSWsTGC_rdo_globalPosZ = nullptr;

  return;
}
