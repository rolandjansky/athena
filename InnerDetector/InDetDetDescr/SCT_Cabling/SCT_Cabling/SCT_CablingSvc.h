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
#include <map>
#include <set>
#include <algorithm>
//uint32 in std: namespace
#include <cstdint>
//#include "boost/cstdint.hpp"
//fixed-size array
//#include "boost/array.hpp"
#include <array>

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
class SCT_CablingSvc: virtual public ISCT_CablingSvc, virtual public IIncidentListener, public AthService{
  friend class SvcFactory<SCT_CablingSvc>;
  public:
  
    enum {MAX_HASH=8175, NUMBER_OF_HASHES=8176};
    ///Incident listener method
    virtual void handle(const Incident& beginRunIncident);
    
    //@name Service methods, reimplemented
    //@{
    SCT_CablingSvc(const std::string &name, ISvcLocator * svc);  
    virtual ~SCT_CablingSvc(){}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    //interfaceID() implementation is in the baseclass
    virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
    //@}
  
    //@name ISCT_CablingSvc methods implemented, these are visible to clients
    //@{
    /// return offline hash, given the online Id (used by decoders)
    virtual IdentifierHash getHashFromOnlineId(const SCT_OnlineId & onlineId, const bool withWarnings=true);
   
    /// return the online Id, given a hash (used by simulation encoders)
    virtual SCT_OnlineId getOnlineIdFromHash(const IdentifierHash & hash);
  
    /// return the online Id, given an offlineId
    virtual SCT_OnlineId getOnlineIdFromOfflineId(const Identifier & offlineId);
  
    /// return the rob/rod Id, given a hash (used by simulation encoders)
    virtual std::uint32_t getRobIdFromHash(const IdentifierHash & hash) {
      return getOnlineIdFromHash(hash).rod();
    }
    
    /// return the rob/rod Id, given an offlineId (used by simulation encoders)
    virtual std::uint32_t getRobIdFromOfflineId(const Identifier & offlineId)  {
      return getOnlineIdFromOfflineId(offlineId).rod();
    }

    /// size of the data structure (for the SCT should be 8176, one for each module side)
    virtual unsigned int size() const;
    
    /// is the data structure empty?
    virtual bool empty() const;
    
    /// get hash from a module serial number, needed in the conditions service because configurations are stored by module s/n
    virtual IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber & sn) ;

  
    /// get module serial number from hash, needed during filling of data structure
    virtual SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash & hash) ;
  
    /// fill a users vector with all the RodIds
    virtual void getAllRods(std::vector<std::uint32_t> & usersVector) { std::copy(m_rodIdSet.begin(), m_rodIdSet.end(), std::back_inserter(usersVector)); }
    
    /// fill a user's vector with all the hash ids which belong to a given rod
    virtual void getHashesForRod(std::vector<IdentifierHash> & usersVector, const std::uint32_t rodId);
    //@}
  
    /// insert the hashId, onlineId and serial number to the data, used only within this package to fill the data structure
    bool insert(const IdentifierHash & hash, const SCT_OnlineId & onlineId, const SCT_SerialNumber & sn);  

    
  private:
   /**
    * The underlying data structure for IdHash vs. onlineId is a boost fixed size array of 9216 elements
    * since the SCT crates have identifiers of the form 0x2X000Y and 0x2X010Y, where X is 1-4
    * and X is 0-0xb (so there are 96 possibilities). Further, each crate can have a maximum of
    * 96 fibres, so 96 * 96 = 9216 possibilities. In practice, there are 8176 modules so the crate
    * cabling occupancy is 89%; this is probably still better than using a map.
    **/
    std::array<IdentifierHash, SCT_OnlineId::NUMBER_OF_INDICES> m_onlineId2HashArray;

    std::map<SCT_SerialNumber, IdentifierHash> m_sn2HashMap; //!<Data map for serial number to hash
    std::array<SCT_OnlineId, NUMBER_OF_HASHES> m_hash2OnlineIdArray; //!<Array for hash to onlineId; hash goes from 0-8175
    std::set<std::uint32_t> m_rodIdSet; //!<set of robIds
  
    ServiceHandle<ISCT_FillCabling> m_cablingFiller; //!< The cabling filler by baseclass handle; the concrete class is decided by job options
    StringProperty m_cablingDataSource; //!< the name of the data source
    unsigned int m_hashEntries; //!< number of entries successfully entered in the data structures
    const SCT_ID *  m_idHelper; //!< helper for offlineId/hash conversions
    bool m_usingDatabase;
};



#endif
