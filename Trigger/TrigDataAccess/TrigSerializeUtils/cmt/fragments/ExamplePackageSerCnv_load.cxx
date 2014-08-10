/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ExamplePackageCnv_load.cxx
 * @brief Loads component instantiation factories for an example SerCnv converter
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 * a copy of poolcnv by @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: ExamplePackageSerCnv_load.cxx,v 1.1.1.1 2008-05-22 21:33:31 masik Exp $
 */

#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(${package}SerCnv)
