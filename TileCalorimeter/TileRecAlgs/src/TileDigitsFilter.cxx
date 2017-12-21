/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsFilter.cxx
//  Author   : Alexander Solodkov
//  Created  : June 2008
//
//  DESCRIPTION
// 
//  Copy TileDigits and TileRawChannel from input container to output container
//  keeping only channels with (max-min) sample above threshold
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileRecAlgs/TileDigitsFilter.h"

// Atlas includes
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"

//C++ STL includes
#include <vector>
#include <set>

//
// Constructor
//
TileDigitsFilter::TileDigitsFilter(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(0)
{
  declareProperty("LowGainThereshold", m_threshold[0] = 0); // keep all LG except zeros
  declareProperty("HighGainThereshold", m_threshold[1] = 10); // keep signals above ~128(106) MeV in A,BC(D) samplings
}

TileDigitsFilter::~TileDigitsFilter() {
}

//
// Alg standard initialize function
//
StatusCode TileDigitsFilter::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_MSG_INFO( "Input digits container: '" << m_inputDigitsContainerKey.key()
                << "'  output container: '" << m_outputDigitsContainerKey.key() << "'" );

  ATH_MSG_INFO( "Input raw channel container: '" << m_inputRawChannelContainerKey.key()
                << "'  output container: '" << m_outputRawChannelContainerKey.key() << "'" );

  ATH_MSG_INFO( "Threshold low gain: " << m_threshold[0]
                << " counts,  high gain: " << m_threshold[1] << " counts" );


  if (!m_inputDigitsContainerKey.key().empty()) {
    ATH_CHECK( m_inputDigitsContainerKey.initialize() );
  }

  if (!m_outputDigitsContainerKey.key().empty()) {
    ATH_CHECK( m_outputDigitsContainerKey.initialize() );
  }

  if (!m_inputRawChannelContainerKey.key().empty()) {
    ATH_CHECK( m_inputRawChannelContainerKey.initialize() );
  }

  if (!m_outputRawChannelContainerKey.key().empty()) {
    ATH_CHECK( m_outputRawChannelContainerKey.initialize() );
  }


  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}
/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsFilter::execute() {

  ATH_MSG_DEBUG( "in execute()" );

  int digCounter = 0;
  int rchCounter = 0;


  // Create new container for filtered digits
  auto outputCont = std::make_unique<TileDigitsContainer>(false, SG::VIEW_ELEMENTS);
  if (!outputCont) {
    ATH_MSG_FATAL( "Could not create a new TileDigitsContainer instance as requested!" );
    return StatusCode::FAILURE;
  }

  // Initialize iterators to some dummy values
  TileDigitsContainer::const_iterator collItr, lastColl;
  collItr = lastColl = outputCont->end();

  // Get digit container from TES
  if (!m_inputDigitsContainerKey.key().empty()) {
    SG::ReadHandle<TileDigitsContainer> inputDigitsContainer(m_inputDigitsContainerKey);

    if (inputDigitsContainer.isValid()) {
      collItr = inputDigitsContainer->begin();
      lastColl = inputDigitsContainer->end();
    } else {
      ATH_MSG_WARNING( "can't retrieve TileDigitsContainer with name '"
                       << m_inputDigitsContainerKey.key() << "' from TDS" );
    }

  }

  TileRawChannelContainer::const_iterator firstRchColl, lastRchColl, collRchItr;
  TileFragHash::TYPE type = TileFragHash::Digitizer;
  TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts;
  bool emptyRch = true;

  // Get rawChannel container from TES
  if (!m_inputRawChannelContainerKey.key().empty()) {
    SG::ReadHandle<TileRawChannelContainer> inputRawChannelContainer(m_inputRawChannelContainerKey);

    if (inputRawChannelContainer.isValid()) {
      collRchItr = firstRchColl = inputRawChannelContainer->begin();
      lastRchColl = inputRawChannelContainer->end();
      emptyRch = (firstRchColl == lastRchColl);
      type = inputRawChannelContainer->get_type();
      unit = inputRawChannelContainer->get_unit();
    } else {
      ATH_MSG_WARNING( "can't retrieve TileRawChannelContainer with name '"
                       << m_inputRawChannelContainerKey.key() << "' from TDS" );
    }

  }

  // Create new container for filtered raw channels
  auto outRchCont = std::make_unique<TileRawChannelContainer>(false, type, unit, SG::VIEW_ELEMENTS);

  // Iterate over all collections (drawers)
  for (; collItr != lastColl; ++collItr) {

    const TileDigitsCollection* coll = *collItr;
    std::set<HWIdentifier> ids;

    auto outColl = std::make_unique<ConstDataVector<TileDigitsCollection> >
      (SG::VIEW_ELEMENTS, coll->identify());

    // Iterate over all digits in this collection
    for (const TileDigits* digit : *coll) {
      float smin = 99999., smax = -99999.;
      for (float samp : digit->samples()) {
        smin = std::min(samp, smin);
        smax = std::max(samp, smax);
      }
      HWIdentifier adcId = digit->adc_HWID();
      if (smax - smin > m_threshold[m_tileHWID->adc(adcId)]) {
        outColl->push_back(digit);
        ids.insert(adcId);
        ++digCounter;
        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << (m_tileHWID->adc(adcId) ? " HG " : " LG ")
                            << "id=" << m_tileHWID->to_string(adcId)
                            << " samples=";

          for (float samp : digit->samples()) {
            msg(MSG::VERBOSE) << " " << samp;
          }

          msg(MSG::VERBOSE) << endmsg;
        }
      }

      //      else if (msgLvl(MSG::VERBOSE)){
      //        msg(MSG::VERBOSE) << "Filtered Out "
      //                          << (m_tileHWID->adc(adcId) ? " HG " : " LG ")
      //                          << "id=" << m_tileHWID->to_string(adcId)
      //                          << " samples=";
      //        for(idig=digits.begin(); idig!=iend; ++idig)
      //          msg(MSG::VERBOSE) << " " << (*idig);
      //
      //        msg(MSG::VERBOSE) << endmsg;
      //      }

    }

    ATH_CHECK( outputCont->addCollection (outColl.release()->asDataVector(),
                                          collItr.hashId()) );

    if (ids.size() == 0 || emptyRch)
      continue;

    while (collRchItr != lastRchColl
        && (*collRchItr)->identify() < (*collItr)->identify()) {
      ++collRchItr;
    }
    if (collRchItr == lastRchColl) {
      ATH_MSG_WARNING( MSG::hex
          << "TileDigitsCollection ID 0x" << (*collItr)->identify()
          << " but corresponding TileRawChannelCollection is missing"
          << MSG::dec );

    } else if ((*collItr)->identify() != (*collRchItr)->identify()) {
      ATH_MSG_WARNING(  MSG::hex
          << "TileDigitsCollection ID 0x" << (*collItr)->identify()
          << " and TileRawChannelCollection ID 0x" << (*collRchItr)->identify()
          << " do not match " << MSG::dec );

    } else {
      const TileRawChannelCollection* collRch = *collRchItr;
      auto outRchColl = std::make_unique<ConstDataVector<TileRawChannelCollection> >
        (SG::VIEW_ELEMENTS, collRch->identify());

      // Iterate over all rawChannels in this collection
      for (const TileRawChannel* rch : *collRch) {
        HWIdentifier adcId = rch->adc_HWID();
        if (ids.find(adcId) != ids.end()) {
          outRchColl->push_back (rch);
          ++rchCounter;
          ATH_MSG_VERBOSE( (m_tileHWID->adc(adcId) ? " HG " : " LG ")
                            << "id=" << m_tileHWID->to_string(adcId)
                            << " amp=" << rch->amplitude() );
        }
      }
      ATH_CHECK( outRchCont->addCollection (outRchColl.release()->asDataVector(),
                                            collRchItr.hashId()) );
    }
  }

  ATH_MSG_DEBUG( digCounter << " digits and "
                << rchCounter << " raw channels stored in output containers");

  if (!m_outputDigitsContainerKey.key().empty()) {
    // register new container in the TES
    SG::WriteHandle<TileDigitsContainer> outputDigitsContainer(m_outputDigitsContainerKey);
    ATH_CHECK( outputDigitsContainer.record(std::move(outputCont)) );

    ATH_MSG_DEBUG( "TileDigitsContainer registered successfully (" << m_outputDigitsContainerKey.key() << ")");

  }

  if (!m_outputRawChannelContainerKey.key().empty()) {
    // register new container in the TES
    SG::WriteHandle<TileRawChannelContainer> outputRawChannelContainer(m_outputRawChannelContainerKey);
    ATH_CHECK( outputRawChannelContainer.record(std::move(outRchCont)) );

    ATH_MSG_DEBUG( "TileRawChannelContainer registered successfully (" 
                   << m_outputRawChannelContainerKey.key() << ")");
  }

  // Execution completed.
  ATH_MSG_DEBUG( "execution completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsFilter::finalize() {

  ATH_MSG_INFO( "in finalize()" );

  return StatusCode::SUCCESS;
}

