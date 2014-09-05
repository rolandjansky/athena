/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileID.h"

// Tile includes
#include "TileConditions/TileCondIdTransforms.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"


//
//____________________________________________________________________
static const InterfaceID IID_TileCondIdTransforms("TileCondIdTransforms", 1, 0);

const InterfaceID& TileCondIdTransforms::interfaceID() {
  return IID_TileCondIdTransforms;
}

//
//_____________________________________________________________________________
TileCondIdTransforms::TileCondIdTransforms(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileHWID(0)
    , m_tileID(0)
{
  declareInterface<TileCondIdTransforms>(this);
}

//
//_____________________________________________________________________________
StatusCode TileCondIdTransforms::initialize() {

  ATH_MSG_DEBUG( "in initialize()" );

  //=== TileHWID
  CHECK( detStore()->retrieve(m_tileHWID) );

  //=== TileID
  CHECK( detStore()->retrieve(m_tileID) );

  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
StatusCode TileCondIdTransforms::finalize() {
  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
void TileCondIdTransforms::getIndices(const HWIdentifier &hwid
                                      , unsigned int& ros
                                      , unsigned int& drawer
                                      , unsigned int& channel
                                      , unsigned int& adc) const {

  ros = m_tileHWID->ros(hwid);
  drawer = m_tileHWID->drawer(hwid);
  channel = m_tileHWID->channel(hwid);
  adc = m_tileHWID->adc(hwid);
}

//
//_____________________________________________________________________________
void TileCondIdTransforms::getIndices(const HWIdentifier &hwid
                                      , unsigned int& drawerIdx
                                      , unsigned int& channel
                                      , unsigned int& adc) const {

  unsigned int ros = m_tileHWID->ros(hwid);
  unsigned int drawer = m_tileHWID->drawer(hwid);
  drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  channel = m_tileHWID->channel(hwid);
  adc = m_tileHWID->adc(hwid);
}

//
//_____________________________________________________________________________
void TileCondIdTransforms::getIndices(const HWIdentifier &hwid
                                      , unsigned int& drawerIdx
                                      , unsigned int& channel) const {

  unsigned int ros = m_tileHWID->ros(hwid);
  unsigned int drawer = m_tileHWID->drawer(hwid);
  drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
  channel = m_tileHWID->channel(hwid);
}

//
//_____________________________________________________________________________
void TileCondIdTransforms::getIndices(IdentifierHash hash_id
                                      , unsigned int& drawerIdx
                                      , unsigned int& channel) const {

  if (hash_id < m_tileHWID->channel_hash_max()) {
    HWIdentifier hwid = m_tileHWID->channel_id(hash_id);
    unsigned int ros = m_tileHWID->ros(hwid);
    unsigned int drawer = m_tileHWID->drawer(hwid);
    drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    channel = m_tileHWID->channel(hwid);
  } else {
    drawerIdx = 0;
    channel = 0;
  }

}

