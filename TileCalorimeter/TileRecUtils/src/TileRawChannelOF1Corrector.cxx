/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRecUtils/TileRawChannelOF1Corrector.h"
#include "TileEvent/TileRawChannel.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCondToolTiming.h"

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
    , m_tileHWID(0)
    , m_tileToolNoiseSample("TileCondToolNoiseSample")
    , m_tileCondToolOfc("TileCondToolOfcCool/TileCondToolOfcCoolOF1")
    , m_tileToolTiming("TileCondToolTiming/TileCondToolOnlineTiming")
{
  declareInterface<ITileRawChannelTool>(this);
  declareInterface<TileRawChannelOF1Corrector>(this);

  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);
  declareProperty("TileCondToolOfc", m_tileCondToolOfc);
  declareProperty("TileCondToolTiming", m_tileToolTiming);
}

//========================================================
// Initialize
StatusCode TileRawChannelOF1Corrector::initialize() {

  ATH_MSG_INFO("Initializing...");
  
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

    for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

      int fragId = rawChannelCollection->identify();
      unsigned int drawer = (fragId & 0x3F);
      unsigned int ros = fragId >> 8;
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

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
                         << " ch" << channel << " gain " << gain << ": " 
                         << "online pedestal difference: " << onlinePedestalDifference
                         << "; OFC weights (a) sum: " << weightsSum
                         << " => energy correction: " << energyCorrection);

        rawChannel->setAmplitude(rawChannel->amplitude() + energyCorrection);
      }
    }


    //    

  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// finalize
StatusCode TileRawChannelOF1Corrector::finalize() {
  return StatusCode::SUCCESS;
}

