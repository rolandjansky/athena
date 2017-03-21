///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CondInputLoader.cxx 
// Implementation file for class CondInputLoader
/////////////////////////////////////////////////////////////////// 

#include "CondInputLoader.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "xAODEventInfo/EventInfo.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CondInputLoader::CondInputLoader( const std::string& name, 
                                  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_dump(false),
  m_condStore("StoreGateSvc/ConditionStore", name),
  m_condSvc("CondSvc",name),
  m_IOVSvc("IOVSvc",name)

{
  //
  // Property declaration
  // 
  declareProperty( "Load", m_load)->declareUpdateHandler(&CondInputLoader::loader, this);
  declareProperty( "ShowEventDump", m_dump=false);

}

// Destructor
///////////////
CondInputLoader::~CondInputLoader()
{}

//-----------------------------------------------------------------------------

// Athena Algorithm's Hooks
////////////////////////////
StatusCode 
CondInputLoader::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_condSvc.isValid()) {
    ATH_MSG_ERROR("could not get the CondSvc");
    return StatusCode::FAILURE;
  }

  if (!m_condStore.isValid()) {
    ATH_MSG_ERROR("could not get the ConditionStore");
    return StatusCode::FAILURE;
  }

  StatusCode sc(StatusCode::SUCCESS);

  std::ostringstream str;
  str << "Will create WriteCondHandle dependencies for the following DataObjects:";
  for (auto &e : m_load) {
    str << "\n    + " << e;
    if (e.key() == "") {
      sc = StatusCode::FAILURE;
      str << "   ERROR: empty key is not allowed!";
    } else {
      Gaudi::DataHandle dh(e, Gaudi::DataHandle::Writer, this);
      if (m_condSvc->regHandle(this, dh, e.key()).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle " << dh.fullKey());
        sc = StatusCode::FAILURE;
      }
      // if (m_condSvc->regCondHandle(this, dh, e.key()).isFailure()) {
      //   ATH_MSG_ERROR("Unable to register managed handle " << dh.fullKey());
      //   sc = StatusCode::FAILURE;
      // }
      m_IOVSvc->ignoreProxy(dh.fullKey().clid(), e.key());
    }
  }

  ATH_MSG_INFO(str.str());

  return sc;
}

//-----------------------------------------------------------------------------

StatusCode 
CondInputLoader::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------

StatusCode 
CondInputLoader::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef ATHENAHIVE
  
  // if (m_first) {
  //   StatusCode sc(StatusCode::SUCCESS);
  //   auto managedIDs = m_condSvc->managedIDs();
  //   for (auto id : managedIDs) {
  //     if (m_load.find(id) == m_load.end()) {
  //       ATH_MSG_ERROR(" not configured to provide WriteCondHandle for "
  //                     << id);
  //       sc = StatusCode::FAILURE;
  //     }
  //   }

  //   DataObjIDColl::iterator itr;
  //   for (itr = m_load.begin(); itr != m_load.end(); ) {
  //     if ( managedIDs.find( *itr )== managedIDs.end() ) {
  //       ATH_MSG_DEBUG(" removing ID " << *itr 
  //                     << " from Loader list as it's not used");
  //       itr = m_load.erase(itr);
  //     } else {
  //       ++itr;
  //     }
  //   }

  //   m_first = false;

  //   if (sc.isFailure()) { return sc; }

  // }

  if (m_first) {
    DataObjIDColl::iterator itr;
    for (itr = m_load.begin(); itr != m_load.end(); ) {
      //      if ( ! m_condStore->contains(itr->clid(), itr->key() ) ){
      if ( ! m_condStore->contains<CondContBase>( itr->key() ) ){
        ATH_MSG_INFO("ConditionStore does not contain a CondCont<> of "
                     << *itr
                     << ". Either a ReadCondHandle was not initailized or "
                     << "no other Algorithm is using this Handle");
        itr = m_load.erase(itr);
      } else {
        ++itr;
      }
    }
    m_first = false;
  }
        
          


  EventIDBase now;
  if(getContext()==nullptr) {
    const xAOD::EventInfo* thisEventInfo;
    if(evtStore()->retrieve(thisEventInfo)!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Unable to get Event Info");
      return StatusCode::FAILURE;
    }
    now.set_run_number(thisEventInfo->runNumber());
    now.set_event_number(thisEventInfo->eventNumber());
    now.set_time_stamp(thisEventInfo->timeStamp());
  }
  else {
    now.set_run_number(getContext()->eventID().run_number());
    now.set_event_number(getContext()->eventID().event_number());
    now.set_time_stamp(getContext()->eventID().time_stamp());
  }

  IOVTime t(now.run_number(), now.event_number(), now.time_stamp());

  EventIDRange r;
  std::string tag;
  //  for (auto &obj: extraOutputDeps()) {
  for (auto &obj: m_load) {

    ATH_MSG_DEBUG( "handling id: " << obj );

    CondContBase* ccb(0);
    if (! m_condStore->retrieve(ccb, obj.key()).isSuccess()) {
      ATH_MSG_ERROR( "unable to get CondContBase* for " << obj 
                     << " from ConditionStore" );
      continue;
    }
   

    if (ccb->valid(now)) {
      ATH_MSG_INFO( "  CondObj " << obj << " is still valid at " << now );
      evtStore()->addedNewTransObject(obj.clid(), obj.key());
      continue;
    }


    if (m_IOVSvc->createCondObj( ccb, obj, now ).isFailure()) {
      ATH_MSG_ERROR("unable to create Cond object for " << obj);
      return StatusCode::FAILURE;
    } else {
      evtStore()->addedNewTransObject(obj.clid(), obj.key());
    }

    // IOVRange range;
    // IOpaqueAddress* ioa;
  
    // if (m_IOVSvc-> getRangeFromDB(obj.clid(), obj.key(),
    //                               t, range, tag, ioa).isFailure()) {
    //   ATH_MSG_ERROR( "unable to get range from db for " 
    //                  << obj.clid() << " " << obj.key() );
    //   continue;
    // }
     
    // ATH_MSG_INFO( " new range: " << range );

    // if (ccb->proxy() == nullptr) { 
    //   SG::DataProxy *dp = m_condSvc->getProxy( obj );
    //   ccb->setProxy(dp);
    // }

    // // this will talk to the IOVDbSvc, get current run/event from EventInfo 
    // // object, load
    // SG::DataProxy* dp = ccb->proxy();
    // DataObject* dobj(0);
    // void* v(0);

    // if (dp->loader()->createObj(ioa, dobj).isFailure()) {
    //   ATH_MSG_ERROR(" could not create a new DataObject ");
    // } else {
    //   ATH_MSG_INFO(" created new obj at " << dobj );

    //   v = SG::Storable_cast(dobj, obj.clid());

    // }

    // // v = SG::DataProxy_cast(ccb->proxy(), obj.clid());
    // DataObject *d2 = static_cast<DataObject*>(v);
  
    // ATH_MSG_INFO( " obj: " << d2 );
  
    // EventIDRange r2(EventIDBase(range.start().run(), range.start().event()),
    //                 EventIDBase(range.stop().run(), range.stop().event()));
  
    // ccb->insert( r2, d2);
  
    // ATH_MSG_INFO ("inserted new object at " << d2 );

    // evtStore()->addedNewTransObject(obj.clid(), obj.key());

    // ATH_MSG_INFO ("addedNewTransObj");


    // SG::DataProxy* dp = evtStore()->proxy(obj.clid(), obj.key());
    // if (dp != 0) {
    //   ATH_MSG_DEBUG("loading obj " << obj);
    //   if (dp->transientAddress()->provider() == 0) {
    //     ATH_MSG_DEBUG("   obj " << obj << " has no provider, and is only Transient" );
    //   }
    //   evtStore()->addedNewTransObject(obj.clid(), obj.key());
    // } else {
    //   ATH_MSG_ERROR("unable to get proxy for " << obj);
    // }

  }

#endif

  if (m_dump) {
    ATH_MSG_DEBUG(evtStore()->dump()); 
  }
  

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------

void
CondInputLoader::loader(Property& p ) {

  ATH_MSG_DEBUG("setting prop ExtraOutputs to " <<  p.toString());

  if (!setProperty("ExtraOutputs", p).isSuccess()) {
    ATH_MSG_WARNING("failed setting property ExtraOutputs");
  }
}
