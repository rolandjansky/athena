// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENAOUTPUTSTREAM_H
#define ATHENASERVICES_ATHENAOUTPUTSTREAM_H

// STL include files
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <mutex>

// Required for inheritance
#include "GaudiKernel/IDataSelector.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/IIoComponent.h"

#include "SelectionVetoes.h"
#include "CompressionInfo.h"

// forward declarations
class IClassIDSvc;
class OutputStreamSequencerSvc;
class StoreGateSvc;
class IAthenaOutputStreamTool;
class IAthenaOutputTool;
class IItemListSvc;
class MetaDataSvc;

namespace SG {
   class DataProxy;
   class IFolder;
   class IAuxStoreIO;
   class FolderItem;
}

/** @class AthenaOutputStream
   * @brief algorithm that marks for write data objects in SG
   * 
   * @author srinir@bnl.gov
   * $Id: AthenaOutputStream.h,v 1.12 2008-12-15 19:14:58 binet Exp $
   */
class AthenaOutputStream : public FilteredAlgorithm,
                           virtual public IIncidentListener,
                           virtual public IIoComponent {

public:
   typedef std::vector<SG::DataProxy*>     Items;

protected:
   /// handle to the @c StoreGateSvc store where the data we want to
   /// write out resides
   ServiceHandle<StoreGateSvc> m_dataStore;
   ServiceHandle<StoreGateSvc> m_metadataStore;
   ServiceHandle<StoreGateSvc>* m_currentStore;

   ServiceHandle<IItemListSvc>  m_itemSvc;
   ServiceHandle<MetaDataSvc>   m_metaDataSvc;

   /// Name of the persistency service capable to write data from the store
   std::string              m_persName;
   /// Name of the OutputStreamTool used for writing
   StringProperty           m_writingTool;
   /// Name of the output file
   std::string              m_outputName;
   std::string              m_outputAttributes;
   
   typedef ServiceHandle<IClassIDSvc> IClassIDSvc_t;
   IClassIDSvc_t m_pCLIDSvc;
   ServiceHandle<OutputStreamSequencerSvc>  m_outSeqSvc;
  
   /// Vector of item names
   StringArrayProperty      m_itemList{this,"ItemList",{},"List of items to write","OrderedSet<std::string>"};
   /// Vector of item names
   StringArrayProperty      m_metadataItemList;
   /// Vector of item names
   StringArrayProperty      m_excludeList;
   /// Number of commits after which to do a flush when using in memory files
   IntegerProperty m_autoSend{this,"AutoSend",-1};
   /// Vector of item names
   StringArrayProperty      m_compressionListHigh;
   /// Vector of item names
   StringArrayProperty      m_compressionListLow;
   /// Number of mantissa bits in the float compression
   unsigned int             m_compressionBitsHigh;
   /// Number of mantissa bits in the float compression
   unsigned int             m_compressionBitsLow;
   /// List of items that are known to be present in the transient store
   /// (and hence we can make input dependencies on them).
   StringArrayProperty      m_transientItems;
   /// the top-level folder with items to be written
   ToolHandle<SG::IFolder>  m_p2BWritten;
   /// the top-level folder with items to be written
   ToolHandle<SG::IFolder>  m_decoder;
   /// the top-level folder with items to be compressed high
   ToolHandle<SG::IFolder>  m_compressionDecoderHigh;
   /// the top-level folder with items to be compressed low
   ToolHandle<SG::IFolder>  m_compressionDecoderLow;
   /// Decoded list of transient ids.
   ToolHandle<SG::IFolder>  m_transient;
   /// map of (clid,key) pairs to be excluded (comes from m_excludeList)
   std::multimap<CLID,std::string> m_CLIDKeyPairs;
   /// Collection of objects being selected
   IDataSelector m_objects;
   /// Objects overridden by `exact' handling.
   IDataSelector m_altObjects;
   /// Collection of DataObject instances owned by this service.
   /// FIXME: it would be simpler to just have m_objects be a vector
   /// of DataObjectSharedPtr<DataObject>, but that implies interface changes.
   std::vector<std::unique_ptr<DataObject> > m_ownedObjects;
   /// Number of events written to this output stream
   std::atomic<int> m_events;
   /// set to true to force read of data objects in item list
   bool m_forceRead;
   /// set to false to omit adding the current DataHeader into the DataHeader history
   /// this will cause the input file to be neglected for back navigation (replace mode).
   bool m_extendProvenanceRecord;
   /// set to true to trigger streaming of data on execute()
   bool m_writeOnExecute;
   /// set to true to trigger streaming of data on finalize()
   bool m_writeOnFinalize;
   /// set to write out everything in input DataHeader
   bool m_itemListFromTool;
   /// set to true to check for number of times each object is written
   bool m_checkNumberOfWrites;
   /// map to record number of writes per object
   typedef std::map<std::string, unsigned int>  CounterMapType;
   CounterMapType  m_objectWriteCounter;
   /// Vector of names of AlgTools that are executed by this stream

   /// pointer to AthenaOutputStreamTool
   ToolHandle<IAthenaOutputStreamTool> m_streamer;
   /// vector of AlgTools that that are executed by this stream
   ToolHandleArray<IAthenaOutputTool> m_helperTools;

   // ------- Event Ranges handling in MT -------
   /// map of filenames assigned to active slots
   std::map< unsigned, std::string >  m_slotRangeMap;
   /// map of streamerTools handling event ranges in MT
   std::map< std::string, std::unique_ptr<IAthenaOutputStreamTool> > m_streamerMap;
   /// mutex for this Stream write() and handle() methods
   typedef std::recursive_mutex mutex_t;
   mutex_t  m_mutex;

private:
   /// Key used for recording selected dynamic variable information
   /// to the event store.
   SG::WriteHandleKey<SG::SelectionVetoes> m_selVetoesKey
   { this, "SelVetoesKey", "" };

   /// Key used for recording lossy float compressed variable information
   /// to the event store.
   SG::WriteHandleKey<SG::CompressionInfo> m_compInfoKey
   { this, "CompInfoKey", "" };

protected:
   /// Handler for ItemNames Property
   void itemListHandler(Gaudi::Details::PropertyBase& /* theProp */);
   /// Handler for ItemNames Property
   void excludeListHandler(Gaudi::Details::PropertyBase& /* theProp */);
   /// Handler for ItemNames Property
   void compressionListHandlerHigh(Gaudi::Details::PropertyBase& /* theProp */);
   /// Handler for ItemNames Property
   void compressionListHandlerLow(Gaudi::Details::PropertyBase& /* theProp */);

public:
   typedef std::vector<std::pair<std::string, std::string> > TypeKeyPairs;
   
   /// Standard algorithm Constructor
   AthenaOutputStream(const std::string& name, ISvcLocator* pSvcLocator); 
   /// Standard Destructor
   virtual ~AthenaOutputStream();

   /// \name implement IAlgorithm
   //@{
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;
   virtual StatusCode execute() override;
   virtual StatusCode stop() override;
   //@}
   /// Stream the data
   virtual StatusCode write();
   /// Clear list of selected objects
   void clearSelection();
   /// Collect data objects for output streamer list
   StatusCode collectAllObjects();
   /// Return the list of selected objects
   IDataSelector* selectedObjects() {
      return &m_objects;
   }
   /// Incident service handle listening for MetaDataStop 
   virtual void handle(const Incident& incident) override;
   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit() override;
   virtual StatusCode io_finalize() override;

private:
   /// Add item data objects to output streamer list
  void addItemObjects(const SG::FolderItem&, SG::SelectionVetoes& vetoes, SG::CompressionInfo& compInfo);

   void handleVariableSelection (SG::IAuxStoreIO& auxio,
                                 SG::DataProxy& itemProxy,
                                 const std::string& tns,
                                 const std::string& aux_attr,
                                 SG::SelectionVetoes& vetoes) const;

   /// tokenize a string based on a substring
   void tokenizeAtSep( std::vector<std::string>&, const std::string&, const std::string& ) const;

   /// Try to match a DataProxy to a vector of strings
   bool matchKey(const std::vector<std::string>& key, const SG::DataProxy* proxy) const;

   /// Write MetaData for this stream (by default) or for a substream outputFN (in ES mode)
   void writeMetaData( const std::string outputFN="" );

   /// Helper function for building the compression lists
   std::set<std::string> buildCompressionSet (const ToolHandle<SG::IFolder>& handle,
                                              const CLID& item_id,
                                              const std::string& item_key) const;
};

#endif // ATHENASERVICES_OUTPUTSTREAM_H
