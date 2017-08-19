/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMNAVIGATIONPROVIDERSVC_H
#define BYTESTREAMNAVIGATIONPROVIDERSVC_H

/** @file ByteStreamNavigationProviderSvc.h
 *  @brief This file contains the class definition for the ByteStreamNavigationProviderSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaKernel/IAddressProvider.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

#include "ByteStreamData/RawEvent.h"

#include <map>

// Forward declarations
class ActiveStoreSvc;
class StoreGateSvc;
class IByteStreamSingleEventInputSvc;
class IROBDataProviderSvc;

template <class TYPE> class SvcFactory;

/** @class ByteStreamNavigationProviderSvc
 *  @brief This class implements IAddressProvider for ByteStream Back navigation.
 **/
class ByteStreamNavigationProviderSvc : public ::AthService, public virtual IAddressProvider {

   /// Allow the factory class access to the constructor
   friend class SvcFactory<ByteStreamNavigationProviderSvc>;

public:
   ByteStreamNavigationProviderSvc(const std::string& name, ISvcLocator* svcloc);
   virtual ~ByteStreamNavigationProviderSvc();

   /// Service initialize
   virtual StatusCode initialize();
   /// Service finalize
   virtual StatusCode finalize();

   /// IAddressProvider interface.
   /// Load the address
   virtual StatusCode loadAddresses(StoreID::type id, tadList& tlist);

   /// Update an existing transient Address
   virtual StatusCode updateAddress(StoreID::type id,
                                    SG::TransientAddress* tad,
                                    const EventContext& ctx);

private:
   /// Return pointer to active event SG
   StoreGateSvc* eventStore();

   const RawEvent* m_rawEvent;

   /// type and name of the objects to create the address for.
   Gaudi::Property<std::vector<std::string>> m_typeNames;

   typedef ServiceHandle<ActiveStoreSvc> ActiveStoreSvc_t;
   ActiveStoreSvc_t m_activeStoreSvc;

   typedef ServiceHandle<IAddressProvider> IAddressProvider_t;
   //IAddressProvider_t m_addressProvider;
   IAddressProvider* m_addressProvider;

   ServiceHandle<IByteStreamSingleEventInputSvc> m_singleEventInputSvc;
   ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;

   /// Read one BytestreamEvent
   StatusCode readEvent();
};

#endif
