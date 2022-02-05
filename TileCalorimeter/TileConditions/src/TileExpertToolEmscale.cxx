/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileExpertToolEmscale.h"
#include "TileConditions/Exception.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

//
//____________________________________________________________________
static const InterfaceID IID_TileExpertToolEmscale("TileExpertToolEmscale", 1, 0);

const InterfaceID& TileExpertToolEmscale::interfaceID() {
  return IID_TileExpertToolEmscale;
}

//
//____________________________________________________________________
TileExpertToolEmscale::TileExpertToolEmscale(const std::string& type, const std::string& name,
    const IInterface* parent)
    : TileCondToolEmscale(type, name, parent)
    , m_setOnceCounter(false)
    , m_emOptions(0)
{
  declareInterface<TileExpertToolEmscale>(this);
}

//
//___________________________________________________________________
float TileExpertToolEmscale::channelCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                                          , TileRawChannelUnit::UNIT rawDataUnitIn, TileRawChannelUnit::UNIT rawDataUnitOut) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);

  //=== Undo online calibration, result is offline ADC counts
  if (rawDataUnitIn >= TileRawChannelUnit::OnlineADCcounts) {
    if (rawDataUnitIn > TileRawChannelUnit::OnlineMegaElectronVolts) {
      throw TileCalib::InvalidRawChanUnit("TileExpertToolEmscale::channelCalib(onl)", rawDataUnitIn);
    } else if (rawDataUnitIn > TileRawChannelUnit::OnlineADCcounts) {
      amplitude = undoOnlCalib(drawerIdx, channel, adc, amplitude, rawDataUnitIn);
    }
    rawDataUnitIn = TileRawChannelUnit::ADCcounts;
  }

  //=== Allow only "forward" calibration from here on
  if (rawDataUnitOut < rawDataUnitIn || rawDataUnitOut > TileRawChannelUnit::MegaElectronVolts) {
    ATH_MSG_ERROR( "Invalid unit combination: "
                  << "in=" << rawDataUnitIn << " ,"
                  << "out="<< rawDataUnitOut );
    throw TileCalib::InvalidRawChanUnit("TileExpertToolEmscale::channelCalib(out)", rawDataUnitOut);
  }

  //=== Decide which calibrations to apply
  switch (rawDataUnitIn) {

    case TileRawChannelUnit::ADCcounts:
    case TileRawChannelUnit::OnlineADCcounts:
      if (rawDataUnitOut > TileRawChannelUnit::ADCcounts) {
        amplitude = doCalibCis(drawerIdx, channel, adc, amplitude);
        if (rawDataUnitOut > TileRawChannelUnit::PicoCoulombs) {
          amplitude = doCalibLas(drawerIdx, channel, amplitude);
          amplitude = doCalibCes(drawerIdx, channel, amplitude);
          if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
            amplitude = doCalibEms(drawerIdx, channel, amplitude);
          }
        }
      }
      break;

    case TileRawChannelUnit::PicoCoulombs:
      if (rawDataUnitOut > TileRawChannelUnit::PicoCoulombs) {
        amplitude = doCalibLas(drawerIdx, channel, amplitude);
        amplitude = doCalibCes(drawerIdx, channel, amplitude);
        if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
          amplitude = doCalibEms(drawerIdx, channel, amplitude);
        }
      }
      break;

    case TileRawChannelUnit::CesiumPicoCoulombs:
      if (rawDataUnitOut > TileRawChannelUnit::CesiumPicoCoulombs) {
        amplitude = doCalibEms(drawerIdx, channel, amplitude);
      }
      break;

    case TileRawChannelUnit::MegaElectronVolts:
      break;

    default:
      ATH_MSG_ERROR( "We should never come here: "
                    << "in=" << rawDataUnitIn << " ,"
                    << "out=" << rawDataUnitOut );

      throw TileCalib::InvalidRawChanUnit("TileExpertToolEmscale::channelCalib", rawDataUnitIn);

  }

  return amplitude;

}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibEms(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflEms)) {
    return TileCondToolEmscale::doCalibEms(drawerIdx, channel, amplitude);
  } else {
    return amplitude;
  }
}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibCes(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflCes)) {
    return TileCondToolEmscale::doCalibCes(drawerIdx, channel, amplitude, applyLasCorr);
  } else {
    return amplitude;
  }

}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibLas(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);

  //=== Linear correction
  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflLasLin)) {
    amplitude = emScale->applyLaserLinearCalibration(drawerIdx, channel, amplitude);
  } // OflLasLin

  //=== Non-linear correction
  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflLasNln)) {
    amplitude = emScale->applyLaserNonLinearCalibration(drawerIdx, channel, amplitude);
  } // OflLasNln

  return amplitude;
}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibCis(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);

  //=== Linear correction
  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflCisFitLin)) {
    amplitude = emScale->applyChargLinearCalibration(drawerIdx, channel, adc, amplitude);
  } // OflCisFitLin

  //=== non-linear correction
  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflCisFitNln)) {
    amplitude = emScale->applyChargeNonLinearCalibration(drawerIdx, channel, adc, amplitude);
  } // OflCisFitNln

  return amplitude;
}

//
//____________________________________________________________________
float TileExpertToolEmscale::channelCalibOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                             , float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  ATH_MSG_VERBOSE( "Recalculating total online constant for "
                  << drawerIdx << "/" << channel << "/" << adc << " , " << onlUnit );

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);

  //=== CIS calibration
  if (onlUnit > TileRawChannelUnit::OnlineADCcounts) {
    amplitude = doCalibCisOnl(drawerIdx, channel, adc, amplitude);
    ATH_MSG_VERBOSE( "---> amplitude after CIS: " << amplitude );

    //=== LAS+CES calibration
    if (onlUnit > TileRawChannelUnit::OnlinePicoCoulombs) {
      amplitude = doCalibLasOnl(drawerIdx, channel, amplitude);
      ATH_MSG_VERBOSE( "---> amplitude after LAS: " << amplitude );

      amplitude = doCalibCesOnl(drawerIdx, channel, amplitude);
      ATH_MSG_VERBOSE( "---> amplitude after CES: " << amplitude );

    }
    //=== EMSCALE calibration
    if (onlUnit > TileRawChannelUnit::OnlineCesiumPicoCoulombs) {
      amplitude = doCalibEmsOnl(drawerIdx, channel, amplitude);
      ATH_MSG_VERBOSE( "---> amplitude after EMS: " << amplitude );
    }
  }

  //=== convert float to 16 bit fixed point DSP precision
  amplitude = TileCalibUtils::fixedPointPrecision(amplitude);
  ATH_MSG_VERBOSE( "---> amplitude in fixed point precision: " << amplitude );

  return amplitude;

}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibEmsOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OnlEms)) {
    return TileCondToolEmscale::doCalibEmsOnl(drawerIdx, channel, amplitude);
  } else {
    return amplitude;
  }

}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibCesOnl(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OnlCes)) {
    return TileCondToolEmscale::doCalibCesOnl(drawerIdx, channel, amplitude, applyLasCorr);
  } else {
    return amplitude;
  }

}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibLasOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OnlLasLin)) {
    return TileCondToolEmscale::doCalibLasOnl(drawerIdx, channel, amplitude);
  } else {
    return amplitude;
  }

}

//
//____________________________________________________________________
float TileExpertToolEmscale::doCalibCisOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OnlCisLin)) {
    return TileCondToolEmscale::doCalibCisOnl(drawerIdx, channel, adc, amplitude);
  } else {
    return amplitude;
  }

}

//
//____________________________________________________________________
float TileExpertToolEmscale::undoOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                                          , float amplitude, TileRawChannelUnit::UNIT onlUnit) const {

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey);

  //=== Check if online folders are available
  if (emScale->getOnlineCacheUnit() == TileRawChannelUnit::Invalid) {
    ATH_MSG_FATAL( "Trying to undo online calibration, but COOL folders were not loaded" );
    std::abort();
  }

  //=== Check for valid unit request
  if (onlUnit < TileRawChannelUnit::OnlineADCcounts
      || onlUnit > TileRawChannelUnit::OnlineMegaElectronVolts) {
    throw TileCalib::InvalidRawChanUnit("TileExpertToolEmscale::channelCalib(onl)", onlUnit);
  }

  float val(1.);
  //=== Compute total calib constant on the fly
  val = channelCalibOnl(drawerIdx, channel, adc, val, onlUnit);

  //=== Sanity check
  if (val == 0.) {
    throw TileCalib::InvalidValue("TileExpertToolEmscale::undoOnlCalib", val);
  }

  return amplitude / val;
}

//
//____________________________________________________________________
float TileExpertToolEmscale::getLasPartition(unsigned int drawerIdx) const {

  if (m_emOptions.getEmscaleCalibBit(TileEmscaleCalibOptions::OflLasPart)) {
    return TileCondToolEmscale::getLasPartition(drawerIdx);
  } else {
    return 1.0;
  }

}

//
//____________________________________________________________________
void TileExpertToolEmscale::setEmOptions(const TileEmscaleCalibOptions& emOptions) {
  //=== check whether the m_emOptions were not already set (except for the constructor)
  if (!m_setOnceCounter) {
    m_emOptions = emOptions;
    m_setOnceCounter = true;
    ATH_MSG_INFO( "m_emOptions set to = " << m_emOptions.getEmscaleCalibOptions() );
  } else {
    ATH_MSG_WARNING( "m_emOptions already set, original value is kept = "
                     << m_emOptions.getEmscaleCalibOptions() );
  }
}
