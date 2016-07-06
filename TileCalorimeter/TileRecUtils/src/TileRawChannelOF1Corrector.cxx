/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRecUtils/TileRawChannelOF1Corrector.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

static const InterfaceID IID_ITileRawChannelOF1Corrector("TileRawChannelOF1Corrector", 1, 0);

const InterfaceID& TileRawChannelOF1Corrector::interfaceID() {
  return IID_ITileRawChannelOF1Corrector;
}

//========================================================
// constructor
TileRawChannelOF1Corrector::TileRawChannelOF1Corrector(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileHWID(nullptr)
    , m_tileID(nullptr)
    , m_tileTBID(nullptr)
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_tileCondToolOfc("TileCondToolOfcCool/TileCondToolOfcCoolOF1")
    , m_tileToolTiming("TileCondToolTiming/TileCondToolOnlineTiming")
    , m_tileToolEms("TileCondToolEmscale")
    , m_thresholds(MAX_TYPES, 2048.0)
{
  declareInterface<ITileRawChannelTool>(this);
  declareInterface<TileRawChannelOF1Corrector>(this);

  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("TileCondToolOfc", m_tileCondToolOfc);
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolEmscale", m_tileToolEms);

  declareProperty("TileDigitsContainer", m_digitsContainerName = "TileDigitsCnt");
  declareProperty("ZeroAmplitudeWithoutDigits", m_zeroAmplitudeWithoutDigits = true);
  declareProperty("NegativeAmplitudeThresholdToZero", m_negativeAmplitudeThreshold = -10.0);
  declareProperty("PositiveAmplitudeThresholdToZero", m_positiveAmplitudeThreshold = 10.0);

  declareProperty("E1AmplitudeThresholdToZero", m_E1AmplitudeThreshold = 90.0);
  declareProperty("E2AmplitudeThresholdToZero", m_E2AmplitudeThreshold = 100.0);
  declareProperty("E3AmplitudeThresholdToZero", m_E3AmplitudeThreshold = 140.0);
  declareProperty("E4AmplitudeThresholdToZero", m_E4AmplitudeThreshold = 220.0);
  declareProperty("E4PrimeAmplitudeThresholdToZero", m_E4PrimeAmplitudeThreshold = 140.0);
  declareProperty("InnerMBTSAmplitudeThresholdToZero", m_innerMBTSAmplitudeThreshold = 400.0);
  declareProperty("OuterMBTSAmplitudeThresholdToZero", m_outerMBTSAmplitudeThreshold = 400.0);
  declareProperty("SpecialC10AmplitudeThresholdToZero", m_specialC10AmplitudeThreshold = 10.0);

}

//========================================================
// Initialize
StatusCode TileRawChannelOF1Corrector::initialize() {

  ATH_MSG_INFO("Initializing...");

  m_thresholds[NORMAL] = m_positiveAmplitudeThreshold;
  m_thresholds[E1] = m_E1AmplitudeThreshold;
  m_thresholds[E2] = m_E2AmplitudeThreshold;
  m_thresholds[E3] = m_E3AmplitudeThreshold;
  m_thresholds[E4] = m_E4AmplitudeThreshold;
  m_thresholds[E4PRIME] = m_E4PrimeAmplitudeThreshold;
  m_thresholds[INNERMBTS] = m_innerMBTSAmplitudeThreshold;
  m_thresholds[OUTERMBTS] = m_outerMBTSAmplitudeThreshold;
  m_thresholds[SPECIALC10] = m_specialC10AmplitudeThreshold;


  const IGeoModelSvc* geoModel = 0;
  CHECK( service("GeoModelSvc", geoModel) );
  
  // dummy parameters for the callback:
  int dummyInt = 0;
  std::list<std::string> dummyList;
  
  if (geoModel->geoInitialized()) {
    return geoInit(dummyInt, dummyList);
  } else {
    CHECK( detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel,
                              &TileRawChannelOF1Corrector::geoInit, this) );
  }
  

  return StatusCode::SUCCESS;
}


//
// 
StatusCode TileRawChannelOF1Corrector::geoInit(IOVSVC_CALLBACK_ARGS) {
  
  CHECK( detStore()->retrieve(m_tileHWID) );
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileTBID) );

  //=== TileCondToolEmscale
  if (m_zeroAmplitudeWithoutDigits) CHECK( m_tileToolEms.retrieve() );

  //=== get TileCondToolOfc
  CHECK( m_tileCondToolOfc.retrieve() );

  //=== get TileCondToolNoiseSample
  CHECK( m_tileToolNoiseSample.retrieve() );

  //=== get TileToolTiming
  CHECK( m_tileToolTiming.retrieve() );

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
      && ((bsFlags >> 26U) ^ 1U)) { // OF1 method

    TileRawChannelUnit::UNIT rawChannelUnit = rawChannelContainer->get_unit();
    ATH_MSG_VERBOSE( "Units in container is " << rawChannelUnit );

    const TileDigitsContainer* digitsContainer(nullptr);
    if (m_zeroAmplitudeWithoutDigits) CHECK( evtStore()->retrieve(digitsContainer, m_digitsContainerName) );

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

        if (checkDigits && noDigits[channel]) {

          int index;
          int pmt;
          static const TileCablingService* s_cabling = TileCablingService::getInstance();
          Identifier cell_id = s_cabling->h2s_cell_id_index(ros, drawer, channel, index, pmt);
          if (!cell_id.is_valid()) continue;

          int sample = m_tileID->sample(cell_id);              
          int tower = m_tileID->tower(cell_id);

          CHANNEL_TYPE channelType(NORMAL);
        
          if (index > -1) {

            if (sample == TileID::SAMP_E) {
              if      (tower == 10) channelType = E1;
              else if (tower == 11) channelType = E2;
              else if (tower == 13) channelType = E3;
              else                  channelType = E4;

            } else if (sample == TileID::SAMP_C
                       && m_tileID->section(cell_id) == TileID::GAPDET 
                       && !s_cabling->C10_connected(drawer)) {
              
              channelType = SPECIALC10;
            }
            
          } else if (index == -2) { // MBTS cells

            if (m_tileTBID->eta(cell_id) == 0) channelType = INNERMBTS;
            else                               channelType = OUTERMBTS;

          } else if (index == -3) { // E4' cells
            channelType = E4PRIME;
          } 

          float amplitude = m_tileToolEms->undoOnlCalib(drawerIdx, channel, gain, rawChannel->amplitude(), rawChannelUnit);
          
          if ((channelType == NORMAL && amplitude < m_negativeAmplitudeThreshold)
              || (amplitude > m_thresholds[channelType]) ) {

            ATH_MSG_VERBOSE( TileCalibUtils::getDrawerString(ros, drawer) 
                             << " ch" << channel << (gain ? " HG:" : " LG:")
                             << " amplitude: " << amplitude << " [ADC]" 
                             << (amplitude > 0.0F ? " > threshold: " : " < threshold: ")
                             << (amplitude > 0.0F ? m_thresholds[channelType] : m_negativeAmplitudeThreshold) << " [ADC]"
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

