/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaOutputStreamTool.cxx
 *  @brief This file contains the implementation for the AthenaOutputStreamTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaOutputStreamTool.h"

// Gaudi
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Athena
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IDecisionSvc.h"
#include "AthenaBaseComps/AthCnvSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/SGIFolder.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/TokenAddress.h"

namespace {

/// Check to see if a DataHeader has been marked as input
/// by MakeInputDataHeader.
bool hasInputAlias (const SG::DataProxy& dp)
{
  std::string inputName = dp.name() + "_Input";
  return dp.hasAlias (inputName);
}


} // anonymous namespace

/// Constructor
AthenaOutputStreamTool::AthenaOutputStreamTool(const std::string& type,
		const std::string& name,
		const IInterface* parent) : ::AthAlgTool(type, name, parent),
	m_store("DetectorStore", name),
	m_conversionSvc("AthenaPoolCnvSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_decSvc("DecisionSvc/DecisionSvc", name),
	m_dataHeader(nullptr),
	m_dataHeaderKey(name),
	m_connectionOpen(false),
	m_extendProvenanceRecord(false) {
   // Declare IAthenaOutputStreamTool interface
   declareInterface<IAthenaOutputStreamTool>(this);

   // Remove "ToolSvc." from m_dataHeaderKey.
   if (m_dataHeaderKey.find("ToolSvc.") == 0) {
      m_dataHeaderKey = m_dataHeaderKey.substr(8);
      // Remove "Tool" from m_dataHeaderKey.
      if (m_dataHeaderKey.find("Tool") == m_dataHeaderKey.size() - 4) {
         m_dataHeaderKey = m_dataHeaderKey.substr(0, m_dataHeaderKey.size() - 4);
      }
   } else {
      const INamedInterface* parentAlg = dynamic_cast<const INamedInterface*>(parent);
      if (parentAlg != 0) {
         m_dataHeaderKey = parentAlg->name();
      }
   }
   m_processTag = m_dataHeaderKey;
   declareProperty("SaveDecisions",         m_extend = false, "Set to true to add streaming decisions to an attributeList");
}
//__________________________________________________________________________
AthenaOutputStreamTool::~AthenaOutputStreamTool() {
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   // Get the ClassIDSvc - to get typename for clid
   if (m_clidSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Cannot get IClassIDSvc interface of the CLIDSvc");
      return(StatusCode::FAILURE);
   }
   if (m_conversionSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Cannot get IConversionSvc interface of the ConversionSvc");
      return(StatusCode::FAILURE);
   }

   { // handle the AttrKey overwite
     const std::string keyword = "[AttributeListKey=";
     std::string::size_type pos = m_outputName.value().find(keyword);
     if( (pos != std::string::npos) ) {
       ATH_MSG_INFO("The AttrListKey will be overwritten/set by the value from the OutputName: " << m_outputName);
       const std::string attrListKey = m_outputName.value().substr(pos + keyword.size(),
								   m_outputName.value().find("]", pos + keyword.size()) - pos - keyword.size());
       m_attrListKey = attrListKey;
     }
   }
   if ( ! m_attrListKey.key().empty() ) {
     ATH_CHECK(m_attrListKey.initialize());
     m_attrListWrite = m_attrListKey.key() + "Decisions";
     //ATH_CHECK(m_attrListWrite.initialize());
   }
   if (m_extend) ATH_CHECK(m_decSvc.retrieve());
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::finalize() {
   m_decSvc.release().ignore();
   if (m_conversionSvc.release().isFailure()) {
      ATH_MSG_WARNING("Cannot release AthenaPoolCnvSvc");
   }
   if (m_clidSvc.release().isFailure()) {
      ATH_MSG_WARNING("Cannot release the CLIDSvc");
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::connectServices(const std::string& dataStore,
	const std::string& cnvSvc,
	bool extendProvenenceRecord) {
   // Release old data store
   if (m_store.isValid()) {
      if (m_store.release().isFailure()) {
         ATH_MSG_ERROR("Could not release " << m_store.typeAndName() << " store");
      }
   }
   m_store = ServiceHandle<StoreGateSvc>(dataStore, this->name());
   if (cnvSvc != m_conversionSvc.type() && cnvSvc != "EventPersistencySvc") {
      if (m_conversionSvc.release().isFailure()) {
         ATH_MSG_ERROR("Could not release " << m_conversionSvc.type());
      }
      m_conversionSvc = ServiceHandle<IConversionSvc>(cnvSvc, this->name());
      if (m_conversionSvc.retrieve().isFailure() || m_conversionSvc == 0) {
         ATH_MSG_ERROR("Could not locate " << m_conversionSvc.type());
         return(StatusCode::FAILURE);
      }
   }
   m_extendProvenanceRecord = extendProvenenceRecord;
   return(connectServices());
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::connectServices() {
   // Find the data store
   if (m_store.retrieve().isFailure() || m_store == 0) {
      ATH_MSG_ERROR("Could not locate " << m_store.typeAndName() << " store");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::connectOutput(const std::string& outputName) {
   ATH_MSG_DEBUG("In connectOutput " << outputName);

   // Use arg if not empty, save the output name
   if (!outputName.empty()) {
      m_outputName.setValue(outputName);
   }
   if (m_outputName.value().empty()) {
      ATH_MSG_ERROR("No OutputName provided");
      return(StatusCode::FAILURE);
   }
   // Connect services if not already available
   if (m_store == 0 || m_conversionSvc == 0) {
      if (connectServices().isFailure()) {
         ATH_MSG_ERROR("Unable to connect services");
         return(StatusCode::FAILURE);
      }
   }
   // Connect the output file to the service
   if (m_conversionSvc->connectOutput(m_outputName.value()).isFailure()) {
      ATH_MSG_ERROR("Unable to connect output " << m_outputName.value());
      return(StatusCode::FAILURE);
   } else {
      ATH_MSG_DEBUG("Connected to " << m_outputName.value());
   }

   // Remove DataHeader with same key if it exists
   if (m_store->contains<DataHeader>(m_dataHeaderKey)) {
      const DataHeader* preDh = nullptr;
      if (m_store->retrieve(preDh, m_dataHeaderKey).isSuccess()) {
         if (m_store->removeDataAndProxy(preDh).isFailure()) {
            ATH_MSG_ERROR("Unable to get proxy for the DataHeader with key " << m_dataHeaderKey);
            return(StatusCode::FAILURE);
         }
         ATH_MSG_DEBUG("Released DataHeader with key " << m_dataHeaderKey);
      }
   }

   // Create new DataHeader
   m_dataHeader = new DataHeader();
   m_dataHeader->setProcessTag(m_processTag);

   // Retrieve all existing DataHeaders from StroreGate
   const DataHeader* dh = nullptr;
   std::vector<std::string> dhKeys;
   m_store->keys<DataHeader>(dhKeys);
   for (std::vector<std::string>::const_iterator dhKey = dhKeys.begin(), dhKeyEnd = dhKeys.end();
	   dhKey != dhKeyEnd; dhKey++) {
      bool primaryDH = false;
      if (!m_store->transientContains<DataHeader>(*dhKey)) {
         if (*dhKey == "EventSelector") primaryDH = true;
         ATH_MSG_DEBUG("No transientContains DataHeader with key " << *dhKey);
      }
      if (m_store->retrieve(dh, *dhKey).isFailure()) {
         ATH_MSG_DEBUG("Unable to retrieve the DataHeader with key " << *dhKey);
      }
      SG::DataProxy* dhProxy = m_store->proxy(dh);
      if (dh->isInput() || hasInputAlias (*dhProxy) || primaryDH) {
         // Add DataHeader token to new DataHeader
         if (m_extendProvenanceRecord) {
            std::string pTag;
            SG::TransientAddress* dhTransAddr = 0;
            for (std::vector<DataHeaderElement>::const_iterator i = dh->begin(), iEnd = dh->end();
                i != iEnd; i++) {
               if (i->getPrimaryClassID() == ClassID_traits<DataHeader>::ID()) {
                  pTag = i->getKey();
                  delete dhTransAddr; dhTransAddr = i->getAddress(0);
               }
            }
            // Update dhTransAddr to handle fast merged files.
            if (dhProxy != 0 && dhProxy->address() != 0) {
              delete dhTransAddr; dhTransAddr = 0;
              m_dataHeader->insertProvenance(DataHeaderElement(dhProxy,
                                                               dhProxy->address(),
                                                               pTag));
            }
            else if (dhTransAddr != nullptr) {
              m_dataHeader->insertProvenance(DataHeaderElement(dhTransAddr,
                                                               dhTransAddr->address(),
                                                               pTag));
              delete dhTransAddr; dhTransAddr = 0;
            }
         }
         for (std::vector<DataHeaderElement>::const_iterator i = dh->beginProvenance(),
	      iEnd = dh->endProvenance(); i != iEnd; i++) {
            m_dataHeader->insertProvenance(*i);
         }
      }
   }

   // Attach the attribute list to the DataHeader if requested
   if (!m_attrListKey.key().empty() && m_store->storeID() == StoreID::EVENT_STORE) {
      auto attrListHandle = SG::makeHandle(m_attrListKey);
      if (!attrListHandle.isValid()) {
         ATH_MSG_WARNING("Unable to retrieve AttributeList with key " << m_attrListKey);
      } else {
         m_dataHeader->setAttributeList(attrListHandle.cptr());
         if (m_extend) {  // Add streaming decisions
            ATH_MSG_DEBUG("Adding stream decisions to " << m_attrListWrite);
            // Look for attribute list created for mini-EventInfo
            const AthenaAttributeList* attlist(attrListHandle.cptr());

            // Build new attribute list for modification
            AthenaAttributeList* newone = new AthenaAttributeList(attlist->specification());
            newone->copyData(*attlist);

            // Now loop over stream definitions and add decisions
            auto streams = m_decSvc->getStreams();
            for (auto it  = streams.begin();
                      it != streams.end(); ++it) {
               newone->extend(*it,"bool");
               (*newone)[*it].data<bool>() = m_decSvc->isEventAccepted(*it,Gaudi::Hive::currentContext());
               ATH_MSG_DEBUG("Added stream decision for " << *it << " to " << m_attrListKey);
            }
            // record new attribute list with old key + suffix
            const AthenaAttributeList* attrList2 = nullptr;
            if (!m_store->contains<AthenaAttributeList>(m_attrListWrite)) {
               if (m_store->record(newone,m_attrListWrite).isFailure()) {
                  ATH_MSG_ERROR("Unable to record att list " << m_attrListWrite);
               }
            } else {
               ATH_MSG_DEBUG("Decisions already added by a different stream");
            }
            if (m_store->retrieve(attrList2,m_attrListWrite).isFailure()) {
               ATH_MSG_ERROR("Unable to record att list " << m_attrListWrite);
            } else {
               m_dataHeader->setAttributeList(attrList2);
            }
/*
            SG::WriteHandle<AthenaAttributeList> attrWrite(m_attrListWrite);
            std::unique_ptr<AthenaAttributeList> uptr = std::make_unique<AthenaAttributeList>(*newone);
            if ( attrWrite.record(std::move(uptr)).isFailure() ) {
               ATH_MSG_ERROR("Unable to record att list " << m_attrListWrite);
            } else {
               ATH_MSG_DEBUG("Decisions already added by a different stream");
            }
*/
            //m_dataHeader->setAttributeList(newone);
         }    // list extend check
      }       // list retrieve check
   }          // list property check

   // Record DataHeader in StoreGate
   SG::WriteHandle<DataHeader> wh(m_dataHeaderKey, m_store->name());
   if (wh.record(std::unique_ptr<DataHeader>(m_dataHeader)).isFailure()) {
      ATH_MSG_ERROR("Unable to record DataHeader with key " << m_dataHeaderKey);
      return(StatusCode::FAILURE);
   } else {
      ATH_MSG_DEBUG("Recorded DataHeader with key " << m_dataHeaderKey);
   }
   m_dataHeader->setStatus(DataHeader::Output);
   // Set flag that connection is open
   m_connectionOpen = true;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::commitOutput(bool doCommit) {
   ATH_MSG_DEBUG("In commitOutput");
   // Connect the output file to the service
   if (m_conversionSvc->commitOutput(m_outputName.value(), doCommit).isFailure()) {
      ATH_MSG_ERROR("Unable to commit output " << m_outputName.value());
      return(StatusCode::FAILURE);
   }
   // Set flag that connection is closed
   m_connectionOpen = false;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::finalizeOutput() {
   AthCnvSvc* athConversionSvc = dynamic_cast<AthCnvSvc*>(m_conversionSvc.get());
   if (athConversionSvc != 0) {
      if (athConversionSvc->disconnectOutput(m_outputName.value()).isFailure()) {
         ATH_MSG_ERROR("Unable to finalize output " << m_outputName.value());
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::streamObjects(const TypeKeyPairs& typeKeys, const std::string& outputName) {
   ATH_MSG_DEBUG("In streamObjects");
   // Check that a connection has been opened
   if (!m_connectionOpen) {
      ATH_MSG_ERROR("Connection NOT open. Please open a connection before streaming out objects.");
      return(StatusCode::FAILURE);
   }
   // Use arg if not empty, save the output name
   if (!outputName.empty()) {
      m_outputName.setValue(outputName);
   }
   if (m_outputName.value().empty()) {
      ATH_MSG_ERROR("No OutputName provided");
      return(StatusCode::FAILURE);
   }
   // Now iterate over the type/key pairs and stream out each object
   std::vector<DataObject*> dataObjects;
   for (TypeKeyPairs::const_iterator first = typeKeys.begin(), last = typeKeys.end();
	   first != last; ++first) {
      const std::string& type = (*first).first;
      const std::string& key  = (*first).second;
      // Find the clid for type name from the CLIDSvc
      CLID clid;
      if (m_clidSvc->getIDOfTypeName(type, clid).isFailure()) {
         ATH_MSG_ERROR("Could not get clid for typeName " << type);
         return(StatusCode::FAILURE);
      }
      DataObject* dObj = 0;
      // Two options: no key or explicit key
      if (key.empty()) {
         ATH_MSG_DEBUG("Get data object with no key");
         // Get DataObject without key
         dObj = m_store->accessData(clid);
      } else {
         ATH_MSG_DEBUG("Get data object with key");
         // Get DataObjects with key
         dObj = m_store->accessData(clid, key);
      }
      if (dObj == 0) {
         // No object - print warning and return
         ATH_MSG_DEBUG("No object found for type " << type << " key " << key);
         return(StatusCode::SUCCESS);
      } else {
         ATH_MSG_DEBUG("Found object for type " << type << " key " << key);
      }
      // Save the dObj
      dataObjects.push_back(dObj);
   }
   // Stream out objects
   if (dataObjects.size() == 0) {
      ATH_MSG_DEBUG("No data objects found");
      return(StatusCode::SUCCESS);
   }
   StatusCode status = streamObjects(dataObjects, m_outputName.value());
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Could not stream out objects");
      return(status);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::streamObjects(const DataObjectVec& dataObjects, const std::string& outputName) {
   // Check that a connection has been opened
   if (!m_connectionOpen) {
      ATH_MSG_ERROR("Connection NOT open. Please open a connection before streaming out objects.");
      return(StatusCode::FAILURE);
   }
   // Connect the output file to the service
   std::string outputConnectionString = outputName;
   if (!m_outputCollection.value().empty() && outputConnectionString.find("[OutputCollection=") == std::string::npos) {
      outputConnectionString += "[OutputCollection=" + m_outputCollection.value() + "]";
   }
   if (!m_containerPrefix.value().empty() && outputConnectionString.find("[PoolContainerPrefix=") == std::string::npos) {
      outputConnectionString += "[PoolContainerPrefix=" + m_containerPrefix.value() + "]";
   }
   if (m_containerNameHint.value() != "0" && outputConnectionString.find("[TopLevelContainerName=") == std::string::npos) {
      outputConnectionString += "[TopLevelContainerName=" + m_containerNameHint.value() + "]";
   }
   if (m_branchNameHint.value() != "0" && outputConnectionString.find("[SubLevelBranchName=") == std::string::npos) {
      outputConnectionString += "[SubLevelBranchName=" + m_branchNameHint.value() + "]";
   }
   // Check that the DataHeader is still valid
   DataObject* dataHeaderObj = m_store->accessData(ClassID_traits<DataHeader>::ID(), m_dataHeaderKey);
   std::map<DataObject*, IOpaqueAddress*> written;
   for (std::vector<DataObject*>::const_iterator doIter = dataObjects.begin(), doLast = dataObjects.end();
	   doIter != doLast; doIter++) {
      // Do not write the DataHeader via the explicit list
      if ((*doIter)->clID() == ClassID_traits<DataHeader>::ID()) {
         ATH_MSG_DEBUG("Explicit request to write DataHeader: " << (*doIter)->name() << " - skipping it.");
      // Do not stream out same object twice
      } else if (written.find(*doIter) != written.end()) {
         // Print warning and skip
         ATH_MSG_DEBUG("Trying to write DataObject twice (clid/key): " << (*doIter)->clID() << " " << (*doIter)->name());
         ATH_MSG_DEBUG("    Skipping this one.");
      } else {
         // Write object
         IOpaqueAddress* addr = new TokenAddress(0, (*doIter)->clID(), outputConnectionString);
         if (m_conversionSvc->createRep(*doIter, addr).isSuccess()) {
            written.insert(std::pair<DataObject*, IOpaqueAddress*>(*doIter, addr));
         } else {
            ATH_MSG_ERROR("Could not create Rep for DataObject (clid/key):" << (*doIter)->clID() << " " << (*doIter)->name());
            return(StatusCode::FAILURE);
         }
      }
   }
   // End of loop over DataObjects, write DataHeader
   if (m_conversionSvc.type() == "AthenaPoolCnvSvc" && dataHeaderObj != nullptr) {
      IOpaqueAddress* addr = new TokenAddress(0, dataHeaderObj->clID(), outputConnectionString);
      if (m_conversionSvc->createRep(dataHeaderObj, addr).isSuccess()) {
         written.insert(std::pair<DataObject*, IOpaqueAddress*>(dataHeaderObj, addr));
      } else {
         ATH_MSG_ERROR("Could not create Rep for DataHeader");
         return(StatusCode::FAILURE);
      }
   }
   for (std::vector<DataObject*>::const_iterator doIter = dataObjects.begin(), doLast = dataObjects.end();
	   doIter != doLast; doIter++) {
      // call fillRepRefs of persistency service
      SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>((*doIter)->registry());
      if (proxy != nullptr && written.find(*doIter) != written.end()) {
         IOpaqueAddress* addr(written.find(*doIter)->second);
         if ((m_conversionSvc->fillRepRefs(addr, *doIter)).isSuccess()) {
            if ((*doIter)->clID() != 1 || addr->par()[0] != "\n") {
               if ((*doIter)->clID() != ClassID_traits<DataHeader>::ID()) {
                  m_dataHeader->insert(proxy, addr);
                  if (m_store->storeID() != StoreID::EVENT_STORE) proxy->setAddress(addr);
               } else {
                  m_dataHeader->insert(proxy, addr, m_processTag);
               }
               if (m_store->storeID() == StoreID::EVENT_STORE) {
                  delete addr; addr = nullptr;
               }
            }
         } else {
            ATH_MSG_ERROR("Could not fill Object Refs for DataObject (clid/key):" << (*doIter)->clID() << " " << (*doIter)->name());
            return(StatusCode::FAILURE);
         }
      } else {
         ATH_MSG_WARNING("Could cast DataObject " << (*doIter)->clID() << " " << (*doIter)->name());
      }
   }
   m_dataHeader->addHash(&*m_store);
   if (m_conversionSvc.type() == "AthenaPoolCnvSvc" && dataHeaderObj != nullptr) {
      // End of DataObjects, fill refs for DataHeader
      SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(dataHeaderObj->registry());
      if (proxy != nullptr && written.find(dataHeaderObj) != written.end()) {
         IOpaqueAddress* addr(written.find(dataHeaderObj)->second);
         if ((m_conversionSvc->fillRepRefs(addr, dataHeaderObj)).isSuccess()) {
            if (dataHeaderObj->clID() != 1 || addr->par()[0] != "\n") {
               if (dataHeaderObj->clID() != ClassID_traits<DataHeader>::ID()) {
                  m_dataHeader->insert(proxy, addr);
               } else {
                  m_dataHeader->insert(proxy, addr, m_processTag);
               }
               delete addr; addr = nullptr;
            }
         } else {
            ATH_MSG_ERROR("Could not fill Object Refs for DataHeader");
            return(StatusCode::FAILURE);
         }
      } else {
         ATH_MSG_ERROR("Could not cast DataHeader");
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::getInputItemList(SG::IFolder* p2BWrittenFromTool) {
   const std::string hltKey = "HLTAutoKey";
   SG::ConstIterator<DataHeader> beg;
   SG::ConstIterator<DataHeader> ending;
   if (m_store->retrieve(beg, ending).isFailure() || beg == ending) {
      ATH_MSG_DEBUG("No DataHeaders present in StoreGate");
   } else {
      for ( ; beg != ending; ++beg) {
         if (m_store->transientContains<DataHeader>(beg.key()) && beg->isInput()) {
            for (std::vector<DataHeaderElement>::const_iterator it = beg->begin(), itLast = beg->end();
	            it != itLast; ++it) {
               // Only insert the primary clid, not the ones for the symlinks!
               CLID clid = it->getPrimaryClassID();
               if (clid != ClassID_traits<DataHeader>::ID()) {
                  //check the typename is known ... we make an exception if the key contains 'Aux.' ... aux containers may not have their keys known yet in some cases
                  //see https://its.cern.ch/jira/browse/ATLASG-59 for the solution
                  std::string typeName;
                  if (m_clidSvc->getTypeNameOfID(clid, typeName).isFailure() && it->getKey().find("Aux.") == std::string::npos) {
                     if (m_skippedItems.find(it->getKey()) == m_skippedItems.end()) {
                        ATH_MSG_WARNING("Skipping " << it->getKey() << " with unknown clid " << clid << " . Further warnings for this item are suppressed" );
                        m_skippedItems.insert(it->getKey());
                     }
                     continue;
                  }
                  ATH_MSG_DEBUG("Adding " << typeName << "#" << it->getKey() << " (clid " << clid << ") to itemlist");
                  const std::string keyName = it->getKey();
                  if (keyName.size() > 10 && keyName.substr(0, 10) == hltKey) {
                     p2BWrittenFromTool->add(clid, hltKey + "*").ignore();
                  } else if (keyName.size() > 10 && keyName.substr(keyName.size() - 10, 10) == hltKey) {
                     p2BWrittenFromTool->add(clid, "*" + hltKey).ignore();
                  } else {
                     p2BWrittenFromTool->add(clid, keyName).ignore();
                  }
               }
            }
         }
      }
   }
   ATH_MSG_DEBUG("Adding DataHeader for stream " << name());
   return(StatusCode::SUCCESS);
}
