/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/AlgTool.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "AthenaKernel/IItemListSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandle.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/SGIFolder.h"
#include "AthenaKernel/CLIDRegistry.h"
#include "xAODCore/AuxSelection.h"
#include "xAODCore/AuxCompression.h"

#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "OutputStreamSequencerSvc.h"
#include "MetaDataSvc.h"
#include "SelectionVetoes.h"

#include <boost/tokenizer.hpp>
#include <cassert>
#include <mutex>
#include <string>
#include <vector>
#include <sstream>

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
    // Extra constructor to create a temporary proxy without aliases, for objects in MetaContainers
    AltDataBucket(void* ptr, CLID clid, const std::type_info& tinfo, const std::string name) :
       m_proxy(this, new SG::TransientAddress(clid, name) ),
       m_ptr(ptr), m_clid(clid), m_tinfo(tinfo)
    {
      addRef();
    }

    virtual const CLID& clID() const override { return m_clid; }
    virtual void* object() override { return m_ptr; }
    virtual const std::type_info& tinfo() const override { return m_tinfo; }
    using DataBucketBase::cast;
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
	m_metaDataSvc("MetaDataSvc", name),
	m_outputAttributes(),
        m_pCLIDSvc("ClassIDSvc", name),
        m_outSeqSvc("OutputStreamSequencerSvc", name),
        m_p2BWritten(string("SG::Folder/") + name + string("_TopFolder"), this),
        m_decoder(string("SG::Folder/") + name + string("_excluded"), this),
        m_compressionDecoderHigh(string("SG::Folder/") + name + string("_compressed_high"), this),
        m_compressionDecoderLow(string("SG::Folder/") + name + string("_compressed_low"), this),
        m_transient(string("SG::Folder/") + name + string("_transient"), this),
        m_events(0),
        m_streamer(string("AthenaOutputStreamTool/") + name + string("Tool"), this),
        m_helperTools(this)
{
   assert(pSvcLocator);
   declareProperty("MetadataItemList",       m_metadataItemList);
   declareProperty("TransientItems",         m_transientItems);
   declareProperty("OutputFile",             m_outputName="DidNotNameOutput.root");
   declareProperty("EvtConversionSvc",       m_persName="EventPersistencySvc");
   declareProperty("WritingTool",            m_streamer);
   declareProperty("Store",                  m_dataStore);
   declareProperty("MetadataStore",          m_metadataStore);
   declareProperty("ForceRead",              m_forceRead=true);
   declareProperty("ExtendProvenanceRecord", m_extendProvenanceRecord=true);
   declareProperty("WriteOnExecute",         m_writeOnExecute=true);
   declareProperty("WriteOnFinalize",        m_writeOnFinalize=false);
   declareProperty("TakeItemsFromInput",     m_itemListFromTool=false);
   declareProperty("CheckNumberOfWrites",    m_checkNumberOfWrites=true);
   declareProperty("ExcludeList",            m_excludeList);
   declareProperty("HelperTools",            m_helperTools);
   declareProperty("CompressionListHigh",    m_compressionListHigh);
   declareProperty("CompressionListLow",     m_compressionListLow);
   declareProperty("CompressionBitsHigh",    m_compressionBitsHigh = 7);
   declareProperty("CompressionBitsLow",     m_compressionBitsLow = 15);
   
   // Associate action handlers with the AcceptAlgs,
   // RequireAlgs & VetoAlgs properties
   m_itemList.declareUpdateHandler(&AthenaOutputStream::itemListHandler, this);
   m_excludeList.declareUpdateHandler(&AthenaOutputStream::excludeListHandler, this);
   m_compressionListHigh.declareUpdateHandler(&AthenaOutputStream::compressionListHandlerHigh, this);
   m_compressionListLow.declareUpdateHandler(&AthenaOutputStream::compressionListHandlerLow, this);
}

// Standard Destructor
AthenaOutputStream::~AthenaOutputStream() {
   m_streamerMap.clear();
}

// initialize data writer
StatusCode AthenaOutputStream::initialize() {
   ATH_CHECK( this->FilteredAlgorithm::initialize() );
   ATH_MSG_DEBUG("In initialize");
   // Reset the number of events written
   m_events = 0;

   // set up the SG service:
   ATH_CHECK( m_dataStore.retrieve() );
   ATH_MSG_DEBUG("Found " << m_dataStore.typeAndName() << " store.");
   assert(static_cast<bool>(m_dataStore));
   if (!m_metadataItemList.value().empty()) {
      ATH_CHECK( m_metadataStore.retrieve() );
      ATH_MSG_DEBUG("Found " << m_metadataStore.typeAndName() << " store.");
      assert(static_cast<bool>(m_metadataStore));
   }

   // set up the CLID service:
   ATH_CHECK( m_pCLIDSvc.retrieve() );

   // set up the ItemListSvc service:
   assert(static_cast<bool>(m_pCLIDSvc));
   ATH_CHECK( m_itemSvc.retrieve() );
   assert(static_cast<bool>(m_itemSvc));

   // set up the OutputStreamSequencer service:
   ATH_CHECK( m_outSeqSvc.retrieve() );
   assert(static_cast<bool>(m_outSeqSvc));

   // Get Output Stream tool for writing
   ATH_CHECK( m_streamer.retrieve() );
   ATH_CHECK( m_streamer->connectServices(m_dataStore.typeAndName(), m_persName, m_extendProvenanceRecord) );

   ATH_CHECK( m_helperTools.retrieve() );
   ATH_MSG_INFO("Found " << m_helperTools << endmsg << "Data output: " << m_outputName);

   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
     ATH_CHECK( tool->postInitialize() );
   }

   // Register this algorithm for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   ATH_CHECK( iomgr.retrieve() );
   ATH_CHECK( iomgr->io_register(this) );
   ATH_CHECK( iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, m_outputName) );
   ATH_CHECK( this->io_reinit() );

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

   // listen to event range incidents if incident name is configured
   if( !m_outSeqSvc->incidentName().empty() ) {
      ServiceHandle<IIncidentSvc> incsvc("IncidentSvc", this->name());
      if (!incsvc.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get IncidentSvc.");
         return(StatusCode::FAILURE);
      }
      incsvc->addListener(this, IncidentType::BeginProcessing, 100);
      incsvc->addListener(this, IncidentType::EndProcessing, 100);
   }

   // Check compression settings and print some information about the configuration
   // Both should be between [5, 23] and high compression should be < low compression
   if(m_compressionBitsHigh < 5 || m_compressionBitsHigh > 23) {
     ATH_MSG_INFO("Float compression mantissa bits for high compression " <<
                  "(" << m_compressionBitsHigh << ") is outside the allowed range of [5, 23].");
     ATH_MSG_INFO("Setting it to the appropriate limit.");
     m_compressionBitsHigh = m_compressionBitsHigh < 5 ? 5 : 23;
   }
   if(m_compressionBitsLow < 5 || m_compressionBitsLow > 23) {
     ATH_MSG_INFO("Float compression mantissa bits for low compression " <<
                  "(" << m_compressionBitsLow << ") is outside the allowed range of [5, 23].");
     ATH_MSG_INFO("Setting it to the appropriate limit.");
     m_compressionBitsLow = m_compressionBitsLow < 5 ? 5 : 23;
   }
   if(m_compressionBitsLow <= m_compressionBitsHigh) {
     ATH_MSG_ERROR("Float compression mantissa bits for low compression " <<
                   "(" << m_compressionBitsLow << ") is lower than or equal to high compression " <<
                   "(" << m_compressionBitsHigh << ")! Please check the configuration! ");
     return StatusCode::FAILURE;
   }
   if(m_compressionListHigh.value().empty() && m_compressionListLow.value().empty()) {
     ATH_MSG_VERBOSE("Both high and low float compression lists are empty. Float compression will NOT be applied.");
   } else {
     ATH_MSG_INFO("Either high or low (or both) float compression lists are defined. Float compression will be applied.");
     ATH_MSG_INFO("High compression will use " << m_compressionBitsHigh << " mantissa bits, and " <<
                  "low compression will use " << m_compressionBitsLow << " mantissa bits.");
   }

   /// Set SG key for selected variable information.
   // For CreateOutputStream.py, the algorithm name is the same as the stream
   // name.  But OutputStreamConfig.py adds `OutputStream' to the front.
   std::string streamName = this->name();
   if (streamName.substr (0, 12) == "OutputStream") {
     streamName.erase (0, 12);
   }
   m_selVetoesKey = "SelectionVetoes_" + streamName;
   ATH_CHECK( m_selVetoesKey.initialize() );

   m_compInfoKey = "CompressionInfo_" + streamName;
   ATH_CHECK( m_compInfoKey.initialize() );

   ATH_MSG_DEBUG("End initialize");
   return StatusCode::SUCCESS;
}

StatusCode AthenaOutputStream::stop()
{
   ATH_MSG_DEBUG("AthenaOutputStream " << this->name() << " ::stop()");
   return StatusCode::SUCCESS;
}


void AthenaOutputStream::handle(const Incident& inc)
{
   EventContext::ContextID_t slot = inc.context().slot();
   ATH_MSG_DEBUG("slot " << slot << "  handle() incident type: " << inc.type());
   // mutex shared with write() which is called from writeMetaData
   std::unique_lock<mutex_t>  lock(m_mutex);

   // Handle Event Ranges for Event Service
   if( m_outSeqSvc->inUse() and m_outSeqSvc->inConcurrentEventsMode() )
   {
      if( inc.type() == "MetaDataStop" )  {
         // all substreams should be closed by this point
         ATH_MSG_DEBUG("Ignoring MetaDataStop incident in ES mode");
         return;
      }
      if( slot == EventContext::INVALID_CONTEXT_ID ) {
         throw GaudiException("Received Incident with invalid slot in ES mode", name(), StatusCode::FAILURE);
      }
      if( inc.type() == IncidentType::BeginProcessing ) {
         // remember in which output filename this event should be stored
         m_slotRangeMap[ slot ] = m_outSeqSvc->buildSequenceFileName(m_outputName);
         ATH_MSG_DEBUG("slot " << slot << " assigned filename: " << m_slotRangeMap[ slot ] );
      } else if( inc.type() == IncidentType::EndProcessing ) {
         std::string rangeFN = m_slotRangeMap[ slot ];
         if( !rangeFN.empty() ) {
            // check how many events there are still for the given range
            int n = 0;
            for( auto& elem : m_slotRangeMap ) {
               if( elem.second == rangeFN ) n++;
            }
            if( n == 1 ) {
               // this was the last event in this range, finalize it
               ATH_MSG_DEBUG("slot " << slot << " writing MetaData to " << rangeFN);
               // MN: not calling StopMetaDataIncident here - OK for Sim, check others
               // metadata tools like CutFlowSvc are not able to handle this yet
               writeMetaData( rangeFN );
               ATH_MSG_INFO("Finished writing Event Sequence to " << rangeFN );
               auto strm_iter = m_streamerMap.find( rangeFN );
               strm_iter->second->finalizeOutput().ignore();
               strm_iter->second->finalize().ignore();
               m_streamerMap.erase(strm_iter);
	       m_outSeqSvc->publishRangeReport(rangeFN);            
            }
            m_slotRangeMap[ slot ].clear();
         } else {
            throw GaudiException("Failed to handle EndProcessing incident - range filename not found",
                                 name(), StatusCode::FAILURE);
         }
      }
   } else {
      // not in Event Service
      if( inc.type() == "MetaDataStop" )  {
         writeMetaData();
      }
   }
   
   ATH_MSG_DEBUG("Leaving incident handler for " << inc.type());
}

// method to write MetaData for this stream
// in ES mode the range substream is determined by the current Event slot
// called from the incident handler - returns void and throws GaudiExceptions on errors
void AthenaOutputStream::writeMetaData(const std::string outputFN)
{
   // use main stream tool by default, or per outputFile in ES mode
   IAthenaOutputStreamTool* streamer = outputFN.empty()? &*m_streamer : m_streamerMap[outputFN].get();

   // Moved preFinalize of helper tools to stop - want to optimize the
   // output file in finalize RDS 12/2009
   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
      if (!tool->preFinalize().isSuccess()) {
         throw GaudiException("Cannot finalize helper tool", name(), StatusCode::FAILURE);
      }
   }
   if( m_metaDataSvc->prepareOutput(outputFN).isFailure() ) {
      throw GaudiException("Failed on MetaDataSvc prepareOutput", name(), StatusCode::FAILURE);
   }
   // Always force a final commit in stop - mainly applies to AthenaPool
   if (m_writeOnFinalize) {
      if (write().isFailure()) {  // true mean write AND commit
         throw GaudiException("Cannot write on finalize", name(), StatusCode::FAILURE);
      }
      ATH_MSG_INFO("Records written: " << m_events);
   }
   ATH_MSG_DEBUG("metadataItemList: " << m_metadataItemList.value() );
   if (!m_metadataItemList.value().empty()) {
      m_currentStore = &m_metadataStore;
      StatusCode status = streamer->connectServices(m_metadataStore.typeAndName(), m_persName, false);
      if (status.isFailure()) {
         throw GaudiException("Unable to connect metadata services", name(), StatusCode::FAILURE);
      }
      m_checkNumberOfWrites = false;
      m_outputAttributes = "[OutputCollection=MetaDataHdr][PoolContainerPrefix=MetaData][AttributeListKey=]";
      m_p2BWritten->clear();
      IProperty *pAsIProp(nullptr);
      if ((m_p2BWritten.retrieve()).isFailure() ||
          nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
          (pAsIProp->setProperty("ItemList", m_metadataItemList.toString())).isFailure()) {
         throw GaudiException("Folder property [metadataItemList] not found", name(), StatusCode::FAILURE);
      }
      if (write().isFailure()) {  // true mean write AND commit
         throw GaudiException("Cannot write metadata", name(), StatusCode::FAILURE);
      }
      m_outputAttributes.clear();
      m_currentStore = &m_dataStore;
      status = streamer->connectServices(m_dataStore.typeAndName(), m_persName, m_extendProvenanceRecord);
      if (status.isFailure()) {
         throw GaudiException("Unable to re-connect services", name(), StatusCode::FAILURE);
      }
      m_p2BWritten->clear();
      if ((pAsIProp->setProperty(m_itemList)).isFailure()) {
         throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
      }
      ATH_MSG_INFO("Metadata records written: " << m_events);
   }
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
   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
      if (!tool->preExecute().isSuccess()) {
         failed = true;
      }
   }
   // Test whether this event should be output
   if (m_writeOnExecute && isEventAccepted()) {
      if (write().isFailure()) {
         failed = true;
      }
   }
   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
      if(!tool->postExecute().isSuccess()) {
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
   EventContext::ContextID_t slot = Gaudi::Hive::currentContext().slot();
   IAthenaOutputStreamTool* streamer = &*m_streamer;
   std::string outputFN;

   std::unique_lock<mutex_t>  lock(m_mutex);

   // Handle Event Ranges
   if( m_outSeqSvc->inUse() and m_outSeqSvc->inConcurrentEventsMode() ) {
      outputFN = m_slotRangeMap[ slot ];
      ATH_MSG_DEBUG( "Writing event sequence to " << outputFN );
      
      streamer = m_streamerMap[ outputFN ].get();
      if( !streamer ) {
         // new range, needs a new streamer tool
         IAlgTool* st = AlgTool::Factory::create( m_streamer->type(), m_streamer->type(), m_streamer->name(), this ).release();
         st->addRef();
         streamer = dynamic_cast<IAthenaOutputStreamTool*>( st );
         if( streamer->initialize().isFailure()
             || streamer->connectServices(m_dataStore.typeAndName(), m_persName, m_extendProvenanceRecord).isFailure() ) {
            ATH_MSG_FATAL("Unable to initialize OutputStreamTool for " << outputFN );
            return StatusCode::FAILURE;
         }
         m_streamerMap[ outputFN ].reset( streamer );
      }
   } else {
      outputFN = m_outSeqSvc->buildSequenceFileName(m_outputName);
   }
   
   // Clear any previously existing item list
   clearSelection();
   ATH_CHECK( collectAllObjects() );

   // keep a local copy of the object lists so they are not overwritten when we release the lock
   IDataSelector objects = std::move( m_objects );
   IDataSelector altObjects = std::move( m_altObjects );
   std::vector<std::unique_ptr<DataObject> > ownedObjects = std::move( m_ownedObjects );
   
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
   // prepare before releasing lock because m_outputAttributes change in metadataStop
   const std::string connectStr = outputFN + m_outputAttributes;

   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
     ATH_CHECK( tool->preStream() );
   }

   // MN: would be nice to release the Stream lock here
   // lock.unlock();

   // Connect the output file to the service
   if (!streamer->connectOutput(connectStr).isSuccess()) {
      ATH_MSG_FATAL("Could not connectOutput");
      return StatusCode::FAILURE;
   }
   ATH_MSG_DEBUG("connectOutput done for " + outputFN);
   StatusCode currentStatus = streamer->streamObjects(objects, connectStr);
   // Do final check of streaming
   if (!currentStatus.isSuccess()) {
      if (!currentStatus.isRecoverable()) {
         ATH_MSG_FATAL("streamObjects failed.");
         failed = true;
      } else {
         ATH_MSG_DEBUG("streamObjects failed.");
      }
   }
   bool doCommit = false;
   if (m_events % m_autoSend.value() == 0 && outputFN.find("?pmerge=") != std::string::npos) {
      doCommit = true;
      ATH_MSG_DEBUG("commitOutput sending data.");
   }
   if (!streamer->commitOutput(doCommit).isSuccess()) {
      ATH_MSG_FATAL("commitOutput failed.");
      failed = true;
   }
   if (failed) {
      return(StatusCode::FAILURE);
   }
   m_events++;
   return(StatusCode::SUCCESS);
}

// Clear collected object list
void AthenaOutputStream::clearSelection()     {
   m_objects.erase(m_objects.begin(), m_objects.end());
   m_ownedObjects.clear();
   m_altObjects.clear();
}

StatusCode AthenaOutputStream::collectAllObjects() {
   if (m_itemListFromTool) {
      if (!m_streamer->getInputItemList(&*m_p2BWritten).isSuccess()) {
         ATH_MSG_WARNING("collectAllObjects() could not get ItemList from Tool.");
      }
   }

   auto vetoes = std::make_unique<SG::SelectionVetoes>();
   auto compInfo = std::make_unique<SG::CompressionInfo>();

   m_p2BWritten->updateItemList(true);
   std::vector<CLID> folderclids;
   // Collect all objects that need to be persistified:
   //FIXME refactor: move this in folder. Treat as composite
   for (SG::IFolder::const_iterator i = m_p2BWritten->begin(), iEnd = m_p2BWritten->end(); i != iEnd; i++) {
     addItemObjects(*i, *vetoes, *compInfo);
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
      if ((*it)->name().length() > 4 && (*it)->name().substr((*it)->name().length() - 4) == "Aux.") {
         m_objects.push_back(*it);  // first copy aux store new into previous
      }
   }
   for (auto it = prunedList.begin(); it != prunedList.end(); ++it) {
      if ((*it)->name().length() <= 4 || (*it)->name().substr((*it)->name().length() - 4) != "Aux.") {
         m_objects.push_back(*it);  // then copy others new into previous
      }
   }

   // If there were any variable selections, record the information in SG.
   if (!vetoes->empty()) {
     ATH_CHECK( SG::makeHandle (m_selVetoesKey).record (std::move (vetoes)) );
   }

   // Store the lossy float compression information in the SG.
   if (!compInfo->empty()) {
     ATH_CHECK( SG::makeHandle (m_compInfoKey).record (std::move (compInfo)) );
   }

   return StatusCode::SUCCESS;
}

//FIXME refactor: move this in folder. Treat as composite
void AthenaOutputStream::addItemObjects(const SG::FolderItem& item,
                                        SG::SelectionVetoes& vetoes,
                                        SG::CompressionInfo& compInfo)
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
   CLID item_id = item.id();
   ATH_MSG_DEBUG("addItemObjects(" << item_id << ",\"" << item_key << "\") called");
   ATH_MSG_DEBUG("           Key:" << item_key );
   if( aux_attr.size() ) {
   ATH_MSG_DEBUG("      Aux Attr:" << aux_attr );
   }
   static const std::string wildCard = "*";
   std::set<std::string> clidKeys;
   for (SG::IFolder::const_iterator iter = m_decoder->begin(), iterEnd = m_decoder->end();
           iter != iterEnd; iter++) {
      if (iter->id() == item_id) {
         clidKeys.insert(iter->key());
      }
   }

   // Here we build the list of attributes for the lossy float compression
   // Note that we do not allow m_compressionBitsHigh >= m_compressionBitsLow
   // Otherwise is, in any case, a logical error and they'd potentially overwrite each other
   std::map< unsigned int, std::set< std::string > > comp_attr_map;
   comp_attr_map[ m_compressionBitsHigh ] = buildCompressionSet( m_compressionDecoderHigh, item_id, item_key );
   comp_attr_map[ m_compressionBitsLow  ] = buildCompressionSet( m_compressionDecoderLow, item_id, item_key );

   // Print some debugging information regarding the lossy float compression configuration
   for( const auto& it : comp_attr_map ) {
     ATH_MSG_DEBUG("     Comp Attr " << it.second.size() << " with " << it.first << " mantissa bits.");
     if ( it.second.size() > 0 ) {
       for( const auto& attr : it.second ) {
          ATH_MSG_DEBUG("       >> " << attr);
       }
     }
   }

   // For MetaData objects of type T that are kept in MetaContainers get the MetaCont<T> ID
   const CLID remapped_item_id = m_metaDataSvc->remapMetaContCLID( item_id );
   SG::ConstProxyIterator iter, end;
   // Look for the clid in storegate
   if (((*m_currentStore)->proxyRange(remapped_item_id, iter, end)).isSuccess()) {
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
                  ATH_MSG_ERROR(" Could not get data object for id " << remapped_item_id << ",\"" << itemProxy->name());
               }
            }
            if (nullptr != itemProxy->object()) {
               if( std::find(m_objects.begin(), m_objects.end(), itemProxy->object()) == m_objects.end() &&
                   std::find(m_altObjects.begin(), m_altObjects.end(), itemProxy->object()) == m_altObjects.end() )
               {
                  if( item_id != remapped_item_id ) {
                     // For MetaCont<T>: - 
                     // create a temporary DataObject for an entry in the  container to pass to CnvSvc
                     DataBucketBase* dbb = static_cast<DataBucketBase*>( itemProxy->object() );
                     const MetaContBase* metaCont = static_cast<MetaContBase*>( dbb->cast( ClassID_traits<MetaContBase>::ID() ) );
                     if( metaCont ) {
                        void* obj = metaCont->getAsVoid( m_outSeqSvc->currentRangeID() );
                        auto altbucket = std::make_unique<AltDataBucket>(
                           obj, item_id, *CLIDRegistry::CLIDToTypeinfo(item_id), itemProxy->name() );
                        m_objects.push_back( altbucket.get() );
                        m_ownedObjects.push_back( std::move(altbucket) );
                        m_altObjects.push_back( itemProxy->object() ); // only for duplicate prevention
                     } else {
                        ATH_MSG_ERROR("Failed to retrieve object from MetaCont with key=" << item_key << " for EventRangeID=" << m_outSeqSvc->currentRangeID() );
                        return;
                     }
                  } else if (item.exact()) {
                   // If the exact flag is set, make a new DataObject
                   // holding the object as the requested type.
                   DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (itemProxy->object());
                   if (!dbb) std::abort();
                   void* ptr = dbb->cast (item_id);
                   if (!ptr) {
                     // Hard cast
                     ptr = dbb->object();
                   }
                   auto altbucket =
                     std::make_unique<AltDataBucket>
                       (ptr, item_id,
                        *CLIDRegistry::CLIDToTypeinfo (item_id),
                        *itemProxy);
                   m_objects.push_back(altbucket.get());
                   m_ownedObjects.push_back (std::move(altbucket));
                   m_altObjects.push_back (itemProxy->object());
                 }
                 else
                   m_objects.push_back(itemProxy->object());
                 ATH_MSG_DEBUG(" Added object " << item_id << ",\"" << itemProxy->name() << "\"");
               }

               // Build ItemListSvc string
               std::string tn;
               std::stringstream tns;
               if (!m_pCLIDSvc->getTypeNameOfID(item_id, tn).isSuccess()) {
                  ATH_MSG_ERROR(" Could not get type name for id "
                         << item_id << ",\"" << itemProxy->name());
                  tns << item_id << '_' << itemProxy->name();
               } else {
                  tn += '_' + itemProxy->name();
                  tns << tn;
               }

               /// Handle variable selections.
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

                  if (auxio) {
                    handleVariableSelection (*auxio, *itemProxy,
                                             tns.str(), aux_attr,
                                             vetoes);
                  }

                  // Here comes the compression logic using ThinningInfo
                  SG::IAuxStore* auxstore( nullptr );
                  try {
                    SG::fromStorable( itemProxy->object(), auxstore, true );
                  } catch( const std::exception& ) {
                    ATH_MSG_DEBUG( "Error in casting object with CLID "
                                   << itemProxy->clID() << " to SG::IAuxStore*" );
                    auxstore = nullptr;
                  }
                  if ( auxstore ) {
                    // Get a hold of all AuxIDs for this store (static, dynamic etc.)
                    const SG::auxid_set_t allVars = auxstore->getAuxIDs();

                    // Get a handle on the compression information for this store
                    std::string key = item_key;
                    key.erase (key.size()-4, 4);
                    SG::ThinningInfo::compression_map_t& compMap = compInfo[key];

                    // Build the compression list, retrieve the relevant AuxIDs and
                    // store it in the relevant map that is going to be inserted into
                    // the ThinningCache later on by the ThinningCacheTool
                    xAOD::AuxCompression compression;
                    compression.setCompressedAuxIDs( comp_attr_map );
                    for( const auto& it : compression.getCompressedAuxIDs( allVars ) ) {
                      if( it.second.size() > 0 ) // insert only if the set is non-empty
                        compMap[ it.first ] = it.second;
                    }

                    for( const auto& it : compMap ) {
                      ATH_MSG_DEBUG( "Lossy float compression level " << it.first <<
                                     " contains " << it.second.size() <<  " elements"
                                     " for container " << key );
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
         ATH_MSG_DEBUG(" No object matching " << item_id << ",\"" << item_key  << "\" found");
      } else if (removed) {
         ATH_MSG_DEBUG(" Object being excluded based on property setting "
                 << item_id << ",\"" << item_key  << "\". Skipping");
      }
   } else {
      ATH_MSG_DEBUG(" Failed to receive proxy iterators from StoreGate for "
              << item_id << ",\"" << item_key  << "\". Skipping");
   }
}

/// Here we build the list of attributes for the float compression
/// CompressionList follows the same logic as the ItemList
/// We find the matching keys, read the string after "Aux.",
/// tokenize by "." and build an std::set of these to be
/// communicated to ThinningInfo elsewhere in the code.
std::set<std::string>
AthenaOutputStream::buildCompressionSet (const ToolHandle<SG::IFolder>& handle,
                                         const CLID& item_id,
                                         const std::string& item_key) const
{
  // Create an empty result
  std::set<std::string> result;

  // Check the item is indeed Aux.
  if(item_key.find("Aux.") == string::npos) {
    return result;
  }

  // First the high compression list
  for (SG::IFolder::const_iterator iter = handle->begin(), iterEnd = handle->end();
         iter != iterEnd; iter++) {
    // First match the IDs for early rejection.
    if (iter->id() != item_id) {
      continue;
    }
    // Then find the compression item key and the compression list string
    size_t seppos = iter->key().find(".");
    string comp_item_key{""}, comp_str{""};
    if(seppos != string::npos) {
      comp_item_key = iter->key().substr(0, seppos+1);
      comp_str = iter->key().substr(seppos+1);
    } else {
      comp_item_key = iter->key();
    }
    // Proceed only if the keys match and the
    // compression list string is not empty
    if (!comp_str.empty() && comp_item_key == item_key) {
      std::stringstream ss(comp_str);
      std::string attr;
      while( std::getline(ss, attr, '.') ) {
         result.insert(attr);
      }
    }
  }

  // All done, return the result
  return result;
}

void AthenaOutputStream::handleVariableSelection (SG::IAuxStoreIO& auxio,
                                                  SG::DataProxy& itemProxy,
                                                  const std::string& tns,
                                                  const std::string& aux_attr,
                                                  SG::SelectionVetoes& vetoes) const
{
  // collect dynamic Aux selection (parse the line, attributes separated by dot)
  std::set<std::string> attributes;
  if( aux_attr.size() ) {
    std::stringstream ss(aux_attr);
    std::string attr;
    while( std::getline(ss, attr, '.') ) {
      attributes.insert(attr);
      std::stringstream temp;
      temp << tns << attr;
      if (m_itemSvc->addStreamItem(this->name(),temp.str()).isFailure()) {
        ATH_MSG_WARNING("Unable to record item " << temp.str() << " in Svc");
      }
    }
  }

  // Return early if there's no selection.
  if (attributes.empty()) {
    return;
  }

  std::string key = itemProxy.name();
  if (key.size() >= 4 && key.substr (key.size()-4, 4) == "Aux.")
  {
    key.erase (key.size()-4, 4);
  }

  SG::auxid_set_t dynVars = auxio.getSelectedAuxIDs();

  // Find the entry for the selection.
  SG::auxid_set_t& vset = vetoes[key];

  // Form the veto mask for this object.
  xAOD::AuxSelection sel;
  sel.selectAux (attributes);
  vset = sel.getSelectedAuxIDs (dynVars);

  vset.flip();
}


void AthenaOutputStream::itemListHandler(Gaudi::Details::PropertyBase& /* theProp */) {
   // Assuming concrete SG::Folder also has an itemList property
   IProperty *pAsIProp(nullptr);
   if ((m_p2BWritten.retrieve()).isFailure() ||
           nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_p2BWritten)) ||
           (pAsIProp->setProperty(m_itemList)).isFailure()) {
      throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
   }
}

void AthenaOutputStream::excludeListHandler(Gaudi::Details::PropertyBase& /* theProp */) {
   IProperty *pAsIProp(nullptr);
   if ((m_decoder.retrieve()).isFailure() ||
           nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_decoder)) ||
           (pAsIProp->setProperty("ItemList", m_excludeList.toString())).isFailure()) {
      throw GaudiException("Folder property [itemList] not found", name(), StatusCode::FAILURE);
   }
}

void AthenaOutputStream::compressionListHandlerHigh(Gaudi::Details::PropertyBase& /* theProp */) {
   IProperty *pAsIProp(nullptr);
   if ((m_compressionDecoderHigh.retrieve()).isFailure() ||
           nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_compressionDecoderHigh)) ||
           (pAsIProp->setProperty("ItemList", m_compressionListHigh.toString())).isFailure()) {
      throw GaudiException("Folder property [ItemList] not found", name(), StatusCode::FAILURE);
   }
}

void AthenaOutputStream::compressionListHandlerLow(Gaudi::Details::PropertyBase& /* theProp */) {
   IProperty *pAsIProp(nullptr);
   if ((m_compressionDecoderLow.retrieve()).isFailure() ||
           nullptr == (pAsIProp = dynamic_cast<IProperty*>(&*m_compressionDecoderLow)) ||
           (pAsIProp->setProperty("ItemList", m_compressionListLow.toString())).isFailure()) {
      throw GaudiException("Folder property [ItemList] not found", name(), StatusCode::FAILURE);
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
   // For 'write on finalize', we set up listener for 'MetaDataStop'
   // and perform write at this point. This happens at 'stop' of the
   // event selector. RDS 04/2010
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get the IncidentSvc");
      return StatusCode::FAILURE;
   }
   incSvc->addListener(this, "MetaDataStop", 50);
   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
      if (!tool->postInitialize().isSuccess()) {
          ATH_MSG_ERROR("Cannot initialize helper tool");
      }
   }
   return StatusCode::SUCCESS;
}


StatusCode AthenaOutputStream::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   for (ToolHandle<IAthenaOutputTool>& tool : m_helperTools) {
      if (!tool->preFinalize().isSuccess()) {
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
