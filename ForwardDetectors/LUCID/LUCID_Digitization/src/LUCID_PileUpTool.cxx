/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_PileUpTool.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GeneratorObjects/HepMcParticleLink.h"


#include "GaudiKernel/ITHistSvc.h"

#include <map>
#include <algorithm>

/// ----------------------------------------------------------------------------------------------------
LUCID_PileUpTool::LUCID_PileUpTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  PileUpToolBase     (type, name, parent),
  m_numTubes         (40),
  m_qdcChannelsPerPE (15.0),
  m_qdcPedestal      (150.0),
  m_qdcFedNoiseFactor(3.0),
  m_tdcPmtNoiseFactor(2.8),
  m_tdcFedNoiseFactor(0.8),
  m_TotalPMTgain     (1.25e6),
  m_AmpFactor        (7.5),
  m_Q1bin            (0.1e-12),
  m_NoiseCharge      (541666),
  m_numDyinodes      (10),
  m_dynodeGammaFactor(1.356),
  m_pmtSmearing      (1, 0.000001),
  m_pmtScaling       (1, 0.000001),
  m_gasScaling       (1, 0.000001),
  m_npeThreshold     (15, 0.000001),
  m_fillRootTree     (false)
{

  m_SimHitCollectionName = "LucidSimHitsVector";
  m_digitsContainerName  = "LucidDigitsCnt";
  m_key_digitCnt         = "Lucid_Digits";

  declareProperty("SimHitCollection"    , m_SimHitCollectionName, "Name of the input Collection of simulated hits");
  declareProperty("LucidDigitsContainer", m_digitsContainerName , "Name of the Container to hold the output from the digitization");

  declareProperty("numTubes"            , m_numTubes);
  declareProperty("qdcChannelsPerPE"    , m_qdcChannelsPerPE);
  declareProperty("qdcPedestal"         , m_qdcPedestal);
  declareProperty("qdcFedNoiseFactor"   , m_qdcFedNoiseFactor);
  declareProperty("tdcPmtNoiseFactor"   , m_tdcPmtNoiseFactor);
  declareProperty("tdcFedNoiseFactor"   , m_tdcFedNoiseFactor);

  declareProperty("TotalPMTgain"        , m_TotalPMTgain);
  declareProperty("AmpFactor"           , m_AmpFactor);
  declareProperty("Q1bin"               , m_Q1bin);
  declareProperty("NoiseCharge"         , m_NoiseCharge);
  declareProperty("numDyinodes"         , m_numDyinodes);
  declareProperty("dynodeGammaFactor"   , m_dynodeGammaFactor);

  declareProperty("pmtSmearing"         , m_pmtSmearing);
  declareProperty("pmtScaling"          , m_pmtScaling);
  declareProperty("gasScaling"          , m_gasScaling);
  declareProperty("npeThreshold"        , m_npeThreshold);

  declareProperty("fillRootTree"        , m_fillRootTree);
}

StatusCode LUCID_PileUpTool::initialize()
{

  ATH_MSG_DEBUG ( "LUCID_PileUpTool::initialize() called " );

  ATH_MSG_DEBUG ( " numTubes         : " << m_numTubes          << endmsg
                  << " qdcChannelsPerPE : " << m_qdcChannelsPerPE  << endmsg
                  << " qdcPedestal      : " << m_qdcPedestal       << endmsg
                  << " qdcFedNoiseFactor: " << m_qdcFedNoiseFactor << endmsg
                  << " tdcPmtNoiseFactor: " << m_tdcPmtNoiseFactor << endmsg
                  << " tdcFedNoiseFactor: " << m_tdcFedNoiseFactor << endmsg
                  << " fillRootTree     : " << m_fillRootTree      );

  ATH_CHECK(m_randomSvc.retrieve());
  ATH_MSG_DEBUG ( "Retrieved RandomNumber Service"           );

  ATH_CHECK(m_mergeSvc.retrieve());
  ATH_MSG_DEBUG ( "Retrieved PileUpMergeSvc" );

  m_digitToolBox = new LUCID_DigitizationToolBox(m_numTubes,
                                                 m_qdcChannelsPerPE,
                                                 m_qdcPedestal,
                                                 m_qdcFedNoiseFactor,
                                                 m_tdcPmtNoiseFactor,
                                                 m_tdcFedNoiseFactor,
                                                 m_TotalPMTgain,
                                                 m_AmpFactor,
                                                 m_Q1bin,
                                                 m_NoiseCharge,
                                                 m_numDyinodes,
                                                 m_dynodeGammaFactor,
                                                 m_pmtSmearing,
                                                 m_pmtScaling,
                                                 m_gasScaling,
                                                 m_npeThreshold,
                                                 m_fillRootTree);

  m_mergedhitList = new LUCID_SimHitCollection();

  if (m_fillRootTree)
    {
      CHECK(service("THistSvc", m_digitHistSvc));
      ATH_MSG_DEBUG ( "Retrieved Histogram Service" );

      CHECK(m_digitToolBox->setDebugTree(m_digitHistSvc));
      ATH_MSG_DEBUG ( "Retrieved ROOT tree" );
    }

  return StatusCode::SUCCESS;
}

/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_PileUpTool::prepareEvent(const EventContext& /*ctx*/, const unsigned int nInputEvents)
{
  ATH_MSG_DEBUG ( "prepareEvent() called for " << nInputEvents << " input events" );

  CHECK(m_digitToolBox->recordContainers(this->evtStore(), m_key_digitCnt));
  ATH_MSG_DEBUG ( " Digit container is recorded in StoreGate " );

  m_mergedhitList->clear();

  return StatusCode::SUCCESS;
}

/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_PileUpTool::processBunchXing(int bunchXing,
                                              SubEventIterator bSubEvents,
                                              SubEventIterator eSubEvents)
{
  ATH_MSG_DEBUG ( "LUCID_PileUpTool::processBunchXing() " << bunchXing );

  SubEventIterator iEvt = bSubEvents;

  for (; iEvt!=eSubEvents; ++iEvt)
    {

      StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
      ATH_MSG_VERBOSE ( "SubEvt EventInfo from StoreGate " << seStore.name() << " :"
                        << " bunch crossing : " << bunchXing
                        << " time offset : "    << iEvt->time()
                        << " event number : "   << iEvt->ptr()->eventNumber()
                        << " run number : "     << iEvt->ptr()->runNumber()
                        );
      const LUCID_SimHitCollection* tmpColl = 0;

      if (!seStore.retrieve(tmpColl,m_SimHitCollectionName).isSuccess())
        {

          ATH_MSG_ERROR ( "SubEvent BCM SiHitCollection not found in StoreGate " << seStore.name() );

          return StatusCode::FAILURE;
        }

      ATH_MSG_DEBUG ( "LUCID_SimHitCollection found with " << tmpColl->size() << " hits" );

      LUCID_SimHitCollection::const_iterator i = tmpColl->begin();
      LUCID_SimHitCollection::const_iterator e = tmpColl->end();

      for (; i!=e; ++i) m_mergedhitList->push_back((*i));

    }

  return StatusCode::SUCCESS;
}

/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_PileUpTool::mergeEvent(const EventContext& ctx)
{
  ATHRNG::RNGWrapper* rngWrapper = m_randomSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName, ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);
  ATH_CHECK(m_digitToolBox->fillDigitContainer(m_mergedhitList, rngEngine));
  ATH_MSG_DEBUG ( " LUCID_DigitContainer successfully registered in StoreGate " );

  return StatusCode::SUCCESS;
}

/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_PileUpTool::processAllSubEvents(const EventContext& ctx)
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );

  typedef PileUpMergeSvc::TimedList<LUCID_SimHitCollection>::type TimedHitCollList;

  TimedHitCollList hitCollList;

  ATH_CHECK(m_mergeSvc->retrieveSubEvtsData(m_SimHitCollectionName, hitCollList));
  ATH_MSG_DEBUG ( "Retrieved TimedHitCollList" );

  ATH_MSG_DEBUG ( " PileUp: Merge " << hitCollList.size() << " LUCID_SimHitCollection with key " << m_SimHitCollectionName );

  TimedHitCollection<LUCID_SimHit> thpclucid;

  TimedHitCollList::iterator iColl  (hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  while (iColl != endColl)
    {
      const LUCID_SimHitCollection* tmpColl(iColl->second);

      thpclucid.insert(iColl->first, tmpColl);

      ATH_MSG_DEBUG ( " LUCID_SimHitCollection found with " << tmpColl->size() << " hits " << iColl->first );

      ++iColl;
    }


  ATH_CHECK(m_digitToolBox->recordContainers(this->evtStore(), m_key_digitCnt));
  ATH_MSG_DEBUG ( " Digit container is recorded in StoreGate " );

  ATHRNG::RNGWrapper* rngWrapper = m_randomSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName, ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);
  ATH_CHECK(m_digitToolBox->fillDigitContainer(thpclucid, rngEngine));
  ATH_MSG_DEBUG ( " Digit container was filled successfully " );

  return StatusCode::SUCCESS;
}
/// ----------------------------------------------------------------------------------------------------
StatusCode LUCID_PileUpTool::finalize()
{
  return StatusCode::SUCCESS;
}
