/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCondToolEmscale.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolNoiseRawChn("TileCondToolNoiseRawChn", 1, 0);
const InterfaceID&
TileCondToolNoiseRawChn::interfaceID() {
  return IID_TileCondToolNoiseRawChn;
}

//
//____________________________________________________________________
TileCondToolNoiseRawChn::TileCondToolNoiseRawChn(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
  , m_tileToolEms("TileCondToolEmscale")
  , m_pryNoiseRawChn( "TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_NoiseRawChn", this)
{
  declareInterface<ITileCondToolNoise>(this);
  declareInterface<TileCondToolNoiseRawChn>(this);
  declareProperty("ProxyNoiseRawChn", m_pryNoiseRawChn);
}

//
//____________________________________________________________________
TileCondToolNoiseRawChn::~TileCondToolNoiseRawChn() {
}

//
//____________________________________________________________________
StatusCode TileCondToolNoiseRawChn::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== TileCondToolEmscale
  CHECK( m_tileToolEms.retrieve() );

  //=== retrieve proxy
  CHECK( m_pryNoiseRawChn.retrieve() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolNoiseRawChn::finalize() {
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
float TileCondToolNoiseRawChn::getElectronicNoise(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc, TileRawChannelUnit::UNIT unit) const {
  float val = m_pryNoiseRawChn->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
  if (unit > TileRawChannelUnit::ADCcounts) {
    val = m_tileToolEms->channelCalib(drawerIdx, channel, adc, val, TileRawChannelUnit::ADCcounts,
        unit);
  }
  return val;
}

//
//____________________________________________________________________
float TileCondToolNoiseRawChn::getPileUpNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  return m_pryNoiseRawChn->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}

