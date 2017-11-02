// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENAOUTPUTSTREAM_H
#define ATHENASERVICES_ATHENAOUTPUTSTREAM_H

// STL include files
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>

// Required for inheritance
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"
#include "GaudiKernel/IIoComponent.h"

// forward declarations
class IClassIDSvc;
class OutputStreamSequencerSvc;
class StoreGateSvc;
class IAthenaOutputStreamTool;
class IAthenaOutputTool;
class IItemListSvc;
class UserDataSvc;

namespace SG {
   class DataProxy;
   class IFolder;
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
   friend class UserDataSvc;

public:
   typedef std::vector<SG::DataProxy*>     Items;

protected:
   /// handle to the @c StoreGateSvc store where the data we want to
   /// write out resides
   ServiceHandle<StoreGateSvc> m_dataStore;
   ServiceHandle<StoreGateSvc> m_metadataStore;
   ServiceHandle<StoreGateSvc>* m_currentStore;

   ServiceHandle<IItemListSvc>  m_itemSvc;

   /// Name of the persistency service capable to write data from the store
   std::string              m_persName;
   /// Name of the OutputStreamTool used for writing
   StringProperty           m_writingTool;
   /// Name of the output file
   std::string              m_outputName;
   /// tag of processing stage:
   StringProperty           m_processTag;
   
   typedef ServiceHandle<IClassIDSvc> IClassIDSvc_t;
   IClassIDSvc_t m_pCLIDSvc;
   
   typedef ServiceHandle<OutputStreamSequencerSvc> OutputStreamSequencerSvc_t;
   OutputStreamSequencerSvc_t m_outSeqSvc;
   
   /// Vector of item names
   StringArrayProperty      m_itemList;
   /// Vector of item names
   StringArrayProperty      m_metadataItemList;
   /// Vector of item names
   StringArrayProperty      m_excludeList;
   /// the top-level folder with items to be written
   ToolHandle<SG::IFolder>  m_p2BWritten;
   /// the top-level folder with items to be written
   ToolHandle<SG::IFolder>  m_decoder;
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
   int                      m_events;
   /// set to true to force read of data objects in item list
   bool m_forceRead;
   /// set to true to allow data objects being copied persistent to persistent (without SG retrieve).
   bool m_persToPers;
   std::vector<unsigned int> m_exemptPersToPers;
   /// set to true to allow defaults being provided for non-existent data objects.
   bool m_provideDef;
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

protected:
   /// Handler for ItemNames Property
   void itemListHandler(Property& /* theProp */);
   /// Handler for ItemNames Property
   void excludeListHandler(Property& /* theProp */);

public:
   typedef std::vector<std::pair<std::string, std::string> > TypeKeyPairs;
   
   /// Standard algorithm Constructor
   AthenaOutputStream(const std::string& name, ISvcLocator* pSvcLocator); 
   /// Standard Destructor
   virtual ~AthenaOutputStream();

   /// \name implement IAlgorithm
   //@{
   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();
   //@}
   /// Stream the data
   virtual StatusCode write();
   /// Clear list of selected objects
   void clearSelection();
   /// Collect data objects for output streamer list
   void collectAllObjects();
   /// Add folder data objects to output stramer list
   void addDataHeaderObjects();
   /// Return the list of selected objects
   IDataSelector* selectedObjects() {
      return &m_objects;
   }
   /// Incident service handle listening for LastInputFile
   void handle(const Incident& incident);
   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit();
   virtual StatusCode io_finalize();

private:
   /// Add item data objects to output streamer list
   void addItemObjects(const SG::FolderItem&);

   /// tokenize a string based on a substring
   void tokenizeAtSep( std::vector<std::string>&, const std::string&, const std::string& ) const;

   /// Try to match a DataProxy to a vector of strings
   bool matchKey(const std::vector<std::string>& key, const SG::DataProxy* proxy) const;

};

#endif // ATHENASERVICES_OUTPUTSTREAM_H
