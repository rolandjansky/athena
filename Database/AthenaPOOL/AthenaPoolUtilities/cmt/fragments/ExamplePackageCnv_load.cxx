/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ExamplePackageCnv_load.cxx
 * @brief Loads component instantiation factories for an example POOL converter
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: ExamplePackageCnv_load.cxx,v 1.3 2005-01-12 20:47:08 kkarr Exp $
 */

#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(${package}PoolCnv)
