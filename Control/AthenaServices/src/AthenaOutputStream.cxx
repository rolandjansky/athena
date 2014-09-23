/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <string>
using std::string;
#include <vector>
using std::vector;

// Framework include files
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "AthenaKernel/IItemListSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/SGIFolder.h"

#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"


#include "AthenaOutputStream.h"
#include "OutputStreamSequencerSvc.h"

// Standard Constructor
AthenaOutputStream::AthenaOutputStream(const string& name, ISvcLocator* pSvcLocator)
		: FilteredAlgorithm(name, pSvcLocator),
		m_dataStore("StoreGateSvc", name),
		m_metadataStore("MetaDataStore", name),
		m_currentStore(&m_dataStore),
                m_itemSvc("ItemListSvc", name),
  		m_pCLIDSvc("ClassIDSvc", name),
  		m_outSeqSvc("OutputStreamSequencerSvc", name),
		m_p2BWritten(string("SG::Folder/") + name + string("_TopFolder"), this),
		m_decoder(string("SG::Folder/") + name + string("_excluded"), this),
                m_events(0),
		m_streamer(string("AthenaOutputStreamTool/") + name + string("Tool"), this),
   m_helperTools(this) {
   assert(pSvcLocator);
   declareProperty("ItemList",               m_itemList);
   declareProperty("MetadataItemList",       m_metadataItemList);
   declareProperty("OutputFile",             m_outputName="DidNotNameOutput.root");
   declareProperty("EvtConversionSvc",       m_persName="EventPersistencySvc");
   declareProperty("WritingTool",            m_streamer);
   declareProperty("Store",                  m_dataStore);
   declareProperty("MetadataStore",          m_metadataStore);
   declareProperty("ProcessingTag",          m_processTag=name);
   declareProperty("ForceRead",              m_forceRead=false);
   declareProperty("PersToPers",             m_persToPers=false);
   declareProperty("ExemptPersToPers",       m_exemptPersToPers);
   declareProperty("ProvideDef",             m_provideDef=false);
   declareProperty("ExtendProvenanceRecord", m_extendProvenanceRecord=true);
   declareProperty("WriteOnExecute",         m_writeOnExecute=true);
   declareProperty("WriteOnFinalize",        m_writeOnFinalize=false);
   declareProperty("TakeItemsFromInput",     m_itemListFromTool=false);
   declareProperty("CheckNumberOfWrites",    m_checkNumberOfWrites=true);
   declareProperty("ExcludeList",            m_excludeList);
   declareProperty("HelperTools",            m_helperTools);

   // Associate action handlers with the AcceptAlgs,
   // RequireAlgs & VetoAlgs properties
   m_itemList.declareUpdateHandler(&AthenaOutputStream::itemListHandler, this);
   m_excludeList.declareUpdateHandler(&AthenaOutputStream::excludeListHandler, this);
}

// Standard Destructor
AthenaOutputStream::~AthenaOutputStream() {
}

// initialize data writer
StatusCode AthenaOutputStream::initialize() {
   StatusCode status(StatusCode::FAILURE);
   StatusCode baseStatus = this->FilteredAlgorithm::initialize();
   ATH_MSG_DEBUG("In initialize");
   // Reset the number of events written
   m_events = 0;

   // set up the SG service:
   status = m_dataStore.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could not locate default store");
      return(status);
   } else {
      ATH_MSG_DEBUG("Found " << m_dataStore.type() << " store.");
   }
   assert(static_cast<bool>(m_dataStore));
   if (!m_metadataItemList.value().empty()) {
      status = m_metadataStore.retrieve();
      if (!status.isSuccess()) {
         ATH_MSG_FATAL("Could not locate metadata store");
         return(status);
      } else {
         ATH_MSG_DEBUG("Found " << m_metadataStore.type() << " store.");
      }
      assert(static_cast<bool>(m_metadataStore));
   }

   // set up the CLID service:
   status = m_pCLIDSvc.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could not locate default ClassIDSvc");
      return(status);
   }

   // set up the ItemListSvc service:
   assert(static_cast<bool>(m_pCLIDSvc));
   status = m_itemSvc.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could not locate default ItemListSvc");
      return(status);
   }
   assert(static_cast<bool>(m_itemSvc));

   // set up the OutputStreamSequencer service:
   status = m_outSeqSvc.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could not locate OutputStreamSequencerSvc");
      return(status);
   }
   assert(static_cast<bool>(m_outSeqSvc));

   // Get Output Stream tool for writing
   status = m_streamer.retrieve();
   if (status.isFailure()) {
      ATH_MSG_FATAL("Cannot find " << m_streamer);
      return(status);
   }
   status = m_streamer->connectServices(m_dataStore.type(), m_persName, m_extendProvenanceRecord);
   if (status.isFailure()) {
      ATH_MSG_FATAL("Unable to connect services");
      return(status);
   }

   status = m_helperTools.retrieve();
   if (status.isFailure()) {
      ATH_MSG_FATAL("Cannot find " << m_helperTools);
      return(status);
   }
   ATH_MSG_INFO("Found " << m_helperTools << endreq << "Data output: " << m_outputName);

   for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
		   iter != m_helperTools.end(); iter++) {
      if (!(*iter)->postInitialize().isSuccess()) {
         status = StatusCode::FAILURE;
      }
   }

   // Register this algorithm for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   status = iomgr.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Cannot get the IoComponentMgr");
      return(status);
   }
   status = iomgr->io_register(this);
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could not register myself with the IoComponentMgr");
      return(status);
   }
   status = iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, m_outputName);
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could not register [" << m_outputName << "] for output !");
      return(status);
   }
   status = this->io_reinit();
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Could re-init I/O component");
      return(status);
   }
   ATH_MSG_DEBUG("End initialize");
   if (baseStatus == StatusCode::FAILURE) return StatusCode::FAILURE;
   return(status);
}

void AthenaOutputStream::handle(const Incident& inc) {
   ATH_MSG_DEBUG("handle() incident type: " << inc.type());
   if (inc.type() == "MetaDataStop") {
      // Moved preFinalize of helper tools to stop - want to optimize the
      // output file in finalize RDS 12/2009
      for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
           iter != m_helperTools.end(); iter++) {
         if (!(*iter)->preFinalize().isSuccess()) {
             ATH_MSG_ERROR("Cannot finalize helper tool");
         }
      }
      // Always force a final commit in stop - mainly applies to AthenaPool 
      if (m_writeOnFinalize) {
         if (write().isFailure()) {  // true mean write AND commit
            ATH_MSG_ERROR("Cannot write on finalize");
         }
         ATH_MSG_INFO("Records written: " << m_events);
      }
      if (!m_metadataItemList.value().empty()) {
         m_currentStore = &m_metadataStore;
         StatusCode status = m_streamer->connectServices(m_metadataStore.type(), m_persName, false);
         if (status.isFailure()) {
            throw GaudiException("Unable to connect metadata services", name(), StatusCode::FAILURE);
         }
         m_checkNumberOfWrites = false;
         const std::string outputName0 = m_outputName;
         m_outputName += "[OutputCollection=MetaDataHdr][PoolContainerPrefix=MetaData][AttributeListKey=][DataHeaderSatellites=]";
         // BackwardCompatibility: Get MetadataItemList properties from ItemList of Stream_FH 
         ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc", name());
         if (!joSvc.retrieve().isSuccess()) {
            throw GaudiException("Cannot get JobOptionsSvc", name(), StatusCode::FAILURE);
         }
         const std::vector<const Property*>* fhProps = joSvc->getProperties(name() + "_FH");
         if (fhProps != 0) {
            StringArrayProperty fhProperty("ItemList", std::vector<std::string>());
            for (std::vector<const Property*>::const_iterator iter = fhProps->begin(),
                            last = fhProps->end(); iter != last; iter++) {
               if ((*iter)->name() == fhProperty.name()) {
                  (*iter)->load(fhProperty);
                  std::vector<std::string> vProp = m_metadataItemList.value();
                  for (std::vector<std::string>::const_iterator iter2 = fhProperty.value().begin(),
                                  last2 = fhProperty.value().end(); iter2 != last2; iter2++) {
                     vProp.push_back(*iter2);
                  }
                  if (this->setProperty("MetadataItemList", vProp).isFailure()) {
                     throw GaudiException("Folder property [metadataItemList] cannot be set", name(), StatusCode::FAILURE);
                  }
                  break;
               }
            }
         }
         m_p2BWritten->clear();
         IProperty *pAsIProp(0);
         if ((m_p2BWritten.retrieve()).isFailure() ||
                        0 == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
                        (pAsIProp->setProperty("ItemList", m_metadataItemList.toString())).isFailure()) {
            throw GaudiException("Folder property [metadataItemList] not found", name(), StatusCode::FAILURE);
         }
         if (write().isFailure()) {  // true mean write AND commit
            ATH_MSG_ERROR("Cannot write metadata");
         }
         m_outputName = outputName0;
         m_currentStore = &m_dataStore;
         status = m_streamer->connectServices(m_dataStore.type(), m_persName, m_extendProvenanceRecord);
         if (status.isFailure()) {
            throw GaudiException("Unable to re-connect services", name(), StatusCode::FAILURE);
         }
         m_p2BWritten->clear();
         if ((pAsIProp->setProperty(m_itemList)).isFailure()) {
            throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
         }
         ATH_MSG_INFO("Records written: " << m_events);
         for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
	         iter != m_helperTools.end(); iter++) {
            if (!(*iter)->postInitialize().isSuccess()) {
                ATH_MSG_ERROR("Cannot initialize helper tool");
            }
         }
      }
   }
   ATH_MSG_DEBUG("Leaving handle");
}

// terminate data writer
StatusCode AthenaOutputStream::finalize() {
   bool failed = false;
   ATH_MSG_DEBUG("finalize: Optimize output");
   // Connect the output file to the service
   if (!m_streamer->finalizeOutput().isSuccess()) {
      failed = true;
   }
   ATH_MSG_DEBUG("finalize: end optimize output");
   if (!m_helperTools.release().isSuccess()) {
      failed = true;
   }
   if (!m_streamer.release().isSuccess()) {
      failed = true;
   }
   if (failed) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

StatusCode AthenaOutputStream::execute() {
   bool failed = false;
   for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
		   iter != m_helperTools.end(); iter++) {
      if (!(*iter)->preExecute().isSuccess()) {
         failed = true;
      }
   }
   // Test whether this event should be output
   if (m_writeOnExecute && isEventAccepted()) {
      if (write().isFailure()) {
         failed = true;
      }
   }
   for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
		   iter != m_helperTools.end(); iter++) {
      if(!(*iter)->postExecute().isSuccess()) {
         failed = true;
      }
   }
   if (failed) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

// Work entry point
StatusCode AthenaOutputStream::write() {
   bool failed = false;
   // Clear any previously existing item list
   clearSelection();
   // Connect the output file to the service
   // FIXME: this double looping sucks... got to query the
   // data store for object of a given type/key.
   if (m_streamer->connectOutput(m_outSeqSvc->buildSequenceFileName(m_outputName)).isSuccess()) {
      // First check if there are any new items in the list
      collectAllObjects();
      // print out info about objects collected
      if (m_checkNumberOfWrites && !m_provideDef) {
         bool checkCountError = false;
         ATH_MSG_DEBUG(" Collected objects:");
         bool first = true;
         unsigned int lastCount = 0;
         for (CounterMapType::iterator cit = m_objectWriteCounter.begin(),
		         clast = m_objectWriteCounter.end(); cit != clast; ++cit) {
            bool isError = false;
            if (first) {
               lastCount = (*cit).second;
               first = false;
            } else if (lastCount != (*cit).second) {
               isError = true;
               //Complain, but don't abort
               checkCountError = true;
            }
            if (isError) {
               ATH_MSG_ERROR(" INCORRECT Object/count: "
		               << (*cit).first << ", " << (*cit).second << " should be: " << lastCount);
            } else {
               ATH_MSG_DEBUG(" Object/count: " << (*cit).first << ", " << (*cit).second);
            }
         }
         if (checkCountError) {
            ATH_MSG_FATAL("Check number of writes failed. See messages above "
		            "to identify which continer is not always written");
            return(StatusCode::FAILURE);
         }
      }

      StatusCode currentStatus = m_streamer->streamObjects(m_objects);
      // Do final check of streaming
      if (!currentStatus.isSuccess()) {
         if (!currentStatus.isRecoverable()) {
            ATH_MSG_FATAL("streamObjects failed.");
            failed = true;
         } else {
            ATH_MSG_DEBUG("streamObjects failed.");
         }
      }
      currentStatus = m_streamer->commitOutput();
      if (!currentStatus.isSuccess()) {
         ATH_MSG_FATAL("commitOutput failed.");
         failed = true;
      }
      clearSelection();
      if (!failed) {
         m_events++;
      }
   }
   if (failed) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

// Clear collected object list
void AthenaOutputStream::clearSelection()     {
   m_objects.erase(m_objects.begin(), m_objects.end());
}

void AthenaOutputStream::collectAllObjects() {
   if (m_itemListFromTool) {
      if (!m_streamer->getInputItemList(&*m_p2BWritten).isSuccess()) {
         ATH_MSG_WARNING("collectAllObjects() could not get ItemList from Tool.");
      }
   }
   m_p2BWritten->updateItemList(true);
   // Collect all objects that need to be persistified:
//FIXME refactor: move this in folder. Treat as composite
   for (SG::IFolder::const_iterator i = m_p2BWritten->begin(), iEnd = m_p2BWritten->end(); i != iEnd; i++) {
      addItemObjects(*i);
   }
}

//FIXME refactor: move this in folder. Treat as composite
void AthenaOutputStream::addItemObjects(const SG::FolderItem& item)
{
   // anything after a dot is a list of dynamic Aux attrubutes, separated by dots
   size_t dotpos = item.key().find('.');
   string item_key, aux_attr;
   if( dotpos != string::npos ) {
      item_key = item.key().substr(0, dotpos+1);
      aux_attr = item.key().substr(dotpos+1);
   } else {
      item_key = item.key();
   }
   ATH_MSG_DEBUG("addItemObjects(" << item.id() << ",\"" << item.key() << "\") called");
   ATH_MSG_DEBUG("           Key:" << item_key );
   if( aux_attr.size() ) {
   ATH_MSG_DEBUG("      Aux Attr:" << aux_attr );
   }
   static const std::string wildCard = "*";
   std::set<std::string> clidKeys;
   for (SG::IFolder::const_iterator iter = m_decoder->begin(), iterEnd = m_decoder->end();
		   iter != iterEnd; iter++) {
      if (iter->id() == item.id()) {
         clidKeys.insert(iter->key());
      }
   }

   SG::ConstProxyIterator iter, end;
   // Look for the clid in storegate
   if (((*m_currentStore)->proxyRange(item.id(), iter, end)).isSuccess()) {
      bool added = false, removed = false;
      // For item list entry
      // Check for wildcard within string, i.e. 'xxx*yyy', and save the matching parts
      std::pair<std::string, std::string> key = breakAtSep(item_key, wildCard);
      SG::TransientAddress* tAddr = 0;
      // Now loop over any found proxies
      for (; iter != end; iter++) {
         SG::DataProxy* itemProxy(iter->second);
         // Does this key match the proxy key name - allow for wildcarding and aliases
         bool keyMatch = (item_key == "*" || item_key == itemProxy->name()
		         || itemProxy->alias().find(item_key) != itemProxy->alias().end());
         if (!keyMatch) {
            ATH_MSG_DEBUG("Proxy name=" << itemProxy->name() );
            keyMatch = matchKey(key, itemProxy);
         }

         // Now undo the flag based on a similar analysis of excluded wildcard keys
         bool xkeyMatch = false;
         for (std::set<std::string>::const_iterator c2k_it = clidKeys.begin(), c2k_itEnd = clidKeys.end();
		         keyMatch && c2k_it != c2k_itEnd; c2k_it++) {
            if (*c2k_it == wildCard) {
               xkeyMatch = true;  // wildcard first
            } else {
               // Look for wildcard in key
               std::string::size_type xsep = c2k_it->find(wildCard);
               // If wildcard not found, then check whether the second is an excluded key
               if (xsep == std::string::npos) {
                  if (*c2k_it == itemProxy->name()) {
                     xkeyMatch = true;
                  }
               } else { // Otherwise take before and after wildcard for later use
                  std::pair<std::string, std::string> xkey = breakAtSep(*c2k_it, wildCard);
                  ATH_MSG_DEBUG("x Proxy name=" << itemProxy->name() );
                  xkeyMatch = matchKey(xkey, itemProxy);
               }
            }
         }
         // All right, it passes key match find in itemList, but not in excludeList
         if (keyMatch && !xkeyMatch) {
            if (m_forceRead && itemProxy->isValid()) {
               if (!m_persToPers) {
                  if (0 == itemProxy->accessData()) {
                     ATH_MSG_ERROR(" Could not get data object for id "
		                     << item.id() << ",\"" << itemProxy->name());
                  }
               } else if (true /*m_exemptPersToPers.find(item.id()) != m_exemptPersToPers.end()*/) {
                  if (0 == itemProxy->accessData()) {
                     ATH_MSG_ERROR(" Could not get data object for id "
		                     << item.id() << ",\"" << itemProxy->name());
                  }
               }
            }
            if (0 != itemProxy->object()) {
               SG::IAuxStoreIO* auxio(0);
               try {
                  SG::fromStorable(itemProxy->object(), auxio, true);
               }catch( const std::exception& ) {
                  // exception from Control/StoreGateBindings/src/SgPyDataModel.cxx:71
                  ATH_MSG_DEBUG( "Error in casting object with CLID "
                                 << itemProxy->clID() << " to SG::IAuxStoreIO*" );
                  auxio = 0;
               }
               if( std::find(m_objects.begin(), m_objects.end(), itemProxy->object()) == m_objects.end() ) {
                  m_objects.push_back(itemProxy->object());
                  ATH_MSG_DEBUG(" Added object " << item.id() << ",\"" << itemProxy->name() << "\"");
                  // clear any old selections
                  if( auxio ) {
                     auxio->selectAux( std::set<string>() );
                  }
               }

               if( auxio ) {
                  // collect dynamic Aux selection (parse the line, attributes separated by dot)
                  std::set<std::string> attributes;
                  if( aux_attr.size() ) {
                     std::stringstream ss(aux_attr);
                     std::string attr;
                     while( std::getline(ss, attr, '.') ) {
                        attributes.insert(attr);
                     }
                     // don't let keys with wildcard overwrite existing selections
                     if( auxio->getSelectedAuxIDs().size() == auxio->getDynamicAuxIDs().size()
                         || item_key.find('*') == string::npos )
                        auxio->selectAux(attributes);
                  }
               }

               added = true;
               if (!m_provideDef) {
               //if (m_checkNumberOfWrites && !m_provideDef) {
                  std::string tn;
                  std::stringstream tns;
                  if (!m_pCLIDSvc->getTypeNameOfID(item.id(), tn).isSuccess()) {
                     ATH_MSG_ERROR(" Could not get type name for id "
		                     << item.id() << ",\"" << itemProxy->name());
                     tns << item.id() << '_' << itemProxy->name();
                  } else {
                     tn += '_' + itemProxy->name();
                     tns << tn;
                     if (m_checkNumberOfWrites) {
                        CounterMapType::iterator cit = m_objectWriteCounter.find(tn);
                        if (cit == m_objectWriteCounter.end()) {
                          // First time through
                          //std::pair<CounterMapType::iterator, bool> result =
                          m_objectWriteCounter.insert(CounterMapType::value_type(tn, 1));
                        } else {
                           // set to next iteration (to avoid double counting)
                           // StreamTools will eliminate duplicates.
                           (*cit).second = m_events + 1;
                        }
                     }
                  }
                  if (m_itemSvc->addStreamItem(this->name(),tns.str()).isFailure()) {
                     ATH_MSG_WARNING("Unable to record item " << tns.str() << " in Svc");
                  }
               }
            } else if (!m_forceRead && m_persToPers && itemProxy->isValid()) {
               tAddr = itemProxy->transientAddress();
            } //if data object there
         } else if (keyMatch && xkeyMatch) {
            removed = true;
         }
      } // proxy loop
      if (!added && !removed) {
         if (m_persToPers && tAddr != 0) {
            ATH_MSG_DEBUG(" Going to attempt direct persistent copy for "
		            << item.id() << ",\"" << item_key  << "\"");
            DataObject* ics = new DataObject();
            SG::DataProxy* proxy = new SG::DataProxy(ics, tAddr);
            m_objects.push_back(proxy->object());
         } else if (m_provideDef) {
            ATH_MSG_DEBUG(" Going to attempt providing persistent default for "
		            << item.id() << ",\"" << item_key  << "\"");
            tAddr = new SG::TransientAddress(item.id(), item_key);
            DataObject* ics = new DataObject();
            SG::DataProxy* proxy = new SG::DataProxy(ics, tAddr);
            m_objects.push_back(proxy->object());
         } else {
            ATH_MSG_DEBUG(" No object matching " << item.id() << ",\"" << item_key  << "\" found");
         }
      } else if (removed) {
         ATH_MSG_DEBUG(" Object being excluded based on property setting "
		         << item.id() << ",\"" << item_key  << "\". Skipping");
      }
   } else {
      ATH_MSG_DEBUG(" Failed to receive proxy iterators from StoreGate for "
		      << item.id() << ",\"" << item_key  << "\". Skipping");
   }
}

void AthenaOutputStream::itemListHandler(Property& /* theProp */) {
   // Assuming concrete SG::Folder also has an itemList property
   IProperty *pAsIProp(0);
   if ((m_p2BWritten.retrieve()).isFailure() ||
		   0 == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
		   (pAsIProp->setProperty(m_itemList)).isFailure()) {
      throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
   }
}

void AthenaOutputStream::excludeListHandler(Property& /* theProp */) {
   IProperty *pAsIProp(0);
   if ((m_decoder.retrieve()).isFailure() ||
		   0 == (pAsIProp = dynamic_cast<IProperty*>(&*m_decoder)) ||
		   (pAsIProp->setProperty("ItemList", m_excludeList.toString())).isFailure()) {
      throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
   }
}

std::pair<std::string,std::string> AthenaOutputStream::breakAtSep(const std::string portia,
		const std::string sepchar) const {
   std::pair<std::string, std::string> key;
   std::string::size_type sep = portia.find(sepchar);
   if (sep != std::string::npos) {
      key.first = portia.substr(0, sep);
      key.second = portia.substr(sep + 1, portia.size());
   }
   return(key);
}

bool AthenaOutputStream::matchKey(const std::pair<std::string, std::string>& key,
		const SG::DataProxy* proxy) const {
   bool keyMatch = false;
   // Allow wildcarding within the key
   if (!key.first.empty() && !key.second.size()) {
      // Must match both pre- and post- strings
      if (proxy->name().find(key.first) != std::string::npos &&
		      proxy->name().find(key.second) != std::string::npos) {
         keyMatch = true;
      }
   } else if (!key.first.empty()) {
      // Must match both pre- and post- strings
      if (proxy->name().find(key.first) != std::string::npos) {
         keyMatch = true;
      }
   } else if (!key.second.empty()) {
      // Must match both pre- and post- strings
      if (proxy->name().find(key.second) != std::string::npos) {
         keyMatch = true;
      }
   }
   return(keyMatch);
}

StatusCode AthenaOutputStream::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
   // For 'write on finalize', we set up listener for 'LastInputFile'
   // and perform write at this point. This happens at 'stop' of the
   // event selector. RDS 04/2010
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get the IncidentSvc");
      return StatusCode::FAILURE;
   }
   incSvc->addListener(this, "MetaDataStop", 50);
   for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
	   iter != m_helperTools.end(); iter++) {
      if (!(*iter)->postInitialize().isSuccess()) {
          ATH_MSG_ERROR("Cannot initialize helper tool");
      }
   }
   return StatusCode::SUCCESS;
}
StatusCode AthenaOutputStream::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   for (std::vector<ToolHandle<IAthenaOutputTool> >::const_iterator iter = m_helperTools.begin();
	   iter != m_helperTools.end(); iter++) {
      if (!(*iter)->preFinalize().isSuccess()) {
          ATH_MSG_ERROR("Cannot finalize helper tool");
      }
   }
   const Incident metaDataStopIncident(name(), "MetaDataStop");
   this->handle(metaDataStopIncident);
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get the IncidentSvc");
      return StatusCode::FAILURE;
   }
   incSvc->removeListener(this, "MetaDataStop");
   if (!m_streamer->finalizeOutput().isSuccess()) {
      ATH_MSG_FATAL("Cannot finalize Output file");
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}
