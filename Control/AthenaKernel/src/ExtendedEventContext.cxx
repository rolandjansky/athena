/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/src/ExtendedEventContext.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Out-of-line functions to manipulate the extended event context.
 *
 * See the header for motivation.
 */


#include "AthenaKernel/ExtendedEventContext.h"


namespace Atlas {


/**
 * @brief Test whether a context object has an extended context installed.
 */
bool hasExtendedEventContext (const EventContext& ctx)
{
  return ctx.hasExtension<ExtendedEventContext>();
}


/**
 * @brief Retrieve an extended context from a context object.
 */
const ExtendedEventContext& getExtendedEventContext (const EventContext& ctx)
{
  return ctx.getExtension<ExtendedEventContext>();
}


/**
 * @brief Retrieve an extended context from a context object.
 */
ExtendedEventContext& getExtendedEventContext (EventContext& ctx)
{
  return ctx.getExtension<ExtendedEventContext>();
}


/**
 * @brief Move an extended context into a context object.
 */
void setExtendedEventContext (EventContext& ctx,
                              ExtendedEventContext&& ectx)
{
  ctx.setExtension (std::move (ectx));
}


} // namespace Atlas
