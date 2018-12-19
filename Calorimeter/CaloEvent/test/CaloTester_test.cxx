/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CaloEvent/test/CaloTester_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Unit test for CaloTester.
 */


#undef NDEBUG
#include "CaloEvent/CaloTester.h"
#include "StoreGate/setupStoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <iostream>
#include <cassert>


void test1 (CaloTester& tester)
{
  std::cout << "test1\n";
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "test");
  assert( detStore.retrieve().isSuccess() );
  const CaloDetDescrManager* mgr = nullptr;
  assert( detStore->retrieve (mgr, "CaloMgr").isSuccess() );
  assert (mgr != nullptr);
  assert (&tester.mgr() == mgr);

  Identifier cell_id = tester.caloID().cell_id (CaloCell_ID::LAREM,
                                                1, // + barrel
                                                2, // sampling 2
                                                0, // region,
                                                1, // eta
                                                1); // phi
  IdentifierHash cell_hash = tester.caloID().calo_cell_hash (cell_id);

  const CaloDetDescriptor* dd = tester.find_dd (cell_id);
  assert (tester.find_dd (cell_hash) == dd);

  Identifier reg_id = tester.caloID().region_id (cell_id);
  assert (mgr->get_descriptor (reg_id) == dd);

  std::unique_ptr<CaloCell> cell = tester.make_cell (cell_hash);
  assert (cell->caloDDE()->calo_hash());

  std::vector<CaloCell*> cells1 = tester.get_cells();
  std::vector<const CaloCell*> cells2 = tester.get_const_cells();
  std::unique_ptr<CaloCellContainer> ccc = tester.make_ccc();

  assert (cells1.size() == tester.caloID().calo_cell_hash_max());
  assert (cells2.size() == tester.caloID().calo_cell_hash_max());
  assert (ccc->size() == tester.caloID().calo_cell_hash_max());
  for (size_t i = 0; i < ccc->size(); i++) {
    assert (cells1[i] == (*ccc)[i]);
    assert (cells2[i] == (*ccc)[i]);
  }
}


int main (int /*argc*/, char** argv)
{
  std::cout << "CaloEvent/CaloTester_test\n";
  Athena_test::setupStoreGate (argv[0]);
  CaloTester tester;
  assert( tester.record_mgr().isSuccess() );
  test1 (tester);
  return 0;
}
