/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RegistrationStreamDefMeta.cxx
 *
 * @brief AthAlgorithm which takes references provided by RegStream 
 * finds the appropriate CORAL object in storegate and stores 
 * them in a POOL collection. 
 *
 * $Id: RegistrationStreamDefMeta.cxx,v 1.2 2009-05-01 13:41:51 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

//#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"

// Framework include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/GaudiException.h"

#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <ctime>

#include "DBDataModel/CollectionMetadata.h"

#include "RegistrationStreamDefMeta.h"

// Standard Constructor
RegistrationStreamDefMeta::RegistrationStreamDefMeta(const std::string& name,
                                                     ISvcLocator* pSvcLocator)
    :
    AthAlgorithm(name,pSvcLocator),
    m_storeName("StoreGateSvc/MetaDataStore"),
    m_passnum(999),
    m_passname(""),
    m_stream(""),
    m_config(""),
    m_project("")
{
    declareProperty("StoreName",    m_storeName, "Store to find input metadata during finalize");
    declareProperty("Pass",         m_passnum,   "Pass number, collection metadata");
    declareProperty("PassName",     m_passname,  "Pass name, collection metadata");
    declareProperty("Stream",       m_stream,    "Stream name, collection metadata");
    declareProperty("ConfigTag",    m_config,    "AMI config tag, collection metadata");
    declareProperty("Project",      m_project,   "Project name, collection metadata");
}

// Standard Destructor
RegistrationStreamDefMeta::~RegistrationStreamDefMeta()   {
}

// initialize data writer
StatusCode 
RegistrationStreamDefMeta::initialize() 
{
   ATH_MSG_DEBUG("In initialize ");

   std::map<std::string, std::string> defMeta;

   // First grab metadata from input
   ServiceHandle<StoreGateSvc> metadataStore(m_storeName, this->name());
   StatusCode status = metadataStore.retrieve();
   if (status.isSuccess()) {
      ATH_MSG_DEBUG("Found metadata store, MetaDataStore");   
   }

   // Now grab information from the properties
   std::stringstream newkey; 

   if (m_passnum > 0) {
      std::stringstream used; used << m_passnum;
      newkey.str(""); newkey << "PassNumber";
      defMeta.insert(std::make_pair(newkey.str(),used.str()));
   }

   if (m_passname.size() > 0) {
      newkey.str(""); newkey << "PassName";
      defMeta.insert(std::make_pair(newkey.str(),m_passname));
   }

   if (m_stream.size() > 0) {
      newkey.str(""); newkey << "Stream";
      defMeta.insert(std::make_pair(newkey.str(),m_stream));
   }

   if (m_config.size() > 0) {
      newkey.str(""); newkey << "ConfigTag";
      defMeta.insert(std::make_pair(newkey.str(),m_config));
   }

   if (m_project.size() > 0) {
      newkey.str(""); newkey << "Project";
      defMeta.insert(std::make_pair(newkey.str(),m_project));
   }

   ATH_MSG_DEBUG("Filled a metadata container of size " << defMeta.size());

   CollectionMetadata* def = new CollectionMetadata(defMeta);
   CollectionMetadataContainer* cont = new CollectionMetadataContainer();
   cont->push_back(def);
   StatusCode rc = metadataStore->record(cont,"Default");
   if (!rc.isSuccess()) {
      ATH_MSG_ERROR("Unable to store Default CollectionMetadataContainer");
   }
   else {
      ATH_MSG_DEBUG("Stored CollectionMetadataContainer in " << m_storeName);
      ATH_MSG_DEBUG("Size: " << def->size());
      ATH_MSG_DEBUG("Contents: ");
      CollectionMetadata::const_iterator i = def->begin();
      for (CollectionMetadata::const_iterator j=i; j != def->end(); ++j) {
         ATH_MSG_DEBUG("    "<<j->first<<" "<<j->second);
      }
   }

   return AthAlgorithm::initialize();
}

// initialize data writer
StatusCode 
RegistrationStreamDefMeta::execute() 
{
    ATH_MSG_DEBUG("In execute ");
    return StatusCode::SUCCESS;
}


// terminate data writer
StatusCode 
RegistrationStreamDefMeta::finalize() 
{
    ATH_MSG_DEBUG("In finalize ");
    return AthAlgorithm::finalize();
}

