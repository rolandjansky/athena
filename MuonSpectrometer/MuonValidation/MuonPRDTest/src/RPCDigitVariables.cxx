/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigit.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonDigitContainer/RpcDigitCollection.h"

#include "TTree.h"
#include <TString.h> // for Form
/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode RpcDigitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
  ATH_MSG_DEBUG(" do fillRpcDigitVariables()");
  CHECK( this->clearVariables() );

  const RpcDigitContainer* RpcDigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(RpcDigitContainer, m_ContainerName.c_str()) );

  ATH_MSG_DEBUG("retrieved RPC Digit Container with size "<<RpcDigitContainer->digit_size());

  if(RpcDigitContainer->size()==0) ATH_MSG_DEBUG(" RPC Digit Continer empty ");
  for(const RpcDigitCollection* coll : *RpcDigitContainer) {
    ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
    for (unsigned int digitNum=0; digitNum<coll->size(); digitNum++) {
      const RpcDigit* digit = coll->at(digitNum);
      Identifier Id = digit->identify();

      std::string stName   = m_RpcIdHelper->stationNameString(m_RpcIdHelper->stationName(Id));
      int stationIndex  = m_RpcIdHelper->stationName(Id);
      int stationEta       = m_RpcIdHelper->stationEta(Id);
      int stationPhi       = m_RpcIdHelper->stationPhi(Id);
      int doubletR          = m_RpcIdHelper->doubletR(Id);
      int doubletZ          = m_RpcIdHelper->doubletZ(Id);
      int doubletPhi        = m_RpcIdHelper->doubletPhi(Id);
      int gas_gap          = m_RpcIdHelper->gasGap(Id);
      bool measuresPhi     = m_RpcIdHelper->measuresPhi(Id);
      int strip            = m_RpcIdHelper->strip(Id);
      
      ATH_MSG_DEBUG(     "RPC Digit Offline id:  Station Name [" << stName << " ]"
                         << " Station Eta ["  << stationEta      << "]"
                         << " Station Phi ["  << stationPhi      << "]"
                         << " GasGap ["       << gas_gap         << "]"
                         << " Strip ["         << strip        << "]");
      
      const MuonGM::RpcReadoutElement* rdoEl = MuonDetMgr->getRpcReadoutElement(Id);
      if (!rdoEl) return StatusCode::FAILURE;
      
      int stripNumber = 0;
      bool stripPosition = 0;
      Amg::Vector3D gpos(0.,0.,0.);
      Amg::Vector2D lpos(0.,0.);
      double localX{};
      double localY{};
      
      if ( !rdoEl->stripPosition(Id,lpos) ) {
            ATH_MSG_WARNING("RPCDigitVariables: failed to associate a valid local position for strip n. " << strip
                         << "; associated positions will be set to 0.0.");
          } else {
            localX = lpos.x();
            localY = lpos.y();
          }
      
      rdoEl->surface(Id).localToGlobal(lpos, gpos, gpos);
      stripNumber = rdoEl->stripNumber(lpos,Id);
      stripPosition = rdoEl->stripPosition(Id,lpos);
      
      m_RPC_dig_globalPosX.push_back( gpos.x() );
      m_RPC_dig_globalPosY.push_back( gpos.y() );
      m_RPC_dig_globalPosZ.push_back( gpos.z() );
      m_RPC_dig_localPosX.push_back(localX);
      m_RPC_dig_localPosY.push_back(localY);
      
      m_RPC_dig_stationName.push_back(stName);
      m_RPC_dig_stationIndex.push_back(stationIndex);
      m_RPC_dig_time.push_back(digit->time());
      m_RPC_dig_stationEta.push_back(stationEta);
      m_RPC_dig_stationPhi.push_back(stationPhi);
      m_RPC_dig_gas_gap.push_back(gas_gap);
      m_RPC_dig_doubletR.push_back(doubletR);
      m_RPC_dig_doubletZ.push_back(doubletZ);
      m_RPC_dig_doubletPhi.push_back(doubletPhi);
      m_RPC_dig_strip.push_back(strip); 
      m_RPC_dig_stripNumber.push_back(stripNumber);  
      m_RPC_dig_stripPosition.push_back(stripPosition);
      m_RPC_dig_measuresPhi.push_back(measuresPhi);
      
      m_RPC_nDigits++;
    }
  }
  ATH_MSG_DEBUG(" finished fillRpcDigitVariables()");
  return StatusCode::SUCCESS;
}

/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode RpcDigitVariables::clearVariables()
{
  m_RPC_nDigits = 0;
  m_RPC_dig_time.clear();
  m_RPC_dig_stationName.clear();
  m_RPC_dig_stationIndex.clear();
  m_RPC_dig_stationEta.clear();
  m_RPC_dig_stationPhi.clear();
  m_RPC_dig_gas_gap.clear();
  m_RPC_dig_doubletR.clear();
  m_RPC_dig_doubletZ.clear();
  m_RPC_dig_doubletPhi.clear();
  m_RPC_dig_strip.clear();   
  m_RPC_dig_stripNumber.clear();     
  m_RPC_dig_stripPosition.clear();  
  m_RPC_dig_measuresPhi.clear();  

  m_RPC_dig_localPosX.clear();
  m_RPC_dig_localPosY.clear();
  m_RPC_dig_globalPosX.clear();
  m_RPC_dig_globalPosY.clear();
  m_RPC_dig_globalPosZ.clear();

  return StatusCode::SUCCESS;
}

/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode RpcDigitVariables::initializeVariables()
{
  if(m_tree) {
    ATH_MSG_DEBUG("RPC digit:  init m_tree ");
    m_tree->Branch("Digits_RPC",              &m_RPC_nDigits, "Digits_RPC_n/i");
    m_tree->Branch("Digits_RPC_time", &m_RPC_dig_time); 
    m_tree->Branch("Digits_RPC_stationName", &m_RPC_dig_stationName);
    m_tree->Branch("Digits_RPC_stationIndex", &m_RPC_dig_stationIndex);
    m_tree->Branch("Digits_RPC_stationEta",  &m_RPC_dig_stationEta);
    m_tree->Branch("Digits_RPC_stationPhi",  &m_RPC_dig_stationPhi);
    m_tree->Branch("Digits_RPC_gas_gap",     &m_RPC_dig_gas_gap);
    m_tree->Branch("Digits_RPC_doubletR",     &m_RPC_dig_doubletR);
    m_tree->Branch("Digits_RPC_doubletZ",     &m_RPC_dig_doubletZ);
    m_tree->Branch("Digits_RPC_doubletPhi",     &m_RPC_dig_doubletPhi);
    m_tree->Branch("Digits_RPC_strip",  &m_RPC_dig_strip);
    m_tree->Branch("Digits_RPC_stripNumber",  &m_RPC_dig_stripNumber);
    m_tree->Branch("Digits_RPC_stripPosition",  &m_RPC_dig_stripPosition);
    m_tree->Branch("Digits_RPC_measuresPhi",  &m_RPC_dig_measuresPhi);
    
    m_tree->Branch("Digits_RPC_localPosX", &m_RPC_dig_localPosX);
    m_tree->Branch("Digits_RPC_localPosY", &m_RPC_dig_localPosY);
    m_tree->Branch("Digits_RPC_globalPosX", &m_RPC_dig_globalPosX);
    m_tree->Branch("Digits_RPC_globalPosY",  &m_RPC_dig_globalPosY);
    m_tree->Branch("Digits_RPC_globalPosZ",  &m_RPC_dig_globalPosZ);
  }

  return StatusCode::SUCCESS;
}

