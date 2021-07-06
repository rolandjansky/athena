/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolNoiseRawChn("TileCondToolNoiseRawChn", 1, 0);
const InterfaceID& TileCondToolNoiseRawChn::interfaceID() {
  return IID_TileCondToolNoiseRawChn;
}

//
//____________________________________________________________________
TileCondToolNoiseRawChn::TileCondToolNoiseRawChn(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
{
  declareInterface<ITileCondToolNoise>(this);
  declareInterface<TileCondToolNoiseRawChn>(this);
}

//
//____________________________________________________________________
TileCondToolNoiseRawChn::~TileCondToolNoiseRawChn() {
}

//
//____________________________________________________________________
StatusCode TileCondToolNoiseRawChn::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  ATH_CHECK( m_calibRawChannelNoiseKey.initialize() );
  ATH_CHECK( m_emScaleKey.initialize() );

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
float TileCondToolNoiseRawChn::getElectronicNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                                  TileRawChannelUnit::UNIT unit, const EventContext &ctx) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibRawChannelNoise(m_calibRawChannelNoiseKey, ctx);
  float val = calibRawChannelNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);

  if (unit > TileRawChannelUnit::ADCcounts) {
    SG::ReadCondHandle<TileEMScale> emScale (m_emScaleKey, ctx);
    val = emScale->calibrateChannel(drawerIdx, channel, adc, val, TileRawChannelUnit::ADCcounts, unit);
  }

  return val;

}

//
//____________________________________________________________________
float TileCondToolNoiseRawChn::getPileUpNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                              const EventContext &ctx) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibRawChannelNoise(m_calibRawChannelNoiseKey, ctx);
  return calibRawChannelNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);

}
