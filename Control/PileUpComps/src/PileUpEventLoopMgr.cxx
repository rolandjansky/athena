/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "PileUpEventLoopMgr.h"

// Athena includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ITimeKeeper.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/EventContextClid.h"

#include "EventInfo/PileUpEventInfo.h" // OLD EDM
#include "EventInfo/EventID.h"         // OLD EDM
#include "EventInfo/EventType.h"       // OLD EDM
#include "EventInfo/EventIncident.h"   // OLD EDM

#include "PileUpTools/IBeamIntensity.h"
#include "PileUpTools/IBeamLuminosity.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "PileUpTools/IBkgStreamsCache.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM
#include "xAODEventInfo/EventInfoContainer.h"    // NEW EDM
#include "xAODEventInfo/EventInfoAuxContainer.h" // NEW EDM
#include "xAODCnvInterfaces/IEventInfoCnvTool.h"

// xAOD include(s):
//#include "xAODCore/tools/PrintHelpers.h"

// Gaudi headers
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataIncident.h" // For ContextIncident
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/Algorithm.h"

// std library headers
#include <cmath>
//#include <vector>

//=========================================================================
// Standard Constructor
//=========================================================================
PileUpEventLoopMgr::PileUpEventLoopMgr(const std::string& name,
                                       ISvcLocator* svcLoc)
  : MinimalEventLoopMgr(name, svcLoc),
    m_incidentSvc("IncidentSvc", name), //FIXME should this be configurable?
    m_mergeSvc("PileUpMergeSvc", name),
    m_evtStore("StoreGateSvc/StoreGateSvc",  name),
    //m_nInputs(0), m_nStores(0),
    m_origSel("EventSelector", name),
    m_signalSel("", name),
    m_caches(this),
    m_maxCollPerXing(23.0),
    m_xingFreq(25), m_firstXing(-2), m_lastXing(+1),
    m_timeKeeper("",name), m_allowSubEvtsEOF(true),
    m_xingByXing(false), m_isEventOverlayJob(false),
    m_failureMode(1),
    m_beamInt("FlatBM", name),
    m_beamLumi("LumiProfileSvc", name),
    m_xAODCnvTool("xAODMaker::EventInfoCnvTool/EventInfoCnvTool", this),//Will need a special configuration here + extra config tweaks
    m_currentRun(0), m_firstRun(true),
    m_msg( name ),
    m_maxBunchCrossingPerOrbit(3564),
    m_nevt(0),
    m_ncurevt(0),
    m_skipExecAlgs(false),
    m_loadProxies(true),
    m_eventContext(nullptr),
    m_isEmbedding(false),
    m_allowSerialAndMPToDiffer(true)
{
  declareProperty("MaxBunchCrossingPerOrbit", m_maxBunchCrossingPerOrbit, "The number of slots in each LHC beam. Default: 3564.");
  declareProperty("OrigSelector", m_origSel, "EventSelector for original (physics) events stream" );
  declareProperty("SignalSelector", m_signalSel, "EventSelector for signal (hard-scatter) events stream" );
  declareProperty("XingFrequency", m_xingFreq, "ns");
  declareProperty("firstXing", m_firstXing, "time of first xing / XingFrequency (0th xing is 1st after trigger)");
  declareProperty("lastXing", m_lastXing, "time of last xing / XingFrequency (0th xing is 1st after trigger)");
  declareProperty("MaxMinBiasCollPerXing", m_maxCollPerXing, "Set to digitization numberOfCollisions prop. for variable-mu and RunDMC jobs.");
  declareProperty("bkgCaches", m_caches, "list of tools managing bkg events");
  declareProperty("TimeKeeper", m_timeKeeper, "time keeper service, terminates event loop if needed. NOP by default");
  declareProperty("AllowSubEvtsEOF", m_allowSubEvtsEOF, "if true(default) an EOF condition in the BkgStreamsCaches is not considered to be an error IF maxevt=-1 (loop over all available events)");
  declareProperty("XingByXing", m_xingByXing, "if set to true we will not cache bkg events from one xing to then next. This greatly increases the amount of I/O and greatly reduces the memory required to run a job");
  declareProperty("IsEventOverlayJob", m_isEventOverlayJob, "if set to true will prevent the BCID from being overridden.");
  declareProperty("IsEventOverlayJobMC", m_isEventOverlayJobMC, "if set to true events will be marked as simulation");
  declareProperty("FailureMode", m_failureMode,
                  "Controls behaviour of event loop depending on return code of"
                  " Algorithms. 0: all non-SUCCESSes terminate job. "
                  "1: RECOVERABLE skips to next event, FAILURE terminates job "
                  "(DEFAULT). 2: RECOVERABLE and FAILURE skip to next events");

  declareProperty("BeamInt", m_beamInt,
                  "The service providing the beam intensity distribution");
  declareProperty("BeamLuminosity", m_beamLumi,
                  "The service providing the beam luminosity distribution vs. run");
  declareProperty("PileUpMergeSvc", m_mergeSvc, "PileUp Merge Service");
  declareProperty( "xAODCnvTool", m_xAODCnvTool );
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
  try
    {
      CHECK(setProperties());
      // configure our MsgStream
      m_msg.get().setLevel( m_outputLevel.value() );
    }
  catch (...)
    {
      ATH_MSG_WARNING ( "Caught exception thrown reading in properties" );
    }

  ATH_MSG_INFO ( "Initializing " << this->name() << " - package version " << PACKAGE_VERSION ) ;
  if(!m_allowSerialAndMPToDiffer)
    {
      ATH_MSG_WARNING ( "AllowSerialAndMPToDiffer=False! This will incur serious performance penalties! But Serial and MP output will be the same." );
    }

  //locate the StoreGateSvc and initialize our local ptr
  CHECK(m_evtStore.retrieve());

  //reset output store proxy provider: we don't want to have one!
  m_evtStore->setProxyProviderSvc(0);

  //locate the IncidentSvc and initialize our local ptr
  CHECK(m_incidentSvc.retrieve());

  // Retrieve the converter tool:
  CHECK(m_xAODCnvTool.retrieve());

  //locate the PileUpMergeSvc and initialize our local ptr
  CHECK(m_mergeSvc.retrieve());

  //-------------------------------------------------------------------------
  // Setup Event Selector
  //-------------------------------------------------------------------------
  CHECK(this->setupStreams());
  //-------------------------------------------------------------------------
  // Setup TimeKeeper service
  //-------------------------------------------------------------------------

  // We do not expect a TimeKeeper necessarily being declared
  if (!m_timeKeeper.empty())
    {
      CHECK(m_timeKeeper.retrieve());
    }
  else
    {
      ATH_MSG_INFO ( "No TimeKeeper selected. No time limit control on event loop." );
    }

  // Get the value of SkipEvents. It is needed for seeking
  SmartIF<IProperty> prpMgr(serviceLocator());
  IEvtSelector* evtSelector(0);
  if(prpMgr.isValid())
    {
      // Get event selector name. Retrieve EventSelector
      std::string evtSelName = prpMgr->getProperty("EvtSel").toString();
      CHECK(serviceLocator()->service(evtSelName,evtSelector));
    }
  else
    {
      ATH_MSG_ERROR ( "IProperty interface not found in ApplicationMgr" );
      return StatusCode::FAILURE;
    }

  SmartIF<IProperty> prpMgr1(evtSelector);
  if(prpMgr1.isValid())
    {
      std::string skipEventsStr = prpMgr1->getProperty("SkipEvents").toString();
      m_ncurevt = std::atoi(skipEventsStr.c_str());
    }
  else
    {
      ATH_MSG_ERROR ( "IProperty interface not found on the event selector" );
      return StatusCode::FAILURE;
    }

  // Get the AlgExecStateSvc
  m_aess = serviceLocator()->service("AlgExecStateSvc");
  if( !m_aess.isValid() ) {
    fatal() << "Error retrieving AlgExecStateSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  // create the EventContext object
  m_eventContext = new EventContext();


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
  ATH_MSG_INFO ( "Finalizing " << this->name() << " - package version " << PACKAGE_VERSION );

  //we need to release all our BkgStreamCaches
  ToolHandleArray<IBkgStreamsCache>::iterator cacheIterator(m_caches.begin());
  ToolHandleArray<IBkgStreamsCache>::iterator endOfCaches(m_caches.end());
  while (cacheIterator != endOfCaches)
    {
      (*cacheIterator++)->release();
    }
  m_caches.clear();

  //and to clean up the store the stream owns
  CHECK(m_origStream.finalize());
  if (m_isEventOverlayJob) {
    CHECK(m_signalStream.finalize());
  }

  delete m_eventContext; m_eventContext = nullptr;

  return MinimalEventLoopMgr::finalize();
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
  if (0 == maxevt) { return StatusCode::SUCCESS; }


  static int        total_nevt = 0;

  //locate the ActiveStoreSvc and initialize our local ptr
  ActiveStoreSvc* pActiveStore(nullptr);
  CHECK(serviceLocator()->service("ActiveStoreSvc", pActiveStore));

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
  const EventInfo* pEventSignal(0);
  // loop over events if the maxevt (received as input) is different from -1.
  // if evtmax is -1 it means infinite loop (till time limit that is)
  bool noTimeLimit(false);
  while( (maxevt == -1 || m_nevt < maxevt) &&
         (noTimeLimit = (m_timeKeeper.empty() || m_timeKeeper->nextIter()) )  &&
         0 != (pEvent = m_origStream.nextEventPre()) ) //FIXME this call has to be able to cope with xAOD::EventInfo rather than EventInfo being in m_origStream
    {
      if (m_isEventOverlayJob) {
        pEventSignal = m_signalStream.nextEventPre();
      }

      // Check if there is a scheduled stop issued by some algorithm/sevice
      if ( m_scheduledStop )
        {
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
      if (!m_isEventOverlayJob)
        {
          // Use the position in the beam intensity array to set a BCID-like quantity
          t0BCID = m_beamInt->getCurrentT0BunchCrossing();
          pOvrID->set_bunch_crossing_id(t0BCID);
        }
      else if (m_isEventOverlayJobMC)
        {
          pOvrID->set_event_number(pEventSignal->event_ID()->event_number());
        }
      ATH_MSG_VERBOSE ( "BCID =" << pOvrID->bunch_crossing_id() );
      EventType *pOvrEt = new EventType(); //FIXME
      pOvrEt->set_user_type("Overlaid"); //FIXME
      if (m_isEventOverlayJob)
        {
          ATH_MSG_INFO ( "nextEvent(): using signal event " <<
                         pEventSignal->event_type()->mc_channel_number() << '/' <<
                         pEventSignal->event_type()->mc_event_number() );

          pOvrEt->set_mc_channel_number(pEventSignal->event_type()->mc_channel_number());
          pOvrEt->set_mc_event_number(pEventSignal->event_type()->mc_event_number());
          const unsigned int nWeights(pEventSignal->event_type()->n_mc_event_weights());
          for (unsigned int iWeight(0); iWeight<nWeights; ++iWeight)
            {
              pOvrEt->set_mc_event_weight(pEventSignal->event_type()->mc_event_weight(iWeight),iWeight);
            }

          // MC+MC overlay should always be marked as simulation
          if (m_isEventOverlayJobMC)
            {
              pOvrEt->add_type(EventType::IS_SIMULATION);
            }
        }
      else
        {
          pOvrEt->set_mc_channel_number(pEvent->event_type()->mc_channel_number());
          pOvrEt->set_mc_event_number(pEvent->event_type()->mc_event_number());
          const unsigned int nWeights(pEvent->event_type()->n_mc_event_weights());
          for (unsigned int iWeight(0); iWeight<nWeights; ++iWeight)
            {
              pOvrEt->set_mc_event_weight(pEvent->event_type()->mc_event_weight(iWeight),iWeight);
            }

          // Overlay RDO files should be treated like data for reco
          // purposes, so only set this for SimHit level pile-up.
          pOvrEt->add_type(EventType::IS_SIMULATION);
        }

      PileUpEventInfo *pOverEvent = new PileUpEventInfo(pOvrID, pOvrEt);
      if(m_isEmbedding || m_isEventOverlayJobMC)
        {
          pOverEvent->setActualInteractionsPerCrossing(pEvent->actualInteractionsPerCrossing());
          pOverEvent->setAverageInteractionsPerCrossing(pEvent->averageInteractionsPerCrossing());
        }
      if(m_isEventOverlayJob)
        {
          // Propagate core event flags
          pOverEvent->setEventFlags(EventInfo::Core, (pEventSignal->eventFlags(EventInfo::Core) | pEvent->eventFlags(EventInfo::Core)));
          pOverEvent->setErrorState(EventInfo::Core, std::max(pEventSignal->errorState(EventInfo::Core), pEvent->errorState(EventInfo::Core)));
        }
      ATH_MSG_VERBOSE ( "BCID =" << pOverEvent->event_ID()->bunch_crossing_id() );

      // when doing overlay add the hard-scatter event as sub-event
      if (m_isEventOverlayJob)
        {
          pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal,
                                pEventSignal, &m_signalStream.store());
        }

      //  register as sub event of the overlaid
      bool addpEvent(true);
      if(m_isEmbedding || m_isEventOverlayJobMC)
        {
          const PileUpEventInfo* pOldEvent(dynamic_cast<const PileUpEventInfo*>(pEvent));
          if(pOldEvent)
            {
              PileUpEventInfo::SubEvent::const_iterator it  = pOldEvent->beginSubEvt();
              PileUpEventInfo::SubEvent::const_iterator end = pOldEvent->endSubEvt();
              if (m_isEventOverlayJobMC)
                {
                  // we can skip the first sub-event when doing MC+MC overlay
                  ++it;
                }
              else
                {
                  if (it != end) { addpEvent=false; }
                }
              for (; it != end; ++it)
                {
                  const EventInfo* sevt = (*it).pSubEvt;
                  pOverEvent->addSubEvt((*it).time() , (*it).type(), sevt, &m_origStream.store());
                }
            }
        }
      if(addpEvent)
        {
          pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal,
                                pEvent, &m_origStream.store());
        }

      CHECK(m_evtStore->record(pOverEvent, "McEventInfo"));
      //add an alias to "OverlayEvent" (backward compatibility)
      CHECK(m_evtStore->setAlias(pOverEvent, "OverlayEvent"));
      //add an alias to "MyEvent" (backward compatibility)
      CHECK(m_evtStore->setAlias(pOverEvent, "MyEvent"));

      //symlink to EventInfo to support transparent access for standard code
      const EventInfo *pEI(0);
      CHECK(m_evtStore->symLink(pOverEvent, pEI));

      //FIXME at this point one may want to look into the original event
      //FIXME to decide whether to skip it or to do the pile-up

      ///ask the BeamLuminositySvc to check for a new scalefactor
      bool needupdate;
      float sf = m_beamLumi->scaleFactor(pEvent->event_ID()->run_number(), pEvent->event_ID()->lumi_block(), needupdate );
      float currentaveragemu(sf*m_maxCollPerXing);
      if(!m_isEmbedding && !m_isEventOverlayJobMC)
        {
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
      while (cacheIterator != endOfCaches)
        {
          if (needupdate) (*cacheIterator)->resetEvtsPerXingScaleFactor(sf);//set cache scale factor. some caches should ignore it?
          (*cacheIterator++)->newEvent(); //inform cache we overlay a new event
        }
      //-----------------------------------------------------------------------
      // loop over x-ings
      //-----------------------------------------------------------------------
      for (int iXing=m_firstXing; iXing<=m_lastXing; iXing++)
        {
          // if (m_xingByXing.value()) test that PileUpToolsAlg is there?
          //-----------------------------------------------------------------------
          // Read input events in bkg stores and link them to overlay store
          //-----------------------------------------------------------------------
          int t0BinCenter((int)m_xingFreq*iXing);
          unsigned int currentBCID = getBCID((t0BinCenter/25), t0BCID); //Should account for m_xingFreq>25. FIXME - hardcoded 25ns
          ATH_MSG_VERBOSE ( "Background BCID =" << currentBCID );
          cacheIterator = m_caches.begin();
          while (cacheIterator != endOfCaches)
            {
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

      //////////// START OF TEMPORARY HACK /////////////////////
      ATH_MSG_DEBUG("Converting PileUpEventInfo to xAOD::EventInfo");
      // Copy completed PileUpEventInfo into xAODEventInfo at this point temporarily
      // Create the xAOD object(s):
      std::unique_ptr< xAOD::EventAuxInfo > pxAODEventAuxInfo(new xAOD::EventAuxInfo());
      std::unique_ptr< xAOD::EventInfo > pxAODEventInfo( new xAOD::EventInfo());
      pxAODEventInfo->setStore( pxAODEventAuxInfo.get() );
      CHECK(m_xAODCnvTool->convert( pOverEvent, pxAODEventInfo.get(), true, false ));
       // Before the record, keep a pointer to the EventInfo object, to be able
      // to add ElementLinks to it afterwards.
      xAOD::EventInfo* pLocalXAODEventInfo = pxAODEventInfo.get();

      //ATH_MSG_INFO ("Dumping xAOD::EventInfo prior to adding SubEvents");
      //xAOD::dump( *pLocalXAODEventInfo );
      // Record the xAOD object(s):
      CHECK( m_evtStore->record( std::move( pxAODEventAuxInfo ), "EventInfoAux." ) );
      CHECK( m_evtStore->record( std::move( pxAODEventInfo ), "EventInfo" ) );

      // Create an EventInfoContainer for the pileup events:
      std::unique_ptr< xAOD::EventInfoContainer > puei(new xAOD::EventInfoContainer());
      std::unique_ptr< xAOD::EventInfoAuxContainer > puaux(new xAOD::EventInfoAuxContainer());
      puei->setStore( puaux.get() );

      // Sub-events for the main EventInfo object:
      std::vector< xAOD::EventInfo::SubEvent > subEvents;

      // A map translating between the AOD and xAOD pileup event types:
      static std::map< PileUpEventInfo::SubEvent::pileup_type,
                       xAOD::EventInfo::PileUpType > pileupTypeMap;
      if( ! pileupTypeMap.size() ) {
#define DECLARE_SE_TYPE( TYPE )                                         \
         pileupTypeMap[ PileUpTimeEventIndex::TYPE ] = xAOD::EventInfo::TYPE

         DECLARE_SE_TYPE( Unknown );
         DECLARE_SE_TYPE( Signal );
         DECLARE_SE_TYPE( MinimumBias );
         DECLARE_SE_TYPE( Cavern );
         DECLARE_SE_TYPE( HaloGas );
         DECLARE_SE_TYPE( ZeroBias );

#undef DECLARE_SE_TYPE
      }

      // A convenience type declaration:
      typedef ElementLink< xAOD::EventInfoContainer > EiLink;

      // Create xAOD::EventInfo objects for each pileup EventInfo object:
      PileUpEventInfo::SubEvent::const_iterator pu_itr = pOverEvent->beginSubEvt();
      PileUpEventInfo::SubEvent::const_iterator pu_end = pOverEvent->endSubEvt();
      const unsigned int countEvents = std::distance(pu_itr,pu_end);
      ATH_MSG_VERBOSE( "CHECKING: There are " << countEvents << " subevents in this Event." );

      for( ; pu_itr != pu_end; ++pu_itr ) {
         // Create a new xAOD object:
         xAOD::EventInfo* ei = new xAOD::EventInfo();
         puei->push_back( ei );
         // Fill it with information:
         CHECK( m_xAODCnvTool->convert( pu_itr->pSubEvt, ei, true, false ) );
         StoreGateSvc* tmpSG = pu_itr->pSubEvtSG;
         if(tmpSG)
           {
             ei->setEvtStore(tmpSG);
             ATH_MSG_VERBOSE("FOUND A STOREGATE");
           }
         else
           {
             ATH_MSG_ERROR("FAILED TO FIND A STOREGATE");
           }
         // And now add a sub-event to the temporary list:
         auto typeItr = pileupTypeMap.find( pu_itr->type() );
         xAOD::EventInfo::PileUpType type = xAOD::EventInfo::Unknown;
         if( typeItr == pileupTypeMap.end() ) {
            ATH_MSG_WARNING( "PileUpType not recognised: " << pu_itr->type() );
         } else {
            type = typeItr->second;
         }
         ATH_MSG_VERBOSE("PileUpEventInfo: time = " << pu_itr->time() << ", index = " << pu_itr->index());
         subEvents.push_back( xAOD::EventInfo::SubEvent( pu_itr->time(),
                                                         pu_itr->index(),
                                                         type,
                                                         EiLink( "PileUpEventInfo",
                                                                 puei->size() -
                                                                 1,
                                                                 m_evtStore.operator->() ) ) );
         ATH_MSG_VERBOSE("PileUpEventInfo: time = " << subEvents.back().time() << ", index = " << subEvents.back().index());
      }

      // And now update the main EventInfo object with the sub-events:
      pLocalXAODEventInfo->setSubEvents( subEvents );

      // Record the xAOD object(s):
      CHECK( m_evtStore->record( std::move( puaux ), "PileUpEventInfoAux." ) );
      CHECK( m_evtStore->record( std::move( puei ), "PileUpEventInfo" ) );
      //ATH_MSG_INFO("Dumping xAOD::EventInfo after adding SubEvents");
      //xAOD::dump( *pLocalXAODEventInfo );

      //////////// END OF TEMPORARY HACK /////////////////////

      //set active store back to the overlaid one
      pActiveStore->setStore(&(*m_evtStore));

      // Execute event for all required algorithms. The overlaid event
      // (PileUpEventInfo) is considered to be current
      if(m_skipExecAlgs)
        {
          ATH_MSG_INFO ( "Firing reseed incident (" << pOverEvent->event_ID()->event_number() << "," << pOverEvent->event_ID()->run_number() << ")" );
          m_incidentSvc->fireIncident(ContextIncident<std::pair<unsigned,unsigned> >(this->name(),"ReseedIncident",std::pair<unsigned,unsigned>(pOverEvent->event_ID()->event_number(),pOverEvent->event_ID()->run_number())));
          m_incidentSvc->fireIncident(Incident(this->name(),"AfterReseedIncident"));
          ATH_MSG_INFO ( "Not going to process this event" );
        }
      else
        {
          if( !(executeEvent(reinterpret_cast<void*>(pOverEvent)).isSuccess()) )
            {
              ATH_MSG_ERROR ( "Terminating event processing loop due to errors" );
              return StatusCode::FAILURE;
            }
          else
            {
              ATH_MSG_INFO ( "Event processing successful." );
            }
        }
      //if we are not doing overlap xing by xing clean up all stores at the end
      //the clean up is done object by object and controlled by
      //PileUpXingFolder.CacheRefreshFrequency property
      if (!m_xingByXing.value())
        {
          // FIXME test that PileUpToolsAlg is NOT there?
          //clear all stores as configured in
          CHECK(m_mergeSvc->clearDataCaches());
          ATH_MSG_DEBUG ( "Cleared data caches" );
        }
      CHECK(m_evtStore->clearStore());
    }  //end of event loop

  if(m_skipExecAlgs)
    {
      m_nevt--;
      total_nevt--;
    }

  return (noTimeLimit ? StatusCode::SUCCESS : StatusCode::FAILURE);

}

StatusCode PileUpEventLoopMgr::seek(int evt)
{
  int nevtsToAdvance = evt-m_ncurevt;
  int nParam = m_nevt+1;
  ATH_MSG_INFO ( "In PileUpEventLoopMgr::seek. m_ncurevt=" << m_ncurevt <<
                 " nevtsToAdvance=" << nevtsToAdvance );

  m_skipExecAlgs = true;
  for(int i=0; i<nevtsToAdvance; ++i)
    {
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
  //OK now we can run
  // retrieve selector and setup iterator (the selector has been setup as an ExtService)
  CHECK(m_origSel.retrieve());

  m_origStream = PileUpStream("OriginalEvent", serviceLocator(), &*m_origSel);
  if (!m_origStream.setupStore())
    {
      ATH_MSG_ERROR ( "Can not setup original evt store " << m_origSel );
      return StatusCode::FAILURE;
    }
    
  if (m_isEventOverlayJob)
    {
      CHECK(m_signalSel.retrieve());

      m_signalStream = PileUpStream("BkgEvent_0", serviceLocator(), &*m_signalSel);
      if (!m_signalStream.setupStore())
        {
          ATH_MSG_ERROR("Can not setup signal evt store " << m_signalSel);
          return StatusCode::FAILURE;
        }
    }

  //now get the bkg stream caches, and set them up
  CHECK(m_caches.retrieve());
  ATH_MSG_DEBUG ( "retrieved BkgStreamsCaches " << m_caches );

  ToolHandleArray<IBkgStreamsCache>::iterator cacheIterator(m_caches.begin());
  ToolHandleArray<IBkgStreamsCache>::iterator endOfCaches(m_caches.end());
  unsigned int nXings(m_lastXing-m_firstXing+1);
  unsigned int firstStore(0);
  StatusCode sc(StatusCode::SUCCESS);
  while ((cacheIterator != endOfCaches) && sc.isSuccess())
    {
      sc = (*cacheIterator)->setup(m_firstXing, nXings, firstStore, &*m_beamInt);
      firstStore += (*cacheIterator)->nStores() ;
      ++cacheIterator;
    }
  return sc;
}



//=========================================================================
// Run the algorithms beginRun hook
//=========================================================================
StatusCode PileUpEventLoopMgr::beginRunAlgorithms()
{
  // Call the execute() method of all top algorithms
  for ( ListAlg::iterator ita = m_topAlgList.begin();
        ita != m_topAlgList.end();
        ita++ )
    {
      const StatusCode& sc = (*ita)->sysBeginRun();
      if ( !sc.isSuccess() )
        {
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
StatusCode PileUpEventLoopMgr::endRunAlgorithms()
{
  // Call the execute() method of all top algorithms
  for ( ListAlg::iterator ita = m_topAlgList.begin();
        ita != m_topAlgList.end();
        ita++ )
    {
      const StatusCode& sc = (*ita)->sysEndRun();
      if ( !sc.isSuccess() )
        {
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
StatusCode PileUpEventLoopMgr::executeAlgorithms()
{
  // Call the execute() method of all top algorithms
  for ( ListAlg::iterator ita = m_topAlgList.begin();
        ita != m_topAlgList.end();
        ita++ )
    {
      StatusCode sc = (*ita)->sysExecute(*m_eventContext);
      // this duplicates what is already done in Algorithm::sysExecute, which
      // calls Algorithm::setExecuted, but eventually we plan to remove that 
      // function
      m_aess->algExecState(*ita,*m_eventContext).setState(AlgExecState::State::Done, sc);
      if ( !sc.isSuccess() )
        {
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

  m_eventContext->setEventID( *((EventIDBase*) pEvent->event_ID()) );
  m_eventContext->set(m_nevt,0);

  /// Is this correct, or should it be set to a pileup store?
  m_eventContext->setExtension( Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(),
                                                            pEvent->event_ID()->run_number()) );
  Gaudi::Hive::setCurrentContext( m_eventContext );

  m_aess->reset(*m_eventContext);
  if (m_evtStore->record(std::make_unique<EventContext> (*m_eventContext),
                         "EventContext").isFailure())
  {
    m_msg << MSG::ERROR 
          << "Error recording event context object" << endmsg;
    return (StatusCode::FAILURE);
  }

  /// Fire begin-Run incident if new run:
  if (m_firstRun || (m_currentRun != pEvent->event_ID()->run_number()) )
    {
      // Fire EndRun incident unless this is the first run
      if (!m_firstRun)
        {
          m_incidentSvc->fireIncident(Incident(this->name(),IncidentType::EndRun));
          CHECK(this->endRunAlgorithms());
        }
      m_firstRun=false;
      m_currentRun = pEvent->event_ID()->run_number();

      ATH_MSG_INFO ( "  ===>>>  start of run " << m_currentRun << "    <<<===" );

      // Fire BeginRun "Incident"
      m_incidentSvc->fireIncident(EventIncident(this->name(),IncidentType::BeginRun,*m_eventContext));
      CHECK(this->beginRunAlgorithms());
    }

  // Fire BeginEvent "Incident"
  //m_incidentSvc->fireIncident(EventIncident(*pEvent, this->name(),"BeginEvent"));

  // Execute Algorithms
  //  StatusCode sc = MinimalEventLoopMgr::executeEvent(par);

  bool eventFailed(false);
  // Call the execute() method of all top algorithms
  StatusCode sc = executeAlgorithms();

  if(!sc.isSuccess())
    {
      eventFailed = true;
      m_aess->setEventStatus( EventStatus::AlgFail, *m_eventContext );

      /// m_failureMode 1,
      /// RECOVERABLE: skip algorithms, but do not terminate job
      /// FAILURE: terminate job
      if (m_failureMode == 1 && sc.isRecoverable() )
        {
          ATH_MSG_WARNING ( "RECOVERABLE error returned by algorithm. " <<
                            "Skipping remaining algorithms." << std::endl <<
                            "\tNo output will be written for this event, " <<
                            "but job will continue to next event" );
          eventFailed = false;
        }

      /// m_failureMode 2: skip algorithms, but do not terminate job
      if (m_failureMode >= 2)
        {
          ATH_MSG_INFO ( "Skipping remaining algorithms." << std::endl <<
                         "\tNo output will be written for this event, " <<
                         "but job will continue to next event" );
          eventFailed = false;
        }
    }
  else
    {
      m_aess->setEventStatus( EventStatus::Success, *m_eventContext );

      // Call the execute() method of all output streams
      for (ListAlg::iterator ito = m_outStreamList.begin();
           ito != m_outStreamList.end(); ito++ )
        {
          sc = (*ito)->sysExecute(*m_eventContext);
          if( !sc.isSuccess() )
            {
              eventFailed = true;
            }
        }
    }

  // Fire EndEvent "Incident"
  //m_incidentSvc->fireIncident(EventIncident(*pEvent, this->name(),"EndEvent"));

  //------------------------------------------------------------------------
  // Check if there was an error processing current event
  //------------------------------------------------------------------------
  return eventFailed?StatusCode::FAILURE:StatusCode::SUCCESS;

}

StatusCode PileUpEventLoopMgr::queryInterface(const InterfaceID& riid,void** ppvInterface)
{
  if(IEventSeek::interfaceID().versionMatch(riid))
    {
      *ppvInterface = dynamic_cast<IEventSeek*>(this);
    }
  else
    {
      // Interface is not directly available : try out a base class
      return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);
    }
  addRef();
  return StatusCode::SUCCESS;
}
