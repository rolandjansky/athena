/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TagRemappingTool.cxx
 *  @brief This file contains the implementation for the TagRemappingTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: TagRemappingTool.cxx,v 1.5 2009-03-17 09:44:46 gemmeren Exp $
 **/

#include "TagRemappingTool.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"

#include "PersistentDataModel/Token.h"
#include "StoreGate/StoreGateSvc.h"
#include "PoolSvc/IPoolSvc.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

//___________________________________________________________________________
TagRemappingTool::TagRemappingTool(const std::string& type, const std::string& name, const IInterface* parent) : 
	::AthAlgTool(type, name, parent),
		m_eventStore("StoreGateSvc", name), m_poolColl(0), m_poolCursor(0), m_attrListKey(), m_refName() {
   declareProperty("TargetTagFile", m_targetColl = "");
   declareInterface<IAthenaSelectorTool>(this);
}
//___________________________________________________________________________
TagRemappingTool::~TagRemappingTool() {
}
//___________________________________________________________________________
StatusCode TagRemappingTool::initialize() {
   if (!::AthAlgTool::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthAlgTool base class.");
      return(StatusCode::FAILURE);
   }

   // locate the StoreGateSvc and initialize our local ptr
   if (!m_eventStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get StoreGateSvc.");
      return(StatusCode::FAILURE);
   }
   const IService* parentSvc = dynamic_cast<const IService*>(this->parent());
   if (parentSvc != 0) {
      const IProperty* propertyServer = dynamic_cast<const IProperty*>(parentSvc);
      if (propertyServer != 0) {
         StringProperty attrKeyProperty("AttributeListKey", "");
         if (propertyServer->getProperty(&attrKeyProperty).isSuccess()) {
            m_attrListKey = attrKeyProperty.value();
         }
         StringProperty refNameProperty("DerivedRefName", "");
         if (propertyServer->getProperty(&refNameProperty).isSuccess()) {
            m_refName = refNameProperty.value() + "_ref";
         }
      }
   }
   ServiceHandle<IPoolSvc> poolsvc("PoolSvc", name());
   if (!poolsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find PoolSvc");
      return(StatusCode::FAILURE);
   }
   try {
      m_poolColl = poolsvc->createCollection("RootCollection", "", m_targetColl.value());
   } catch (std::exception &e) {
      ATH_MSG_FATAL("Could not create PoolCollection: " << e.what());
      return(StatusCode::FAILURE);
   }
   pool::ICollectionQuery* query = m_poolColl->newQuery();
   query->selectAll();
   m_poolCursor = &query->execute();
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode TagRemappingTool::postInitialize() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode TagRemappingTool::preNext() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode TagRemappingTool::postNext() {
   if (!m_eventStore->contains<AthenaAttributeList>(m_attrListKey)) {
      ATH_MSG_ERROR("Cannot find AthenaAttributeList");
      return(StatusCode::FAILURE);
   }
   DataHandle<AthenaAttributeList> attrList;
   if (m_eventStore->retrieve(attrList, m_attrListKey).isFailure()) {
      ATH_MSG_ERROR("Cannot retrieve AthenaAttributeList");
      return(StatusCode::FAILURE);
   }
   try {
      unsigned int eventNumber = (*attrList)["EventNumber"].data<unsigned int>();
      unsigned int runNumber = (*attrList)["RunNumber"].data<unsigned int>();

      std::string remapToken;
      while (m_poolCursor->next() != 0) {
         if (m_poolCursor->currentRow().attributeList()["EventNumber"].data<unsigned int>() == eventNumber) {
            if (m_poolCursor->currentRow().attributeList()["RunNumber"].data<unsigned int>() == runNumber) {
               remapToken = m_poolCursor->eventRef().toString();
               break;
            }
         }
      }
      if (remapToken.empty()) {
         ATH_MSG_INFO("Restart seach for: RunNumber == " << runNumber << " && EventNumber == " << eventNumber);
         pool::ICollectionQuery* query = m_poolColl->newQuery();
         query->selectAll();
         m_poolCursor = &query->execute();
         while (m_poolCursor->next() != 0) {
            if (m_poolCursor->currentRow().attributeList()["EventNumber"].data<unsigned int>() == eventNumber) {
               if (m_poolCursor->currentRow().attributeList()["RunNumber"].data<unsigned int>() == runNumber) {
                  remapToken = m_poolCursor->eventRef().toString();
                  break;
               }
            }
         }
      }
      if (remapToken.empty()) {
         ATH_MSG_WARNING("No matching event for: RunNumber == " << runNumber << " && EventNumber == " << eventNumber);
         return(StatusCode::RECOVERABLE);
      } else {
         ATH_MSG_DEBUG("Remapped Token: " << m_refName << " = " << remapToken);
         attrList->extend(m_refName, "string");
         (*attrList)[m_refName].data<std::string>() = remapToken;
      }
   } catch (...) {
      ATH_MSG_WARNING("Cannot apply AthenaAttributeList preselection");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode TagRemappingTool::preFinalize() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode TagRemappingTool::finalize() {
   // Close TAG file
   m_poolColl->close();
   // Release TagMetaDataStore
   if (!m_eventStore.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release StoreGateSvc.");
   }
   return(::AthAlgTool::finalize());
}
//__________________________________________________________________________
