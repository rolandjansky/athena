/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TileConditions/TileCondToolDspThreshold.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"



//
//____________________________________________________________________
TileCondToolDspThreshold::TileCondToolDspThreshold(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_pryDspThreshold("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_Threshold", this)
{
  declareInterface<ITileCondToolDspThreshold>(this);
  declareInterface<TileCondToolDspThreshold>(this);
  declareProperty("ProxyDspThreshold", m_pryDspThreshold);
}

//
//____________________________________________________________________
TileCondToolDspThreshold::~TileCondToolDspThreshold() {
}

//
//____________________________________________________________________
StatusCode TileCondToolDspThreshold::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== retrieve proxy
  CHECK( m_pryDspThreshold.retrieve() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolDspThreshold::finalize() {
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}


//
//____________________________________________________________________
float TileCondToolDspThreshold::getMinimumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolDspThreshold::getThreshold", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
}


//
//____________________________________________________________________
float TileCondToolDspThreshold::getMaximumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolDspThreshold::getThreshold", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}


//
//____________________________________________________________________
void TileCondToolDspThreshold::getAmplitudeThresholds(unsigned int drawerIdx, unsigned int channel, unsigned int adc, 
                                                       float& minimumThreshold, float& maximumThreshold) const {
  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolDspThreshold::getThreshold", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  minimumThreshold = m_pryDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
  maximumThreshold = m_pryDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}


//
//____________________________________________________________________
float TileCondToolDspThreshold::getDspThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {
    throw TileCalib::IndexOutOfRange("TileCondToolDspThreshold::getThreshold", drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_pryDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 2);
}

