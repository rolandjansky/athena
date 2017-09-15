/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaKernel/src/SlotSpecificObj.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Maintain a set of objects, one per slot.
 */


#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/ServiceHandle.h"



namespace {
  // Name of the service to use to find the number of slots.
  std::string getNSlots_hiveMgrName ATLAS_THREAD_SAFE = "EventDataSvc";
} // anonymous namespace



namespace SG {


/**
 * @brief Allow setting the name of the whiteboard service.
 *
 * For testing.
 */
void setNSlotsHiveMgrName ATLAS_NOT_THREAD_SAFE (const std::string& s)
{
  getNSlots_hiveMgrName = s;
}


/**
 * @brief Return the number of event slots.
 *
 * This accesses the @c IHiveWhiteBoard service to find the number of slots.
 */
size_t getNSlots_once()
{
  ServiceHandle<IInterface> svc (getNSlots_hiveMgrName, "getNSlots_once");
  if (!svc.retrieve().isSuccess())
    return 1;
  IHiveWhiteBoard* hivesvc = dynamic_cast<IHiveWhiteBoard*> (svc.get());
  if (!hivesvc) return 1;
  unsigned int nslots = hivesvc->getNumberOfStores();
  if (nslots < 1) return 1;
  return nslots;
}


/**
 * @brief Return the number of event slots.
 *
 * Calls @c getNSlots_once the first time we're called and caches the result.
 */
size_t getNSlots()
{
  const static size_t nslots = getNSlots_once();
  return nslots;
}


} // namespace SG
