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



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
SGInputLoader::SGInputLoader( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_dump(false)
{
  //
  // Property declaration
  // 
  declareProperty( "Load", m_load)->declareUpdateHandler(&SGInputLoader::loader, this);
  declareProperty( "ShowEventDump", m_dump=false);

}

// Destructor
///////////////
SGInputLoader::~SGInputLoader()
{}

//---------------------------------------------------------------------------------

// Athena Algorithm's Hooks
////////////////////////////
StatusCode 
SGInputLoader::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  StatusCode sc(StatusCode::SUCCESS);

  std::ostringstream str;
  str << "Will preload the following DataObjects:";
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
  ATH_MSG_DEBUG ("Executing " << name() << "...");

#ifdef ATHENAHIVE
  for (auto &obj: extraOutputDeps()) {
    SG::DataProxy* dp = evtStore()->proxy(obj.clid(), obj.key());
    if (dp != 0) {
      ATH_MSG_DEBUG("loading obj " << obj);
      if (dp->transientAddress()->provider() == 0) {
	ATH_MSG_DEBUG("   obj " << obj << " has no provider, and is only Transient" );
      }
      evtStore()->addedNewTransObject(obj.clid(), obj.key());
    } else {
      ATH_MSG_ERROR("unable to get proxy for " << obj);
    }
  }
#endif

  if (m_dump) {
    ATH_MSG_DEBUG(evtStore()->dump()); 
  }
  

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------

void
SGInputLoader::loader(Property& p ) {

  ATH_MSG_DEBUG("setting prop ExtraOutputs to " <<  p.toString());

  if (!setProperty("ExtraOutputs", p).isSuccess()) {
    ATH_MSG_WARNING("failed setting property ExtraOutputs");
  }
}
