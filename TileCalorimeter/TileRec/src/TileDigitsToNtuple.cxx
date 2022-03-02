/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsToNtuple.cxx
//  Author   : 
//  Created  : 
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h"

// Calo include
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

//TileCalo include
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileDigitsContainer.h"  
#include "TileRec/TileDigitsToNtuple.h"
#include "TileIdentifier/TileHWID.h"


TileDigitsToNtuple::TileDigitsToNtuple(const std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileTBID(0)
  , m_ntuplePtr(0)
  , m_nSamples(7)
{
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsFlt");    
  declareProperty("NTupleLoc", m_ntupleLoc = "/TILE/TileRec");
  declareProperty("NTupleID", m_ntupleID = "h40");
  declareProperty("CommitNtuple", m_commitNtuple = true);
  declareProperty("TileInfoName", m_infoName = "TileInfo");
  declareProperty("SaveMaxChannels", m_saveMaxChannels = 12288);
  declareProperty("SaveAll", m_saveAll = true);
  declareProperty("SaveE4prAndMBTS", m_saveE4prAndMBTS = true);
}

TileDigitsToNtuple::~TileDigitsToNtuple()
{
}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileDigitsToNtuple::initialize() {
   
  ATH_MSG_INFO( "Initialization started" );

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );
  CHECK( detStore()->retrieve(m_tileTBID) );

  const TileInfo* tileInfo = nullptr;
  CHECK( detStore()->retrieve(tileInfo, m_infoName) );
  m_nSamples = tileInfo->NdigitSamples(); // number of time slices for each chan

  m_ntupleLoc = "/NTUPLES" + m_ntupleLoc;
  std::string ntupleCompleteID = m_ntupleLoc + "/" + m_ntupleID;
  NTuplePtr nt(ntupleSvc(), ntupleCompleteID);

  if ( nt ) { // Check if already booked
    m_ntuplePtr = nt;
    ATH_MSG_INFO( "Reaccessing ntuple " << ntupleCompleteID );
  } else {
    m_ntuplePtr = ntupleSvc()->book(ntupleCompleteID, CLID_ColumnWiseTuple, "TileDigits-Ntuple");
    if (m_ntuplePtr) {
      
      CHECK( m_ntuplePtr->addItem("TileDigits/n_channels", m_nChannel, 0, m_saveMaxChannels) );

      CHECK( m_ntuplePtr->addItem("TileDigits/ros", m_nChannel, m_ros, 0, 4) );
      CHECK( m_ntuplePtr->addItem("TileDigits/drawer", m_nChannel, m_drawer, 0, 63) );
      CHECK( m_ntuplePtr->addItem("TileDigits/channel", m_nChannel, m_channel, 0, 47) );
      CHECK( m_ntuplePtr->addItem("TileDigits/gain", m_nChannel, m_gain, 0, 1) );

      CHECK( m_ntuplePtr->addItem("TileDigits/side", m_nChannel, m_side, -1, 1) );
      CHECK( m_ntuplePtr->addItem("TileDigits/section", m_nChannel, m_section, 0, 15) );
      CHECK( m_ntuplePtr->addItem("TileDigits/phi", m_nChannel, m_phi, 0, 63) );
      CHECK( m_ntuplePtr->addItem("TileDigits/eta", m_nChannel, m_eta, 0, 15) );

      CHECK( m_ntuplePtr->addItem("TileDigits/samples", m_nChannel, m_samples, m_nSamples) );

      if (!m_saveAll && !m_saveE4prAndMBTS) {
        CHECK( m_ntuplePtr->addItem("TileDigits/sample", m_nChannel, m_sample, 0, 15) );
        CHECK( m_ntuplePtr->addItem("TileDigits/pmt", m_nChannel, m_pmt, 0, 1) );
        CHECK( m_ntuplePtr->addItem("TileDigits/adc", m_nChannel, m_adc, 0, 1) );
      }
      
    } else {
      ATH_MSG_ERROR( "Failed to book or to retrieve ntuple " << ntupleCompleteID );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Finished booking ntuple " << ntupleCompleteID );
  }

  ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileDigitsToNtuple::execute() {

  // step1: read TileDigitss from TDS
  const TileDigitsContainer* digitsContainer; 
  CHECK( evtStore()->retrieve(digitsContainer, m_digitsContainer) );

  m_nChannel = 0;

  SelectAllObject<TileDigitsContainer> allTileDigits(digitsContainer);

  for (const TileDigits* tile_digits : allTileDigits) {

    HWIdentifier adc_hwid = tile_digits->adc_HWID();
    Identifier adc_id = tile_digits->adc_ID();

    std::vector<float> samples = tile_digits->samples();
    int n_samples = samples.size();
    
    if (m_saveAll) {
      m_ros[m_nChannel] = m_tileHWID->ros(adc_hwid);
      m_drawer[m_nChannel] = m_tileHWID->drawer(adc_hwid);
      m_channel[m_nChannel] = m_tileHWID->channel(adc_hwid);
      m_gain[m_nChannel] = m_tileHWID->adc(adc_hwid);

      if (m_tileTBID->is_tiletb(adc_id)) {
        m_section[m_nChannel] = m_tileTBID->section(adc_id);
        m_side[m_nChannel] = m_tileTBID->side(adc_id);
        m_phi[m_nChannel] = m_tileTBID->phi(adc_id);
        m_eta[m_nChannel] = m_tileTBID->eta(adc_id);
      } else {
        m_section[m_nChannel] = m_tileID->section(adc_id);
        m_side[m_nChannel] = m_tileID->side(adc_id);
        m_phi[m_nChannel] = m_tileID->module(adc_id);
        m_eta[m_nChannel] = m_tileID->tower(adc_id);
      }

      for (int i = 0; i < n_samples && i < m_nSamples; ++i) m_samples[m_nChannel][i] = samples[i];

      ++m_nChannel;

    } else if (m_saveE4prAndMBTS) {
      if (m_tileTBID->is_tiletb(adc_id)) {
        m_ros[m_nChannel] = m_tileHWID->ros(adc_hwid);
        m_drawer[m_nChannel] = m_tileHWID->drawer(adc_hwid);
        m_channel[m_nChannel] = m_tileHWID->channel(adc_hwid);
        m_gain[m_nChannel] = m_tileHWID->adc(adc_hwid);

        m_section[m_nChannel] = m_tileTBID->section(adc_id);
        m_side[m_nChannel] = m_tileTBID->side(adc_id);
        m_phi[m_nChannel] = m_tileTBID->phi(adc_id);
        m_eta[m_nChannel] = m_tileTBID->eta(adc_id);

        for (int i = 0; i < n_samples && i < m_nSamples; ++i) m_samples[m_nChannel][i] = samples[i];

        ++m_nChannel;
      }
    } else {
      if (!m_tileTBID->is_tiletb(adc_id)) {
        m_ros[m_nChannel] = m_tileHWID->ros(adc_hwid);
        m_drawer[m_nChannel] = m_tileHWID->drawer(adc_hwid);
        m_channel[m_nChannel] = m_tileHWID->channel(adc_hwid);
        m_gain[m_nChannel] = m_tileHWID->adc(adc_hwid);

        m_section[m_nChannel] = m_tileID->section(adc_id);
        m_side[m_nChannel] = m_tileID->side(adc_id);
        m_phi[m_nChannel] = m_tileID->module(adc_id);
        m_eta[m_nChannel] = m_tileID->tower(adc_id);

        m_sample[m_nChannel] = m_tileID->sample(adc_id);
        m_pmt[m_nChannel] = m_tileID->pmt(adc_id);
        m_adc[m_nChannel] = m_tileID->adc(adc_id);

        for (int i = 0; i < n_samples && i < m_nSamples; ++i) m_samples[m_nChannel][i] = samples[i];

        ++m_nChannel;
      }
    }

    if (m_nChannel >= m_saveMaxChannels) {
      ATH_MSG_DEBUG( "Number of channels exceeds maximum (" << m_saveMaxChannels << "), ignore all the rest" );
      break;
    }
  }


  if ( m_commitNtuple ) {
    ATH_MSG_DEBUG( "Committing Ntuple" );
    CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  }

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode TileDigitsToNtuple::finalize() {
  ATH_MSG_INFO( "finalize() completed successfully" );
  return StatusCode::SUCCESS;
}

