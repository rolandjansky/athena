// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "Gaudi/Property.h"  // no forward decl: typedef
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IInputRename.h"
#include "AthenaKernel/RCUObject.h"
#include "GaudiKernel/IAlgResourcePool.h" 
#include "AthenaBaseComps/AthService.h"
#include "SGTools/TransientAddress.h"
#include "CxxUtils/checker_macros.h"

#include <vector>
#include <string>
#include <mutex>

// Forward declarations
class IClassIDSvc;
class IProxyDict;
class IRCUSvc;

/** @class AddressRemappingSvc
 *  @brief This class provides the interface to the LCG POOL persistency software.
 **/
class AddressRemappingSvc
  : public extends1<AthService, Athena::IInputRename>, public IAddressProvider
{

public: 

  // Constructor and Destructor
   /// Standard Service Constructor
   AddressRemappingSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~AddressRemappingSvc();

public: // Non-static members
   /// Required of all Gaudi services:
   virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
   /// Required of all Gaudi services:
   virtual StatusCode finalize() override;

   /// Get all addresses from provider. Called before begin event.
   /// @param storeID [IN] store ID, this function only preloads detector store addresses.
   /// @param tads [OUT] list of the transient addresses which were preloaded.
   virtual StatusCode preLoadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads) override;

   /// Implementation of the loadAddresses function without any functionality.
   virtual StatusCode loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads) override;

   /// Update a transient address. Do mapping from one address to another
   /// @param tad [IN] transient address to be updated.
  virtual StatusCode updateAddress(StoreID::type /*storeID*/,
				   SG::TransientAddress* pTad,
                                   const EventContext& ctx) override;


  /**
   * @brief Retrieve a pointer to the input rename map.
   *
   * May return null if no renaming is to be done.
   */
  virtual const IInputRename::InputRenameRCU_t* inputRenameMap() const override;

private:
   CLID getClid(std::string type) const;

private: // Data
   ServiceHandle<IClassIDSvc> m_clidSvc;
  ServiceHandle<IProxyDict> m_proxyDict{this,"ProxyDict","StoreGateSvc",
      "the IProxyDict we want to apply the remapping to (by default the event store)"};
  ServiceHandle<Athena::IRCUSvc> m_RCUSvc;
  ServiceHandle<IAlgResourcePool> m_algResourcePool{this,"AlgResourcePool","AlgResourcePool",
      "Algorithm resource pool service."};

  /// TypeKeyOverwriteMaps, map for type#key overwrites.
  StringArrayProperty m_overwriteMaps{this,"TypeKeyOverwriteMaps",{},"","OrderedSet<std::string>"};
  std::vector<SG::TransientAddress> m_oldTads;
  std::vector<SG::TransientAddress> m_newTads;

   /// Property: list of requested input renames.
  Gaudi::Property<std::vector<std::string> >m_typeKeyRenameMaps{this,"TypeKeyRenameMaps",{},
      "List of renamings to apply to input objects.  This is distinct from the "
	"mappings given by TypeKeyOverwriteMaps in that objects listed under "
	"TypeKeyOverwriteMaps are accessible by both the old and new names; "
	"while for TypeKeyRenameMaps, only the new names are visible (so the old names "
	"may be rewritten).  Overwriting may also change the visible type of an object, "
	"while renaming may not.  Format of list elements is OLDNAME#TYPE->NEWNAME.","OrderedSet<std::string>"};

   /// Map of sgkey->sgkey for input renames.
   /// This object is exported via inputRenameMap and is synchronized
   /// via RCU.
  typedef Athena::IInputRename::InputRenameMap_t InputRenameMap_t;
  typedef Athena::IInputRename::InputRenameRCU_t InputRenameRCU_t;
   std::unique_ptr<InputRenameRCU_t> m_inputRenames;

  Gaudi::Property<bool> m_skipBadRemappings{this,"SkipBadRemappings",false,
      "If true, will delay the remapping setup until the first load, and will check against the given file"};

   bool m_haveDeletes=false;
   std::unordered_multimap<std::string, CLID> m_deletes;

   // FIXME: calling getFlatAlgList() can result in a recursive call!
   typedef std::recursive_mutex mutex_t;
   typedef std::lock_guard<mutex_t> lock_t;
   mutable mutex_t m_mutex;


private:
  /**
   * @brief Merge in additional input rename mappings.
   * @param toadd Mappings to add.
   *
   * Additional sgkey->sgkey input rename mappings are merged into the rename map,
   * using RCU synchronization.
   */
   void addInputRenames (const InputRenameMap_t& toadd);


   /**
    * @brief Set up input rename mappings during initialization.
    */
   StatusCode initInputRenames ATLAS_NOT_THREAD_SAFE ();
   StatusCode renameTads (IAddressProvider::tadList& tads);

   void initDeletes();
   bool isDeleted (const SG::TransientAddress& tad) const;
};
#endif // !SGCOMPS_ADDRESSREMAPPINGSVC_H
