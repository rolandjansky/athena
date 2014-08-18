// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataVectorConvert.h,v 1.3 2008-08-20 05:14:42 yyao Exp $

/**
 * @file DataModelAthenaPool/DataVectorConvert.h
 * @author scott snyder
 * @date Oct 2007, from earlier code of Nov 2005
 * @brief Provide backwards compatibility for reading @c DataVector classes.
 */

#ifndef DATAMODELATHENAPOOL_DATAVECTORCONVERT_H
#define DATAMODELATHENAPOOL_DATAVECTORCONVERT_H


#include "RootUtils/ILogger.h"


namespace DataModelAthenaPool {


/**
 * @brief Provide backwards compatibility for reading @c DataVector classes.
 *
 * When setting up @c DataVector inheritance using @c DATAVECTOR_BASE,
 * a side effect is that the persistent form of the data may change.
 * Including this service will fix up the root metadata to allow
 * reading old data.  In fact, any sort of @c DataVector\<A> to
 * @c DataVector\<B> conversions will be allowed, provided that
 * @c A* can be converted to @c B*.
 *
 * The only reason to make this a class rather than a free function
 * is that Root's autoloading seems to work only with classes.
 */
class DataVectorConvert
{
public:
  /**
   * @brief Scan all known @c DataVector classes and fix up the Root
   *        data for any that need conversion.
   * @param logfn Callback for logging debug messages.
   *              Null for no logging.
   *
   *        It's ok to call this more than once to look for any new classes
   *        (we remember which classes we've already processed to avoid
   *        doing them again).
   */
  static void initialize (RootUtils::ILogger* logfn = 0);
};


} // namespace DataModelAthenaPool


#endif // not DATAMODELATHENAPOOL_DATAVECTORCONVERT_H
