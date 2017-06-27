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

//---------------------------------------------------------------------------------


SGInputLoader::SGInputLoader( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_dump(false)
{
  //
  // Property declaration
  // 
  declareProperty( "Load", m_load)->declareUpdateHandler(&SGInputLoader::loader, this);
  declareProperty( "ShowEventDump", m_dump=false);
  declareProperty( "FailIfNoProxy", m_failEvt=true, 
                   "execute returns FAILURE if a requested proxy is not found in store");
  declareProperty( "LoadExtraProxies", m_loadProxies=true,
                   "load the Proxies attributed to the SGInputLoader by the Scheduler");
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
    if (m_loadProxies) {
      for (auto obj : outputDataObjs() ) {
        m_load.emplace(obj);
      }
    }
    m_first = false;
  }

  bool b = loadObjs( m_load );

  if (m_dump) {
    ATH_MSG_DEBUG(evtStore()->dump()); 
  }

  if (m_failEvt && !b) {
    ATH_MSG_ERROR("autoload of objects failed. aborting event processing");
    sc = StatusCode::FAILURE;
  }

  return sc;
}

//---------------------------------------------------------------------------------

void
SGInputLoader::loader(Property& p ) {

  ATH_MSG_DEBUG("setting prop ExtraOutputs to " <<  p.toString());

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
      if (dp->transientAddress()->provider() == 0) {
	ATH_MSG_DEBUG("   obj " << obj << " has no provider, and is only Transient" );
      }

      // Tell hive about the object.
      evtStore()->addedNewTransObject(obj.clid(), vhk.key());

      // Also add all aliases...
      for (const std::string& alias : dp->alias()) {
        evtStore()->addedNewTransObject(obj.clid(), alias);
      }

      // ... and linked classes.
      for (CLID clid2 : dp->transientAddress()->transientID()) {
        if (clid2 != obj.clid())
          evtStore()->addedNewTransObject(clid2, vhk.key());
      }
    } else {
      ok = false;
      if (m_failEvt) {
        ATH_MSG_ERROR("unable to get proxy for " << obj);
      } else {
        ATH_MSG_WARNING("unable to get proxy for " << obj);
      }
    }
  }

  return ok;

}
