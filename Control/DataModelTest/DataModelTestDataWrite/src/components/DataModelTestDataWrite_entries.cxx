// $Id: DataModelTestDataWrite_entries.cxx,v 1.2 2006-04-07 18:34:15 ssnyder Exp $
/**
 * @file  src/components/DataModelTestDataWrite_entries.cxx
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Gaudi algorithm factory declarations.
 */

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

DECLARE_COMPONENT( DMTest::DMTestWrite )
DECLARE_COMPONENT( DMTest::AuxDataTestWrite )
DECLARE_COMPONENT( DMTest::xAODTestWrite )
DECLARE_COMPONENT( DMTest::xAODTestWriteCVec )
DECLARE_COMPONENT( DMTest::xAODTestWriteHVec )
DECLARE_COMPONENT( DMTest::xAODTestWriteCView )
DECLARE_COMPONENT( DMTest::xAODTestWriteCVecConst )
DECLARE_COMPONENT( DMTest::xAODTestWriteSymlinks )
DECLARE_COMPONENT( DMTest::HLTResultWriter )

DECLARE_COMPONENT( DMTest::xAODTestWriteCInfoTool )

