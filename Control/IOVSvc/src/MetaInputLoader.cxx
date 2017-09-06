///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MetaInputLoader.cxx 
// Implementation file for class MetaInputLoader
/////////////////////////////////////////////////////////////////// 

#include "MetaInputLoader.h"

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "StoreGate/ReadHandle.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/errorcheck.h"

#include "PersistentDataModel/DataHeader.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
MetaInputLoader::MetaInputLoader( const std::string& name, 
                                  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ), m_dump(false),
  m_metaStore("StoreGateSvc/MetaDataStore", name),
  m_inputStore("StoreGateSvc/InputMetaDataStore", name)
{
  //
  // Property declaration
  // 
  declareProperty( "Load", m_load); 
  declareProperty( "Dump", m_dump=false);
}

// Destructor
///////////////
MetaInputLoader::~MetaInputLoader()
{}

//-----------------------------------------------------------------------------

// Athena Algorithm's Hooks
////////////////////////////
StatusCode 
MetaInputLoader::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_inputStore.isValid()) {
    ATH_MSG_ERROR("could not get the InputMetaDataStore");
    return StatusCode::FAILURE;
  }

  if (!m_metaStore.isValid()) {
    ATH_MSG_ERROR("could not get the MetaDataStore");
    return StatusCode::FAILURE;
  }


  // Update the properties, set the ExtraOutputs for Alg deps
  const Property &p = getProperty("Load");

  ATH_MSG_DEBUG("setting prop ExtraOutputs to " <<  p.toString());
  if (!setProperty("ExtraOutputs", p).isSuccess()) {
    ATH_MSG_ERROR("failed setting property ExtraOutputs");
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
    }
  }

  ATH_MSG_INFO(str.str());

  return sc;
}

//-----------------------------------------------------------------------------

StatusCode 
MetaInputLoader::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------

StatusCode 
MetaInputLoader::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if (m_first) {
    DataObjIDColl::iterator itr;
    for (itr = m_load.begin(); itr != m_load.end(); ) {
      if ( ! m_metaStore->contains<MetaContBase>( "MetaDataStore+"+itr->key() ) ){
        ATH_MSG_INFO("ConditionStore does not contain a MetaCont<> of "
                     << *itr
                     << ". Either a ReadMetaHandle was not initialized or "
                     << "no other Algorithm is using this Handle");
        itr = m_load.erase(itr);
      } else {
        ++itr;
      }
    }
    m_first = false;
  }

  const DataHeader* thisDH;
  if(evtStore()->retrieve(thisDH)!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR("Unable to get Event Info");
    return StatusCode::FAILURE;
  }
  // Get string guid for SourceID
  const MetaContBase::SourceID sid(thisDH->begin()->getToken()->dbID().toString());

  if (sid.size()==0) {
    ATH_MSG_ERROR("Unable to get source id from dataheader");
    return StatusCode::FAILURE;
  }

  for (auto &obj: m_load) {

    ATH_MSG_DEBUG( "handling id: " << obj );

    MetaContBase* mcb(0);
    if (! m_metaStore->retrieve(mcb, "MetaDataStore+"+obj.key()).isSuccess()) {
      ATH_MSG_ERROR( "unable to get MetaContBase* for " << obj 
                     << " from MetaDataStore" );
      continue;
    }

    ATH_MSG_INFO("Retrieved mcb with entries = " << mcb->entries());
   
    if (mcb->valid(sid)) {
      ATH_MSG_INFO( "  MetaObj " << obj << " is still valid for " << sid );
      //evtStore()->addedNewTransObject(obj.clid(), obj.key());
      continue;
    }

    if (createMetaObj( mcb, obj.clid(), obj.key(), sid ).isFailure()) {
      ATH_MSG_ERROR("unable to create Metadata object for " << obj << " SID: " 
                    << sid);
      return StatusCode::FAILURE;
    } 
    else {
      m_metaStore->addedNewTransObject(obj.clid(), obj.key());
    }

  }

  if (m_dump) {
    ATH_MSG_DEBUG(m_metaStore->dump()); 
  }
  

  return StatusCode::SUCCESS;
}

StatusCode MetaInputLoader::createMetaObj(MetaContBase* mcb,
                                          const CLID& clid,
                                          const std::string key,
                                          MetaContBase::SourceID sid)
{
  ATH_MSG_INFO("createMetaObj for " << key << " and sid " << sid);

  // Get proxy from input store
  const SG::DataProxy* dpin = m_inputStore->proxy(clid,key,true);
  if (dpin!=0) {
    // If the input store has a new proxy for the clid/key
    // Get the current proxy from the output store
    SG::DataProxy* dp1 = m_metaStore->proxy(clid,key,true);
    ATH_MSG_INFO(" address0 " << dpin->address());
    if (dp1==0) {
      // If none in the output store, create it
      ATH_CHECK(m_metaStore->recordAddress(key,dpin->address()));
    }
    else {
      // If already there, update it
      dp1->setAddress(dpin->address());
    }
    // Get the proxy again, in case it was zero before
    SG::DataProxy* dp = m_metaStore->proxy(clid,key,true);
    ATH_MSG_INFO(" address1 " << dpin->address());
    // If the proxy is now good, add it to the container for this sid
    if (dp!=0) {
      mcb->setProxy(dp);
      SG::DataProxy* dpcont = mcb->proxy();
      DataObject* dobj(0);
      void* v(0);
      if (dpcont->loader()->createObj(dpcont->address(), dobj).isFailure()) {
        ATH_MSG_ERROR(" could not create new DataObject");
        return StatusCode::FAILURE;
      }
      v = SG::Storable_cast(dobj,clid);
      if (!mcb->insert(sid,v)) {
        ATH_MSG_ERROR("Could not insert object " << clid << " with key " << key << " for SID=" << sid);
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Now have mcb with " << mcb->entries() << " entries");
    }
    else {
      ATH_MSG_INFO("Unable to create proxy for " <<clid<< ", " <<key<< " in metadata store");
    }
  }
  else { 
    ATH_MSG_INFO("No proxy in input store for " << clid << " " << key);
  }

  return StatusCode::SUCCESS;
}
  
  
//-----------------------------------------------------------------------------

