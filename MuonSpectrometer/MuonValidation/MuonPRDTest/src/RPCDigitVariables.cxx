#include <TString.h> // for Form
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
      int stationEta       = m_RpcIdHelper->stationEta(Id);
      int stationPhi       = m_RpcIdHelper->stationPhi(Id);
      int doubletR          = m_RpcIdHelper->doubletR(Id);
      int doubletZ          = m_RpcIdHelper->doubletZ(Id);
      int doubletPhi        = m_RpcIdHelper->doubletPhi(Id);
      int gas_gap          = m_RpcIdHelper->gasGap(Id);
      
      int stationEtaMin    = m_RpcIdHelper->stationEtaMin(Id);
      int stationEtaMax    = m_RpcIdHelper->stationEtaMax(Id);
      int stationPhiMin    = m_RpcIdHelper->stationPhiMin(Id);
      int stationPhiMax    = m_RpcIdHelper->stationPhiMax(Id);
      int doubletRMin       = m_RpcIdHelper->doubletRMin(Id);
      int doubletRMax       = m_RpcIdHelper->doubletRMax(Id);
      int doubletZMin       = m_RpcIdHelper->doubletZMin(Id);
      int doubletZMax       = m_RpcIdHelper->doubletZMax(Id);
      int doubletPhiMin     = m_RpcIdHelper->doubletPhiMin(Id);
      int doubletPhiMax     = m_RpcIdHelper->doubletPhiMax(Id);
      int gas_gapMin       = m_RpcIdHelper->gasGapMin(Id);
      int gas_gapMax       = m_RpcIdHelper->gasGapMax(Id);

      ATH_MSG_DEBUG(     "RPC Digit Offline id:  Station Name [" << stName << " ]"
                         << " Station Eta ["  << stationEta      << "]"
                         << " Station Phi ["  << stationPhi      << "]"
                         << " GasGap ["       << gas_gap         << "]"
                         << " Station EtaMin ["  << stationEtaMin      << "]"
                         << " Station EtaMax ["  << stationEtaMax      << "]"
                         << " Station PhiMin ["  << stationPhiMin      << "]"
                         << " Station PhiMax ["  << stationPhiMax      << "]");

      const MuonGM::RpcReadoutElement* rdoEl = MuonDetMgr->getRpcReadoutElement(Id);
      if (!rdoEl) return StatusCode::FAILURE;

      Amg::Vector3D gpos(0.,0.,0.);
      Amg::Vector2D lpos(0.,0.);

      rdoEl->stripPosition(Id,lpos);
      rdoEl->surface(Id).localToGlobal(lpos, gpos,gpos);
      
      m_RPC_dig_localPosX.push_back( lpos.x() );
      m_RPC_dig_localPosY.push_back( lpos.y() );
      m_RPC_dig_globalPosX.push_back( gpos.x() );
      m_RPC_dig_globalPosY.push_back( gpos.y() );
      m_RPC_dig_globalPosZ.push_back( gpos.z() );
      
      m_RPC_dig_stationName.push_back(stName);
      m_RPC_dig_stationEta.push_back(stationEta);
      m_RPC_dig_stationPhi.push_back(stationPhi);
      m_RPC_dig_gas_gap.push_back(gas_gap);
      m_RPC_dig_stationEtaMin.push_back(stationEtaMin);
      m_RPC_dig_stationEtaMax.push_back(stationEtaMax);
      m_RPC_dig_stationPhiMin.push_back(stationPhiMin);
      m_RPC_dig_stationPhiMax.push_back(stationPhiMax);
      m_RPC_dig_doubletR.push_back(doubletR);
      m_RPC_dig_doubletZ.push_back(doubletZ);
      m_RPC_dig_doubletPhiMax.push_back(doubletPhi);
      m_RPC_dig_doubletRMin.push_back(doubletRMin);
      m_RPC_dig_doubletRMax.push_back(doubletRMax);
      m_RPC_dig_doubletZMin.push_back(doubletZMin);
      m_RPC_dig_doubletZMax.push_back(doubletZMax);
      m_RPC_dig_doubletPhiMin.push_back(doubletPhiMin);
      m_RPC_dig_doubletPhiMax.push_back(doubletPhiMax);
      m_RPC_dig_gas_gapMin.push_back(gas_gapMin);
      m_RPC_dig_gas_gapMax.push_back(gas_gapMax);
      
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
  m_RPC_dig_stationName.clear();
  m_RPC_dig_stationEta.clear();
  m_RPC_dig_stationPhi.clear();
  m_RPC_dig_gas_gap.clear();
  
  m_RPC_dig_stationEtaMin.clear();
  m_RPC_dig_stationEtaMax.clear();
  m_RPC_dig_stationPhiMin.clear();
  m_RPC_dig_stationPhiMax.clear();
  m_RPC_dig_doubletRMin.clear();
  m_RPC_dig_doubletRMax.clear();
  m_RPC_dig_doubletZMin.clear();
  m_RPC_dig_doubletZMax.clear();
  m_RPC_dig_doubletPhiMin.clear();
  m_RPC_dig_doubletPhiMax.clear();
  m_RPC_dig_gas_gapMin.clear();
  m_RPC_dig_gas_gapMax.clear();

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
    m_tree->Branch("Digits_RPC_stationName", &m_RPC_dig_stationName);
    m_tree->Branch("Digits_RPC_stationEta",  &m_RPC_dig_stationEta);
    m_tree->Branch("Digits_RPC_stationPhi",  &m_RPC_dig_stationPhi);
    m_tree->Branch("Digits_RPC_gas_gap",     &m_RPC_dig_gas_gap);
    m_tree->Branch("Digits_RPC_stationEtaMin",  &m_RPC_dig_stationEtaMin);
    m_tree->Branch("Digits_RPC_stationEtaMax",  &m_RPC_dig_stationEtaMax);
    m_tree->Branch("Digits_RPC_stationPhiMin",  &m_RPC_dig_stationPhiMin);
    m_tree->Branch("Digits_RPC_stationPhiMax",  &m_RPC_dig_stationPhiMax);
    m_tree->Branch("Digits_RPC_doubletRmin",  &m_RPC_dig_doubletRMin);
    m_tree->Branch("Digits_RPC_doubletRMax",  &m_RPC_dig_doubletRMax);
    m_tree->Branch("Digits_RPC_doubletZmin",  &m_RPC_dig_doubletZMin);
    m_tree->Branch("Digits_RPC_doubletZMax",  &m_RPC_dig_doubletZMax);
    m_tree->Branch("Digits_RPC_doubletPhiMin",  &m_RPC_dig_doubletPhiMin);
    m_tree->Branch("Digits_RPC_doubletPhiMax",  &m_RPC_dig_doubletPhiMax);
    m_tree->Branch("Digits_RPC_gas_gapMin",  &m_RPC_dig_gas_gapMin);
    m_tree->Branch("Digits_RPC_gas_gapMax",  &m_RPC_dig_gas_gapMax);

    m_tree->Branch("Digits_RPC_localPosX", &m_RPC_dig_localPosX);
    m_tree->Branch("Digits_RPC_localPosY", &m_RPC_dig_localPosY);
    m_tree->Branch("Digits_RPC_globalPosX", &m_RPC_dig_globalPosX);
    m_tree->Branch("Digits_RPC_globalPosY",  &m_RPC_dig_globalPosY);
    m_tree->Branch("Digits_RPC_globalPosZ",  &m_RPC_dig_globalPosZ);
  }

  return StatusCode::SUCCESS;
}

