/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLADDRESSPROVIEDERSVC_H
#define ATHENAPOOLADDRESSPROVIEDERSVC_H

/** @file AthenaPoolAddressProviderSvc.h
 *  @brief This file contains the class definition for the AthenaPoolAddressProviderSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  // no forward decl: typedef

#include "AthenaKernel/IAddressProvider.h"
#include "AthenaBaseComps/AthService.h"
#include "PersistentDataModel/Guid.h"

#include <map>
#include <string>

class DataHeader;
class ActiveStoreSvc;
class IClassIDSvc;

/** @class AthenaPoolAddressProviderSvc
 *  @brief This class is the EventSelector and AddressProvider for event data.
 **/
class AthenaPoolAddressProviderSvc : public ::AthService, virtual public IAddressProvider {
public: // Constructor and Destructor
   /// Standard Service Constructor
   AthenaPoolAddressProviderSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AthenaPoolAddressProviderSvc();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   /// Required of all Gaudi Services
   virtual StatusCode finalize();
   /// Does this object satisfy a given interface?  See Gaudi documentation for details.
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Get all addresses from provider. Called before begin event.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   virtual StatusCode preLoadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Get all addresses for this event from provider.
   /// @param storeID [IN] store ID, this function only preloads event store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   virtual StatusCode loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Update a transient address.
   /// @param storeID [IN] store ID, this function only preloads event store addresses.
   /// @param tad [IN] transient address to be updated.
   virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                                    const EventContext& ctx);

private: // data
   ServiceHandle<ActiveStoreSvc> m_activeStoreSvc;
   ServiceHandle<IClassIDSvc> m_clidSvc;
   Guid m_guid;

private: // properties
   /// BackNavigation, switch on back navigation to find objects in input streams: default = false.
   BooleanProperty m_backNavigationFlag;
   /// BackNavigationScope, vector with '<processTag>::<type>#<key>' hints for backNavigation.
   StringArrayProperty m_backNavigationScope;
   std::map<std::string, std::string> m_backNavigationMap;

   /// DataHeaderKey, StoreGate key of event entry object (type DataHeader).
   StringProperty m_dataHeaderKey;

   /// DataHeaderIterator, switch to turn on iteration on Token, rather than DataHeader reading.
   /// Assumes nice/homogeneous input files and may not work for all use cases: default = false.
   BooleanProperty m_dataHeaderIterator;

private: // internal helper functions
   /// Use back navigation through DataHeaders to locate object, if possible
   StatusCode chaseAddress(SG::TransientAddress* tad,
	   const DataHeader* dataHeader,
	   const std::string& processTag = "");
   /// Return pointer to active event SG
   StoreGateSvc* eventStore() const;
};

#endif
