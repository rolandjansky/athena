/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"

// Tile includes
#include "TileConditions/TileCondToolMuID.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"


//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolMuID("TileCondToolMuID", 1, 0);
const InterfaceID&
TileCondToolMuID::interfaceID() {
  return IID_TileCondToolMuID;
}

//
//____________________________________________________________________
TileCondToolMuID::TileCondToolMuID(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
  , m_caloID(0)
  , m_pryMuID( "TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_MuID", this)
{
  declareInterface<TileCondToolMuID>(this);
  declareProperty("ProxyMuID", m_pryMuID);
}

//
//____________________________________________________________________
TileCondToolMuID::~TileCondToolMuID() {
}

//
//____________________________________________________________________
StatusCode TileCondToolMuID::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== retrieve proxy
  CHECK( m_pryMuID.retrieve() );

  // Retrieve CaloIdManager
  CHECK( detStore()->retrieve(m_caloID) );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolMuID::finalize() {
  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
void TileCondToolMuID::getLowThreshold(Identifier & id, int & ros, int & module, int & index) {

  module = m_caloID->module(id);

  if (m_caloID->is_tile_barrel(id) && m_caloID->is_tile_positive(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id);
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) + 10;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 19;
    }

    ros = 1;

  }

  if (m_caloID->is_tile_barrel(id) && m_caloID->is_tile_negative(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id);
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) + 10;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 19;
    }

    ros = 2;

  }

  if (m_caloID->is_tile_extbarrel(id) && m_caloID->is_tile_positive(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id) - 11;
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) - 4;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 7;
    }

    ros = 3;

  }

  if (m_caloID->is_tile_gap(id) && m_caloID->is_tile_positive(id)) {

    if (m_caloID->sample(id) == 1) {
      index = 5;
    }

    if (m_caloID->sample(id) == 2) {
      index = 11;
    }

    ros = 3;

  }

  if (m_caloID->is_tile_extbarrel(id) && m_caloID->is_tile_negative(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id) - 11;
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) - 4;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 7;
    }

    ros = 4;

  }

  if (m_caloID->is_tile_gap(id) && m_caloID->is_tile_negative(id)) {

    if (m_caloID->sample(id) == 1) {
      index = 5;
    }

    if (m_caloID->sample(id) == 2) {
      index = 11;
    }

    ros = 4;

  }

}

//
//____________________________________________________________________
void TileCondToolMuID::getHighThreshold(Identifier & id, int & ros, int & module, int & index) {

  index = ros = -999;
  module = m_caloID->module(id);

  if (m_caloID->is_tile_barrel(id) && m_caloID->is_tile_positive(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id) + 23;
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) + 10 + 23;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 19 + 23;
    }

    ros = 1;

  }

  if (m_caloID->is_tile_barrel(id) && m_caloID->is_tile_negative(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id) + 23;
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) + 10 + 23;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 19 + 23;
    }

    ros = 2;

  }

  if (m_caloID->is_tile_extbarrel(id) && m_caloID->is_tile_positive(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id) - 11 + 14;
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) - 4 + 14;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 7 + 14;
    }

    ros = 3;

  }

  if (m_caloID->is_tile_gap(id) && m_caloID->is_tile_positive(id)) {

    if (m_caloID->sample(id) == 1) {
      index = 5 + 14;
    }

    if (m_caloID->sample(id) == 2) {
      index = 11 + 14;
    }

    ros = 3;

  }

  if (m_caloID->is_tile_extbarrel(id) && m_caloID->is_tile_negative(id)) {

    if (m_caloID->sample(id) == 0) {
      index = m_caloID->tower(id) - 11 + 14;
    }

    if (m_caloID->sample(id) == 1) {
      index = m_caloID->tower(id) - 4 + 14;
    }

    if (m_caloID->sample(id) == 2) {
      index = m_caloID->tower(id) / 2 + 7 + 14;
    }

    ros = 4;

  }

  if (m_caloID->is_tile_gap(id) && m_caloID->is_tile_negative(id)) {

    if (m_caloID->sample(id) == 1) {
      index = 5 + 14;
    }

    if (m_caloID->sample(id) == 2) {
      index = 11 + 14;
    }

    ros = 4;

  }

}

//
//____________________________________________________________________
float TileCondToolMuID::getLowThresholdValue(Identifier id) {
  int ros, module, index, blob_size;
  getLowThreshold(id, ros, module, index);
  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, module);
  blob_size = m_pryMuID->getCalibDrawer(drawerIdx)->getObjSizeUint32();
  if (index >= blob_size) {
    ATH_MSG_ERROR( "getLowThresholdValue: index out of range! ->  ros/mod/blob_size/ind "
                  << ros << " " << module << " " << blob_size << " " << index );
    return -999.;
  } else {
    ATH_MSG_DEBUG( "getLowThresholdValue: ros/mod/drawer_ind/blob_size/ind " << ros << " "
                  << module << " " << drawerIdx << " " << blob_size << " " << index );

    return m_pryMuID->getCalibDrawer(drawerIdx)->getData(0, 0, index);
  }
}

//
//____________________________________________________________________
float TileCondToolMuID::getHighThresholdValue(Identifier id) {
  int ros, module, index, blob_size;
  getHighThreshold(id, ros, module, index);
  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, module);
  blob_size = m_pryMuID->getCalibDrawer(drawerIdx)->getObjSizeUint32();
  if (index >= blob_size) {
    ATH_MSG_ERROR( "getHighThresholdValue: index out of range! ->  ros/mod/blob_size/ind "
                  << ros << " " << module << " " << blob_size << " " << index );

    return -999.;
  } else {
    ATH_MSG_DEBUG( "getHighThresholdValue: ros/mod/drawer_ind/blob_size/ind " << ros << " "
                  << module << " " << drawerIdx << " " << blob_size << " " << index );

    return m_pryMuID->getCalibDrawer(drawerIdx)->getData(0, 0, index);
  }
}
