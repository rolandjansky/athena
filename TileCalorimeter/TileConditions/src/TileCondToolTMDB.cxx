/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// Tile includes
#include "TileConditions/TileCondToolTMDB.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"


//
//____________________________________________________________________
TileCondToolTMDB::TileCondToolTMDB(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<ITileCondToolTMDB>(this);
  declareInterface<TileCondToolTMDB>(this);
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
  ATH_CHECK( m_calibThresholdKey.initialize() );
  ATH_CHECK( m_calibDelayKey.initialize() );
  ATH_CHECK( m_calibTmfKey.initialize() );
  ATH_CHECK( m_calibDataKey.initialize() );

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
float TileCondToolTMDB::getThreshold(unsigned int drawerIdx, unsigned int threshold) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibThreshold(m_calibThresholdKey);
  return calibThreshold->getCalibDrawer(drawerIdx)->getData(threshold, 0, 0);

}


//
//____________________________________________________________________
float TileCondToolTMDB::getDelay(unsigned int drawerIdx, unsigned int channel) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibDelay(m_calibDelayKey);
  return calibDelay->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);

}

//
//____________________________________________________________________
void TileCondToolTMDB::getCalib(unsigned int drawerIdx, unsigned int channel, float& a, float& b) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibData(m_calibDataKey);
  a = calibData->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);
  b = calibData->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);

}


//
//____________________________________________________________________
unsigned int TileCondToolTMDB::getWeights(unsigned int drawerIdx, unsigned int channel, TMDB::Weights& weights) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibTMF(m_calibTmfKey);
  unsigned int nWeights = calibTMF->getCalibDrawer(drawerIdx)->getObjSizeUint32();

  if (weights.size() < nWeights ) {
    ATH_MSG_ERROR("Not enough space in output array to put all weights from DB (nothing to be done!): "
                  << weights.size() << " < " << nWeights);
    return 0;
  }

  for (unsigned int iWeight = 0; iWeight < nWeights; ++iWeight) {
    weights[iWeight] = calibTMF->getCalibDrawer(drawerIdx)->getData(channel, 0, iWeight);
  }

  return nWeights;
}




//
//____________________________________________________________________
float TileCondToolTMDB::channelCalib(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibData(m_calibDataKey);
  return amplitude * calibData->getCalibDrawer(drawerIdx)->getData(channel, 0, 0)
    + calibData->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);

}


//
//____________________________________________________________________
float TileCondToolTMDB::channelCalib(unsigned int drawerIdx, unsigned int channel, const std::vector<float>& samples) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibTMF(m_calibTmfKey);

  if (samples.size() != calibTMF->getCalibDrawer(drawerIdx)->getObjSizeUint32()) {
    ATH_MSG_ERROR("Number of samples and weights in DB are not compatible (nothing to be done!): "
                  << samples.size() << " =! " << calibTMF->getCalibDrawer(drawerIdx)->getObjSizeUint32());
    return 0;
  }

  float amplitude(0.0F);
  unsigned int iWeight(0);
  for (float sample : samples) {
    amplitude += sample * calibTMF->getCalibDrawer(drawerIdx)->getData(channel, 0, iWeight);
    ++iWeight;
  }

  return channelCalib(drawerIdx, channel, amplitude);
}
