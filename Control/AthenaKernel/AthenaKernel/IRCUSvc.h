// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/IRCUSvc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2016
 * @brief read-copy-update (RCU) style synchronization for Athena.
 */


#ifndef ATHENAKERNEL_IRCUSVC_H
#define ATHENAKERNEL_IRCUSVC_H


#include "AthenaKernel/RCUObject.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/IInterface.h"
#include <memory>


namespace Athena {


/**
 * @brief Interface for RCU service.
 *
 * See RCUObject.h for details of the RCU facility.
 *
 * The RCU service allows creating new RCU objects and registering
 * them with the service, and also allows removing them from the service.
 * The service will declare all registered RCU objects as quiescent
 * at the end of an event.
 */
class IRCUSvc
  : virtual public IInterface
{
public:
  DeclareInterfaceID (IRCUSvc,1,0);


  /**
   * @brief Make a new RCU object.
   * @param args... Arguments to pass to the @c T constructor.
   *
   * The object will be registered with the service.
   */
  template <class T, typename... Args>
  std::unique_ptr<RCUObject<T> > newrcu (Args&&... args)
  {
    auto obj = CxxUtils::make_unique<RCUObject<T> > (*this,
                                                     std::forward<Args>(args)...);
    add (obj.get());
    return obj; 
  }


  /**
   * @brief Remove an object from the service.
   * @param obj The object to remove.
   */
  virtual StatusCode remove (IRCUObject* obj) = 0;


  /**
   * @brief Return the number of event slots.
   */
  virtual size_t getNumSlots() const = 0;


private:
  /**
   * @brief Remove an object from the service.
   * @param obj The object to add.
   */
  virtual void add (IRCUObject* obj) = 0;
};


} // namespace Athena


#endif // not ATHENAKERNEL_IRCUSVC_H
