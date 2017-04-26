/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file DataModeltestDataCommon/src/components/DataModelTestDataCommon_entries.cxx
 * @author snyder@bnl.gov
 * @date Apr, 2017
 * @brief Gaudi algorithm factory declarations.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../xAODTestReadSymlink.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestReadSymlink)
