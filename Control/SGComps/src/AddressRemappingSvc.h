// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGCOMPS_ADDRESSREMAPPINGSVC_H
#define SGCOMPS_ADDRESSREMAPPINGSVC_H 1

/** @file AddressRemappingSvc.h
 *  @brief This file contains the class definition for the AddressRemappingSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: AddressRemappingSvc.h,v 1.4 2008-06-06 18:25:04 gemmeren Exp $
 **/

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  // no forward decl: typedef
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaBaseComps/AthService.h"
#include "SGTools/TransientAddress.h"

#include <vector>
#include <string>

// Forward declarations
class IClassIDSvc;
class IProxyDict;

//template <class TYPE> class SvcFactory;

/** @class AddressRemappingSvc
 *  @brief This class provides the interface to the LCG POOL persistency software.
 **/
class AddressRemappingSvc : virtual public IAddressProvider,
                            public ::AthService
{

public: 

  // Constructor and Destructor
   /// Standard Service Constructor
   AddressRemappingSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AddressRemappingSvc();

public: // Non-static members
   /// Required of all Gaudi services:
   StatusCode initialize();
   /// Required of all Gaudi services:
   StatusCode finalize();
   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Get all addresses from provider. Called before begin event.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   virtual StatusCode preLoadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Implementation of the loadAddresses function without any functionality.
   virtual StatusCode loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads);

   /// Update a transient address. Do mapping from one address to another
   /// @param tad [IN] transient address to be updated.
  virtual StatusCode updateAddress(StoreID::type /*storeID*/,
				   SG::TransientAddress* pTad);

private:
   CLID getClid(std::string type) const;

private: // Data
   ServiceHandle<IClassIDSvc> m_clidSvc;
   ServiceHandle<IProxyDict> m_proxyDict;

   /// TypeKeyOverwriteMaps, map for type#key overwrites.
   StringArrayProperty m_overwriteMaps;
   std::vector<SG::TransientAddress> m_oldTads;
   std::vector<SG::TransientAddress> m_newTads;

   bool m_skipBadRemappings;
};
#endif // !SGCOMPS_ADDRESSREMAPPINGSVC_H
