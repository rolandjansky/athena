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
#include "AthContainersInterfaces/IConstAuxStoreMeta.h"
#include "SGTools/DataProxy.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/errorcheck.h"

#include "PersistentDataModel/DataHeader.h"
#include <set>

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
  // Make sure output is sorted in a reproducible manner.
  std::set<DataObjID> ordered (m_load.begin(), m_load.end());
  for (const DataObjID& e : ordered) {
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
      ATH_MSG_DEBUG("Creating record for " << itr->fullKey());
      std::string key = itr->key();
      bool found = m_metaStore->contains<MetaContBase>( key );
      // If the MetaCont<> is not found for an aux store object, then
      // go ahead and create one with a payload type of SG::IConstAuxStore.
      if (!found && key.size() > 4 && key.substr (key.size()-4) == "Aux.") {
        auto cont = new MetaCont<SG::IConstAuxStore> (*itr);
        if (m_metaStore->record (std::move (cont), itr->key()).isFailure()) {
          ATH_MSG_ERROR ("Could not record aux store MetaCont<> " <<
                         itr->key());
        }
        // Need to symlink it as IConstAuxStore.
        if (m_metaStore->symLink (ClassID_traits<MetaCont<SG::IConstAuxStore> >::ID(),
                                  itr->key(),
                                  ClassID_traits<SG::IConstAuxStore >::ID()).isFailure()) {
          ATH_MSG_ERROR ("Could not symlink aux store MetaCont<> " <<
                         itr->key());
        }
      }

      if ( ! m_metaStore->contains<MetaContBase>( itr->key() ) ){
        ATH_MSG_INFO("MetaDataStore does not contain a MetaCont<> of "
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
  if(evtStore()->retrieve(thisDH, "EventSelector")!=StatusCode::SUCCESS) {
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
    if (! m_metaStore->retrieve(mcb, obj.key()).isSuccess()) {
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
      DataObject* dobj = nullptr;
      {
        SG::CurrentEventStore::Push push (&*m_inputStore);
        if (m_inputStore->createObj (dpcont->loader(),
                                     dpcont->address(),
                                     dobj).isFailure())
        {
          ATH_MSG_ERROR(" could not create new DataObject");
          return StatusCode::FAILURE;
        }
      }
      void* v = SG::Storable_cast(dobj,clid);
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

