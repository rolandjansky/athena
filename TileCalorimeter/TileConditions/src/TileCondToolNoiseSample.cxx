/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolNoiseSample("TileCondToolNoiseSample", 1, 0);
const InterfaceID& TileCondToolNoiseSample::interfaceID() {
  return IID_TileCondToolNoiseSample;
}

//
//____________________________________________________________________
TileCondToolNoiseSample::TileCondToolNoiseSample(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
{
  declareInterface<ITileCondToolNoise>(this);
  declareInterface<TileCondToolNoiseSample>(this);
}

//
//____________________________________________________________________
TileCondToolNoiseSample::~TileCondToolNoiseSample() {
}

//
//____________________________________________________________________
StatusCode TileCondToolNoiseSample::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== retrieve proxy
  ATH_CHECK( m_calibSampleNoiseKey.initialize() );

  ATH_CHECK( m_calibOnlineSampleNoiseKey.initialize (SG::AllowEmpty));

  ATH_CHECK( m_emScaleKey.initialize() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCondToolNoiseSample::finalize() {
  ATH_MSG_DEBUG( "finalize called" );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getPed(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                      TileRawChannelUnit::UNIT unit) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
  float ped = calibSampleNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);

  if (unit > TileRawChannelUnit::ADCcounts) {
    SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
    ped = emScale->calibrateChannel(drawerIdx, channel, adc, ped, TileRawChannelUnit::ADCcounts, unit);
  }

  return ped;

}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                      TileRawChannelUnit::UNIT unit) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
  float hfn = calibSampleNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);


  if (unit > TileRawChannelUnit::ADCcounts) {
    SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
    hfn = emScale->calibrateChannel(drawerIdx, channel, adc, hfn, TileRawChannelUnit::ADCcounts, unit);
  }

  return hfn;

}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getLfn(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                      TileRawChannelUnit::UNIT unit) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
  const TileCalibDrawerFlt* calibDrawer = calibSampleNoise->getCalibDrawer(drawerIdx);

  //=== check if Lfn is stored already
  if (calibDrawer->getObjSizeUint32() < 3) {
    ATH_MSG_WARNING( "Requested LFN, but value not available in DB" );
    return 0.;
  }

  float lfn = calibDrawer->getData(channel, adc, 2);
  if (unit > TileRawChannelUnit::ADCcounts) {
    SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
    lfn = emScale->calibrateChannel(drawerIdx, channel, adc, lfn, TileRawChannelUnit::ADCcounts, unit);
  }

  return lfn;

}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfn1(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
  const TileCalibDrawerFlt* calibDrawer = calibSampleNoise->getCalibDrawer(drawerIdx);

  //=== check if Hfn1 is stored already, if not - return old Hfn
  float hfn1 = (calibDrawer->getObjSizeUint32() < 4) ? calibDrawer->getData(channel, adc, 1)
                                                     : calibDrawer->getData(channel, adc, 3);

  return hfn1;

}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfn2(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
  const TileCalibDrawerFlt* calibDrawer = calibSampleNoise->getCalibDrawer(drawerIdx);

  //=== check if Hfn2 is stored already, if not - return zero
  float hfn2 = (calibDrawer->getObjSizeUint32() < 5) ? 0.0 : calibDrawer->getData(channel, adc, 4);

  return hfn2;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfnNorm(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
  const TileCalibDrawerFlt* calibDrawer = calibSampleNoise->getCalibDrawer(drawerIdx);

  //=== check if HfnNorm is stored already, if not - return zero
  float hfnNorm = (calibDrawer->getObjSizeUint32() < 6) ? 0.0 : calibDrawer->getData(channel, adc, 5);

  return hfnNorm;

}


float TileCondToolNoiseSample::getOnlinePedestalDifference(unsigned int drawerIdx, unsigned int channel,
                                                           unsigned int adc, TileRawChannelUnit::UNIT onlineUnit) const {

  float pedestalDifference(0.0);

  if (!m_calibOnlineSampleNoiseKey.empty()) {
    SG::ReadCondHandle<TileCalibDataFlt> calibSampleNoise(m_calibSampleNoiseKey);
    SG::ReadCondHandle<TileCalibDataFlt> calibOnlineSampleNoise(m_calibOnlineSampleNoiseKey);

    float pedestal = calibSampleNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
    float onlinePedestal = calibOnlineSampleNoise->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);

    SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);
    pedestalDifference = emScale->calibrateOnlineChannel(drawerIdx, channel, adc, (onlinePedestal - pedestal), onlineUnit);
  }

  return pedestalDifference;
}
