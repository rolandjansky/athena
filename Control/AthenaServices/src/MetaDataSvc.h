/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METADATASVC_H
#define METADATASVC_H

/** @file MetaDataSvc.h
 *  @brief This file contains the class definition for the MetaDataSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"  // no forward decl: typedef
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IFileMgr.h"  // for FILEMGR_CALLBACK_ARGS
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaBaseComps/AthService.h"

#include "boost/bind.hpp"

#include <map>

// Forward declarations
class IAddressCreator;
class StoreGateSvc;
class IAlgTool;

namespace Io {
   class FileAttr;
}

static const InterfaceID IID_MetaDataSvc("MetaDataSvc", 1, 0);

template <class TYPE> class SvcFactory;

/** @class MetaDataSvc
 *  @brief This class provides the interface to the LCG POOL persistency software.
 **/
class MetaDataSvc : public ::AthService,
	virtual public IAddressProvider,
	virtual public IIncidentListener,
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
   StatusCode initialize();
   /// Required of all Gaudi services:
   StatusCode stop();
   /// Required of all Gaudi services:
   StatusCode finalize();
   /// Required of all Gaudi services:  see Gaudi documentation for details

   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Get all addresses from provider. Called before begin event.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   StatusCode preLoadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Implementation of the loadAddresses function without any functionality.
   StatusCode loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Update a transient address.
   /// @param tad [IN] transient address to be updated.
   StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                            const EventContext& ctx);

   /// Incident service handle listening for BeginInputFile and EndInputFile.
   void handle(const Incident& incident);

   /// Transition output metadata file - fire MeteDataStop incident to transition OutputStream
   StatusCode transitionMetaDataFile(bool ignoreInputFile = false);

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   StatusCode io_reinit();

   StatusCode rootOpenAction(FILEMGR_CALLBACK_ARGS);

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

   long m_storageType;
   bool m_clearedInputDataStore;
   bool m_allowMetaDataStop;
   std::map<std::string, CLID> m_persToClid;
   std::map<CLID, std::string> m_toolForClid;
   std::map<std::string, std::string> m_streamForKey;

private: // properties
   /// MetaDataContainer, POOL container name for MetaData.
   StringProperty                 m_metaDataCont;
   /// MetaDataTools, vector with the MetaData tools.
   ToolHandleArray<IAlgTool> m_metaDataTools;
};

#endif
