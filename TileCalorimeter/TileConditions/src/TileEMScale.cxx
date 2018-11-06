/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileEMScale.h"
#include "TileConditions/Exception.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"


//
//____________________________________________________________________
TileEMScale::TileEMScale() :
  m_onlCacheUnit( TileRawChannelUnit::Invalid),
  m_onlCache(std::vector<float>(0)),
  m_isOflLasLinUsed(true),
  m_isOflLasNlnUsed(true),
  m_useOflLasFib(false),
  m_maxChannels(0),
  m_maxGains(0),
  m_drawerCacheSize(0)
{

}

//
//____________________________________________________________________
TileEMScale::~TileEMScale() {
}

//
//____________________________________________________________________
bool TileEMScale::initialize() {

  m_useOflLasFib = (m_calibOflLasFib != nullptr);
  m_isOflLasLinUsed = checkIfOflLaserLinearCalibrationUsed();
  m_isOflLasNlnUsed = checkIfOflLaserNonLinearCalibrationUsed();

  m_drawerCacheSize = m_maxChannels * m_maxGains;
  m_onlCache.resize(m_drawerCacheSize * TileCalibUtils::MAX_DRAWERIDX);

  if (m_onlCacheUnit >= TileRawChannelUnit::OnlineADCcounts
      && m_onlCacheUnit <= TileRawChannelUnit::OnlineMegaElectronVolts) {

    resetOnlineCache();
  }

  return true;
}


//
//____________________________________________________________________
void TileEMScale::resetOnlineCache() {

  //=== recalculate online cache
  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++ drawerIdx) {
    for (unsigned int channel = 0; channel < m_maxChannels; ++channel) {
      for (unsigned int adc = 0; adc < m_maxGains; ++adc) {
        m_onlCache[getCacheIndex(drawerIdx, channel, adc)] = getOnlineCalibration(drawerIdx, channel, adc, m_onlCacheUnit);
      }
    }
  }

}

//
//____________________________________________________________________
float TileEMScale::getOnlineCalibration(unsigned int drawerIdx, unsigned int channel,
                                unsigned int adc, TileRawChannelUnit::UNIT onlUnit) const {

  float onlCalib(1.);
  //=== CIS calibration
  if (onlUnit > TileRawChannelUnit::OnlineADCcounts) {
    onlCalib = m_calibOnlCis->getCalibDrawer(drawerIdx)->getCalib(channel, adc, onlCalib);

    //=== LAS+CES calibration
    if (onlUnit > TileRawChannelUnit::OnlinePicoCoulombs) {
      onlCalib = m_calibOnlLas->getCalibDrawer(drawerIdx)->getCalib(channel, adc, onlCalib);
      onlCalib = m_calibOnlCes->getCalibDrawer(drawerIdx)->getCalib(channel, adc, onlCalib);
    }
    //=== EMSCALE calibration
    if (onlUnit > TileRawChannelUnit::OnlineCesiumPicoCoulombs) {
      onlCalib = m_calibOnlEms->getCalibDrawer(drawerIdx)->getCalib(channel, adc, onlCalib);
    }
  }
  //=== Convert float to 16 bit fixed point DSP precision
  onlCalib = TileCalibUtils::fixedPointPrecision(onlCalib);

  return onlCalib;
}

//
//____________________________________________________________________
float TileEMScale::undoOnlineChannelCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                 float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  //=== Check if online folders are available
  if (m_onlCacheUnit == TileRawChannelUnit::Invalid) {
    throw TileCalib::InvalidRawChanUnit("TileCalibEms::channelCalib(onl)", onlUnit);
  }

  //=== Check for valid unit request
  if (onlUnit < TileRawChannelUnit::OnlineADCcounts
      || onlUnit > TileRawChannelUnit::OnlineMegaElectronVolts) {
    throw TileCalib::InvalidRawChanUnit("TileCalibEms::channelCalib(onl)", onlUnit);
  }

  float onlCalib(0.);
  //=== Look up total calib constant in cache if possible ...
  if (onlUnit == m_onlCacheUnit) {
    unsigned int idx = getCacheIndex(drawerIdx, channel, adc);
    if (idx >= m_onlCache.size()) {
      throw TileCalib::IndexOutOfRange("TileCalibEms::undoOnlineCalib", idx, m_onlCache.size());
    }

    onlCalib = m_onlCache[idx];
  } else {  //=== ... otherwise compute on the fly

    onlCalib = getOnlineCalibration(drawerIdx, channel, adc, onlUnit);

  }

  //=== Sanity check
  if (onlCalib == 0.) {
    throw TileCalib::InvalidValue("TileCalibEms::undoOnlCalib", onlCalib);
  }

  return amplitude / onlCalib;
}

//
//____________________________________________________________________
float TileEMScale::calibrateChannel(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                 float amplitude, TileRawChannelUnit::UNIT rawDataUnitIn,
                                 TileRawChannelUnit::UNIT rawDataUnitOut) const {

  //=== Undo online calibration, result is offline ADC counts
  if (rawDataUnitIn >= TileRawChannelUnit::OnlineADCcounts) {
    if (rawDataUnitIn > TileRawChannelUnit::OnlineMegaElectronVolts) {
      throw TileCalib::InvalidRawChanUnit("TileCalibEms::channelCalib(onl)", rawDataUnitIn);
    } else if (rawDataUnitIn > TileRawChannelUnit::OnlineADCcounts) {
      amplitude = undoOnlineChannelCalibration(drawerIdx, channel, adc, amplitude, rawDataUnitIn);
    }
    rawDataUnitIn = TileRawChannelUnit::ADCcounts;
  }

  //=== Allow only "forward" calibration from here on
  if (rawDataUnitOut < rawDataUnitIn || rawDataUnitOut > TileRawChannelUnit::MegaElectronVolts) {
    throw TileCalib::InvalidRawChanUnit("TileCalibEms::channelCalib(out)", rawDataUnitOut);
  }

  //=== decide which calibrations to apply
  switch (rawDataUnitIn) {

    case TileRawChannelUnit::ADCcounts:
    case TileRawChannelUnit::OnlineADCcounts:
      if (rawDataUnitOut > TileRawChannelUnit::ADCcounts) {
        amplitude = applyChargeCalibration(drawerIdx, channel, adc, amplitude);
        if (rawDataUnitOut > TileRawChannelUnit::PicoCoulombs) {
          amplitude = applyLaserCalibration(drawerIdx, channel, amplitude);
          amplitude = applyCesiumCalibration(drawerIdx, channel, amplitude);
          if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
            amplitude = applyEMScaleCalibration(drawerIdx, channel, amplitude);
          }
        }
      }
      break;

    case TileRawChannelUnit::PicoCoulombs:
      if (rawDataUnitOut > TileRawChannelUnit::PicoCoulombs) {
        amplitude = applyLaserCalibration(drawerIdx, channel, amplitude);
        amplitude = applyCesiumCalibration(drawerIdx, channel, amplitude);
        if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
          amplitude = applyEMScaleCalibration(drawerIdx, channel, amplitude);
        }
      }
      break;

    case TileRawChannelUnit::CesiumPicoCoulombs:
      if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
        amplitude = applyEMScaleCalibration(drawerIdx, channel, amplitude);
      }
      break;

    case TileRawChannelUnit::MegaElectronVolts:
      break;

    default:
      throw TileCalib::InvalidRawChanUnit("TileCalibEms::channelCalib", rawDataUnitIn);

  }

  return amplitude;
}

//
//____________________________________________________________________
float TileEMScale::applyEMScaleCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  return m_calibOflEms->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
}

//
//____________________________________________________________________
float TileEMScale::applyCesiumCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  amplitude = m_calibOflCes->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);

  if (applyLasCorr) {
    //=== At some point a correction relative to the mono-energy laser reference
    //=== taken with each Cesium calibration run needs to be implemented here.
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileEMScale::applyLaserCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  //=== Linear correction
  if (m_isOflLasLinUsed) {
    amplitude = m_calibOflLasLin->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  }

  //=== Non-linear correction
  if (m_isOflLasNlnUsed) {
    amplitude = m_calibOflLasNln->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileEMScale::applyLaserLinearCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  //=== Linear correction
  if (m_isOflLasLinUsed) {
    amplitude = m_calibOflLasLin->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileEMScale::applyLaserNonLinearCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  //=== Non-linear correction
  if (m_isOflLasNlnUsed) {
    amplitude = m_calibOflLasNln->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  }

  return amplitude;
}


//
//____________________________________________________________________
float TileEMScale::applyChargeCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  //=== Linear correction
  amplitude = m_calibOflCisLin->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);

  //=== Non-linear correction
  return m_calibOflCisNln->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
}

//
//____________________________________________________________________
float TileEMScale::applyChargLinearCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  //=== Linear correction
  return m_calibOflCisLin->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);

}

//
//____________________________________________________________________
float TileEMScale::applyChargeNonLinearCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  //=== Non-linear correction
  return m_calibOflCisNln->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
}

//
//____________________________________________________________________
float TileEMScale::calibrateOnlineChannel(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                    float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  //=== CIS calibration
  if (onlUnit > TileRawChannelUnit::OnlineADCcounts) {
    amplitude = m_calibOnlCis->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);

    //=== LAS+CES calibration
    if (onlUnit > TileRawChannelUnit::OnlinePicoCoulombs) {
      amplitude = m_calibOnlLas->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
      amplitude = m_calibOnlCes->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
    }

    //=== EMSCALE calibration
    if (onlUnit > TileRawChannelUnit::OnlineCesiumPicoCoulombs) {
      amplitude = m_calibOnlEms->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
    }
  }

  //=== convert float to 16 bit fixed point DSP precision
  amplitude = TileCalibUtils::fixedPointPrecision(amplitude);

  return amplitude;

}

//
//____________________________________________________________________
float TileEMScale::applyOnlineEMScaleCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  return m_calibOnlEms->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
}

//
//____________________________________________________________________
float TileEMScale::applyOnlineCesiumCalibration(unsigned int drawerIdx, unsigned int channel,
                                  float amplitude, bool applyLasCorr) const {

  amplitude = m_calibOnlCes->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
  if (applyLasCorr) {
    //=== At some point a correction relative to the mono-energy laser reference
    //=== taken with each Cesium calibration run needs to be implemented here.
  }

  return amplitude;
}

//
//____________________________________________________________________
float TileEMScale::applyOnlineLaserCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  //=== Linear correction only
  return m_calibOnlLas->getCalibDrawer(drawerIdx)->getCalib(channel, 0, amplitude);
}

//
//____________________________________________________________________
float TileEMScale::applyOnlineChargeCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {


  //=== Linear correction only
  return m_calibOnlCis->getCalibDrawer(drawerIdx)->getCalib(channel, adc, amplitude);
}

//
//____________________________________________________________________
float TileEMScale::getCesiumReferenceLaserGain(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  return m_calibOflCes->getCalibDrawer(drawerIdx)->getData(channel, adc, 1);
}

//
//____________________________________________________________________
float TileEMScale::getCesiumReferenceHV(unsigned int drawerIdx, unsigned int channel) const {

  return m_calibOflCes->getCalibDrawer(drawerIdx)->getData(channel, 0, 2);
}

//
//____________________________________________________________________
float TileEMScale::getCesiumReferenceTemperature(unsigned int drawerIdx, unsigned int channel) const {

  return m_calibOflCes->getCalibDrawer(drawerIdx)->getData(channel, 0, 3);
}

//
//____________________________________________________________________
float TileEMScale::getLaserFiberVariation(unsigned int drawerIdx, unsigned int channel) const {

  return m_calibOflLasFib->getCalibDrawer(drawerIdx)->getData(channel, 0, 0);
}

//
//____________________________________________________________________
float TileEMScale::getLaserPartitionVariation(unsigned int drawerIdx) const {

  unsigned int firstDrawerIdx = TileCalibUtils::getFirstDrawerInPartitionIdx(drawerIdx);
  return m_calibOflLasFib->getCalibDrawer(firstDrawerIdx)->getData( TileCalibUtils::LAS_PART_CHAN, 0, 0);
}

//
//____________________________________________________________________
float TileEMScale::getLaserReferenceHV(unsigned int drawerIdx, unsigned int channel) const {

  float val = -4.;

  const TileCalibDrawerFlt* calibDrawer = m_calibOflLasLin->getCalibDrawer(drawerIdx);
  int size = calibDrawer->getObjSizeUint32();
  if (size > 1) {
    val = m_calibOflLasLin->getCalibDrawer(drawerIdx)->getData(channel, 0, 1);
  }

  return val;
}
//

void TileEMScale::setCalibOflCisLin(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflCisLin = std::move(calibData);
}

void TileEMScale::setCalibOflCisNln(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflCisNln = std::move(calibData);
}

void TileEMScale::setCalibOflLasLin(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflLasLin = std::move(calibData);
}

void TileEMScale::setCalibOflLasNln(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflLasNln = std::move(calibData);
}

void TileEMScale::setCalibOflLasFib(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflLasFib = std::move(calibData);
}

void TileEMScale::setCalibOflCes(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflCes = std::move(calibData);
}

void TileEMScale::setCalibOflEms(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOflEms = std::move(calibData);
}


void TileEMScale::setCalibOnlCis(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOnlCis = std::move(calibData);
}

void TileEMScale::setCalibOnlLas(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOnlLas = std::move(calibData);
}

void TileEMScale::setCalibOnlCes(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOnlCes = std::move(calibData);
}

void TileEMScale::setCalibOnlEms(std::unique_ptr<TileCalibDataFlt> calibData) {
  m_calibOnlEms = std::move(calibData);
}


bool TileEMScale::checkIfOflLaserLinearCalibrationUsed() const {

  const float defval = 1.0;
  const float epsilon = defval * m_epsilon;
  float val(defval);
  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    for (unsigned int chn = 0; chn < TileCalibUtils::MAX_CHAN; ++chn) {
      val = m_calibOflLasLin->getCalibDrawer(drawerIdx)->getCalib(chn, 0, defval);
      if (fabs(val - defval) > epsilon) {
        return true;
      }
    }
  }

  return false;
}

bool TileEMScale::checkIfOflLaserNonLinearCalibrationUsed() const {

  const float defval = 100.0;
  const float epsilon = defval * m_epsilon;
  float val(defval);

  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    for (unsigned int chn = 0; chn < TileCalibUtils::MAX_CHAN; ++chn) {
      val = m_calibOflLasNln->getCalibDrawer(drawerIdx)->getCalib(chn, 0, defval);
      if (fabs(val - defval) > epsilon) {
        return true;
      }
    }
  }

  return false;
}
