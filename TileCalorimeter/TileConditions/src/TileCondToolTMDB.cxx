/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TileConditions/TileCondToolTMDB.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"



//
//____________________________________________________________________
TileCondToolTMDB::TileCondToolTMDB(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_pryThreshold("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_Threshold", this)
  , m_pryDelay("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_Delay", this)
  , m_pryTMF("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_TMF", this)
  , m_pryCalib("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_Calib", this)
{
  declareInterface<ITileCondToolTMDB>(this);
  declareInterface<TileCondToolTMDB>(this);
  declareProperty("ProxyThreshold", m_pryThreshold);
  declareProperty("ProxyDelay", m_pryDelay);
  declareProperty("ProxyTMF", m_pryTMF);
  declareProperty("ProxyCalib", m_pryCalib);
}

//
//____________________________________________________________________
TileCondToolTMDB::~TileCondToolTMDB() {
}

//
//____________________________________________________________________
StatusCode TileCondToolTMDB::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== retrieve proxy
  CHECK( m_pryThreshold.retrieve() );
  CHECK( m_pryDelay.retrieve() );
  CHECK( m_pryTMF.retrieve() );
  CHECK( m_pryCalib.retrieve() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolTMDB::finalize() {
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}


//
//____________________________________________________________________
float TileCondToolTMDB::getThreshold(unsigned int drawerIdx, TMDB::THRESHOLD threshold) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolTMDB::getThreshold", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryThreshold->getCalibDrawer(drawerIdx)->getData(threshold, 0, 0);
}


//
//____________________________________________________________________
float TileCondToolTMDB::getDelay(unsigned int drawerIdx, TMDB::CHANNEL channel) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolTMDB::getDelay", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryDelay->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);
}

//
//____________________________________________________________________
void TileCondToolTMDB::getCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, float& a, float& b) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolTMDB::getCalib", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  a = m_pryCalib->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);
  b = m_pryCalib->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);
}


//
//____________________________________________________________________
unsigned int TileCondToolTMDB::getWeights(unsigned int drawerIdx, TMDB::CHANNEL channel, TMDB::Weights& weights) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolTMDB::getWeights", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  unsigned int nWeights = m_pryTMF->getCalibDrawer(drawerIdx)->getObjSizeUint32();

  if (weights.size() < nWeights ) {
    ATH_MSG_ERROR("Not enough space in output array to put all weights from DB (nothing to be done!): " 
                  << weights.size() << " < " << nWeights);
    return 0;
  }

  for (unsigned int iWeight = 0; iWeight < nWeights; ++iWeight) {
    weights[iWeight] = m_pryTMF->getCalibDrawer(drawerIdx)->getData(channel, 0, iWeight);
  }
  

  return nWeights;
}




//
//____________________________________________________________________
float TileCondToolTMDB::channelCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, float amplitude) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolTMDB::channelCalib", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryCalib->getCalibDrawer(drawerIdx)->getData(channel, 0, 0) 
    + amplitude * m_pryCalib->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);
}


//
//____________________________________________________________________
float TileCondToolTMDB::channelCalib(unsigned int drawerIdx, TMDB::CHANNEL channel, const std::vector<float>& samples) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolTMDB::channelCalib", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  if (samples.size() != m_pryTMF->getCalibDrawer(drawerIdx)->getObjSizeUint32()) {
    ATH_MSG_ERROR("Number of samples and weights in DB are not compatible (nothing to be done!): " 
                  << samples.size() << " =! " << m_pryTMF->getCalibDrawer(drawerIdx)->getObjSizeUint32());
    return 0;
  }

  float amplitude(0.0F);
  unsigned int iWeight(0);
  for (float sample : samples) {
    amplitude += sample * m_pryTMF->getCalibDrawer(drawerIdx)->getData(channel, 0, iWeight);
    ++iWeight;
  }

  return channelCalib(drawerIdx, channel, amplitude);
}



