/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class IAddressProvider
 * @brief interface for IOA providers
 *
 * @author <pcalafiura@lbl.gov> - ATLAS Collaboration
 *
 * $Id: IAddressProvider.h,v 1.9 2003-04-16 01:12:23 calaf Exp $
 ***************************************************************************/

#ifndef ATHENAKERNEL_IADDRESSPROVIDER_H
# define ATHENAKERNEL_IADDRESSPROVIDER_H

#include <list>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/StoreID.h"

class EventContext;
namespace SG {
  class TransientAddress;
}

class IAddressProvider
  : virtual public IInterface
{
public:
  DeclareInterfaceID(IAddressProvider, 1, 0);

  typedef std::list<SG::TransientAddress*> tadList;
  typedef tadList::iterator tadListIterator;

  ///get all addresses from Provider : Called before Begin Event
  virtual StatusCode preLoadAddresses(StoreID::type /* storeID */,
				      tadList& /* list */) {
     return StatusCode::SUCCESS;
  }

  /// get all new addresses from Provider for this Event.
  virtual StatusCode loadAddresses(StoreID::type /* storeID */,
				   tadList& /* list */) {
     return StatusCode::SUCCESS;
  }

  /// update a transient Address
  /// The ctx argument will give the current event information.
  /// If we're not dealing with an event store (conditions, etc), then
  /// the context will be empty (default-initialized).
  virtual StatusCode updateAddress(StoreID::type storeID,
				   SG::TransientAddress* pTAd,
                                   const EventContext& ctx) = 0;

  virtual ~IAddressProvider() {}
};


#endif // ATHENAKERNEL_IADDRESSPROVIDER_H



