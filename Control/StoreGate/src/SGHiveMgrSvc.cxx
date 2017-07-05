/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/CloneService.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/StoreID.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/tools/SGImplSvc.h"
#include "SGHiveMgrSvc.h"

using namespace SG;

__thread HiveEventSlot* s_current(0);

HiveMgrSvc::HiveMgrSvc(const std::string& name, 
                       ISvcLocator* svc) : Service(name, svc),
                                           m_hiveStore(StoreID::storeName(StoreID::EVENT_STORE), name),
                                           m_nSlots(1)
{
  declareProperty("HiveStoreSvc", m_hiveStore);
  declareProperty("NSlots", m_nSlots, "number of event slots");
}



/** Activate an given 'slot' for all subsequent calls within the
 * same thread id.
 *
 * @param  slot     [IN]     Slot number (event slot)   *
 * @return Status code indicating failure or success.
 */
StatusCode HiveMgrSvc::selectStore(size_t slotIndex) {
  s_current = &m_slots[slotIndex];
  StoreGateSvc::setSlot(s_current);
  return StatusCode::SUCCESS;
}

/** Clear a given 'slot'.
 *
 * @param  slot     [IN]     Slot number (event slot)   *
 * @return Status code indicating failure or success.
 */
StatusCode HiveMgrSvc::clearStore(size_t slotIndex) {
  StatusCode rc(StatusCode::FAILURE);
  if (slotIndex < m_nSlots) {
    rc=m_slots[slotIndex].pEvtStore->clearStore();
    if (rc.isSuccess()) debug() << "cleared store " << slotIndex << endmsg;
  }    
  if (!rc.isSuccess()) error() << "could not clear store " << slotIndex << endmsg;
  return rc;
}
  
/** Set the number of 'slots'.
 *
 * @param  slot     [IN]     Slot number (event slot)   *
 * @return Status code indicating failure or success.
 */
StatusCode HiveMgrSvc::setNumberOfStores(size_t slots) {
  //FIXME what if running?
  if(FSMState() ==  Gaudi::StateMachine::INITIALIZED) {
    fatal() << "Too late to change the number of slots!" << endmsg;
    return StatusCode::FAILURE;
  } else {
    m_slots.resize(slots);
    m_nSlots = slots;
    return StatusCode::SUCCESS;
  }
}

size_t HiveMgrSvc::getNumberOfStores() const {
  return m_nSlots;
}


  
/** Allocate a store slot for new event
 *
 * @param     evtnumber     [IN]     Event number
 * @param     slot     [OUT]    Returned slot or slot number
 * @return Slot number (npos to indicate an error).
 */
size_t HiveMgrSvc::allocateStore( int evtNumber ) {
  for (size_t index=0; index<m_nSlots; ++index) {
    if( m_slots[index].eventNumber == evtNumber) {
      error() << "Attempt to allocate an event slot for an event that is still active: event number " << evtNumber << endmsg;
      return std::string::npos;
    } else if (m_slots[index].eventNumber == -1) {
      m_slots[index].eventNumber = evtNumber;
      debug() << "Slot " << index 
              << " allocated to event number "<< evtNumber << endmsg;
      return index;
    }
  }
  error() << "No slots available for event number " << evtNumber << endmsg;
  return std::string::npos;
}
  
/** Free a store slot
 *
 * @param     slot     [IN]     Slot number
 * @return Status code indicating failure or success.
 */
StatusCode HiveMgrSvc::freeStore( size_t slotIndex ) {
  if (slotIndex < m_nSlots) {
    m_slots[slotIndex].eventNumber = -1;
    debug() << "Freed slot " << slotIndex << endmsg;
    return StatusCode::SUCCESS;
  } else {
    error() << "no slot at " << slotIndex << endmsg;
    return StatusCode::FAILURE;
  }
}
  
  
/** Get the slot number corresponding to a given event
 *
 * @param     evtNumber     [IN]     Event number
 * @return    slot number (npos to indicate an error).
 */
size_t HiveMgrSvc::getPartitionNumber(int evtNumber) const {
  for (size_t index=0; index<m_nSlots; ++index) {
    if( m_slots[index].eventNumber == evtNumber) return index;
  }
  return std::string::npos;
}


StatusCode HiveMgrSvc::getNewDataObjects(DataObjIDColl& products) {
  return m_hiveStore->getNewDataObjects(products);
}

bool HiveMgrSvc::newDataObjectsPresent() {
  return m_hiveStore->newDataObjectsPresent();
} 

StatusCode HiveMgrSvc::initialize() {
  verbose() << "Initializing " << name() 
            << " - package version " << PACKAGE_VERSION << endmsg ;

  if ( !(Service::initialize().isSuccess()) )  {
    fatal() << "Unable to initialize base class" << endmsg;
    return StatusCode::FAILURE;
  }
  //this sets the hiveStore pointer to StoreGateSvc.defaultStore
  if (!(m_hiveStore.retrieve()).isSuccess()) {
    fatal() << "Unable to get hive event store" << endmsg;
    return StatusCode::FAILURE;
  }

  //use hiveStore default impl store as prototype
  Service* child(0);
  SGImplSvc* pSG(0);
  ServiceHandle<IIncidentSvc> pincSvc("IncidentSvc",name());
  if(!(pincSvc.retrieve().isSuccess())){
    error()<<"Failed to retrieve incident Svc"<<endmsg;
    return StatusCode::FAILURE;
  }
  const int PRIORITY=100;
  pincSvc->addListener(this, "EndEvent",PRIORITY);
  pincSvc->addListener(this, "BeginEvent", PRIORITY);

  for( size_t i = 0; i< m_nSlots; ++i) {
    std::ostringstream oss;
    oss << i << '_' << m_hiveStore->currentStore()->name();
    if (CloneService::clone(m_hiveStore->currentStore(), oss.str(), child).isSuccess() &&
        child->initialize().isSuccess() &&
        0 != (pSG = dynamic_cast<SGImplSvc*>(child)) )
      {
        pSG->setSlotNumber (i, m_nSlots);
        m_slots.push_back(SG::HiveEventSlot(pSG));
      } else {
      fatal() << "Unable to clone event store " << oss.str() << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  return selectStore(0);
}

void HiveMgrSvc::handle(const Incident &inc) {
  m_slots.at(inc.context().slot()).pEvtStore->handle(inc);
}

StatusCode HiveMgrSvc::finalize() {
  info() <<  "Finalizing " << name() 
         << " - package version " << PACKAGE_VERSION << endmsg ;

  for (SG::HiveEventSlot& s : m_slots) {
    // The impl services are not set to active, so ServiceManager
    // won't finalize them.
    CHECK( s.pEvtStore->finalize() );
    s.pEvtStore->release();
  }

  return StatusCode::SUCCESS;
}

StatusCode HiveMgrSvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) {
  if ( IHiveWhiteBoard::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IHiveWhiteBoard*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
