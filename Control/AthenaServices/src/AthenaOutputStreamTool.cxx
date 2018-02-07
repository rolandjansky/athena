/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

// Athena
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaBaseComps/AthCnvSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/SGIFolder.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "PersistentDataModel/DataHeader.h"

/// Constructor
AthenaOutputStreamTool::AthenaOutputStreamTool(const std::string& type,
		const std::string& name,
		const IInterface* parent) : ::AthAlgTool(type, name, parent),
	m_store("DetectorStore", name),
	m_conversionSvc("AthenaPoolCnvSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_dataHeader(0),
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
   // Properties
   declareProperty("OutputFile",            m_outputName = std::string());
   declareProperty("Store",                 m_store = ServiceHandle<StoreGateSvc>("DetectorStore", name),
	   "Handle to the StoreGateSvc instance from which to read data (to be written out)");
   declareProperty("ProcessingTag",         m_processTag = m_dataHeaderKey);
   declareProperty("OutputCollection",      m_outputCollection = std::string());
   declareProperty("PoolContainerPrefix",   m_containerPrefix = std::string());
   declareProperty("TopLevelContainerName", m_containerNameHint = "0");
   declareProperty("SubLevelBranchName",    m_branchNameHint = "0");
   declareProperty("AttributeListKey",      m_attrListKey = std::string());
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
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::finalize() {
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
         ATH_MSG_ERROR("PETER: Could not release " << m_store.type() << " store");
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
      ATH_MSG_ERROR("PETER: Could not locate " << m_store.type() << " store");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::connectOutput(const std::string& outputName) {
   ATH_MSG_DEBUG("In connectOutput");

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
   std::string outputConnectionString = m_outputName.value();
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
   if (m_conversionSvc->connectOutput(outputConnectionString).isFailure()) {
      ATH_MSG_ERROR("Unable to connect output " << outputConnectionString);
      return(StatusCode::FAILURE);
   } else {
      ATH_MSG_DEBUG("Connected to " << m_outputName.value());
   }

   // Remove DataHeader with same key if it exists
   if (m_store->contains<DataHeader>(m_dataHeaderKey)) {
      const DataHandle<DataHeader> preDh;
      if (m_store->retrieve(preDh, m_dataHeaderKey).isSuccess()) {
         if (m_store->removeDataAndProxy(preDh.cptr()).isFailure()) {
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
   const DataHandle<DataHeader> dh;
   std::vector<std::string> dhKeys;
   m_store->keys<DataHeader>(dhKeys);
   for (std::vector<std::string>::const_iterator dhKey = dhKeys.begin(), dhKeyEnd = dhKeys.end();
	   dhKey != dhKeyEnd; dhKey++) {
      if (!m_store->transientContains<DataHeader>(*dhKey)) { // Do not retrieve BackNavigation DataHeader
         continue;
      }
      if (m_store->retrieve(dh, *dhKey).isFailure()) {
         ATH_MSG_DEBUG("Unable to retrieve the DataHeader with key " << *dhKey);
      }
      if (dh->checkStatus(DataHeader::Primary)) {
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
            SG::DataProxy* dhProxy = m_store->proxy(dh.operator->());
            if (dhProxy != 0 && dhProxy->address() != 0) {
              delete dhTransAddr; dhTransAddr = 0;
              m_dataHeader->insertProvenance(DataHeaderElement(dhProxy,
                                                               dhProxy->address(),
                                                               pTag));
            } else {
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
   std::string attrListKey = m_attrListKey.value();
   std::string::size_type pos = outputConnectionString.find("[AttributeListKey=");
   if (pos != std::string::npos) {
      attrListKey = outputConnectionString.substr(pos + 18, outputConnectionString.find("]", pos + 18) - pos - 18);
   }
   if (!attrListKey.empty()) {
      const DataHandle<AthenaAttributeList> attrList;
      if (m_store->retrieve(attrList, attrListKey).isFailure()) {
         ATH_MSG_WARNING("Unable to retrieve AttributeList with key " << attrListKey);
      } else {
         m_dataHeader->setAttributeList(attrList);
      }
   }
   // Record DataHeader in StoreGate
   if (m_store->record(m_dataHeader, m_dataHeaderKey).isFailure()) {
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
StatusCode AthenaOutputStreamTool::commitOutput() {
   ATH_MSG_DEBUG("In commitOutput");
   m_outputName.setValue(m_outputName.value().substr(0, m_outputName.value().find("[")));
   // Connect the output file to the service
   if (m_conversionSvc->commitOutput(m_outputName.value(), false).isFailure()) {
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
      if (athConversionSvc->disconnectOutput().isFailure()) {
         ATH_MSG_ERROR("Unable to finalize output ");
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::streamObjects(const TypeKeyPairs& typeKeys) {
   ATH_MSG_DEBUG("In streamObjects");
   // Check that a connection has been opened
   if (!m_connectionOpen) {
      ATH_MSG_ERROR("Connection NOT open. Please open a connection before streaming out objects.");
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
   StatusCode status = streamObjects(dataObjects);
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Could not stream out objects");
      return(status);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::streamObjects(const DataObjectVec& dataObjects) {
   // Check that a connection has been opened
   if (!m_connectionOpen) {
      ATH_MSG_ERROR("Connection NOT open. Please open a connection before streaming out objects.");
      return(StatusCode::FAILURE);
   }
   // Check that the DataHeader is still valid
   DataObject* dataHeaderObj = m_store->accessData(ClassID_traits<DataHeader>::ID(), m_dataHeaderKey);
   StatusCode status(StatusCode::SUCCESS);
   std::set<DataObject*> written;
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
         written.insert(*doIter);
         // Write object
         IOpaqueAddress* addr(0);
         if ((m_conversionSvc->createRep(*doIter, addr)).isSuccess()) {
            SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>((*doIter)->registry());
            if (proxy != 0) {
               m_dataHeader->insert(proxy, addr);
               if (proxy->address() == 0) {
                  proxy->setAddress(addr);
               } else {
                  delete addr; addr = 0;
               }
            } else {
               ATH_MSG_WARNING("Could cast DataObject " << (*doIter)->clID() << " " << (*doIter)->name());
            }
         } else {
            ATH_MSG_ERROR("Could not create Rep for DataObject (clid/key):" << (*doIter)->clID() << " " << (*doIter)->name());
            status = StatusCode::FAILURE;
         }
      }
   }
   m_dataHeader->addHash(&*m_store);
   if (m_conversionSvc.type() == "AthenaPoolCnvSvc") {
      // End of loop over DataObjects, write DataHeader
      IOpaqueAddress* addr(0);
      if ((m_conversionSvc->createRep(dataHeaderObj, addr)).isSuccess()) {
         SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(dataHeaderObj->registry());
         if (proxy != 0) {
            m_dataHeader->insert(proxy, addr, m_processTag);
            if (proxy->address() == 0) {
               proxy->setAddress(addr);
            } else {
               delete addr; addr = 0;
            }
         } else {
            ATH_MSG_WARNING("Could cast DataHeader");
            status = StatusCode::FAILURE;
         }
      } else {
         ATH_MSG_ERROR("Could not create Rep for DataHeader");
         status = StatusCode::FAILURE;
      }
   }
   if (!fillObjectRefs(dataObjects).isSuccess()) {
      status = StatusCode::FAILURE;
   }
   if (m_conversionSvc.type() == "AthenaPoolCnvSvc") {
      // End of DataObjects, fill refs for DataHeader
      DataObjectVec dataHeaderObjVec;
      dataHeaderObjVec.push_back(dataHeaderObj);
      if (!fillObjectRefs(dataHeaderObjVec).isSuccess()) {
         status = StatusCode::FAILURE;
      }
   }
   return(status);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::fillObjectRefs(const DataObjectVec& dataObjects) { //Copy
   StatusCode status = StatusCode::SUCCESS;
   for (std::vector<DataObject*>::const_iterator doIter = dataObjects.begin(), doLast = dataObjects.end();
	   doIter != doLast; doIter++) {
      // call fillRepRefs of persistency service
      if (!(m_conversionSvc->fillRepRefs((*doIter)->registry()->address(), *doIter)).isSuccess()) {
         status = StatusCode::FAILURE;
      }
   }
   return(status);
}
//__________________________________________________________________________
StatusCode AthenaOutputStreamTool::getInputItemList(SG::IFolder* p2BWrittenFromTool) {
   const std::string hltKey = "HLTAutoKey";
   const DataHandle<DataHeader> beg, ending;
   if (m_store->retrieve(beg, ending).isFailure() || beg == ending) {
      ATH_MSG_DEBUG("No DataHeaders present in StoreGate");
   } else {
      for ( ; beg != ending; ++beg) {
         if (m_store->transientContains<DataHeader>(beg.key()) && beg->isInput()) {
            for (std::vector<DataHeaderElement>::const_iterator it = beg->begin(), itLast = beg->end();
	            it != itLast; ++it) {
               // Only insert the primary clid, not the ones for the symlinks!
               CLID clid = it->getPrimaryClassID();
                  std::string typeName;
                  if (clid != ClassID_traits<DataHeader>::ID()) {
                  //check the typename is known ... we make an exception if the key contains 'Aux.' ... aux containers may not have their keys known yet in some cases
		  //see https://its.cern.ch/jira/browse/ATLASG-59 for the solution
                  std::string typeName;
                  if( m_clidSvc->getTypeNameOfID(clid,typeName).isFailure() && it->getKey().find("Aux.") == std::string::npos) {
		    if(m_skippedItems.find(it->getKey()) == m_skippedItems.end()) {
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
