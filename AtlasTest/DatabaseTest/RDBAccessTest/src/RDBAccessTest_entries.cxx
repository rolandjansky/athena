/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBAccessTest_entries.cxx
 *
 * @brief Required by Gaudi class factory
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBAccessTest_entries.cxx,v 1.3 2007-01-17 13:29:43 tsulaia Exp $
 */

#include "RDBAccessTest/RDBAccessTest.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(RDBAccessTest)

DECLARE_FACTORY_ENTRIES(RDBAccessTest) {
    DECLARE_ALGORITHM( RDBAccessTest );
}
