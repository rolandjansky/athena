///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ISLIMMINGHDLR_H 
#define ATHENAKERNEL_ISLIMMINGHDLR_H 1

// STL includes
#include <typeinfo>

// FrameWork includes
#include "GaudiKernel/INamedInterface.h"

// Forward declaration

namespace Athena {

/**
 * @class Athena::ISlimmingHdlr
 * @brief This class defines a protocol to slim objects (removing parts of that
 *        object)
 */
class ISlimmingHdlr
{
public:
  /** @brief virtual destructor
   */
  virtual ~ISlimmingHdlr() = default;

  /** @brief returns a pointer to the object being slimmed
   */
  virtual void *object() =0;

  /** @brief returns the type-id of the object being slimmed
   *         (mostly for debugging purposes)
   */
  virtual 
  const std::type_info& type_id() =0;

  /** @brief returns the component who requested the registration of that
   *         slimming handler
   *         (mostly for debugging purposes)
   */
  virtual 
  const ::INamedInterface* requester() =0;

  /** @brief apply the slimming: remove parts of the object
   */
  virtual void commit() =0;

  /** @brief restore object's state as before slimming was applied
   */
  virtual void rollback() =0;
};

} //> namespace Athena

#endif //> ATHENAKERNEL_ISLIMMINGHDLR_H
