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

#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolDspThreshold.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"

// Atlas includes
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
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

  ATH_MSG_INFO( "Input digits container: '" << m_inputDigitsContainerKey.key()
                << "'  output container: '" << m_outputDigitsContainerKey.key() << "'" );

  ATH_CHECK( m_inputDigitsContainerKey.initialize() );
  ATH_CHECK( m_outputDigitsContainerKey.initialize() );

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsThresholdFilter::execute() {

  // Create new container for filtered digits
  SG::WriteHandle<TileDigitsContainer> outputDigitsContainer(m_outputDigitsContainerKey);
  ATH_CHECK( outputDigitsContainer.record(std::make_unique<TileDigitsContainer>(false, SG::VIEW_ELEMENTS)) );
  
  SG::ReadHandle<TileDigitsContainer> inputDigitsContainer(m_inputDigitsContainerKey);
  ATH_CHECK( inputDigitsContainer.isValid() );

  outputDigitsContainer->set_unit(inputDigitsContainer->get_unit());
  outputDigitsContainer->set_type(inputDigitsContainer->get_type());
  outputDigitsContainer->set_bsflags(inputDigitsContainer->get_bsflags());

  TileDigitsContainer::const_iterator collItr = inputDigitsContainer->begin();
  TileDigitsContainer::const_iterator collEnd = inputDigitsContainer->end();

  for (; collItr != collEnd; ++collItr) {
    const TileDigitsCollection* digitsCollection = *collItr; 

    auto outputDigitsCollection = std::make_unique<ConstDataVector<TileDigitsCollection> >
      (SG::VIEW_ELEMENTS, digitsCollection->identify());

    int fragId = digitsCollection->identify();
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdxFromFragId(fragId);

    for (const TileDigits* tile_digits : *digitsCollection) {
      HWIdentifier adcId = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int gain = m_tileHWID->adc(adcId);

      float dspThreshold = m_tileDspThreshold->getDspThreshold(drawerIdx, channel, gain);

      const std::vector<float>& digits = tile_digits->samples();
      auto minMaxDigits = std::minmax_element(digits.begin(), digits.end());
      float minDigit = *minMaxDigits.first;
      float maxDigit = *minMaxDigits.second;

      if (maxDigit - minDigit > dspThreshold) {
        outputDigitsCollection->push_back(tile_digits);
      }

    }
    ATH_CHECK( outputDigitsContainer->addCollection (outputDigitsCollection.release()->asDataVector(), 
                                                     collItr.hashId()) );
    
  }


  return StatusCode::SUCCESS;
}


StatusCode TileDigitsThresholdFilter::finalize() {

  ATH_MSG_DEBUG( "in finalize()" );

  return StatusCode::SUCCESS;
}

