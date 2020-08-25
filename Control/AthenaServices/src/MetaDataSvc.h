/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_METADATASVC_H
#define ATHENASERVICES_METADATASVC_H

/** @file MetaDataSvc.h
 *  @brief This file contains the class definition for the MetaDataSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"  // no forward decl: typedef
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IFileMgr.h"  // for FILEMGR_CALLBACK_ARGS
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "AthenaKernel/IMetaDataSvc.h"

#include "boost/bind.hpp"

#include <map>

// Forward declarations
class IAddressCreator;
class StoreGateSvc;
class IAlgTool;
class OutputStreamSequencerSvc;

namespace Io {
   class FileAttr;
}

static const InterfaceID IID_MetaDataSvc("MetaDataSvc", 1, 0);

template <class TYPE> class SvcFactory;

/** @class MetaDataSvc
 *  @brief This class provides the interface to the LCG POOL persistency software.
 **/
class ATLAS_CHECK_THREAD_SAFETY MetaDataSvc : public ::AthService,
	virtual public IAddressProvider,
	virtual public IIncidentListener,
        virtual public IMetaDataSvc,
	virtual public IIoComponent {
   // Allow the factory class access to the constructor
   friend class SvcFactory<MetaDataSvc>;

public: // Constructor and Destructor
   /// Standard Service Constructor
   MetaDataSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~MetaDataSvc();

   /// Retrieve interface ID
   static const InterfaceID& interfaceID() { return IID_MetaDataSvc; }

public: // Non-static members
   /// Required of all Gaudi services:
   virtual StatusCode initialize() override;
   /// Required of all Gaudi services:
   virtual StatusCode stop() override;
   /// Required of all Gaudi services:
   virtual StatusCode finalize() override;
   /// Required of all Gaudi services:  see Gaudi documentation for details

   /// Function called when a new metadata source becomes available
   virtual StatusCode newMetadataSource(const Incident&);

   /// Function called when a metadata source is closed
   virtual StatusCode retireMetadataSource(const Incident&);

   /// Function called when the current state of metadata must be made 
   /// ready for output
   virtual StatusCode prepareOutput();

   /// version of prepareOutput() for parallel streams
   virtual StatusCode prepareOutput(const std::string& outputName);
  
   virtual StatusCode shmProxy(const std::string& filename) override;

   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

   /// Get all addresses from provider. Called before begin event.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   virtual StatusCode preLoadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads) override;

   /// Implementation of the loadAddresses function without any functionality.
   virtual StatusCode loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads) override;

   /// Update a transient address.
   /// @param tad [IN] transient address to be updated.
   virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                                    const EventContext& ctx) override;

   /// Incident service handle listening for BeginInputFile and EndInputFile.
   virtual void handle(const Incident& incident) override;

   /// Transition output metadata file - fire MeteDataStop incident to transition OutputStream
   StatusCode transitionMetaDataFile();

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit() override;

   StatusCode rootOpenAction(FILEMGR_CALLBACK_ARGS);

   virtual StoreGateSvc* outputDataStore() const override final { return &*m_outputDataStore; }

   virtual const std::string currentRangeID() const override final;

   CLID remapMetaContCLID( const CLID& item_id ) const;

private:
   /// Add proxy to input metadata store - can be called directly or via BeginInputFile incident
   StatusCode addProxyToInputMetaDataStore(const std::string& tokenStr);
   /// Initialize input metadata store - can be called directly or via BeginInputFile incident
   StatusCode initInputMetaDataStore(const std::string& fileName);

private: // data
   ServiceHandle<StoreGateSvc> m_inputDataStore;
   ServiceHandle<StoreGateSvc> m_outputDataStore;
   ServiceHandle<IAddressCreator> m_addrCrtr;
   ServiceHandle<IFileMgr> m_fileMgr;
   ServiceHandle<IIncidentSvc> m_incSvc;
   ServiceHandle<OutputStreamSequencerSvc>  m_outSeqSvc;
 
   long m_storageType;
   bool m_clearedInputDataStore;
   bool m_clearedOutputDataStore;
   bool m_allowMetaDataStop;
   bool m_outputPreprared; 
   std::map<std::string, CLID> m_persToClid;
   std::map<CLID, std::string> m_toolForClid;
   std::map<std::string, std::string> m_streamForKey;

private: // properties
   /// MetaDataContainer, POOL container name for MetaData.
   StringProperty                 m_metaDataCont;
   /// MetaDataTools, vector with the MetaData tools.
   ToolHandleArray<IMetaDataTool> m_metaDataTools;
};
 
#endif
