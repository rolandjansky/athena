/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <vector>

#include "PileUpEventLoopMgr.h"

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/ITimeKeeper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventIncident.h"

#include "PileUpTools/IBeamIntensity.h"
#include "PileUpTools/IBeamLuminosity.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "PileUpTools/IBkgStreamsCache.h"

//=========================================================================
// Standard Constructor
//=========================================================================
PileUpEventLoopMgr::PileUpEventLoopMgr(const std::string& nam,
                                       ISvcLocator* svcLoc)
  : MinimalEventLoopMgr(nam, svcLoc),
    p_incidentSvc(0), p_mergeSvc(0),
    p_SGOver(0),
    //m_nInputs(0), m_nStores(0),
    m_origSel("EventSelector", nam),
    m_caches(this),
    m_maxCollPerXing(23.0),
    m_xingFreq(25), m_firstXing(-2), m_lastXing(+1),
    m_timeKeeper("",nam), m_allowSubEvtsEOF(true),
    m_xingByXing(false), m_isEventOverlayJob(false),
    m_failureMode(1),
    m_beamInt("FlatBM", nam),
    m_beamLumi("LumiProfileSvc", nam),
    m_currentRun(0), m_firstRun(true),
    m_msg( nam ),
    m_maxBunchCrossingPerOrbit(3564),
    m_nevt(0),
    m_ncurevt(0),
    m_skipExecAlgs(false),
    m_loadProxies(true),
    m_isEmbedding(false),
    m_allowSerialAndMPToDiffer(true)
{
  declareProperty("MaxBunchCrossingPerOrbit", m_maxBunchCrossingPerOrbit, "The number of slots in each LHC beam. Default: 3564.");
  declareProperty("OrigSelector", m_origSel, "EventSelector for original (physics) events stream" );
  declareProperty("XingFrequency", m_xingFreq, "ns");
  declareProperty("firstXing", m_firstXing, "time of first xing / XingFrequency (0th xing is 1st after trigger)");
  declareProperty("lastXing", m_lastXing, "time of last xing / XingFrequency (0th xing is 1st after trigger)");
  declareProperty("MaxMinBiasCollPerXing", m_maxCollPerXing, "Set to digitization numberOfCollisions prop. for variable-mu and RunDMC jobs.");
  declareProperty("bkgCaches", m_caches, "list of tools managing bkg events");
  declareProperty("TimeKeeper", m_timeKeeper, "time keeper service, terminates event loop if needed. NOP by default");
  declareProperty("AllowSubEvtsEOF", m_allowSubEvtsEOF, "if true(default) an EOF condition in the BkgStreamsCaches is not considered to be an error IF maxevt=-1 (loop over all available events)");
  declareProperty("XingByXing", m_xingByXing, "if set to true we will not cache bkg events from one xing to then next. This greatly increases the amount of I/O and greatly reduces the memory required to run a job");
  declareProperty("IsEventOverlayJob", m_isEventOverlayJob, "if set to true will prevent the BCID from being overridden.");
  declareProperty("mcRunNumber", m_mcRunNumber=99, "the run number from an EVNT file, used to set the mc_channel_number, for overlay");
  declareProperty("FailureMode", m_failureMode,
                  "Controls behaviour of event loop depending on return code of"
                  " Algorithms. 0: all non-SUCCESSes terminate job. "
                  "1: RECOVERABLE skips to next event, FAILURE terminates job "
                  "(DEFAULT). 2: RECOVERABLE and FAILURE skip to next events");

  declareProperty("BeamInt", m_beamInt,
                  "The service providing the beam intensity distribution");
  declareProperty("BeamLuminosity", m_beamLumi,
                  "The service providing the beam luminosity distribution vs. run");
  declareProperty("IsEmbedding", m_isEmbedding, "Set this to True for embedding jobs.");
  declareProperty("AllowSerialAndMPToDiffer", m_allowSerialAndMPToDiffer, "When set to False, this will allow the code to reproduce serial output in an AthenaMP job, albeit with a significant performance penalty.");
  //  m_caches.push_back("BkgStreamsCache/MinBiasCache");
}

//=========================================================================
// Standard Destructor
//=========================================================================
PileUpEventLoopMgr::~PileUpEventLoopMgr() {}

//=========================================================================
// implementation of IAppMgrUI::initialize
//=========================================================================
StatusCode PileUpEventLoopMgr::initialize()
{
  //-------------------------------------------------------------------------
  // Process Properties
  //-------------------------------------------------------------------------
  try {
    if (!(setProperties()).isSuccess()) return StatusCode::FAILURE;
    // configure our MsgStream
    m_msg.get().setLevel( m_outputLevel.value() );
  } catch (...) {
    ATH_MSG_WARNING ( "Caught exception thrown reading in properties" );
  }


  ATH_MSG_INFO ( "Initializing " << name() << " - package version " << PACKAGE_VERSION ) ;
  if(!m_allowSerialAndMPToDiffer)
    {
      ATH_MSG_WARNING ( "AllowSerialAndMPToDiffer=False! This will incur serious performance penalties! But Serial and MP output will be the same." );
    }

  //locate the StoreGateSvc and initialize our local ptr
  StatusCode sc = service("StoreGateSvc", p_SGOver);
  if (!sc.isSuccess() || 0 == p_SGOver) {
    ATH_MSG_ERROR ( "Could not find StoreGateSvc" );
    return sc;
  }

  //reset output store proxy provider: we don't want to have one!
  p_SGOver->setProxyProviderSvc(0);

  //locate the IncidentSvc and initialize our local ptr
  sc = serviceLocator()->service("IncidentSvc", p_incidentSvc);
  if( !sc.isSuccess() )
    {
      ATH_MSG_ERROR ( "Error retrieving IncidentSvc." );
      return sc;
    }

  //locate the PileUpMergeSvc and initialize our local ptr
  sc = serviceLocator()->service("PileUpMergeSvc", p_mergeSvc);
  if( !sc.isSuccess() )
    {
      ATH_MSG_ERROR ( "Error retrieving IncidentSvc." );
      return sc;
    }

  //-------------------------------------------------------------------------
  // Setup Event Selector
  //-------------------------------------------------------------------------
  if (!(setupStreams().isSuccess()))
    {
      ATH_MSG_ERROR ( "Failed to initialize pile-up streams" );
      return StatusCode::FAILURE;
    }
  //-------------------------------------------------------------------------
  // Setup TimeKeeper service
  //-------------------------------------------------------------------------

  // We do not expect a TimeKeeper necessarily being declared
  if (!m_timeKeeper.empty()) {
    if (!(sc = m_timeKeeper.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "TimeKeeper not found." );
      return sc;
    }
  } else {
    ATH_MSG_INFO ( "No TimeKeeper selected. No time limit control on event loop." );
  }

  // Get the value of SkipEvents. It is needed for seeking
  SmartIF<IProperty> prpMgr(serviceLocator());
  IEvtSelector* evtSelector(0);
  if(prpMgr.isValid()) {
    // Get event selector name. Retrieve EventSelector
    std::string evtSelName = prpMgr->getProperty("EvtSel").toString();
    sc = serviceLocator()->service(evtSelName,evtSelector);
    if(sc.isFailure() || evtSelector==0) {
      ATH_MSG_ERROR ( "Error retrieving EventSelector" );
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_ERROR ( "IProperty interface not found in ApplicationMgr" );
    return StatusCode::FAILURE;
  }

  SmartIF<IProperty> prpMgr1(evtSelector);
  if(prpMgr1.isValid()) {
    std::string skipEventsStr = prpMgr1->getProperty("SkipEvents").toString();
    m_ncurevt = std::atoi(skipEventsStr.c_str());
  }
  else {
    ATH_MSG_ERROR ( "IProperty interface not found on the event selector" );
    return StatusCode::FAILURE;
  }

  //-------------------------------------------------------------------------
  // Base class initialize (done at the end to allow algs to access stores)
  //-------------------------------------------------------------------------

  return MinimalEventLoopMgr::initialize();
}

//=========================================================================
// implementation of IAppMgrUI::finalize
//=========================================================================
StatusCode PileUpEventLoopMgr::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << " - package version " << PACKAGE_VERSION );

  //we need to release all our BkgStreamCaches
  ToolHandleArray<IBkgStreamsCache>::iterator cacheIterator(m_caches.begin());
  ToolHandleArray<IBkgStreamsCache>::iterator endOfCaches(m_caches.end());
  while (cacheIterator != endOfCaches) (*cacheIterator++)->release();
  m_caches.clear();

  //and to clean up the store the stream owns
  StatusCode sc = m_origStream.finalize();

  if (sc.isSuccess()) sc = MinimalEventLoopMgr::finalize();

  return sc;
}

//=========================================================================
// implementation of IAppMgrUI::terminate
//=========================================================================
//  StatusCode PileUpEventLoopMgr::terminate()
//  {
//     m_histoDataMgrSvc = releaseInterface(m_histoDataMgrSvc);
//     m_histoPersSvc    = releaseInterface(m_histoPersSvc);
//     return MinimalEventLoopMgr::terminate();
//  }


//=========================================================================
// implementation of IAppMgrUI::nextEvent
//=========================================================================
StatusCode PileUpEventLoopMgr::nextEvent(int maxevt)
{
  // make nextEvent(0) a dummy call, as needed by athenaMP
  if (0 == maxevt) return StatusCode::SUCCESS;


  static int        total_nevt = 0;

  //locate the ActiveStoreSvc and initialize our local ptr
  ActiveStoreSvc* pActiveStore;
  StatusCode sc = serviceLocator()->service("ActiveStoreSvc", pActiveStore);
  if( !sc.isSuccess() )
    {
      ATH_MSG_FATAL ( "Error retrieving ActiveStoreSvc." );
      return sc;
    }

  // These following two initialization loops are performed here in case new
  // Top level Algorithms or Output Streams have been created interactively
  // at run-time instead of configuration time. Note also that it is possible
  // that some newly created Algorithms are still not initialized as a result
  // of these loops (e.g. where the membership of an existing Sequencer is
  // changed at run-time. In this case, the Algorithm::sysExecute() function
  // will ensure that the Algorithm is correctly initialized. This mechanism
  // actually makes loops redundant, but they do  provide a well defined
  // location for the initialization to take place in the non-interactive case.

  // Initialize the list of Algorithms. Note that existing Algorithms
  // are protected against multiple initialization attempts.
  ListAlg::iterator ita;
  for ( ita = m_topAlgList.begin(); ita != m_topAlgList.end(); ++ita )
    {
      if( !((*ita)->sysInitialize()).isSuccess() )
        {
          ATH_MSG_ERROR ( "Unable to initialize Algorithm: " <<
                          (*ita)->name() );
          return StatusCode::FAILURE;
        }
    }

  // Initialize the list of Output Streams. Note that existing Output Streams
  // are protected against multiple initialization attempts.
  for (ita = m_outStreamList.begin(); ita != m_outStreamList.end(); ++ita )
    {
      if( !((*ita)->sysInitialize()).isSuccess() ) {
        ATH_MSG_ERROR ( "Unable to initialize Output Stream: " <<
                        (*ita)->name() );
        return StatusCode::FAILURE;
      }
    }

  const EventInfo* pEvent(0);
  // loop over events if the maxevt (received as input) is different from -1.
  // if evtmax is -1 it means infinite loop (till time limit that is)
  bool noTimeLimit(false);
  while( (maxevt == -1 || m_nevt < maxevt) &&
         (noTimeLimit = (m_timeKeeper.empty() || m_timeKeeper->nextIter()) )  &&
         0 != (pEvent = m_origStream.nextEventPre()) ) {
    // Check if there is a scheduled stop issued by some algorithm/sevice
    if ( m_scheduledStop ) {
      m_scheduledStop = false;
      ATH_MSG_ALWAYS ( "A stopRun was requested. Terminating event loop." );
      break;
    }
    ++m_nevt; ++total_nevt; ++m_ncurevt;
    ATH_MSG_INFO ( "nextEvent(): overlaying original event " <<
                   pEvent->event_ID()->run_number() << '/' <<
                   pEvent->event_ID()->event_number() << '/' <<
                   pEvent->event_ID()->lumi_block() );

    //-----------------------------------------------------------------------
    // Setup overlaid event in the event store
    //-----------------------------------------------------------------------
    //ask the BeamIntensitySvc to choose (and remember)
    //in which xing this event will be wrto the beam int distribution
    m_beamInt->selectT0();

    EventID* pOvrID = new EventID(*(pEvent->event_ID()));
    unsigned int t0BCID(pOvrID->bunch_crossing_id());
    if (!m_isEventOverlayJob) {
      // Use the position in the beam intensity array to set a BCID-like quantity
      t0BCID = m_beamInt->getCurrentT0BunchCrossing();
      pOvrID->set_bunch_crossing_id(t0BCID);
    }
    ATH_MSG_VERBOSE ( "BCID =" << pOvrID->bunch_crossing_id() );
    EventType *pOvrEt = new EventType(); //FIXME
    pOvrEt->set_user_type("Overlaid"); //FIXME
    pOvrEt->set_mc_channel_number(pEvent->event_type()->mc_channel_number());
    pOvrEt->set_mc_event_number(pEvent->event_type()->mc_event_number());
    const unsigned int nWeights(pEvent->event_type()->n_mc_event_weights());
    for (unsigned int iWeight(0); iWeight<nWeights; ++iWeight)
      {
        pOvrEt->set_mc_event_weight(pEvent->event_type()->mc_event_weight(iWeight));
      }
    if (m_isEventOverlayJob){
      pOvrEt->set_mc_channel_number(m_mcRunNumber);
      ATH_MSG_DEBUG ( "pOvrEt set_mc_channel_number: " << m_mcRunNumber );
    }
    else {
      // Overlay RDO files should be treated like data for reco
      // purposes, so only set this for SimHit level pile-up.
      pOvrEt->add_type(EventType::IS_SIMULATION);
    }

    PileUpEventInfo *pOverEvent = new PileUpEventInfo(pOvrID, pOvrEt);
    if(m_isEmbedding) {
      pOverEvent->setActualInteractionsPerCrossing(pEvent->actualInteractionsPerCrossing());
      pOverEvent->setAverageInteractionsPerCrossing(pEvent->averageInteractionsPerCrossing());
    }
    ATH_MSG_VERBOSE ( "BCID =" << pOverEvent->event_ID()->bunch_crossing_id() );
    //  register as sub event of the overlaid
    bool addpEvent(true);
    if(m_isEmbedding) {
      const PileUpEventInfo* pOldEvent(dynamic_cast<const PileUpEventInfo*>(pEvent));
      if(pOldEvent) {
        PileUpEventInfo::SubEvent::const_iterator it  = pOldEvent->beginSubEvt();
        PileUpEventInfo::SubEvent::const_iterator end = pOldEvent->endSubEvt();
        if (it != end) { addpEvent=false; }
        for (; it != end; ++it) {
          const EventInfo* sevt = (*it).pSubEvt;
          pOverEvent->addSubEvt((*it).time() , (*it).type(), sevt, &m_origStream.store());
        }
      }
    }
    if(addpEvent) {
      pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal,
                            pEvent, &m_origStream.store());
    }

    if( !(p_SGOver->record(pOverEvent, "McEventInfo")).isSuccess() )  {
      ATH_MSG_ERROR ( "Error recording overlayed event object" );
      return StatusCode::FAILURE;
    }
    //add an alias to "OverlayEvent" (backward compatibility)
    if( !(p_SGOver->setAlias(pOverEvent, "OverlayEvent")).isSuccess() )  {
      ATH_MSG_ERROR ( "Error setting OverlayEvent alias for overlayed event object" );
      return StatusCode::FAILURE;
    }
    //add an alias to "MyEvent" (backward compatibility)
    if( !(p_SGOver->setAlias(pOverEvent, "MyEvent")).isSuccess() )  {
      ATH_MSG_ERROR ( "Error setting OverlayEvent alias for overlayed event object" );
      return StatusCode::FAILURE;
    }

    //symlink to EventInfo to support transparent access for standard code
    const EventInfo *pEI(0);
    if( !(p_SGOver->symLink(pOverEvent, pEI)).isSuccess() )  {
      ATH_MSG_ERROR ( "Error symlinking overlayed event object" );
      return StatusCode::FAILURE;
    }


    //FIXME at this point one may want to look into the original event
    //FIXME to decide whether to skip it or to do the pile-up

    ///ask the BeamLuminositySvc to check for a new scalefactor
    bool needupdate;
    float sf = m_beamLumi->scaleFactor(pEvent->event_ID()->run_number(), pEvent->event_ID()->lumi_block(), needupdate );
    float currentaveragemu(sf*m_maxCollPerXing);
    if(!m_isEmbedding) {
      pOverEvent->setAverageInteractionsPerCrossing(currentaveragemu);
      //FIXME check whether actualInteractionsPerCrossing should be set
      //to mu for the central bunch crossing, as below, or whether it
      //should be set to the actual number of minbias piled-up in the
      //central bunch crossing for this particular event.
      pOverEvent->setActualInteractionsPerCrossing(m_beamInt->normFactor(0)*currentaveragemu);
    }
    ATH_MSG_DEBUG ( "BCID = "<< t0BCID <<
                    ", mu =" << pOverEvent->actualInteractionsPerCrossing() <<
                    ", <mu> =" << pOverEvent->averageInteractionsPerCrossing() );
    ToolHandleArray<IBkgStreamsCache>::iterator cacheIterator(m_caches.begin());
    ToolHandleArray<IBkgStreamsCache>::iterator endOfCaches(m_caches.end());
    while (cacheIterator != endOfCaches) {
      if (needupdate) (*cacheIterator)->resetEvtsPerXingScaleFactor(sf);//set cache scale factor. some caches should ignore it?
      (*cacheIterator++)->newEvent(); //inform cache we overlay a new event
    }
    //-----------------------------------------------------------------------
    // loop over x-ings
    //-----------------------------------------------------------------------
    for (int iXing=m_firstXing; iXing<=m_lastXing; iXing++) {
      // if (m_xingByXing.value()) test that PileUpToolsAlg is there?
      //-----------------------------------------------------------------------
      // Read input events in bkg stores and link them to overlay store
      //-----------------------------------------------------------------------
      int t0BinCenter((int)m_xingFreq*iXing);
      unsigned int currentBCID = getBCID((t0BinCenter/25), t0BCID); //Should account for m_xingFreq>25. FIXME - hardcoded 25ns
      ATH_MSG_VERBOSE ( "Background BCID =" << currentBCID );
      cacheIterator = m_caches.begin();
      while (cacheIterator != endOfCaches) {
        // Now set the BCID for background events - also requires changes to PileUpEventInfo, SubEvent
        if (((*cacheIterator)->addSubEvts(iXing-m_firstXing, *pOverEvent, t0BinCenter, m_loadProxies, currentBCID)).isFailure()) {
          //if (((*cacheIterator)->addSubEvts(iXing-m_firstXing, *pOverEvent, t0BinCenter)).isFailure()) {
          if (maxevt == -1 && m_allowSubEvtsEOF) {
            ATH_MSG_INFO ( "No more sub events for " << cacheIterator->name() );
            return StatusCode::SUCCESS;
          } else {
            ATH_MSG_FATAL ( "Error adding sub events to " << cacheIterator->name() );
            return StatusCode::FAILURE;
          }
        }
        ++cacheIterator;
      }
    }  //loop over xings

    //set active store back to the overlaid one
    pActiveStore->setStore(p_SGOver);

    // Execute event for all required algorithms. The overlaid event
    // (PileUpEventInfo) is considered to be current
    if(m_skipExecAlgs) {
      ATH_MSG_INFO ( "Firing reseed incident (" << pOverEvent->event_ID()->event_number() << "," << pOverEvent->event_ID()->run_number() << ")" );
      p_incidentSvc->fireIncident(ContextIncident<std::pair<unsigned,unsigned> >(name(),"ReseedIncident",std::pair<unsigned,unsigned>(pOverEvent->event_ID()->event_number(),pOverEvent->event_ID()->run_number())));
      p_incidentSvc->fireIncident(Incident(name(),"AfterReseedIncident"));
      ATH_MSG_INFO ( "Not going to process this event" );
    }
    else {
      if( !(executeEvent(reinterpret_cast<void*>(pOverEvent)).isSuccess()) )
        {
          ATH_MSG_ERROR ( "Terminating event processing loop due to errors" );
          return StatusCode::FAILURE;
        }
      else
        ATH_MSG_INFO ( "Event processing successfull" );
    }
    //if we are not doing overlap xing by xing clean up all stores at the end
    //the clean up is done object by object and controlled by
    //PileUpXingFolder.CacheRefreshFrequency property
    if (!m_xingByXing.value()) {
      // FIXME test that PileUpToolsAlg is NOT there?
      //clear all stores as configured in
      if( !(p_mergeSvc->clearDataCaches().isSuccess()) ) {
        ATH_MSG_ERROR ( "Can not clear data caches" );
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG ( "Cleared data caches" );
      }
    }
    if (!(p_SGOver->clearStore()).isSuccess() ) {
      ATH_MSG_ERROR ( "Clear of Overlaid Event data store failed" );
      return StatusCode::FAILURE;
    }
  }  //end of event loop

  if(m_skipExecAlgs) {
    m_nevt--;
    total_nevt--;
  }

  return (sc.isSuccess() ?
          (noTimeLimit ? StatusCode::SUCCESS : StatusCode::FAILURE) :
          sc);

}

StatusCode PileUpEventLoopMgr::seek(int evt)
{
  int nevtsToAdvance = evt-m_ncurevt;
  int nParam = m_nevt+1;
  ATH_MSG_INFO ( "In PileUpEventLoopMgr::seek. m_ncurevt=" << m_ncurevt <<
                 " nevtsToAdvance=" << nevtsToAdvance );

  m_skipExecAlgs = true;
  for(int i=0; i<nevtsToAdvance; ++i) {
    if(m_allowSerialAndMPToDiffer) {m_loadProxies = (nevtsToAdvance==(i+1));}
    if(this->nextEvent(nParam).isFailure()) {
      ATH_MSG_ERROR ( "Seek error! Failed to advance to evt=" << m_ncurevt+1 );
      return StatusCode::FAILURE;
    }
  }
  m_skipExecAlgs = false;
  return StatusCode::SUCCESS;
}

int PileUpEventLoopMgr::curEvent() const
{
  return m_nevt;
}

//=========================================================================
// setup input and overlay selctors and iterators
//=========================================================================
StatusCode
PileUpEventLoopMgr::setupStreams()
{
  StatusCode sc(StatusCode::SUCCESS);

  //OK now we can run
  // retrieve selector and setup iterator (the selector has been setup as an ExtService)
  if  (!(m_origSel.retrieve()).isSuccess()) {
    ATH_MSG_ERROR ( "Failed to find selector for " << m_origSel );
    return StatusCode::FAILURE;
  }

  m_origStream = PileUpStream("OriginalEvent", serviceLocator(), &*m_origSel);
  if (!m_origStream.setupStore()) {
    ATH_MSG_ERROR ( "Can not setup original evt store " << m_origSel );
    return StatusCode::FAILURE;
  }

  //now get the bkg stream caches, and set them up
  if( (sc=m_caches.retrieve()).isSuccess() ) {
    ATH_MSG_DEBUG ( "retrieved BkgStreamsCaches " << m_caches );
  } else {
    ATH_MSG_ERROR ( "Can not get BkgStreamsCaches " << m_caches );
    return sc;
  }
  ToolHandleArray<IBkgStreamsCache>::iterator cacheIterator(m_caches.begin());
  ToolHandleArray<IBkgStreamsCache>::iterator endOfCaches(m_caches.end());
  unsigned int nXings(m_lastXing-m_firstXing+1);
  unsigned int firstStore(0);
  while ((cacheIterator != endOfCaches) && sc.isSuccess()) {
    sc = (*cacheIterator)->setup(m_firstXing, nXings, firstStore, &*m_beamInt);
    firstStore += (*cacheIterator)->nStores() ;
    ++cacheIterator;
  }
  return sc;
}



//=========================================================================
// Run the algorithms beginRun hook
//=========================================================================
StatusCode PileUpEventLoopMgr::beginRunAlgorithms() {

  // Call the execute() method of all top algorithms
  for ( ListAlg::iterator ita = m_topAlgList.begin();
        ita != m_topAlgList.end();
        ita++ )
    {
      const StatusCode& sc = (*ita)->sysBeginRun();
      if ( !sc.isSuccess() ) {
        ATH_MSG_INFO ( "beginRun of algorithm " <<
                       (*ita)->name() << " failed with StatusCode::" << sc );
        return sc;
      }
    }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms endRun hook
//=========================================================================
StatusCode PileUpEventLoopMgr::endRunAlgorithms() {

  // Call the execute() method of all top algorithms
  for ( ListAlg::iterator ita = m_topAlgList.begin();
        ita != m_topAlgList.end();
        ita++ )
    {
      const StatusCode& sc = (*ita)->sysEndRun();
      if ( !sc.isSuccess() ) {
        ATH_MSG_INFO ( "endRun of algorithm "  <<
                       (*ita)->name() << " failed with StatusCode::" << sc );
        return sc;
      }
    }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms for the current event
//=========================================================================
StatusCode PileUpEventLoopMgr::executeAlgorithms() {

  // Call the execute() method of all top algorithms
  for ( ListAlg::iterator ita = m_topAlgList.begin();
        ita != m_topAlgList.end();
        ita++ )
    {
      (*ita)->resetExecuted();
      const StatusCode& sc = (*ita)->sysExecute();
      if ( !sc.isSuccess() ) {
        ATH_MSG_INFO ( "Execution of algorithm " <<
                       (*ita)->name() << " failed with StatusCode::" << sc );
        return sc;
      }
    }

  return StatusCode::SUCCESS;
}


//=========================================================================
// executeEvent(void* par)
//=========================================================================
StatusCode PileUpEventLoopMgr::executeEvent(void* par)
{

  const EventInfo* pEvent(reinterpret_cast<PileUpEventInfo*>(par)); //AUIII!
  assert(pEvent);

  /// Fire begin-Run incident if new run:
  if (m_firstRun || (m_currentRun != pEvent->event_ID()->run_number()) ) {
    // Fire EndRun incident unless this is the first run
    if (!m_firstRun) {
      m_incidentSvc->fireIncident(Incident(name(),"EndRun"));
      if (!(this->endRunAlgorithms()).isSuccess()) return (StatusCode::FAILURE);
    }
    m_firstRun=false;
    m_currentRun = pEvent->event_ID()->run_number();

    ATH_MSG_INFO ( "  ===>>>  start of run " << m_currentRun << "    <<<===" );

    // Fire BeginRun "Incident"
    m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),"BeginRun"));
    if (!(this->beginRunAlgorithms()).isSuccess()) return (StatusCode::FAILURE);
  }

  // Fire BeginEvent "Incident"
  m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),"BeginEvent"));

  // Execute Algorithms
  //  StatusCode sc = MinimalEventLoopMgr::executeEvent(par);

  bool eventFailed(false);
  // Call the execute() method of all top algorithms
  StatusCode sc = executeAlgorithms();

  if(!sc.isSuccess()) {
    eventFailed = true;

    /// m_failureMode 1,
    /// RECOVERABLE: skip algorithms, but do not terminate job
    /// FAILURE: terminate job
    if (m_failureMode == 1 && sc.isRecoverable() ) {
      ATH_MSG_WARNING ( "RECOVERABLE error returned by algorithm. " <<
                        "Skipping remaining algorithms." << std::endl <<
                        "\tNo output will be written for this event, " <<
                        "but job will continue to next event" );
      eventFailed = false;
    }

    /// m_failureMode 2: skip algorithms, but do not terminate job
    if (m_failureMode >= 2) {
      ATH_MSG_INFO ( "Skipping remaining algorithms." << std::endl <<
                     "\tNo output will be written for this event, " <<
                     "but job will continue to next event" );
      eventFailed = false;
    }
  }  else {
    // Call the execute() method of all output streams
    for (ListAlg::iterator ito = m_outStreamList.begin();
         ito != m_outStreamList.end(); ito++ ) {
      (*ito)->resetExecuted();
      sc = (*ito)->sysExecute();
      if( !sc.isSuccess() ) {
        eventFailed = true;
      }
    }
  }

  // Fire EndEvent "Incident"
  m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),"EndEvent"));

  //------------------------------------------------------------------------
  // Check if there was an error processing current event
  //------------------------------------------------------------------------
  return eventFailed?StatusCode::FAILURE:StatusCode::SUCCESS;

}

StatusCode PileUpEventLoopMgr::queryInterface(const InterfaceID& riid,void** ppvInterface)
{
  if(IEventSeek::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IEventSeek*>(this);
  }
  else {
    // Interface is not directly available : try out a base class
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
