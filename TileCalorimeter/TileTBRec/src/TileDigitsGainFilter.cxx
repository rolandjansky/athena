/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsGainFilter.cxx
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
#include "TileTBRec/TileDigitsGainFilter.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"

// Atlas includes
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"

//C++ STL includes
#include <vector>
#include <algorithm>
#include <cstring>

//
// Constructor
//
TileDigitsGainFilter::TileDigitsGainFilter(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(nullptr)
{
  declareProperty("InputDigitsContainer", m_inputContainer = "TileDigitsCnt");
  declareProperty("OutputDigitsContainer", m_outputContainer = "TileDigitsFiltered");
  declareProperty("HighGainThreshold", m_threshold = 4095);
}

TileDigitsGainFilter::~TileDigitsGainFilter() {
}

//
// Alg standard initialize function
//
StatusCode TileDigitsGainFilter::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_MSG_INFO( "Input digits container: '" << m_inputContainer
                << "'  output container: '" << m_outputContainer << "'" );


  ATH_MSG_INFO( "Threshold to check overflowed in HG: " << m_threshold);
  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsGainFilter::execute() {

  // Create new container for filtered digits
  auto outputContainer = std::make_unique<TileDigitsContainer>(false, SG::VIEW_ELEMENTS);
  

  const TileDigitsContainer* inputContainer(nullptr);
  CHECK( evtStore()->retrieve(inputContainer, m_inputContainer) );

  outputContainer->set_unit(inputContainer->get_unit());
  outputContainer->set_type(inputContainer->get_type());
  outputContainer->set_bsflags(inputContainer->get_bsflags());

  const TileDigits* digits[2][48] = {{0}}; // 2 gains and 48 channels

  TileDigitsContainer::const_iterator collIt = inputContainer->begin();
  TileDigitsContainer::const_iterator collEnd = inputContainer->end();
  for (; collIt != collEnd; ++collIt) {
    const TileDigitsCollection* digitsCollection = *collIt;

    memset(digits, 0, sizeof(digits));

    for (const TileDigits* tile_digits : *digitsCollection) {

      HWIdentifier adcId = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int gain = m_tileHWID->adc(adcId);
      digits[gain][channel] = tile_digits;

    }

    auto outColl = std::make_unique<ConstDataVector<TileDigitsCollection> >
      (SG::VIEW_ELEMENTS,
       digitsCollection->identify());
    outColl->reserve (TileCalibUtils::MAX_CHAN);
    
    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

      const TileDigits* loGainDigits = digits[0][channel];
      const TileDigits* hiGainDigits = digits[1][channel];

      if (hiGainDigits && loGainDigits) {
        const std::vector<float> digits = hiGainDigits->samples();
        auto minMaxDigits = std::minmax_element(digits.begin(), digits.end());
        float maxDigit = *minMaxDigits.second;
        
        if (maxDigit  < m_threshold) {
          ATH_MSG_VERBOSE("Save HG: " << (std::string) *hiGainDigits);
          outColl->push_back(hiGainDigits);
        } else {
          ATH_MSG_VERBOSE("Overflowed HG: " << (std::string) *hiGainDigits);
          ATH_MSG_VERBOSE("Save LG: " << (std::string) *loGainDigits);
          outColl->push_back(loGainDigits);
        }
      } else if (loGainDigits) {
        ATH_MSG_VERBOSE("Save LG (only available): " << (std::string) *loGainDigits);
        outColl->push_back(loGainDigits);
      } else if (hiGainDigits) {
        ATH_MSG_VERBOSE("Save HG (only available): " << (std::string) *hiGainDigits);
        outColl->push_back(hiGainDigits);
      }

    }
    CHECK( outputContainer->addCollection (outColl.release()->asDataVector(),
                                           collIt.hashId()) );
  }

  CHECK( evtStore()->record(std::move(outputContainer), m_outputContainer, false) );

  return StatusCode::SUCCESS;
}


StatusCode TileDigitsGainFilter::finalize() {

  ATH_MSG_DEBUG( "in finalize()" );

  return StatusCode::SUCCESS;
}

