/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRecUtils/TileRawChannelMaker.h"
#include "TileRecUtils/TileRawChannelBuilder.h"
 
// Atlas includes
#include "AthenaKernel/errorcheck.h"
// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h"
#include "StoreGate/ReadHandle.h"
 
// Gaudi includes

#include <algorithm>
#include <set>

/**
 * Standard constructor
 * @param name Name of algorithm
 * @param pSvcLocator Service locator
 */
TileRawChannelMaker::TileRawChannelMaker(const std::string& name,
    ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
    , m_tileRawChannelBuilderList()
    , m_fitOverflow(false)      
    , m_tileRawChannelBuilderFitOverflow("TileRawChannelBuilder")
{
  declareProperty("TileRawChannelBuilder", m_tileRawChannelBuilderList, "List Of Tools");
  declareProperty("FitOverflow", m_fitOverflow, "Fit or not overflows");
  declareProperty("TileRawChannelBuilderFitOverflow", m_tileRawChannelBuilderFitOverflow, "Tool to fit overflows");

  m_overflowReplaceTimeCut = 50.0;
  m_overflowReplacePedestalCut  = 170.0;
  m_overflowReplaceChi2Cut = 40000.0;  
}

/**
 * Destructor
 */
TileRawChannelMaker::~TileRawChannelMaker() {
}

/**
 * Initialize algorithm
 */
StatusCode TileRawChannelMaker::initialize() {

  ATH_MSG_DEBUG( "starting to retrieve list " << m_tileRawChannelBuilderList);
  CHECK( m_tileRawChannelBuilderList.retrieve());

  ATH_MSG_DEBUG( m_tileRawChannelBuilderList << "retrieved");

  if (m_tileRawChannelBuilderList.begin() == m_tileRawChannelBuilderList.end() ) {
    ATH_MSG_INFO( "TileRawChannelBuilder list is empty - will not do anything");
    m_fitOverflow = false;
  }
  
  if (m_fitOverflow) {
    CHECK( m_tileRawChannelBuilderFitOverflow.retrieve() );
  } else {
    m_tileRawChannelBuilderFitOverflow.disable();
  }

  ATH_CHECK( m_digitsContainerKey.initialize() );

  ATH_MSG_INFO( "Initialization completed successfully");

  return StatusCode::SUCCESS;
}

/**
 * Execute
 */
StatusCode TileRawChannelMaker::execute() {

  // get named TileDigitsContaner from TES
  SG::ReadHandle<TileDigitsContainer> digitsContaner(m_digitsContainerKey);

  if (!digitsContaner.isValid()) {
    ATH_MSG_WARNING( "Can't retrieve TileDigitsContainer '"
                    << m_digitsContainerKey.key() << "' from TDS" );

    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "Got TileDigitsContainer '" << m_digitsContainerKey.key() << "'" );

  // create  RawChannel Containers for all sub-algs
  for (ToolHandle<TileRawChannelBuilder>& rawChannelBuilder : m_tileRawChannelBuilderList) {
    CHECK( rawChannelBuilder->createContainer() );
  }

  //make sure that we clean memory about errors in a drawer
  TileRawChannelBuilder::resetDrawer();

  // clean memory about overflows
  if (m_fitOverflow) {
    for (ToolHandle<TileRawChannelBuilder>& rawChannelBuilder : m_tileRawChannelBuilderList) {
      rawChannelBuilder->resetOverflows();
    }
  }

  // Iterate over all collections (drawers) with digits
  for (const TileDigitsCollection* digitsCollection : *digitsContaner) {

    // Iterate over all sub-algs
    for (ToolHandle<TileRawChannelBuilder>& rawChannelBuilder : m_tileRawChannelBuilderList) {
      // reconstruct all channels in one drawer
      rawChannelBuilder->build(digitsCollection);
    }

  }

  if (m_fitOverflow
      && !(*m_tileRawChannelBuilderList.begin())->getOverflowedChannels().empty()) {
    fitOverflowedChannels();
  }

  // commit RawChannel Containers for all sub-algs
  for (ToolHandle<TileRawChannelBuilder>& rawChannelBuilder : m_tileRawChannelBuilderList) {
    CHECK( rawChannelBuilder->commitContainer() );
  }

  ATH_MSG_DEBUG( "execute completed successfully" );

  return StatusCode::SUCCESS;
}

/**
 *  Finalize
 */
StatusCode TileRawChannelMaker::finalize() {

  ATH_MSG_INFO(" finalize completed successfully" );

  return StatusCode::SUCCESS;
}

void TileRawChannelMaker::fitOverflowedChannels() {

  for (ToolHandle<TileRawChannelBuilder> rawChannelBuilder : m_tileRawChannelBuilderList) {

    Overflows_t overflows = rawChannelBuilder->getOverflowedChannels();

    for (std::pair<TileRawChannel*, const TileDigits*>& overflow : overflows) {

      TileRawChannel* rwCh = overflow.first;
      const TileDigits* pDigits = overflow.second;

      TileRawChannel* fittedRwCh = m_tileRawChannelBuilderFitOverflow->rawChannel(pDigits);

      bool fitOK = ( ( fabs(fittedRwCh->time()) < m_overflowReplaceTimeCut     ) &&
                     ( fittedRwCh->pedestal()   < m_overflowReplacePedestalCut ) && 
                     ( fittedRwCh->quality()    < m_overflowReplaceChi2Cut     ) );

      int nSatSamples = 0;
      std::vector<double> digits = pDigits->get_digits();
      for (size_t ii = 0; ii<digits.size(); ii++) {
        if (digits[ii] > 1022.9) nSatSamples++;
      }
                
      if ( !fitOK || nSatSamples > 2)  {
          // If the fit is bad, reset the energy. 
          //The same if the number of saturated samples is 3 (or bigger)
          
          //If we reject pulse, the quality must be above 9999 in order to mask the
          // channel. So we set it at 10000 * nSatSamples. But if the fit completely fails, 
          // the quality is 100000, and we can keep this info in quality as well.
          float quality = 10000. * ( (nSatSamples) ? nSatSamples : 9);
          if (fittedRwCh->quality() > 99999.9) quality += 100000.;
          rwCh->insert(0.0, 0.0, quality);

          // 20000 - Indicates overflow, 6000 - indicates bad fit or >2 saturations.
          // 30000 - Indicates overflow + underflow, 6000 - indicates bad fit or >2 saturations.
          float pedestal = (rwCh->pedestal() < 29500.) ? (26000.)
                                                       : (36000.);
          rwCh->setPedestal(pedestal); 
      } else {
          //If the fit is OK replace
          
          //The range of the quality factor is very different in the fit (0-100k)
          //than in the OF (0-255). So we rescale by a factor of 400.          
          rwCh->insert(fittedRwCh->amplitude(), 
                       fittedRwCh->time(), 
                       fittedRwCh->quality()*(1./400.));
          // 20000 - Indicates overflow, 3000 - indicates fitted
          // 30000 - Indicates overflow + underflow, 3000 - indicates fitted.
          float pedestal = (rwCh->pedestal() < 29500.) ? (fittedRwCh->pedestal() + 23000.)
                                                       : (fittedRwCh->pedestal() + 33000.);
          rwCh->setPedestal(pedestal); 
      }
		
    }
  }

}
