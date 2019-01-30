/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaOutputStream.h"

// Framework include files
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/FileIncident.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "AthenaKernel/IItemListSvc.h"
#include "CxxUtils/make_unique.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/SGIFolder.h"
#include "SGTools/CLIDRegistry.h"

#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "OutputStreamSequencerSvc.h"
#include "MetaDataSvc.h"

#include <boost/tokenizer.hpp>
#include <cassert>
#include <string>
#include <vector>

using std::string;
using std::vector;
using boost::tokenizer;
using boost::char_separator;


//****************************************************************************


namespace {
  /**
   * @brief Concrete DataBucket class for writing out an object
   *        as a base class.
   *
   * Normally, when an object is selected for writing via ItemList,
   * the object is written as its dynamic type (the type by which it was
   * originally recorded in Storegate) rather than as the type written
   * in the ItemList.  This is because the selection works by forming
   * a list of @DataObject instances; once this list is formed, the
   * types used to select the list are no longer used.
   *
   * However, in some cases, it is useful to be able to write an object
   * as one of its base classes; for example, to write an auxiliary store
   * object as xAOD::AuxContainerBase, in order to get all variables saved
   * as dynamic variables.  This can be requested by adding a ! after
   * the type name in the ItemList, which sets the `exact' flag
   * in the SG::FolderItem.  To make this work, then, when we get
   * an item with the exact flag set, we need to construct a new DataObject
   * instance that holds the object as the requested type.  That is the
   * purpose of this class.
   */
  class AltDataBucket
    : public DataBucketBase
  {
  public:
    AltDataBucket (void* ptr, CLID clid, const std::type_info& tinfo,
                   const SG::DataProxy& proxy)
      : m_proxy(this, makeTransientAddress(clid, proxy).release()),
        m_ptr (ptr), m_clid (clid), m_tinfo (tinfo)
    {
      addRef();
    }

    virtual const CLID& clID() const override { return m_clid; }
    virtual void* object() override { return m_ptr; }
    virtual const std::type_info& tinfo() const override { return m_tinfo; }
    virtual void* cast (CLID /*clid*/,
                        SG::IRegisterTransient* /*irt*/ = nullptr,
                        bool /*isConst*/ = true) override
    { std::abort(); }
    virtual void* cast (const std::type_info& tinfo,
                        SG::IRegisterTransient* /*irt*/ = nullptr,
                        bool /*isConst*/ = true) override
    { if (tinfo == m_tinfo)
        return m_ptr;
      return nullptr;
    }
    virtual void relinquish() override {}
    virtual void lock() override {}

    
  private:
    static
    std::unique_ptr<SG::TransientAddress>
    makeTransientAddress (CLID clid, const SG::DataProxy& oldProxy);

    SG::DataProxy m_proxy;
    void* m_ptr;
    CLID  m_clid;
    const std::type_info& m_tinfo;
  };


  std::unique_ptr<SG::TransientAddress>
  AltDataBucket::makeTransientAddress (CLID clid, const SG::DataProxy& oldProxy)
  {
    auto newTad = std::make_unique<SG::TransientAddress>
      (clid, oldProxy.name());
    newTad->setAlias (oldProxy.alias());
    for (CLID tclid : oldProxy.transientID()) {
      // Note: this will include derived CLIDs.
      // Strictly speaking, that's not right; however, filtering them
      // out can break ElementLinks (for example those used by
      // ShallowAuxContainer).  One will not actually be able to get
      // a pointer of the derived type, as the conversions in StorableConversion
      // only support derived->base, not the other way around.
      newTad->setTransientID (tclid);
    }
    return newTad;
  }


} // anonymous namespace


//****************************************************************************


// Standard Constructor
AthenaOutputStream::AthenaOutputStream(const string& name, ISvcLocator* pSvcLocator)
        : FilteredAlgorithm(name, pSvcLocator),
        m_dataStore("StoreGateSvc", name),
        m_metadataStore("MetaDataStore", name),
        m_currentStore(&m_dataStore),
        m_itemSvc("ItemListSvc", name),
	m_outputAttributes(),
        m_pCLIDSvc("ClassIDSvc", name),
        m_outSeqSvc("OutputStreamSequencerSvc", name),
        m_p2BWritten(string("SG::Folder/") + name + string("_TopFolder"), this),
        m_decoder(string("SG::Folder/") + name + string("_excluded"), this),
        m_transient(string("SG::Folder/") + name + string("_transient"), this),
        m_events(0),
        m_streamer(string("AthenaOutputStreamTool/") + name + string("Tool"), this),
   m_helperTools(this) {
   assert(pSvcLocator);
   declareProperty("ItemList",               m_itemList);
   declareProperty("MetadataItemList",       m_metadataItemList);
   declareProperty("TransientItems",         m_transientItems);
   declareProperty("OutputFile",             m_outputName="DidNotNameOutput.root");
   declareProperty("EvtConversionSvc",       m_persName="EventPersistencySvc");
   declareProperty("WritingTool",            m_streamer);
   declareProperty("Store",                  m_dataStore);
   declareProperty("MetadataStore",          m_metadataStore);
   declareProperty("ForceRead",              m_forceRead=false);
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
   ATH_MSG_INFO("Found " << m_helperTools << endmsg << "Data output: " << m_outputName);

   for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
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

   // Add an explicit input dependency for everything in our item list
   // that we know from the configuration is in the transient store.
   // We don't want to add everything on the list, because configurations
   // often initialize this with a maximal static list of everything
   // that could possibly be written.
   {
     ATH_CHECK( m_transient.retrieve() );
     IProperty *pAsIProp = dynamic_cast<IProperty*> (&*m_transient);
     if (!pAsIProp) {
       ATH_MSG_FATAL ("Bad folder interface");
       return StatusCode::FAILURE;
     }
     ATH_CHECK (pAsIProp->setProperty("ItemList", m_transientItems.toString()));

     for (const SG::FolderItem& item : *m_p2BWritten) {
       const std::string& k = item.key();
       if (k.find('*') != std::string::npos) continue;
       if (k.find('.') != std::string::npos) continue;
       for (const SG::FolderItem& titem : *m_transient) {
         if (titem.id() == item.id() && titem.key() == k) {
           DataObjID id (item.id(), m_dataStore.name() + "+" + k);
           this->addDependency (id, Gaudi::DataHandle::Reader);
           break;
         }
       }
     }
   }

   ATH_MSG_DEBUG("End initialize");
   if (baseStatus == StatusCode::FAILURE) return StatusCode::FAILURE;
   return(status);
}

StatusCode AthenaOutputStream::stop()
{
   ATH_MSG_DEBUG("AthenaOutputStream " << this->name() << " ::stop()");
/*
   for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
        iter != m_helperTools.end(); iter++) {
      if (!(*iter)->preFinalize().isSuccess()) {
          ATH_MSG_ERROR("Cannot finalize helper tool");
      }
   }
   ServiceHandle<MetaDataSvc> mdsvc("MetaDataSvc", name());
   if (mdsvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not retrieve MetaDataSvc for stop actions");
   }
   else {
      ATH_CHECK(mdsvc->prepareOutput());
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
      m_outputAttributes = "[OutputCollection=MetaDataHdr][PoolContainerPrefix=MetaData][AttributeListKey=][DataHeaderSatellites=]";
      m_p2BWritten->clear();
      IProperty *pAsIProp(nullptr);
      if ((m_p2BWritten.retrieve()).isFailure() ||
                     nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
                     (pAsIProp->setProperty("ItemList", m_metadataItemList.toString())).isFailure()) {
         throw GaudiException("Folder property [metadataItemList] not found", name(), StatusCode::FAILURE);
      }
      if (write().isFailure()) {  // true mean write AND commit
         ATH_MSG_ERROR("Cannot write metadata");
      }
      m_outputAttributes.clear();
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
   }
*/
   return StatusCode::SUCCESS;
}

void AthenaOutputStream::handle(const Incident& inc) {
   ATH_MSG_DEBUG("handle() incident type: " << inc.type());
   if (inc.type() == "MetaDataStop") {
      // Moved preFinalize of helper tools to stop - want to optimize the
      // output file in finalize RDS 12/2009
      for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
           iter != m_helperTools.end(); iter++) {
         if (!(*iter)->preFinalize().isSuccess()) {
            ATH_MSG_ERROR("Cannot finalize helper tool");
         }
      }
      // Make sure the metadata tools finalize their output
      ServiceHandle<MetaDataSvc> mdsvc("MetaDataSvc", name());
      if (mdsvc.retrieve().isFailure()) {
         ATH_MSG_ERROR("Could not retrieve MetaDataSvc for stop actions");
      }
      else {
         if (mdsvc->prepareOutput().isFailure()) {
            ATH_MSG_ERROR("Failed on MetaDataSvc prepareOutput");
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
         m_outputAttributes = "[OutputCollection=MetaDataHdr][PoolContainerPrefix=MetaData][AttributeListKey=][DataHeaderSatellites=]";
         m_p2BWritten->clear();
         IProperty *pAsIProp(nullptr);
         if ((m_p2BWritten.retrieve()).isFailure() ||
            nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
            (pAsIProp->setProperty("ItemList", m_metadataItemList.toString())).isFailure()) {
            throw GaudiException("Folder property [metadataItemList] not found", name(), StatusCode::FAILURE);
         }
         if (write().isFailure()) {  // true mean write AND commit
            ATH_MSG_ERROR("Cannot write metadata");
         }
         m_outputAttributes.clear();
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
      }
   } else if (inc.type() == "UpdateOutputFile") {
     const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
     if(fileInc!=nullptr) {
       if(m_outputName != fileInc->fileName()) {
	 m_outputName = fileInc->fileName();
	 ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
	 if(iomgr.retrieve().isFailure()) {
	   ATH_MSG_FATAL("Cannot retrieve IoComponentMgr from within the incident handler");
	   return;
	 }
	 if(iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, m_outputName).isFailure()) {
	   ATH_MSG_FATAL("Cannot register new output name with IoComponentMgr");
	   return;
	 }
       } else {
	 ATH_MSG_DEBUG("New output file name received through the UpdateOutputFile incident is the same as the already defined output name. Nothing to do");
       }
     } else {
       ATH_MSG_FATAL("Cannot dyn-cast the UpdateOutputFile incident to FileIncident");
       return;
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
   m_objects.clear();
   m_objects.shrink_to_fit();
   m_ownedObjects.clear();
   m_altObjects.clear();
   return(StatusCode::SUCCESS);
}

StatusCode AthenaOutputStream::execute() {
   bool failed = false;
   for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
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
   for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
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
   if (m_streamer->connectOutput(m_outSeqSvc->buildSequenceFileName(m_outputName) + m_outputAttributes).isSuccess()) {
      // First check if there are any new items in the list
      collectAllObjects();
      // print out info about objects collected
      if (m_checkNumberOfWrites) {
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
                    "to identify which container is not always written");
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
      ATH_MSG_ERROR("Could not connectOutput");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

// Clear collected object list
void AthenaOutputStream::clearSelection()     {
   m_objects.erase(m_objects.begin(), m_objects.end());
   m_ownedObjects.clear();
   m_altObjects.clear();
}

void AthenaOutputStream::collectAllObjects() {
   if (m_itemListFromTool) {
      if (!m_streamer->getInputItemList(&*m_p2BWritten).isSuccess()) {
         ATH_MSG_WARNING("collectAllObjects() could not get ItemList from Tool.");
      }
   }
   m_p2BWritten->updateItemList(true);
   std::vector<CLID> folderclids;
   // Collect all objects that need to be persistified:
   //FIXME refactor: move this in folder. Treat as composite
   for (SG::IFolder::const_iterator i = m_p2BWritten->begin(), iEnd = m_p2BWritten->end(); i != iEnd; i++) {
      addItemObjects(*i);
      folderclids.push_back(i->id());
   }

   // FIXME This is a bruteforece hack to remove items erroneously 
   // added somewhere in the morass of the addItemObjects logic
   IDataSelector prunedList;
   for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
      if (std::find(folderclids.begin(),folderclids.end(),(*it)->clID())!=folderclids.end()) {
         prunedList.push_back(*it);  // build new list that is correct
      }
      else {
         ATH_MSG_DEBUG("Object " << (*it)->clID() <<","<< (*it)->name() << " found that was not in itemlist");
      }
   }
   m_objects.clear();  // clear previous list
   for (auto it = prunedList.begin(); it != prunedList.end(); ++it) {
      m_objects.push_back(*it);  // copy new into previous
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
      std::vector<std::string> keyTokens;
      keyTokens.reserve(2);
      std::vector<std::string> xkeyTokens;
      xkeyTokens.reserve(2);
      ATH_MSG_VERBOSE("Calling tokenizeAtStep( " << keyTokens << ", " << item_key << ", " << wildCard << ")" );
      this->tokenizeAtSep( keyTokens, item_key, wildCard );
      ATH_MSG_VERBOSE("Done calling tokenizeAtStep( " << keyTokens << ", " << item_key << ", " << wildCard << ")" );
      //std::pair<std::string, std::string> key = breakAtSep(item_key, wildCard);
      // Now loop over any found proxies
      for (; iter != end; ++iter) {
         SG::DataProxy* itemProxy(iter->second);
         // Does this key match the proxy key name - allow for wildcarding and aliases
         bool keyMatch = ( item_key == "*" ||
                           item_key == itemProxy->name() ||
                           itemProxy->hasAlias(item_key) );
         if (!keyMatch) {
            ATH_MSG_VERBOSE("Calling matchKey( " << keyTokens << ", " << itemProxy->name() << ")" );
            keyMatch = matchKey(keyTokens, itemProxy);
            ATH_MSG_VERBOSE("Done calling matchKey( " << keyTokens << ", " << itemProxy->name() << ") with result: " << keyMatch );
        }

         // Now undo the flag based on a similar analysis of excluded wildcard keys
         bool xkeyMatch = false;
         for (std::set<std::string>::const_iterator c2k_it = clidKeys.begin(), c2k_itEnd = clidKeys.end();
                 keyMatch && c2k_it != c2k_itEnd; ++c2k_it) {
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
                  this->tokenizeAtSep( xkeyTokens, *c2k_it, wildCard );
                  ATH_MSG_DEBUG("x Proxy name=" << itemProxy->name() );
                  xkeyMatch = matchKey(xkeyTokens, itemProxy);
               }
            }
         }
         // All right, it passes key match find in itemList, but not in excludeList
         if (keyMatch && !xkeyMatch) {
            if (m_forceRead && itemProxy->isValid()) {
               if (nullptr == itemProxy->accessData()) {
                  ATH_MSG_ERROR(" Could not get data object for id " << item.id() << ",\"" << itemProxy->name());
               }
            }
            if (nullptr != itemProxy->object()) {
               if( std::find(m_objects.begin(), m_objects.end(), itemProxy->object()) == m_objects.end() &&
                   std::find(m_altObjects.begin(), m_altObjects.end(), itemProxy->object()) == m_altObjects.end() )
               {
                 if (item.exact()) {
                   // If the exact flag is set, make a new DataObject
                   // holding the object as the requested type.
                   DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (itemProxy->object());
                   if (!dbb) std::abort();
                   void* ptr = dbb->cast (item.id());
                   if (!ptr) {
                     // Hard cast
                     ptr = dbb->object();
                   }
                   auto altbucket =
                     CxxUtils::make_unique<AltDataBucket>
                       (ptr, item.id(),
                        *CLIDRegistry::CLIDToTypeinfo (item.id()),
                        *itemProxy);
                   m_objects.push_back(altbucket.get());
                   m_ownedObjects.push_back (std::move(altbucket));
                   m_altObjects.push_back (itemProxy->object());
                 }
                 else
                   m_objects.push_back(itemProxy->object());
                 ATH_MSG_DEBUG(" Added object " << item.id() << ",\"" << itemProxy->name() << "\"");
               }

               // Build ItemListSvc string
               std::string tn;
               std::stringstream tns;
               if (!m_pCLIDSvc->getTypeNameOfID(item.id(), tn).isSuccess()) {
                  ATH_MSG_ERROR(" Could not get type name for id "
                         << item.id() << ",\"" << itemProxy->name());
                  tns << item.id() << '_' << itemProxy->name();
               } else {
                  tn += '_' + itemProxy->name();
                  tns << tn;
               }

               // Make the decision whether to try to call
               // SG::IAuxStoreIO::selectAux(...) based on the SG key of the
               // object. Because even if aux_attr is empty, we may need to
               // reset an auxiliary store back to not being slimmed, after
               // a previous stream slimmed it.
               if (item_key.find( "Aux." ) == ( item_key.size() - 4 )) {
                  SG::IAuxStoreIO* auxio(nullptr);
                  try {
                     SG::fromStorable(itemProxy->object(), auxio, true);
                  }catch( const std::exception& ) {
                     // exception from Control/StoreGateBindings/src/SgPyDataModel.cxx:71
                     ATH_MSG_DEBUG( "Error in casting object with CLID "
                                    << itemProxy->clID() << " to SG::IAuxStoreIO*" );
                     auxio = nullptr;
                  }
                  if( auxio ) {
                     // collect dynamic Aux selection (parse the line, attributes separated by dot)
                     std::set<std::string> attributes;
                     // Start by resetting the object. This is needed in case a
                     // previous stream set some selection on it that we don't
                     // need here.
                     auxio->selectAux( attributes );
                     if( aux_attr.size() ) {
                        std::stringstream ss(aux_attr);
                        std::string attr;
                        while( std::getline(ss, attr, '.') ) {
                           attributes.insert(attr);
                           std::stringstream temp;
                           temp << tns.str() << attr;
                           if (m_itemSvc->addStreamItem(this->name(),temp.str()).isFailure()) {
                              ATH_MSG_WARNING("Unable to record item " << temp.str() << " in Svc");
                           }
                        }
                        // don't let keys with wildcard overwrite existing selections
                        if( auxio->getSelectedAuxIDs().size() == auxio->getDynamicAuxIDs().size()
                            || item_key.find('*') == string::npos )
                           auxio->selectAux(attributes);
                     }
                  }
               }

               added = true;
               if (m_checkNumberOfWrites) {
                  CounterMapType::iterator cit = m_objectWriteCounter.find(tn);
                  if (cit == m_objectWriteCounter.end()) {
                    // First time through
                    m_objectWriteCounter.insert(CounterMapType::value_type(tn, 1));
                  } else {
                     // set to next iteration (to avoid double counting)
                     // StreamTools will eliminate duplicates.
                     (*cit).second = m_events + 1;
                  }
               }
               if (m_itemSvc->addStreamItem(this->name(),tns.str()).isFailure()) {
                  ATH_MSG_WARNING("Unable to record item " << tns.str() << " in Svc");
               }
            }
         } else if (keyMatch && xkeyMatch) {
            removed = true;
         }
      } // proxy loop
      if (!added && !removed) {
         ATH_MSG_DEBUG(" No object matching " << item.id() << ",\"" << item_key  << "\" found");
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
   IProperty *pAsIProp(nullptr);
   if ((m_p2BWritten.retrieve()).isFailure() ||
           nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
           (pAsIProp->setProperty(m_itemList)).isFailure()) {
      throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
   }
}

void AthenaOutputStream::excludeListHandler(Property& /* theProp */) {
   IProperty *pAsIProp(nullptr);
   if ((m_decoder.retrieve()).isFailure() ||
           nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_decoder)) ||
           (pAsIProp->setProperty("ItemList", m_excludeList.toString())).isFailure()) {
      throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
   }
}


void AthenaOutputStream::tokenizeAtSep( std::vector<std::string>& subStrings,
                                        const std::string& portia,
                                        const std::string& sepstr ) const {
  subStrings.clear(); // clear from previous iteration step
  // If the portia starts with a wildcard, add an empty string
  if ( portia.find(sepstr) == 0 ) {
    subStrings.push_back("");
  }
  boost::char_separator<char> csep(sepstr.c_str());
  boost::tokenizer<char_separator<char>> tokens(portia, csep);
  for (const std::string& t : tokens) {
    //ATH_MSG_VERBOSE("Now on token: " << t);
    subStrings.push_back(t);
  }
  // If the portia ends with a wildcard, add an empty string
  if ( portia.size() >= sepstr.size() &&
       portia.compare( portia.size() - sepstr.size(), sepstr.size(), sepstr) == 0 ) {
    subStrings.push_back("");
  }
  return;
}

bool AthenaOutputStream::matchKey(const std::vector<std::string>& key,
                                  const SG::DataProxy* proxy) const {
  bool keyMatch = true; // default return

  // Get an iterator to the first (not zeroth!) string in the vector
  std::vector<std::string>::const_iterator itrEnd = key.cend();
  std::vector<std::string>::const_iterator itr = key.cbegin();

  // Walk through the whole proxyName string and try to match to all sub-keys
  // We are using that: std::string::npos!=string.find("") is always true
  const std::string& proxyName = proxy->name();
  std::string::size_type proxyNamePos=0;
  while ( itr != itrEnd &&
          std::string::npos != ( proxyNamePos = proxyName.find(*itr, proxyNamePos) )
          ) {
    // If we are at the begin iterator and the first element is Not an empty string
    ATH_MSG_VERBOSE("If we are at the begin iterator and the first element is Not an empty string");
    if ( !(key.front().empty()) && itr == key.cbegin() && proxyNamePos != 0 ) {
      // We had to match a precise name at the beginning, but didn't find it at the beginning
      ATH_MSG_VERBOSE("We had to match a precise name at the beginning, but didn't find it at the beginning");
      break;
    }
    // If we are at the end iterator and the last element is Not an empty string
    if ( !(key.back().empty()) && itr == --(key.cend()) && (proxyNamePos+itr->size()!=proxyName.size()) ) {
      // We had to match a precise name at the end, but didn't find it at the end
      ATH_MSG_VERBOSE("We had to match a precise name at the end, but didn't find it at the end");
      break;
    }
    ATH_MSG_VERBOSE("Found a match of subkey: " << *itr << " in string: " << proxyName
                    << " at position: " << proxyNamePos );
    // If we have a good match, increment the iterator and the search position
    proxyNamePos += itr->size();
    ++itr;
  }
  // Didn't find everything
  if ( itr != itrEnd ) {
    keyMatch = false;
    ATH_MSG_VERBOSE("Couldn't match every sub-string... return: " << keyMatch);
  }
  else { ATH_MSG_VERBOSE("Did match every sub-string... return: " << keyMatch); }

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
   incSvc->addListener(this, "UpdateOutputFile", 50);
   for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
       iter != m_helperTools.end(); iter++) {
      if (!(*iter)->postInitialize().isSuccess()) {
          ATH_MSG_ERROR("Cannot initialize helper tool");
      }
   }
   return StatusCode::SUCCESS;
}


StatusCode AthenaOutputStream::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator iter = m_helperTools.begin();
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
   return StatusCode::SUCCESS;
}
