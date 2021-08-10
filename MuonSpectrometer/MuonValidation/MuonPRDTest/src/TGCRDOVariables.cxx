/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCRDOVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonRDO/TgcRdoContainer.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "TTree.h"

using namespace Muon;

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode TGCRDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillTGCRDOVariables()");

  // clear variables
  CHECK( this->clearVariables() );

  const TgcRdoContainer* rdo_container = nullptr;

  CHECK( m_evtStore->retrieve(rdo_container, m_ContainerName.c_str()) );

  if(rdo_container->size()==0) ATH_MSG_WARNING(" TGC RDO Container empty ");

  for(const TgcRdo* coll : *rdo_container) {

    for (const TgcRawData* rdo: *coll) {

      bool orFlag = m_tgcCabling->isOredChannel(rdo->subDetectorId(),
                                                 rdo->rodId(),
                                                 rdo->sswId(),
                                                 rdo->slbId(),
                                                 rdo->bitpos());

      Identifier Id;

      bool e_found = m_tgcCabling->getElementIDfromReadoutID(Id,
                                   rdo->subDetectorId(),
                                   rdo->rodId(),
                                   rdo->sswId(),
                                   rdo->slbId(),
                                   rdo->bitpos(), 
                                   orFlag);

      if (!e_found) {
        ATH_MSG_ERROR("could not find Identifier from TgcRawData");
        return StatusCode::FAILURE;
      }

      std::string stName   = m_TgcIdHelper->stationNameString(m_TgcIdHelper->stationName(Id));
      int stationEta       = m_TgcIdHelper->stationEta(Id);
      int stationPhi       = m_TgcIdHelper->stationPhi(Id);
      int gas_gap          = m_TgcIdHelper->gasGap(Id);
      int isStrip          = m_TgcIdHelper->isStrip(Id);
      int channel          = m_TgcIdHelper->channel(Id);


      ATH_MSG_DEBUG(     "MicroMegas RDO Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " isStrip ["      << isStrip         << "]"
                      << " ChNr ["         << channel         << "]" );

      // to be stored in the ntuple
      m_TGC_rdo_stationName.push_back(stName);
      m_TGC_rdo_stationEta.push_back(stationEta);
      m_TGC_rdo_stationPhi.push_back(stationPhi);
      m_TGC_rdo_gas_gap.push_back(gas_gap);
      m_TGC_rdo_isStrip.push_back(isStrip);
      m_TGC_rdo_channel.push_back(channel);

      const MuonGM::TgcReadoutElement* rdoEl = MuonDetMgr->getTgcReadoutElement(Id);
	  if (!rdoEl) {
        ATH_MSG_ERROR("TGCRDOVariables::fillVariables() - Failed to retrieve TgcReadoutElement for" << __FILE__ << __LINE__ << m_TgcIdHelper->print_to_string(Id).c_str());
        return StatusCode::FAILURE;
      }

      Amg::Vector2D localStripPos(0.,0.);
      if ( rdoEl->stripPosition(Id,localStripPos) )  {
        m_TGC_rdo_localPosX.push_back(localStripPos.x());
        m_TGC_rdo_localPosY.push_back(localStripPos.y());
        ATH_MSG_DEBUG("TGC RDO: local pos.:  x=" << localStripPos[0] << ",  y=" << localStripPos[1]);
      } else { 
        ATH_MSG_WARNING("TGC RDO: local Strip position not defined"); 
      }
      
      // asking the detector element to transform this local to the global position
      Amg::Vector3D globalStripPos(0., 0., 0.);
      rdoEl->surface(Id).localToGlobal(localStripPos,Amg::Vector3D(0.,0.,0.),globalStripPos);
      m_TGC_rdo_globalPosX.push_back(globalStripPos.x());
      m_TGC_rdo_globalPosY.push_back(globalStripPos.y());
      m_TGC_rdo_globalPosZ.push_back(globalStripPos.z());

      // rdo counter for the ntuple
      m_TGC_nrdo++;
    }
  }

  ATH_MSG_DEBUG("processed " << m_TGC_nrdo << " MicroMegas rdo");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode TGCRDOVariables::clearVariables()
{
  m_TGC_nrdo = 0;

  m_TGC_rdo_stationName.clear();
  m_TGC_rdo_stationEta.clear();
  m_TGC_rdo_stationPhi.clear();
  m_TGC_rdo_gas_gap.clear();
  m_TGC_rdo_isStrip.clear();
  m_TGC_rdo_channel.clear();
  m_TGC_rdo_localPosX.clear();
  m_TGC_rdo_localPosY.clear();
  m_TGC_rdo_globalPosX.clear();
  m_TGC_rdo_globalPosY.clear();
  m_TGC_rdo_globalPosZ.clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode TGCRDOVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("RDO_TGC_n",           &m_TGC_nrdo);
    m_tree->Branch("RDO_TGC_stationName", &m_TGC_rdo_stationName);
    m_tree->Branch("RDO_TGC_stationEta",  &m_TGC_rdo_stationEta);
    m_tree->Branch("RDO_TGC_stationPhi",  &m_TGC_rdo_stationPhi);
    m_tree->Branch("RDO_TGC_gas_gap",     &m_TGC_rdo_gas_gap);
    m_tree->Branch("RDO_TGC_isStrip",     &m_TGC_rdo_isStrip);
    m_tree->Branch("RDO_TGC_channel",     &m_TGC_rdo_channel);
    m_tree->Branch("RDO_TGC_localPosX",   &m_TGC_rdo_localPosX);
    m_tree->Branch("RDO_TGC_localPosY",   &m_TGC_rdo_localPosY);
    m_tree->Branch("RDO_TGC_globalPosX",  &m_TGC_rdo_globalPosX);
    m_tree->Branch("RDO_TGC_globalPosY",  &m_TGC_rdo_globalPosY);
    m_tree->Branch("RDO_TGC_globalPosZ",  &m_TGC_rdo_globalPosZ);
    
  }
  return StatusCode::SUCCESS;
}

TGCRDOVariables::TGCRDOVariables(StoreGateSvc* evtStore,
		const MuonGM::MuonDetectorManager* detManager,
		const MuonIdHelper* idhelper,
    const ITGCcablingSvc* cabling_svc,
		TTree* tree,
	 	const std::string& containername,
	 	MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_tgcCabling{cabling_svc}  {
    setHelper(idhelper);

  }