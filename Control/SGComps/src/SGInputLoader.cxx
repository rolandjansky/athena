///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SGInputLoader.cxx 
// Implementation file for class SGInputLoader
/////////////////////////////////////////////////////////////////// 

#include "SGInputLoader.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/VarHandleKey.h"
#include "AthenaKernel/StoreID.h"

//---------------------------------------------------------------------------------


SGInputLoader::SGInputLoader( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  declareProperty( "Load", m_load, "create Output data dependencies for these objects")
    ->declareUpdateHandler(&SGInputLoader::loader, this);

}

//---------------------------------------------------------------------------------

SGInputLoader::~SGInputLoader()
{}

//---------------------------------------------------------------------------------

StatusCode 
SGInputLoader::initialize()
{
  ATH_MSG_VERBOSE ("Initializing " << name() << "...");

  StatusCode sc(StatusCode::SUCCESS);

  if (m_load.size() > 0) {
    std::ostringstream str;
    str << "Will explicitly preload the following DataObjects:";
    for (auto &e : m_load) {
      str << "\n    + " << e;
      if (e.key() == "") {
        sc = StatusCode::FAILURE;
        str << "   ERROR: empty key is not allowed!";
      }
    }
    ATH_MSG_INFO(str.str());
  }

  return sc;
}

//---------------------------------------------------------------------------------

StatusCode 
SGInputLoader::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------

StatusCode 
SGInputLoader::execute()
{  
  StatusCode sc(StatusCode::SUCCESS);

  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // add objects automatically added by the Scheduler
  if (m_first) {
    if (m_loadProxies.value()) {
      for (auto obj : outputDataObjs() ) {
        m_load.emplace(obj);
      }
    }

    // check if objects are not in EventStore
    DataObjIDColl toLoad;
    for (auto obj : m_load) {
      SG::VarHandleKey vhk(obj.clid(),obj.key(),Gaudi::DataHandle::Writer);
      if (StoreID::findStoreID(vhk.storeHandle().name()) == StoreID::EVENT_STORE) {
        toLoad.emplace(obj);
      } else {
        ATH_MSG_WARNING("Will not auto-load proxy for non-EventStore object: "
                        << obj);
      }
    }
    m_load = toLoad;
    
    m_first = false;
  }

  bool b = loadObjs( m_load );

  if (m_dump.value()) {
    ATH_MSG_DEBUG(evtStore()->dump()); 
  }

  if (m_failEvt.value() && !b) {
    ATH_MSG_ERROR("autoload of objects failed. aborting event processing");
    sc = StatusCode::FAILURE;
  }

  return sc;
}

//---------------------------------------------------------------------------------

void
SGInputLoader::loader(Property& p ) {

  ATH_MSG_DEBUG("setting prop ExtraOutputs to " <<  p.toString());

  DataObjIDColl toLoad;

  for (auto obj : m_load) {
    // add an explicit storename as needed
    SG::VarHandleKey vhk(obj.clid(),obj.key(),Gaudi::DataHandle::Writer);
    obj.updateKey( vhk.objKey() );
    toLoad.emplace(obj);
  }
  m_load = toLoad;

  if (!setProperty("ExtraOutputs", p).isSuccess()) {
    ATH_MSG_WARNING("failed setting property ExtraOutputs");
  }
}

//---------------------------------------------------------------------------------

bool
SGInputLoader::loadObjs(const DataObjIDColl& objs) const {

  bool ok = true;

  for (auto &obj: objs) {

    // use the parsing built into the VarHandleKey to get the correct
    // StoreGate key
    SG::VarHandleKey vhk(obj.clid(),obj.key(),Gaudi::DataHandle::Reader);

    ATH_MSG_DEBUG("trying to load " << obj << "   sgkey: " << vhk.key() );

    SG::DataProxy* dp = evtStore()->proxy(obj.clid(), vhk.key());
    if (dp != 0) {
      ATH_MSG_DEBUG(" found proxy for " << obj);
      if (dp->provider() == 0) {
	ATH_MSG_DEBUG("   obj " << obj << " has no provider, and is only Transient" );
      }

      // Tell hive about the object.
      evtStore()->addedNewTransObject(obj.clid(), vhk.key());

      // Also add all aliases...
      for (const std::string& alias : dp->alias()) {
        evtStore()->addedNewTransObject(obj.clid(), alias);
      }

      // ... and linked classes.
      for (CLID clid2 : dp->transientID()) {
        if (clid2 != obj.clid())
          evtStore()->addedNewTransObject(clid2, vhk.key());
      }
    } else {
      ok = false;
      if (m_failEvt.value()) {
        ATH_MSG_ERROR("unable to find proxy for " << obj);
      } else {
        ATH_MSG_WARNING("unable to find proxy for " << obj);
      }
    }
  }

  return ok;

}
