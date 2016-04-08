/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileCondToolNoiseSample("TileCondToolNoiseSample", 1, 0);
const InterfaceID&
TileCondToolNoiseSample::interfaceID() {
  return IID_TileCondToolNoiseSample;
}

//
//____________________________________________________________________
TileCondToolNoiseSample::TileCondToolNoiseSample(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
  , m_tileToolEms("TileCondToolEmscale")
  , m_pryNoiseSample("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_NoiseSample", this)
  , m_pryOnlineNoiseSample("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_NoiseSample", this)
  , m_useOnlineNoise(false)
//  m_pryNoiseAutoCr("TileCondProxyFile_TileCalibDrawerFlt_/TileCondProxyDefault_NoiseAutoCr", this)
{
  declareInterface<ITileCondToolNoise>(this);
  declareInterface<TileCondToolNoiseSample>(this);
  declareProperty("ProxyNoiseSample", m_pryNoiseSample);
  declareProperty("ProxyOnlineNoiseSample", m_pryOnlineNoiseSample);
  //  declareProperty("ProxyNoiseAutoCr", m_pryNoiseAutoCr );
}

//
//____________________________________________________________________
TileCondToolNoiseSample::~TileCondToolNoiseSample() {
}

//
//____________________________________________________________________
StatusCode TileCondToolNoiseSample::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== TileCondToolEmscale
  CHECK( m_tileToolEms.retrieve() );

  //=== retrieve proxy
  CHECK( m_pryNoiseSample.retrieve() );

  m_useOnlineNoise = !(m_pryOnlineNoiseSample.empty());

  if (m_useOnlineNoise) {
    CHECK( m_pryOnlineNoiseSample.retrieve());
  }

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
float TileCondToolNoiseSample::getPed(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc, TileRawChannelUnit::UNIT unit) const {
  float ped = m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
  if (unit > TileRawChannelUnit::ADCcounts) {
    ped = m_tileToolEms->channelCalib(drawerIdx, channel, adc, ped, TileRawChannelUnit::ADCcounts, unit);
  }
  return ped;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfn(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc, TileRawChannelUnit::UNIT unit) const {
  float ped = m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
  if (unit > TileRawChannelUnit::ADCcounts) {
    ped = m_tileToolEms->channelCalib(drawerIdx, channel, adc, ped, TileRawChannelUnit::ADCcounts, unit);
  }
  return ped;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getLfn(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc, TileRawChannelUnit::UNIT unit) const {

  const TileCalibDrawerFlt* calibDrawer = m_pryNoiseSample->getCalibDrawer(drawerIdx);

  //=== check if Lfn is stored already
  if (calibDrawer->getObjSizeUint32() < 3) {
    ATH_MSG_WARNING( "Requested LFN, but value not available in DB" );
    return 0.;
  }

  float ped = calibDrawer->getData(channel, adc, 2);
  if (unit > TileRawChannelUnit::ADCcounts) {
    ped = m_tileToolEms->channelCalib(drawerIdx, channel, adc, ped, TileRawChannelUnit::ADCcounts, unit);
  }
  return ped;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfn1(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_pryNoiseSample->getCalibDrawer(drawerIdx);
  //=== check if Hfn1 is stored already, if not - return old Hfn
  float ped =(calibDrawer->getObjSizeUint32() < 4)
      ? m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 1)
      : m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 3);

  return ped;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfn2(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_pryNoiseSample->getCalibDrawer(drawerIdx);
  //=== check if Hfn2 is stored already, if not - return zero
  float ped = (calibDrawer->getObjSizeUint32() < 5)
      ? 0.0
      : m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 4);

  return ped;
}

//
//____________________________________________________________________
float TileCondToolNoiseSample::getHfnNorm(unsigned int drawerIdx, unsigned int channel,
    unsigned int adc) const {
  const TileCalibDrawerFlt* calibDrawer = m_pryNoiseSample->getCalibDrawer(drawerIdx);
  //=== check if HfnNorm is stored already, if not - return zero
  float ped = (calibDrawer->getObjSizeUint32() < 6)
      ? 0.0
      : m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 5);

  return ped;
}


float TileCondToolNoiseSample::getOnlinePedestalDifference(unsigned int drawerIdx, unsigned int channel, 
   unsigned int adc, TileRawChannelUnit::UNIT onlineUnit) const {

  float pedestalDifference(0.0);

  if (m_useOnlineNoise) {
    float pedestal = m_pryNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
    float onlinePedestal = m_pryOnlineNoiseSample->getCalibDrawer(drawerIdx)->getData(channel, adc, 0);
    pedestalDifference = m_tileToolEms->channelCalibOnl(drawerIdx, channel, adc, (onlinePedestal - pedestal), onlineUnit);
  }

  return pedestalDifference;
}


//
//____________________________________________________________________
/*
 void
 TileCondToolNoiseSample::getAutoCorr(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
 std::vector<float>& vec) const
 {
 const TileCalibDrawerFlt* calibDrawer = m_pryNoiseAutoCr->getCalibDrawer(drawerIdx);
 unsigned int nNum = calibDrawer->getObjSizeUint32();
 if(vec.size()!=nNum){ vec.resize(nNum); }
 for(unsigned int i=0; i<nNum; ++i){
 vec[i] = calibDrawer->getData(channel,adc,i);
 }
 }
 */
