/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpStream.h"

#include <cassert>
#include <stdexcept>
#include <string>

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ISvcManager.h"
#include "AthenaKernel/CloneService.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "SGTools/DataProxy.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h" 
#include "EventInfo/PileUpEventInfo.h" 
#include "EventInfo/PileUpTimeEventIndex.h" 

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM
#include "xAODEventInfo/EventInfoContainer.h"    // NEW EDM
#include "xAODEventInfo/EventInfoAuxContainer.h" // NEW EDM
#include "xAODCnvInterfaces/IEventInfoCnvTool.h"
#include "xAODCore/tools/PrintHelpers.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

/* #include "GaudiKernel/IOpaqueAddress.h" */
class IOpaqueAddress;

#include <iostream>
using namespace std;

// EventInfo -> xAOD::EventInfo converter tool
xAODMaker::IEventInfoCnvTool* PileUpStream::m_xAODCnvTool = nullptr;

/// Structors
PileUpStream::PileUpStream():
  m_name("INVALID"), p_svcLoc(0), p_sel(0), p_SG(0), p_iter(0), 
  p_activeStore(0), m_ownEvtIterator(false),
  m_msg("PileUpStream"), m_neverLoaded(true), m_ownStore(false),
  m_used(false), m_hasRing(false), m_iOriginalRing(0)
{ 
}

PileUpStream::PileUpStream(const PileUpStream& rhs):
  m_name(rhs.m_name), p_svcLoc(rhs.p_svcLoc), p_sel(rhs.p_sel), 
  p_SG(rhs.p_SG), p_iter(rhs.p_iter), p_activeStore(rhs.p_activeStore),
  m_ownEvtIterator(rhs.m_ownEvtIterator), m_msg(rhs.m_msg),
  m_neverLoaded(rhs.m_neverLoaded), m_ownStore(rhs.m_ownStore),
  m_used(rhs.m_used), m_hasRing(rhs.m_hasRing), m_iOriginalRing(rhs.m_iOriginalRing)
{ 
  //transferred ownership
  rhs.m_ownEvtIterator=false;
  rhs.m_ownStore=false;
}

PileUpStream&
PileUpStream::operator=(const PileUpStream& rhs) {
  if (this != &rhs) {
    m_name=rhs.m_name;
    p_svcLoc=rhs.p_svcLoc;
    p_sel=rhs.p_sel; 
    p_SG=rhs.p_SG; 
    p_iter=rhs.p_iter;
    p_activeStore=rhs.p_activeStore;
    m_ownEvtIterator=rhs.m_ownEvtIterator;
    rhs.m_ownEvtIterator=false;
    m_msg=rhs.m_msg;
    m_neverLoaded=rhs.m_neverLoaded;
    m_ownStore=rhs.m_ownStore;
    rhs.m_ownStore=false;
    m_used=rhs.m_used;
    m_hasRing=rhs.m_hasRing;
    m_iOriginalRing=rhs.m_iOriginalRing;
  }  
  return *this;
}
 
PileUpStream::PileUpStream(const std::string& name, 
			   ISvcLocator* svcLoc,
			   IEvtSelector* sel):
  m_name(name), p_svcLoc(svcLoc), p_sel(sel), p_SG(0), p_iter(0), 
  m_ownEvtIterator(false), m_msg("PileUpStream"),
  m_neverLoaded(true), m_ownStore(false),
  m_used(false), m_hasRing(false), m_iOriginalRing(0)
{ 
  assert(p_sel);
  assert(p_svcLoc);
  if (!(p_sel->createContext(p_iter).isSuccess() &&
	serviceLocator()->service("ActiveStoreSvc", p_activeStore).isSuccess() ) ) {

    const string errMsg("PileUpStream:: can not create stream");
    ATH_MSG_ERROR ( errMsg );
    throw runtime_error(errMsg);
  } else  m_ownEvtIterator=true;
}

PileUpStream::PileUpStream(const std::string& name, 
			   ISvcLocator* svcLoc,
			   const std::string& selecName):
  m_name(name), p_svcLoc(svcLoc), p_sel(0), p_SG(0), p_iter(0),
  m_ownEvtIterator(false), m_msg("PileUpStream"), 
  m_neverLoaded(true), m_ownStore(false),
  m_used(false), m_hasRing(false), m_iOriginalRing(0)

{
  assert(p_svcLoc);
  if (!(serviceLocator()->service("ActiveStoreSvc", p_activeStore).isSuccess() && 
	serviceLocator()->service(selecName, p_sel).isSuccess() &&
	p_sel->createContext(p_iter).isSuccess() )) {
    const string errMsg("PileUpStream: can not create stream");
    ATH_MSG_ERROR ( errMsg );
    throw runtime_error(errMsg);
  } else  m_ownEvtIterator=true;
}

PileUpStream::~PileUpStream() 
{ 
}

bool PileUpStream::setupStore() 
{
  assert( p_iter );
  assert( p_sel );
  bool rc(true);
  std::string storeName(name() + "_SG");

  //start by looking for the store directly: in overlay jobs it may already be there
  const bool DONOTCREATE(false);
  rc = (serviceLocator()->service(storeName, p_SG, DONOTCREATE)).isSuccess();
  if (rc) {
    m_ownStore = false;
  } else {
    //not there, create one cloning the master store
    Service *child;
    //if the parent store is not there barf
    //remember the clone function also initializes the service if needed
    IService* pIS(0);
    rc = ((serviceLocator()->getService("StoreGateSvc", pIS)).isSuccess() &&
          CloneService::clone(pIS, storeName, child).isSuccess() &&
          0 != (p_SG = dynamic_cast<StoreGateSvc*>(child)));
    if ( rc )  {
      m_ownStore = true;
      // further initialization of the cloned service 
      rc = (p_SG->sysInitialize()).isSuccess();      
      p_SG->setStoreID(StoreID::PILEUP_STORE); //needed by ProxyProviderSvc
    } //clones
  }
  if (rc) {
    //if the selector is an address provider like the AthenaPool one, 
    //create a dedicated ProxyProviderSvc and associate it to the store
    IAddressProvider* pIAP(dynamic_cast<IAddressProvider*>(p_sel));
    if (0 != pIAP) {
      IProxyProviderSvc* pPPSvc(0);
      std::string PPSName(name() + "_PPS");
      ISvcManager* pISM(dynamic_cast<ISvcManager*>(serviceLocator()));
      if ( 0 != pISM &&  
	   (pISM->declareSvcType(PPSName, "ProxyProviderSvc")).isSuccess() &&
	   //check the service is not there then create it
	   (serviceLocator()->service(PPSName,
				      pPPSvc,
				      true)).isSuccess()) {
	pPPSvc->addProvider(pIAP);
        IService* pSAthPoolAddProv(0);
        IAddressProvider* pAthPoolAddProv(0);
        if (serviceLocator()->service("AthenaPoolAddressProviderSvc", pSAthPoolAddProv).isSuccess() &&
            0 != (pAthPoolAddProv = dynamic_cast<IAddressProvider*>(pSAthPoolAddProv))) {
          pPPSvc->addProvider(pAthPoolAddProv);
        } else {
          ATH_MSG_WARNING ( "could not add AthenaPoolAddressProviderSvc as AddresssProvider for "<< PPSName );
        }
	IService* pSAddrRemap(0);
	IAddressProvider* pAddrRemap(0);
	if (serviceLocator()->service("AddressRemappingSvc", pSAddrRemap).isSuccess() &&
	    0 != (pAddrRemap = dynamic_cast<IAddressProvider*>(pSAddrRemap))) {
	  pPPSvc->addProvider(pAddrRemap);
	} else {
	  ATH_MSG_WARNING ( "could not add AddressRemappingSvc as AddresssProvider for "<< PPSName );
	}
	p_SG->setProxyProviderSvc(pPPSvc);
      } 
    } //valid address provider
  }
  return rc;
}

void PileUpStream::setActiveStore()
{
  if (0 == p_activeStore) {
    const string errMsg("PileUpStream::setActiveStore(): no ActiveStoreSvc ptr set, INVALID STREAM STATE ");
    ATH_MSG_ERROR ( errMsg );
    throw runtime_error(errMsg);
  }
  p_activeStore->setStore(&store());
  return;
}

StatusCode PileUpStream::nextRecordPre_Passive()
{
  this->setActiveStore();
  // Clear the store, move to next event
  return (this->store().clearStore().isSuccess() && 
	  this->selector().next(iterator()).isSuccess() ) ?
    StatusCode::SUCCESS :
    StatusCode::FAILURE;
}

StatusCode PileUpStream::nextRecordPre()
{
  // Clear the store, move to next event and load the store
  return (this->nextRecordPre_Passive().isSuccess() && 
	  this->loadStore()) ?
    StatusCode::SUCCESS :
    StatusCode::FAILURE;
}

// bool PileUpStream::isNotEmpty() const
// {
//   //  std::cout << "isNotEmpty " << (0 != iterator() && *(iterator()) != *(selector().end()) ) << std::endl;
//   return (0 != iterator() && *(iterator()) != *(selector()->end()) );
// }

bool PileUpStream::loadStore()
{
  m_neverLoaded=false;

  IOpaqueAddress* paddr(0);
  bool rc = (this->selector().createAddress(iterator(), paddr)).isSuccess();
  if ( 0 != paddr) rc &= this->store().recordAddress(paddr).isSuccess();

  // load store proxies
  rc &= this->store().loadEventProxies().isSuccess();
  return rc;
}

//return next event, load store with next event
const xAOD::EventInfo* PileUpStream::nextEventPre(bool readRecord)
{
   if (m_neverLoaded) readRecord=true;
   else if (readRecord) {
      //do not reset these the first time we call nextEventPre
      this->resetUsed();
      m_hasRing=false;
   }
   //  if (isNotEmpty()) {
   if (readRecord && this->nextRecordPre().isFailure()) {
      ATH_MSG_INFO ( "nextEventPre(): end of the loop. No more events in the selection" );
      return nullptr;
   }

   const xAOD::EventInfo* xAODEventInfo = store().tryConstRetrieve<xAOD::EventInfo>();

   if( !xAODEventInfo ) {
      // Try reading old EventInfo
      const EventInfo* pEvent(0);
      if( !(this->store().retrieve(pEvent)).isSuccess() ) {
         ATH_MSG_DEBUG ( "nextEventPre():	Unable to retrieve Event root object" );
         return nullptr;
      }
      ATH_MSG_DEBUG("Converting PileUpEventInfo to xAOD::EventInfo");
      // Create the xAOD object(s):
      std::unique_ptr< xAOD::EventInfo >  pxAODEventInfo( new xAOD::EventInfo() );
      std::unique_ptr< xAOD::EventAuxInfo > pxAODEventAuxInfo(new xAOD::EventAuxInfo());
      pxAODEventInfo->setStore( pxAODEventAuxInfo.get() );
      if( !m_xAODCnvTool->convert( pEvent, pxAODEventInfo.get(), false, false ).isSuccess() ) {
         ATH_MSG_ERROR("Failed to convert  xAOD::EventInfo in SG");
         return nullptr;
      }

      //ATH_MSG_INFO ("Dumping xAOD::EventInfo prior to adding SubEvents");
      //xAOD::dump( *xAODEventInfo );

      const PileUpEventInfo* pileupEvent(dynamic_cast<const PileUpEventInfo*>(pEvent));
      if( pileupEvent ) {
         cout << "MN: PileUpStream: converting PileUpEventInfo" << endl;
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
         cout << "MN: There are " << countEvents << " subevents in this Event." <<endl;

         for( ; pu_itr != pu_end; ++pu_itr ) {
            // Create a new xAOD::EventInfo object:
            std::unique_ptr< xAOD::EventInfo > ei( new xAOD::EventInfo() );
            // Fill it with information:
            if( ! m_xAODCnvTool->convert( pu_itr->pSubEvt, ei.get(), true, false ).isSuccess() ) {
               ATH_MSG_ERROR("Failed to convert EventInfo to xAOD::EventInfo");
               continue;
            }

            StoreGateSvc* tmpSG = pu_itr->pSubEvtSG;
            cout << "MN: PileUpStream: SG comparison:  p_SG=" << p_SG << "  tmpSG= " << tmpSG << endl;
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
                                                            EiLink( "PileUpEventInfo", puei->size()-1, p_SG )));  // p_SG?
//                                                               m_evtStore.operator->() ) ) );
            ATH_MSG_VERBOSE("PileUpEventInfo: time = " << subEvents.back().time() << ", index = " << subEvents.back().index());
         }

         if( subEvents.size() ) {
            // And now update the main EventInfo object with the sub-events:
            pxAODEventInfo->setSubEvents( subEvents );

            // Record the xAOD object(s):
            if( !store().record( std::move( puaux ), "PileUpEventInfoAux." ).isSuccess()
                || !store().record( std::move( puei ), "PileUpEventInfo" ).isSuccess() ) {  //MN: FIX - make keys configurable
               ATH_MSG_ERROR("Failed to record xAOD::EventInfoContainer in SG");
            }
         }
      }
      
      xAODEventInfo = pxAODEventInfo.get();  // remember pointer to return the new EventInfo
      // Record the xAOD object(s):
      if( ! store().record( std::move( pxAODEventAuxInfo ), "EventInfoAux." ).isSuccess() //MN: FIX? key
          || ! store().record( std::move( pxAODEventInfo ), "EventInfo" ).isSuccess() ) {
         ATH_MSG_ERROR("Failed to record the new xAOD::EventInfo in SG");
      }
   } else {
      ATH_MSG_INFO("Found xAOD::EventInfo");
      ATH_MSG_INFO(" EventInfo has" <<   xAODEventInfo->subEvents().size() << " subevents" );
      // the loop below serves 2 purposes: to recreate subevent links cache
      // and set SG pointer in subevents
      for( auto& subev : xAODEventInfo->subEvents() ) {
         const_cast<xAOD::EventInfo*>(subev.ptr())->setEvtStore( &store() );
      }
   }

   const_cast<xAOD::EventInfo*>(xAODEventInfo)->setEvtStore( &store() );
   ATH_MSG_INFO("Dumping xAOD::EventInfo");
   xAOD::dump( *xAODEventInfo );

   if (readRecord) {
      ATH_MSG_DEBUG ( "nextEventPre(): read new event " 
		      <<  xAODEventInfo->eventNumber() 
		      << " run " << xAODEventInfo->runNumber()
		      << " into store " << this->store().name() );
   }

   return xAODEventInfo;
}

bool PileUpStream::nextEventPre_Passive(bool readRecord) {
  if (m_neverLoaded) readRecord=true;
  else if (readRecord) {
    //do not reset these the first time we call nextEventPre_Passive
    this->resetUsed();
    m_hasRing=false;
  }
  if (readRecord && this->nextRecordPre_Passive().isFailure()) {
    ATH_MSG_INFO ( "nextEventPre_Passive(): end of the loop. No more events in the selection" );
    return false;
  } 
  return true;
}    

StatusCode PileUpStream::finalize() {
  StatusCode sc(StatusCode::SUCCESS);
  if (m_ownEvtIterator) delete p_iter;
  //we own and manage our cloned SG instance
#ifdef GAUDIKERNEL_STATEMACHINE_H_
  if (m_ownStore && Gaudi::StateMachine::INITIALIZED == store().FSMState()) {
    sc = this->store().sysFinalize();
  }
#else
  if (m_ownStore && this->store().state() == IService::INITIALIZED) sc = this->store().sysFinalize();
#endif
  this->store().release();
  return sc;
}
