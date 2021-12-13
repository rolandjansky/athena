/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCRDOVariables.h"
#include "AthenaKernel/errorcheck.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TTree.h"

using namespace Muon;

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */


CSCRDOVariables::CSCRDOVariables(StoreGateSvc* evtStore,
		const MuonGM::MuonDetectorManager* detManager,
		const MuonIdHelper* idhelper,
		TTree* tree,
	 	const std::string& containername,
	 	MSG::Level msglvl,     
   const Muon::ICSC_RDO_Decoder* rdo_decoder
     ) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_rdo_decoder{rdo_decoder}
   {
    setHelper(idhelper);
  
  }

StatusCode CSCRDOVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG("do fillCSCRDOVariables()");

  // clear variables
  CHECK( this->clearVariables() );

  const CscRawDataContainer* rdo_container = nullptr;
  ATH_CHECK( m_evtStore->retrieve(rdo_container, m_ContainerName) );

  if(rdo_container->size()==0) ATH_MSG_DEBUG(" CSC RDO Container empty ");

  for(const CscRawDataCollection* coll : *rdo_container) {
    int strip_num{0};
    for (const CscRawData* rdo: *coll) {
      const Identifier Id { m_rdo_decoder->channelIdentifier(rdo, m_CscIdHelper,strip_num)};
      ++strip_num;

      std::string stName   = m_CscIdHelper->stationNameString(m_CscIdHelper->stationName(Id));
      int stationEta       = m_CscIdHelper->stationEta(Id);
      int stationPhi       = m_CscIdHelper->stationPhi(Id);
      int channel          = m_CscIdHelper->channel(Id);
      int chamberLayer     = m_CscIdHelper->chamberLayer(Id);
      int wireLayer        = m_CscIdHelper->wireLayer(Id);
      int strip            = m_CscIdHelper->strip(Id);
      bool measuresPhi     = m_CscIdHelper->measuresPhi(Id);


      ATH_MSG_DEBUG(     "CSC RDO Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " channel ["      << channel         << "]"
                      << " chamberLayer [" << chamberLayer    << "]"
                      << " wireLayer ["    << wireLayer       << "]"
                      << " strip ["        << strip           << "]"
                      << " measuresPhi ["  << measuresPhi      << "]" );

      const MuonGM::CscReadoutElement* rdoEl = nullptr;
      try{
         rdoEl =  MuonDetMgr->getCscReadoutElement(Id);	   
      } catch (const std::runtime_error&) {
        ATH_MSG_WARNING("CSCRDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ <<" "<< m_CscIdHelper->print_to_string(Id));
        continue;
      }
      if (!rdoEl) {
          ATH_MSG_ERROR("CSCRDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ << m_CscIdHelper->print_to_string(Id));
          return StatusCode::FAILURE;
      }
      /// to be stored in the ntuple
      m_Csc_rdo_stationName.push_back(stName);
      m_Csc_rdo_stationEta.push_back(stationEta);
      m_Csc_rdo_stationPhi.push_back(stationPhi);
      m_Csc_rdo_channel.push_back(channel);
      m_Csc_rdo_chamberLayer.push_back(chamberLayer);
      m_Csc_rdo_wireLayer.push_back(wireLayer);
      m_Csc_rdo_strip.push_back(strip);
      m_Csc_rdo_measuresPhi.push_back(measuresPhi);
      m_Csc_rdo_time.push_back(rdo->time());
      
      Amg::Vector2D localPos(0.,0.);
      Amg::Vector3D globalPos(0., 0., 0.);

      rdoEl->surface(Id).localToGlobal(localPos,globalPos,globalPos);
      m_Csc_rdo_globalPosX.push_back(globalPos.x());
      m_Csc_rdo_globalPosY.push_back(globalPos.y());
      m_Csc_rdo_globalPosZ.push_back(globalPos.z());


      // rdo counter for the ntuple
      m_Csc_nrdo++;
    }
     // Local RDO position information loss after localToGlobal transformation, fill the local positions in another loop for retrieving the local positions
    for (const CscRawData* rdo: *coll) {
      const Identifier Id { m_rdo_decoder->channelIdentifier(rdo, m_CscIdHelper,strip_num)};
      ++strip_num;

      const MuonGM::CscReadoutElement* rdoEl = nullptr;
      try{
         rdoEl =  MuonDetMgr->getCscReadoutElement(Id);	   
      } catch (const std::runtime_error&) {
        ATH_MSG_WARNING("CSCRDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ <<" "<< m_CscIdHelper->print_to_string(Id));
        continue;
      }
      if (!rdoEl) {
          ATH_MSG_ERROR("CSCRDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for" << __FILE__ << __LINE__ << m_CscIdHelper->print_to_string(Id));
          return StatusCode::FAILURE;
      }

      Amg::Vector2D lPos(0.,0.);
      Amg::Vector3D gPos(0., 0., 0.);

      rdoEl->surface(Id).globalToLocal(gPos,gPos,lPos);
      m_Csc_rdo_localPosX.push_back(lPos.x());
      m_Csc_rdo_localPosY.push_back(lPos.y());
    }

  }

  ATH_MSG_DEBUG("processed " << m_Csc_nrdo << " csc rdo");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode CSCRDOVariables::clearVariables()
{
  m_Csc_nrdo = 0;

  m_Csc_rdo_stationName.clear();
  m_Csc_rdo_stationEta.clear();
  m_Csc_rdo_stationPhi.clear();
  m_Csc_rdo_channel.clear();
  m_Csc_rdo_chamberLayer.clear();
  m_Csc_rdo_wireLayer.clear();
  m_Csc_rdo_strip.clear();
  m_Csc_rdo_measuresPhi.clear();
  m_Csc_rdo_time.clear();
  m_Csc_rdo_localPosX.clear();
  m_Csc_rdo_localPosY.clear();
  m_Csc_rdo_globalPosX.clear();
  m_Csc_rdo_globalPosY.clear();
  m_Csc_rdo_globalPosZ.clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode CSCRDOVariables::initializeVariables()
{

  if(m_tree) {
    m_tree->Branch("RDO_CSC_n",             &m_Csc_nrdo);
    m_tree->Branch("RDO_CSC_stationName",   &m_Csc_rdo_stationName);
    m_tree->Branch("RDO_CSC_stationEta",    &m_Csc_rdo_stationEta);
    m_tree->Branch("RDO_CSC_stationPhi",    &m_Csc_rdo_stationPhi);
    m_tree->Branch("RDO_CSC_channel",       &m_Csc_rdo_channel);
    m_tree->Branch("RDO_CSC_chamberLayer",  &m_Csc_rdo_chamberLayer);
    m_tree->Branch("RDO_CSC_wireLayer",     &m_Csc_rdo_wireLayer);
    m_tree->Branch("RDO_CSC_strip",         &m_Csc_rdo_strip);
    m_tree->Branch("RDO_CSC_measuresPhi",   &m_Csc_rdo_measuresPhi);    
    m_tree->Branch("RDO_CSC_time",          &m_Csc_rdo_time);
    m_tree->Branch("RDO_CSC_localPosX",     &m_Csc_rdo_localPosX);
    m_tree->Branch("RDO_CSC_localPosY",     &m_Csc_rdo_localPosY);
    m_tree->Branch("RDO_CSC_globalPosX",    &m_Csc_rdo_globalPosX);
    m_tree->Branch("RDO_CSC_globalPosY",    &m_Csc_rdo_globalPosY);
    m_tree->Branch("RDO_CSC_globalPosZ",    &m_Csc_rdo_globalPosZ);

  }
  return StatusCode::SUCCESS;
}

void CSCRDOVariables::deleteVariables()
{
  return;
}