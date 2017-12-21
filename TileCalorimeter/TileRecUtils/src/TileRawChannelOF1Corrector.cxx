/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRecUtils/TileRawChannelOF1Corrector.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolDspThreshold.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/errorcheck.h"

static const InterfaceID IID_ITileRawChannelOF1Corrector("TileRawChannelOF1Corrector", 1, 0);

const InterfaceID& TileRawChannelOF1Corrector::interfaceID() {
  return IID_ITileRawChannelOF1Corrector;
}

//========================================================
// constructor
TileRawChannelOF1Corrector::TileRawChannelOF1Corrector(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileHWID(0)
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_tileCondToolOfc("TileCondToolOfcCool/TileCondToolOfcCoolOF1")
    , m_tileToolTiming("TileCondToolTiming/TileCondToolOnlineTiming")
    , m_tileToolEms("TileCondToolEmscale")
    , m_tileDspThreshold("TileCondToolDspThreshold")
{
  declareInterface<ITileRawChannelTool>(this);
  declareInterface<TileRawChannelOF1Corrector>(this);

  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("TileCondToolOfc", m_tileCondToolOfc);
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolEmscale", m_tileToolEms);
  declareProperty("TileCondToolDspThreshold", m_tileDspThreshold);

  declareProperty("ZeroAmplitudeWithoutDigits", m_zeroAmplitudeWithoutDigits = true);
  declareProperty("CorrectPedestalDifference", m_correctPedestalDifference = true);
}

//========================================================
// Initialize
StatusCode TileRawChannelOF1Corrector::initialize() {

  ATH_MSG_INFO("Initializing...");

  CHECK( detStore()->retrieve(m_tileHWID) );


  if (m_correctPedestalDifference) {
    //=== get TileCondToolOfc
    CHECK( m_tileCondToolOfc.retrieve() );

    //=== get TileCondToolNoiseSample
    CHECK( m_tileToolNoiseSample.retrieve() );

    //=== get TileToolTiming
    CHECK( m_tileToolTiming.retrieve() );
  }

  //=== TileCondToolEmscale
  if (m_zeroAmplitudeWithoutDigits) {
    CHECK( m_tileToolEms.retrieve() );

    //=== get TileToolTiming
    CHECK( m_tileDspThreshold.retrieve() );
  }

  if (m_zeroAmplitudeWithoutDigits) {
    ATH_CHECK( m_digitsContainerKey.initialize() );
  } else {
    m_digitsContainerKey = "";
  }

  return StatusCode::SUCCESS;
}



// ============================================================================
// process container
StatusCode TileRawChannelOF1Corrector::process(const TileRawChannelContainer* rawChannelContainer) {

  ATH_MSG_DEBUG("in process()");

  TileFragHash::TYPE rawChannelType = rawChannelContainer->get_type();
  uint32_t bsFlags = rawChannelContainer->get_bsflags();

  if ((rawChannelType == TileFragHash::OptFilterDsp 
       || rawChannelType == TileFragHash::OptFilterDspCompressed) // DSP container
      && (bsFlags & (1U << 26U)) == 0) { // OF1 method

    TileRawChannelUnit::UNIT rawChannelUnit = rawChannelContainer->get_unit();
    ATH_MSG_VERBOSE( "Units in container is " << rawChannelUnit );

    const TileDigitsContainer* digitsContainer(nullptr);

    if (m_zeroAmplitudeWithoutDigits) {
      SG::ReadHandle<TileDigitsContainer> allDigits(m_digitsContainerKey);
      digitsContainer = allDigits.cptr();
    }

    for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

      int fragId = rawChannelCollection->identify();
      unsigned int drawer = (fragId & 0x3F);
      unsigned int ros = fragId >> 8;
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      bool checkDigits(false);
      std::vector<bool> noDigits(TileCalibUtils::MAX_CHAN, true);
      if (digitsContainer) {
        IdentifierHash fragHash = (digitsContainer->hashFunc())(fragId);
        const TileDigitsCollection* digitsCollection = *(digitsContainer->indexFind(fragHash));
        if (digitsCollection->getFragBCID() != 0xDEAD) {
          ATH_MSG_VERBOSE(TileCalibUtils::getDrawerString(ros, drawer) << ": digits in bytestream => check digits");
          checkDigits = true;
        } else {
          ATH_MSG_VERBOSE(TileCalibUtils::getDrawerString(ros, drawer) << ": no digits in bytestream => do not check digits");
        }

        for (const TileDigits* tile_digits : *digitsCollection) {
          int channel = m_tileHWID->channel(tile_digits->adc_HWID());
          noDigits[channel] = false;
        }
      }

      for (TileRawChannel* rawChannel : *rawChannelCollection) {
        HWIdentifier adcId = rawChannel->adc_HWID();
        int channel = m_tileHWID->channel(adcId);
        int gain = m_tileHWID->adc(adcId);

        if (m_correctPedestalDifference) {

          float onlinePedestalDifference = m_tileToolNoiseSample->getOnlinePedestalDifference(drawerIdx, channel, gain, rawChannelUnit);
          float phase = -m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
          const TileOfcWeightsStruct* weights = m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, phase, false);
          float weightsSum(0.0);
          for (int i = 0; i < weights->n_samples; ++i) weightsSum += weights->w_a[i];
          float energyCorrection = onlinePedestalDifference * weightsSum;
          ATH_MSG_VERBOSE( TileCalibUtils::getDrawerString(ros, drawer) 
                           << " ch" << channel << (gain ? " HG: " : " LG: ")
                           << "online pedestal difference: " << onlinePedestalDifference
                           << "; OFC weights (a) sum: " << weightsSum
                           << " => energy correction: " << energyCorrection);
          
          rawChannel->setAmplitude(rawChannel->amplitude() + energyCorrection);

        }

        if (checkDigits && noDigits[channel]) {

          float amplitude = m_tileToolEms->undoOnlCalib(drawerIdx, channel, gain, rawChannel->amplitude(), rawChannelUnit);
          float minimuAmplitudeThreshold(-99999.0);
          float maximumAmplitudeThreshold(99999.0);
          m_tileDspThreshold->getAmplitudeThresholds(drawerIdx, channel, gain, minimuAmplitudeThreshold, maximumAmplitudeThreshold);
          if (amplitude < minimuAmplitudeThreshold
               || amplitude > maximumAmplitudeThreshold) 
            {

            ATH_MSG_VERBOSE( TileCalibUtils::getDrawerString(ros, drawer) 
                             << " ch" << channel << (gain ? " HG:" : " LG:")
                             << " amplitude: " << amplitude << " [ADC]"
                             << " outside range: " << minimuAmplitudeThreshold << ".." << maximumAmplitudeThreshold
                             << " without digits => set apmlitude/time/quality: 0.0/0.0/15.0");

            rawChannel->insert(0.0F, 0.0F, 15.0F);

          }

        }

      }
    }

  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// finalize
StatusCode TileRawChannelOF1Corrector::finalize() {
  return StatusCode::SUCCESS;
}

