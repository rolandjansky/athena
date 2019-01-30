/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/ITriggerTime.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/DataStore.h"
#include "PileUpTools/IPileUpXingFolder.h"

#include "EventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h" 
#include "EventInfo/PileUpEventInfo.h" 
#include "EventInfo/PileUpTimeEventIndex.h"
#include "xAODCore/tools/PrintHelpers.h"

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM
#include "xAODEventInfo/EventInfoContainer.h"    // NEW EDM
#include "xAODEventInfo/EventInfoAuxContainer.h" // NEW EDM

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include <algorithm>  
#include <cstdlib>  /* random */
#include <functional>
#include <string>

using std::make_pair;
using std::bind1st;
using std::mem_fun;
using std::string;

/// Standard Constructor
PileUpMergeSvc::PileUpMergeSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc), 
    p_overStore("StoreGateSvc", "StoreGateSvc"),
    p_activeStore("ActiveStoreSvc", "ActiveStoreSvc"),
    m_intervals(this),
    m_pITriggerTime(""),
    m_returnTimedData(true),
    m_xAODCnvTool("xAODMaker::EventInfoCnvTool/EventInfoCnvTool", this)
{
  declareProperty("Intervals", m_intervals, "Folders specifying bunch xing intervals for different data objects");
  declareProperty("TriggerTimeTool", m_pITriggerTime, "allows to apply a trigger time offset");
  declareProperty("ReturnTimedData", m_returnTimedData, 
		  "determine whether the TimedData returned by retrieveSubEvts have non trivial PileUpTimeEventIndex. May be set to false for overlay with real events ");
  declareProperty( "xAODCnvTool", m_xAODCnvTool );
}

/// setup PileUpIntervals
void PileUpMergeSvc::decodeIntervals() {
  if (m_intervals.retrieve().isSuccess()) {
    ToolHandleArray<IPileUpXingFolder>::const_iterator 
      iXing(m_intervals.begin()), iEnd(m_intervals.end());
    while (iXing != iEnd) {
      const IPileUpXingFolder& xing(**iXing);
      if (msg().level() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "decodeIntervals: adding IPileUpXingFolder "
	      << xing.name() << endmsg;
      }
      IPileUpXingFolder::const_iterator 
	item(xing.begin()), endItem(xing.end());
      while(item != endItem) {
	m_ranges[make_pair(item->id(), item->key())]=
	  Range(xing.firstXing(), xing.lastXing(), xing.cacheRefreshFrequency());
	if (msg().level() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE 
		<< "decodeInterval: accepting objects of CLID " << item->id()
		<< " key " << item->key() << " in the Xing range ["
		<< xing.firstXing() << ", " << xing.lastXing() << ']' 
		<< " with cache refresh frequency " 
		<< xing.cacheRefreshFrequency() << endmsg;
	}
	++item;
      }
      ++iXing;
    }
  }
}

//////////////////////////////////////////////////////////////
/// Service initialisation
StatusCode 
PileUpMergeSvc::initialize()    {
  msg() << MSG::INFO << "Initializing AthService " << name() 
	<< " - package version " << PACKAGE_VERSION << endmsg ;
  // set up the SG service:
  if ( !(p_overStore.retrieve()).isSuccess() ) 
  {
      msg() << MSG::FATAL 
	  << "Could not locate default store"
          << endmsg;
      return StatusCode::FAILURE;
  }
  // set up the active store service:
  if ( !(p_activeStore.retrieve()).isSuccess() ) 
  {
      msg() << MSG::FATAL 
	  << "Could not locate ActiveStoreSvc"
          << endmsg;
      return StatusCode::FAILURE;
  }
  if (!m_pITriggerTime.empty() && !(m_pITriggerTime.retrieve()).isSuccess() ) 
  {
      msg() << MSG::FATAL 
	  << "Could not locate ITriggerTime tool"
          << endmsg;
      return StatusCode::FAILURE;
  }
  // Retrieve the converter tool:
  CHECK(m_xAODCnvTool.retrieve());

  decodeIntervals();
  
  // Initialize service:
  return StatusCode::SUCCESS;

}


const xAOD::EventInfo* PileUpMergeSvc::getPileUpEvent( StoreGateSvc* sg, const std::string& einame ) const
{
   if( !sg ) sg = p_overStore.get();
   const xAOD::EventInfo* xAODEventInfo = einame.empty()?
      sg->tryConstRetrieve<xAOD::EventInfo>()
      : sg->tryConstRetrieve<xAOD::EventInfo>( einame );
   if( xAODEventInfo ) {
      ATH_MSG_INFO("Found xAOD::EventInfo");
      ATH_MSG_INFO(" EventInfo has " <<   xAODEventInfo->subEvents().size() << " subevents" );
      if( xAODEventInfo->evtStore() == nullptr ) {
         // SG is 0 only when the xAODEventInfo is first read
         xAODEventInfo->setEvtStore( sg );
         // the loop below serves 2 purposes: to recreate subevent links cache
         // and set SG pointer in subevents 
         for( auto& subev : xAODEventInfo->subEvents() ) {
            subev.ptr()->setEvtStore( sg );
         }
      }
   } else {
      // Try reading old EventInfo
      const EventInfo* pEvent = einame.empty()?
         sg->tryConstRetrieve< ::EventInfo >()
         : sg->tryConstRetrieve< ::EventInfo >( einame );
      if( pEvent ) {
         ATH_MSG_INFO("Found OverlayEvent old type EventInfo" );

         ATH_MSG_DEBUG("Converting (PileUp)EventInfo to xAOD::EventInfo");
         // Create the xAOD object(s):
         std::unique_ptr< xAOD::EventInfo >  pxAODEventInfo( new xAOD::EventInfo() );
         std::unique_ptr< xAOD::EventAuxInfo > pxAODEventAuxInfo(new xAOD::EventAuxInfo());
         pxAODEventInfo->setStore( pxAODEventAuxInfo.get() );
         pxAODEventInfo->setEvtStore( sg );
         if( !m_xAODCnvTool->convert( pEvent, pxAODEventInfo.get(), false, false ).isSuccess() ) {
            ATH_MSG_ERROR("Failed to convert  xAOD::EventInfo in SG");
            return nullptr;
         }

         //ATH_MSG_INFO ("Dumping xAOD::EventInfo prior to adding SubEvents");
         //xAOD::dump( *xAODEventInfo );

         const PileUpEventInfo* pileupEvent(dynamic_cast<const PileUpEventInfo*>(pEvent));
         if( pileupEvent ) {
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
            auto pu_itr = pileupEvent->beginSubEvt();
            auto pu_end = pileupEvent->endSubEvt();
            const unsigned int countEvents = std::distance(pu_itr,pu_end);
            ATH_MSG_VERBOSE( "CHECKING: There are " << countEvents << " subevents in this Event." );
            for( ; pu_itr != pu_end; ++pu_itr ) {
               // Create a new xAOD::EventInfo object:
               std::unique_ptr< xAOD::EventInfo > ei( new xAOD::EventInfo() );
               // Fill it with information:
               if( ! m_xAODCnvTool->convert( pu_itr->pSubEvt, ei.get(), true, false ).isSuccess() ) {
                  ATH_MSG_ERROR("Failed to convert EventInfo to xAOD::EventInfo");
                  continue;
               }

               StoreGateSvc* tmpSG = pu_itr->pSubEvtSG;
               if(tmpSG) {
                  ei->setEvtStore(tmpSG);
                  ATH_MSG_VERBOSE("FOUND A STOREGATE");
               } else {
                  ATH_MSG_ERROR("FAILED TO FIND A STOREGATE");
               }
               // Store new EI into the container
               puei->push_back( ei.release() );
      
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
                                                               EiLink( "PileUpEventInfo", puei->size()-1, sg )));  // p_SG?
               ATH_MSG_VERBOSE("PileUpEventInfo: time = " << subEvents.back().time() << ", index = " << subEvents.back().index());
            }

            if( subEvents.size() ) {
               // And now update the main EventInfo object with the sub-events:
               pxAODEventInfo->setSubEvents( subEvents );

               // Record the xAOD object(s):
               if( !sg->record( std::move( puaux ), "PileUpEventInfoAux." ).isSuccess()
                   || !sg->record( std::move( puei ), "PileUpEventInfo" ).isSuccess() ) {  //MN: FIX - make keys configurable
                  ATH_MSG_ERROR("Failed to record xAOD::EventInfoContainer in SG");
               }
            }
         }
         xAODEventInfo = pxAODEventInfo.get();  // remember pointer to return the new EventInfo
         // Record the xAOD object(s):
         if( ! sg->record( std::move( pxAODEventAuxInfo ), "EventInfoAux." ).isSuccess() //MN: FIX? key
             || ! sg->record( std::move( pxAODEventInfo ), "EventInfo" ).isSuccess() ) {
            ATH_MSG_ERROR("Failed to record the new xAOD::EventInfo in SG");
         }
      }
   }

   if( !xAODEventInfo ) {
      ATH_MSG_DEBUG("Could not find EventInfo '" << einame << "' in store " << sg->name());
   }
   return xAODEventInfo;
}


const InterfaceID& 
PileUpMergeSvc::interfaceID() {
  static const InterfaceID IID_IPileUpMergeSvc(9991, 1, 0); //FIXME
  return IID_IPileUpMergeSvc; 
} 

  // Query the interfaces.
  //   Input: riid, Requested interface ID
  //          ppvInterface, Pointer to requested interface
  //   Return: StatusCode indicating SUCCESS or FAILURE.
  // N.B. Don't forget to release the interface after use!!!
StatusCode 
PileUpMergeSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  //FIXME
  if ( interfaceID().versionMatch(riid) )    {
    *ppvInterface = (PileUpMergeSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool
PileUpMergeSvc::isLive(CLID id, const string& dataKey, int iXing) {
  return m_ranges[make_pair(id, dataKey)].contains(iXing);
}

bool
PileUpMergeSvc::doRefresh(const Range& r, int iXing) {
  //  bool result(r.contains(iXing) && r.doRefresh(double(random())/RAND_MAX));
  //  std::cerr << "doRefresh: id " << id << " key " << dataKey 
  //	    << " xing " << iXing << " result " << result << std::endl;
  return (r.contains(iXing) && r.doRefresh(double(random())*(1./RAND_MAX)));
}

StatusCode
PileUpMergeSvc::clearDataCaches() {
  StatusCode sc(StatusCode::FAILURE);
  const xAOD::EventInfo* pEvent;
  if (0 != (pEvent=getPileUpEvent())) {
    // access the sub events DATA objects...
    for( const xAOD::EventInfo::SubEvent& subEv : pEvent->subEvents() ) {
      StoreGateSvc* pSubEvtSG( subEv.ptr()->evtStore() );
      assert(pSubEvtSG);
      //go object-by-object (driven by PileUpXingFolder settings)
      for (const auto& item : m_ranges) {
        SG::sgkey_t sgkey = pSubEvtSG->stringToKey (item.first.second,
                                                      item.first.first);
        SG::DataProxy* proxy = pSubEvtSG->proxy_exact (sgkey);
        //FIXME turning the double iEvt->time is fraught with peril. Luckily 
        //FIXME it just works, but we should have the beam xing in iEvt
        if (proxy && doRefresh (item.second, int(subEv.time()))) {
          proxy->setObject ((DataObject*)0);
          if (msg().level() <= MSG::DEBUG) {
            msg() << MSG::DEBUG
                  << "clearDataCachesByFolder: object with clid "
                  << item.first.first << " and key " << item.first.second
                  << " removed from cache " 
                  << pSubEvtSG->name() << endmsg;
          }
        }
      }
      //even if we don't clear the store we need to empty the trash...
      pSubEvtSG->emptyTrash();
#ifndef NDEBUG
      if (msg().level() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE
	      << "clearDataCachesByFolder: done with store " << pSubEvtSG->name()
	      << endmsg;
      }
#endif
    } //stores loop
    sc=StatusCode::SUCCESS;
  } //NO PILEUP EVENT?!?
  return sc;
}
