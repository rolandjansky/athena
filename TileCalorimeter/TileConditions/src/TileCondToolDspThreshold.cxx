/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolDspThreshold.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"


//
//____________________________________________________________________
TileCondToolDspThreshold::TileCondToolDspThreshold(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<ITileCondToolDspThreshold>(this);
  declareInterface<TileCondToolDspThreshold>(this);
}

//
//____________________________________________________________________
TileCondToolDspThreshold::~TileCondToolDspThreshold() {
}

//
//____________________________________________________________________
StatusCode TileCondToolDspThreshold::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== Initialize conditions data key with DSP thresholds
  ATH_CHECK( m_calibDspThresholdKey.initialize() );

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

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibDspThreshold(m_calibDspThresholdKey);
  return calibDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);

}


//
//____________________________________________________________________
float TileCondToolDspThreshold::getMaximumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibDspThreshold(m_calibDspThresholdKey);
  return calibDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);

}


//
//____________________________________________________________________
void TileCondToolDspThreshold::getAmplitudeThresholds(unsigned int drawerIdx, unsigned int channel, unsigned int adc, 
                                                       float& minimumThreshold, float& maximumThreshold) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibDspThreshold(m_calibDspThresholdKey);
  minimumThreshold = calibDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
  maximumThreshold = calibDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);

}


//
//____________________________________________________________________
float TileCondToolDspThreshold::getDspThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibDspThreshold(m_calibDspThresholdKey);
  return calibDspThreshold->getCalibDrawer(drawerIdx)->getData(channel, adc, 2);

}

