/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolEmscale.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"


//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolEmscale("TileCondToolEmscale", 1, 0);

const InterfaceID& TileCondToolEmscale::interfaceID() {
  return IID_TileCondToolEmscale;
}

//
//____________________________________________________________________
TileCondToolEmscale::TileCondToolEmscale(const std::string& type, const std::string& name,
    const IInterface* parent)
    : AthAlgTool(type, name, parent)
{
  declareInterface<TileCondToolEmscale>(this);

}

//
//____________________________________________________________________
TileCondToolEmscale::~TileCondToolEmscale() {
}

//
//____________________________________________________________________
StatusCode TileCondToolEmscale::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );
  ATH_CHECK( m_emScaleKey.initialize() );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolEmscale::finalize() {

  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;

}


//
//____________________________________________________________________
float TileCondToolEmscale::undoOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                        , float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->undoOnlineChannelCalibration(drawerIdx, channel, adc, amplitude, onlUnit);

}

//
//____________________________________________________________________
float TileCondToolEmscale::channelCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                        , float amplitude, TileRawChannelUnit::UNIT rawDataUnitIn
                                        , TileRawChannelUnit::UNIT rawDataUnitOut) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->calibrateChannel(drawerIdx, channel, adc, amplitude, rawDataUnitIn, rawDataUnitOut);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibEms(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyEMScaleCalibration(drawerIdx, channel, amplitude);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCes(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyCesiumCalibration(drawerIdx, channel, amplitude, applyLasCorr);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibLas(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyLaserCalibration(drawerIdx, channel, amplitude);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCis(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyChargeCalibration(drawerIdx, channel, adc, amplitude);

}

//
//____________________________________________________________________
float TileCondToolEmscale::channelCalibOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                           , float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->calibrateOnlineChannel(drawerIdx, channel, adc, amplitude, onlUnit);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibEmsOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyOnlineEMScaleCalibration(drawerIdx, channel, amplitude);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCesOnl(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyOnlineCesiumCalibration(drawerIdx, channel, amplitude, applyLasCorr);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibLasOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyOnlineLaserCalibration(drawerIdx, channel, amplitude);

}

//
//____________________________________________________________________
float TileCondToolEmscale::doCalibCisOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->applyOnlineChargeCalibration(drawerIdx, channel, adc, amplitude);

}

//
//____________________________________________________________________
float TileCondToolEmscale::getCesRefLas(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getCesiumReferenceLaserGain(drawerIdx, channel, adc);

}

//
//____________________________________________________________________
float TileCondToolEmscale::getCesRefHv(unsigned int drawerIdx, unsigned int channel) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getCesiumReferenceHV(drawerIdx, channel);

}

//
//____________________________________________________________________
float TileCondToolEmscale::getCesRefTemp(unsigned int drawerIdx, unsigned int channel) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getCesiumReferenceTemperature(drawerIdx, channel);

}

//
//____________________________________________________________________
float TileCondToolEmscale::getLasFiber(unsigned int drawerIdx, unsigned int channel) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getLaserFiberVariation(drawerIdx, channel);

}

//
//____________________________________________________________________
float TileCondToolEmscale::getLasPartition(unsigned int drawerIdx) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getLaserPartitionVariation(drawerIdx);

}

//
//____________________________________________________________________
float TileCondToolEmscale::getLasRefHv(unsigned int drawerIdx, unsigned int channel) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getLaserReferenceHV(drawerIdx, channel);

}


//
//____________________________________________________________________
TileRawChannelUnit::UNIT TileCondToolEmscale::getOnlCacheUnit() const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
  return emScale->getOnlineCacheUnit();

}
