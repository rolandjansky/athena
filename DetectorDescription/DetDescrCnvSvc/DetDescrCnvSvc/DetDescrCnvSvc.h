/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCNVSVC_DETDESCRCNVSVC_H
#define DETDESCRCNVSVC_DETDESCRCNVSVC_H

#include "GaudiKernel/ConversionSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;

class IOpaqueAddress;
class StatusCode;
class ActiveStoreSvc;
class StoreGateSvc;
//  namespace SG {
//    class TransientAddress;
//  }

//  class DetDescrCnvSvc : 
//      public ConversionSvc,
//      public virtual IAddressProvider {

class DetDescrCnvSvc : public ConversionSvc	{
    /// Allow the factory class access to the constructor
    friend class SvcFactory<DetDescrCnvSvc>;
public:

    /// Initialize the service.
    virtual StatusCode initialize();
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    /// Add new address to the Detector Store
    virtual StatusCode addToDetStore(const CLID& clid, const std::string& name) const;

    /// Basic create address
    virtual StatusCode createAddress( long svc_type,
				      const CLID& clid,
				      const std::string* par, 
				      const unsigned long* ip,
				      IOpaqueAddress*& refpAddress);

    /// Create address from string form
    virtual StatusCode createAddress( long svc_type, 
				      const CLID& clid, 
				      const std::string& refAddress,
				      IOpaqueAddress*& refpAddress);

    /// Convert address to string form
    virtual StatusCode convertAddress( const IOpaqueAddress* pAddress,
				       std::string& refAddress);
    
    /// Provide access to Detector Store
    StoreGateSvc * 	detStore() const;
  
//      /// AddressProvider interface
//      typedef IAddressProvider::tadList tadList;
//      typedef IAddressProvider::tadListIterator tadListIterator;
  
//      /// get all addresses that the provider wants to preload in SG
//      /// maps before the event loop
//      virtual StatusCode preLoadAddresses( StoreID::type storeID,
//  					 tadList& list );
    
//      /// get all new addresses from Provider for this Event.
//      virtual StatusCode loadAddresses( StoreID::type storeID,
//  				      tadList& list );
      
//      /// update a transient Address
//      virtual StatusCode updateAddress( SG::TransientAddress* tad );


    /**@name: Object implementation     */
    //@{
    /// Standard Constructor
    DetDescrCnvSvc(const std::string& name, ISvcLocator* svc);

    /// Standard Destructor
    virtual ~DetDescrCnvSvc();


private:

    StatusCode fillTDSRefs(); 

//      SG::TransientAddress* loadAddress(const CLID& clid, 
//  				      const std::string& key);

    void initTDSItems(); 

    StoreGateSvc *			m_detStore;
    StringArrayProperty			m_detMgrs;
    StringArrayProperty			m_detNodes;
    BooleanProperty 			m_decodeIdDict;
    BooleanProperty 			m_idDictFromRDB;
    StringProperty			m_idDictName;
    StringProperty			m_idDictGlobalTag;
    StringProperty			m_idDictATLASName;
    StringProperty			m_idDictInDetName;
    StringProperty			m_idDictLArName;
    StringProperty			m_idDictTileName;
    StringProperty			m_idDictLVL1Name;
    StringProperty			m_idDictMuonName;
    StringProperty			m_idDictLArHighVoltageName;
    StringProperty			m_idDictLArElectrodeName;
    StringProperty			m_idDictForwardName;
    StringProperty			m_fcal2dNeighborsName;
    StringProperty			m_fcal3dNeighborsNextName;
    StringProperty			m_fcal3dNeighborsPrevName;
    StringProperty			m_tileNeighborsName;
    StringProperty			m_fullAtlasNeighborsName;
    BooleanProperty 			m_fromRoot;
    BooleanProperty 			m_fromNova;
    BooleanProperty 			m_detElemsfromDetNodes;
    BooleanProperty 			m_compact_ids_only;
    BooleanProperty 			m_do_checks;
    BooleanProperty 			m_do_neighbours;
};
#endif









