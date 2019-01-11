/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_SGTESTS_H
# define STOREGATE_SGTESTS_H 1
/**
   @brief "library" of SG test functions. 
   Useful to exercise StoreGateSvc templates compilation

   @author ATLAS Collaboration
   $Id: SGtests.h,v 1.7 2008-07-10 00:33:50 calaf Exp $
 ***************************************************************************/

#include "CxxUtils/checker_macros.h"
class StoreGateSvc;

namespace Athena_test 
{
  void testCreate(StoreGateSvc& rSG);

  void testRecord(StoreGateSvc& rSG);

  void testSymLink(StoreGateSvc& rSG);

  void test_symlink2(StoreGateSvc& rSG);

  void test_symlink3(StoreGateSvc& rSG);

  void testContains(StoreGateSvc& rSG);

  void testFolders(StoreGateSvc& rSG);

  void testRetrieve(StoreGateSvc& rSG);

  void testTryRetrieve(StoreGateSvc& rSG);

  void testReadPrivateCopy(StoreGateSvc& rSG);

  void testRetrievePrivateCopy(StoreGateSvc& rSG);

  void testRemove(StoreGateSvc& rSG);

  void testBind ATLAS_NOT_THREAD_SAFE (StoreGateSvc& rSG);

  void testClear(StoreGateSvc& rSG);

  void testTransientSwap(StoreGateSvc& rSG);

  void testVersionedKey(StoreGateSvc& rSG);

  void testKeys(StoreGateSvc& rSG);

  void testRetrieveAux(StoreGateSvc& rSG);

  void testBoundReset(StoreGateSvc& rSG);

  void testRecordObject(StoreGateSvc& rSG);

  void testWriteAux(StoreGateSvc& rSG);

} //end namespace
#endif // TEST_SGTESTS_H
