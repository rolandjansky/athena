/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CablingSvc_h
#define SCT_CablingSvc_h
/**
 * @file SCT_CablingSvc.h
 * Header file for  SCT cabling service
 * @author Shaun Roe
 * @date 20 October, 2008
 **/

//STL includes
#include <string>
#include <vector>
//uint32 in std: namespace
#include <cstdint>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

//package includes
//these are in the baseclass:
//#include "SCT_Cabling/SCT_SerialNumber.h"
//#include "SCT_Cabling/SCT_OnlineId.h"

#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_Cabling/ISCT_FillCabling.h"
#include "SCT_Cabling/SCT_CablingData.h"

//fwd declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class StatusCode;
class InterfaceID;
class Incident;
class SCT_ID;
class Identifier;

/**
 * @class SCT_CablingSvc, providing mappings of online and offline identifiers and also serial numbers
 **/
class SCT_CablingSvc: virtual public ISCT_CablingSvc, virtual public IIncidentListener, public AthService {
  friend class SvcFactory<SCT_CablingSvc>;
 public:

  ///Incident listener method
  virtual void handle(const Incident& beginRunIncident);
    
  //@name Service methods, reimplemented
  //@{
  SCT_CablingSvc(const std::string& name, ISvcLocator* svc);
  virtual ~SCT_CablingSvc() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //interfaceID() implementation is in the baseclass
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  //@}
  
  //@name ISCT_CablingSvc methods implemented, these are visible to clients
  //@{
  /// return offline hash, given the online Id (used by decoders)
  virtual IdentifierHash getHashFromOnlineId(const SCT_OnlineId& onlineId, const bool withWarnings=true) override;
   
  /// return the online Id, given a hash (used by simulation encoders)
  virtual SCT_OnlineId getOnlineIdFromHash(const IdentifierHash& hash) override;
  
  /// return the online Id, given an offlineId
  virtual SCT_OnlineId getOnlineIdFromOfflineId(const Identifier& offlineId) override;
  
  /// return the rob/rod Id, given a hash (used by simulation encoders)
  virtual std::uint32_t getRobIdFromHash(const IdentifierHash& hash) override {
    return getOnlineIdFromHash(hash).rod();
  }
    
  /// return the rob/rod Id, given an offlineId (used by simulation encoders)
  virtual std::uint32_t getRobIdFromOfflineId(const Identifier& offlineId) override {
    return getOnlineIdFromOfflineId(offlineId).rod();
  }

  /// size of the data structure (for the SCT should be 8176, one for each module side)
  virtual unsigned int size() const override;
    
  /// is the data structure empty?
  virtual bool empty() const override;
    
  /// get hash from a module serial number, needed in the conditions service because configurations are stored by module s/n
  virtual IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber& sn) override;

  /// get module serial number from hash, needed during filling of data structure
  virtual SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash& hash) override;

  /// fill a users vector with all the RodIds
  virtual void getAllRods(std::vector<std::uint32_t>& usersVector) override { m_data.getRods(usersVector); }
    
  /// fill a user's vector with all the hash ids which belong to a given rod
  virtual void getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId) override;
  //@}

  /// insert the hashId, onlineId and serial number to the data, used only within this package to fill the data structure
  virtual bool insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn) override;

 private:
  SCT_CablingData m_data;

  ServiceHandle<ISCT_FillCabling> m_cablingFiller; //!< The cabling filler by baseclass handle; the concrete class is decided by job options
  StringProperty m_cablingDataSource; //!< the name of the data source
  const SCT_ID* m_idHelper; //!< helper for offlineId/hash conversions
  bool m_usingDatabase;
};

#endif // SCT_CablingSvc_h
