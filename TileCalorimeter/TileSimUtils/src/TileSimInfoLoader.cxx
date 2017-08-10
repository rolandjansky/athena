/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////
//
// TileSimInfoLoader
//
//	Service to pass some flags and values
//	from jobOptions to simulation
//
// author: <gia@mail.cern.ch>
// December, 2005
//
//////////////////////////////////////////////////

#include "StoreGate/StoreGateSvc.h"

#include  "TileSimUtils/TileG4SimOptions.h"
#include  "TileSimUtils/TileSimInfoLoader.h"


TileSimInfoLoader::TileSimInfoLoader(const std::string& name, ISvcLocator* pSvcLocator)
  : AthService(name, pSvcLocator)
  , m_detStore(0)
  , m_tileSimInfo(0)
  , m_deltaTHit(4)
  , m_timeCut(350.5)// need such huge value for pileup, otherwise 150.5 is enough
  , m_tileTB(false)
  , m_platesToCell(true)
  , m_Ushape(-1) // -1 means not set - use value from GeoModel DB
  , m_doBirk(true)
  , m_doTileRow(false)
  , m_doTOFCorrection(true)
{
  m_deltaTHit[0] = 0.5;  // 0.5 ns inside window
  m_deltaTHit[1] = -75.25;
  m_deltaTHit[2] = 75.25;
  m_deltaTHit[3] = 5.;  // 5 ns outside

  declareProperty("DeltaTHit",   m_deltaTHit);
  declareProperty("TimeCut",     m_timeCut);
  declareProperty("TileTB",      m_tileTB);
  declareProperty("PlateToCell", m_platesToCell);
  declareProperty("Ushape",      m_Ushape);
  declareProperty("DoBirk",      m_doBirk);
  declareProperty("DoTileRow",   m_doTileRow);
  declareProperty("DoTOFCorrection",m_doTOFCorrection);
}


TileSimInfoLoader::~TileSimInfoLoader()
{}


StatusCode TileSimInfoLoader::initialize() {

  ATH_MSG_INFO ( "Initializing " << name() );
  ATH_CHECK( AthService::initialize() );
  ATH_CHECK(  service("DetectorStore",m_detStore) );


  TileG4SimOptions* info = new TileG4SimOptions();
  m_tileSimInfo = info ;

  StatusCode sc = m_detStore->record(info,info->whoami());

  if (sc.isFailure()) {
    ATH_MSG_ERROR
      ( "Failed to register new TileG4SimOptions in the detector store." );
    delete info;

    //retrieve TileSimInfo from detector store.
    ATH_CHECK( m_detStore->retrieve(m_tileSimInfo) );
    ATH_MSG_ERROR ( "Updating existing TileG4SimOptions" );
  }

  //Change TileG4SimOptions by the values from jobOptions
  m_tileSimInfo->SetDeltaTHit(m_deltaTHit);
  m_tileSimInfo->SetTimeCut(m_timeCut);
  m_tileSimInfo->SetTileTBFlag(m_tileTB);
  m_tileSimInfo->SetPlateToCellFlag(m_platesToCell);
  m_tileSimInfo->SetUshape(m_Ushape);
  m_tileSimInfo->SetDoBirk(m_doBirk);
  m_tileSimInfo->SetDoTileRow(m_doTileRow);
  m_tileSimInfo->SetDoTOFCorrection(m_doTOFCorrection);
  
  ATH_MSG_INFO ( (std::string)(*m_tileSimInfo) );

  //log << MSG::INFO << "Locking TileG4SimOptions in StoreGate" << endmsg;
  //StatusCode sc = m_storeGate->setConst(m_tileSimInfo);

  return StatusCode::SUCCESS;
}


StatusCode TileSimInfoLoader::finalize() {
  ATH_MSG_INFO ( "TileSimInfoLoader::finalize()" );
  return Service::finalize();
}

