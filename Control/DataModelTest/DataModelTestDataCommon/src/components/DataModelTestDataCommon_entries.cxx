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


#include "../xAODTestReadSymlink.h"
#include "../xAODTestReadDecor.h"
#include "../xAODTestDecor.h"
#include "../xAODTestAlg.h"
#include "../xAODTestWriteCInfo.h"
#include "../CondWriterAlg.h"
#include "../CondReaderAlg.h"
#include "../xAODTestReadSymlinkTool.h"
#include "../CondAlg1.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestReadSymlink)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestReadDecor)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestDecor)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWriteCInfo)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, CondWriterAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, CondReaderAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, CondAlg1)

DECLARE_NAMESPACE_TOOL_FACTORY(DMTest, xAODTestReadSymlinkTool)

