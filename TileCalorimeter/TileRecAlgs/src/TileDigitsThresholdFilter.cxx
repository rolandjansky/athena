/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsThresholdFilter.cxx
//
//  DESCRIPTION
// 
//  Copy TileDigits and TileRawChannel from input container to output container
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileRecAlgs/TileDigitsThresholdFilter.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolDspThreshold.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"



//C++ STL includes
#include <vector>
#include <algorithm>


//
// Constructor
//
TileDigitsThresholdFilter::TileDigitsThresholdFilter(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(nullptr)
  , m_tileDspThreshold("TileCondToolDspThreshold")
{
  declareProperty("InputDigitsContainer", m_inputContainer = "TileDigitsCnt");
  declareProperty("OutputDigitsContainer", m_outputContainer = "TileDigitsFiltered");
  declareProperty("TileCondToolDspThreshold", m_tileDspThreshold);
}

TileDigitsThresholdFilter::~TileDigitsThresholdFilter() {
}

//
// Alg standard initialize function
//
StatusCode TileDigitsThresholdFilter::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID) );

  CHECK( m_tileDspThreshold.retrieve() );

  ATH_MSG_INFO( "Input digits container: '" << m_inputContainer
                << "'  output container: '" << m_outputContainer << "'" );


  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsThresholdFilter::execute() {

  // Create new container for filtered digits
  TileDigitsContainer* outputContainer = new TileDigitsContainer(true, SG::VIEW_ELEMENTS);
  

  const TileDigitsContainer* inputContainer(nullptr);
  CHECK( evtStore()->retrieve(inputContainer, m_inputContainer) );

  outputContainer->set_unit(inputContainer->get_unit());
  outputContainer->set_type(inputContainer->get_type());
  outputContainer->set_bsflags(inputContainer->get_bsflags());

  for (const TileDigitsCollection* digitsCollection : *inputContainer) {

    int fragId = digitsCollection->identify();
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(fragId);

    for (TileDigits* tile_digits : *digitsCollection) {

      HWIdentifier adcId = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int gain = m_tileHWID->adc(adcId);

      float dspThreshold = m_tileDspThreshold->getDspThreshold(drawerIdx, channel, gain);

      const std::vector<float> digits = tile_digits->samples();
      auto minMaxDigits = std::minmax_element(digits.begin(), digits.end());
      float minDigit = *minMaxDigits.first;
      float maxDigit = *minMaxDigits.second;

      if (maxDigit - minDigit > dspThreshold) {
        outputContainer->push_back(tile_digits);
      }

    }
  }

  CHECK( evtStore()->record(outputContainer, m_outputContainer, false) );

  return StatusCode::SUCCESS;
}


StatusCode TileDigitsThresholdFilter::finalize() {

  ATH_MSG_DEBUG( "in finalize()" );

  return StatusCode::SUCCESS;
}

