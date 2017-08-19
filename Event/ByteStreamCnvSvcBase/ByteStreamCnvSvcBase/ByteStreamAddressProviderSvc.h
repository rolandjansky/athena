/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_BYTESTREAMADDRESSPROVIDERSVC_H
#define BYTESTREAMCNVSVCBASE_BYTESTREAMADDRESSPROVIDERSVC_H

#include "AthenaKernel/IAddressProvider.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include <vector>
#include <string>
#include <map>
#include <set>

class IROBDataProviderSvc;
class IClassIDSvc;

template <class TYPE> class SvcFactory;

/** @class ByteStreamAddressProviderSvc
    @brief An concrete IAddressProvider
*/
class ByteStreamAddressProviderSvc : public ::AthService, public virtual IAddressProvider {
   /// Allow the factory class access to the constructor
   friend class SvcFactory<ByteStreamAddressProviderSvc>;

public:
   ByteStreamAddressProviderSvc(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~ByteStreamAddressProviderSvc();

   // Service initialize
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   // IAddressProvider interface.
   // preload the address
   virtual StatusCode preLoadAddresses(StoreID::type id, tadList& tlist);

   /// update an existing transient Address
   virtual StatusCode updateAddress(StoreID::type id,
                                    SG::TransientAddress* tad,
                                    const EventContext& ctx);

private:
   // type and name of the objects to create the address for.
   std::vector<std::string> m_typeNames;

   ServiceHandle<IROBDataProviderSvc> m_rdpSvc;
   ServiceHandle<IClassIDSvc> m_clidSvc;

   std::map<unsigned int, std::set<std::string> > m_clidKey;

   UnsignedIntegerProperty        m_ctpModuleID;
   UnsignedIntegerProperty        m_muCTPIModuleID;
   UnsignedIntegerArrayProperty   m_jetProcModuleID;
   UnsignedIntegerArrayProperty   m_caloClusterProcModuleID;
   UnsignedIntegerArrayProperty   m_topoProcModuleID;
   int m_storeID;
};

#endif
