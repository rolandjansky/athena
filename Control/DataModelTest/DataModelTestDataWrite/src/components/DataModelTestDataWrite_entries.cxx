// $Id: DataModelTestDataWrite_entries.cxx,v 1.2 2006-04-07 18:34:15 ssnyder Exp $
/**
 * @file  src/components/DataModelTestDataWrite_entries.cxx
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Gaudi algorithm factory declarations.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../DMTestWrite.h"
#include "../AuxDataTestWrite.h"
#include "../xAODTestWrite.h"
#include "../xAODTestWriteCVec.h"
#include "../xAODTestWriteHVec.h"
#include "../xAODTestWriteCView.h"
#include "../xAODTestWriteCVecConst.h"
#include "../xAODTestWriteSymlinks.h"
#include "../HLTResultWriter.h"
#include "../xAODTestWriteCInfoTool.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, DMTestWrite)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, AuxDataTestWrite)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWrite)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWriteCVec)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWriteHVec)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWriteCView)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWriteCVecConst)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWriteSymlinks)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, HLTResultWriter)

DECLARE_NAMESPACE_TOOL_FACTORY(DMTest, xAODTestWriteCInfoTool)
