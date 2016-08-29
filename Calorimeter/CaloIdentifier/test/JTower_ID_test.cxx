/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/GTower_ID_test.cxx
 * @author Walter Hopkins
 * @date Oct 2014
 * @brief Unit test for GTower_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/JTower_ID.h"
#include "IdDictParser/IdDictParser.h"
#include <iostream>


#include "jgtower_id_test_common.cxx"


JTower_ID* make_helper (bool do_neighbours = false)
{
  JTower_ID* idhelper = new JTower_ID;
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("Calorimeter",
                                    "IdDictCalorimeter_DC3-05.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


void test_basic (const JGTowerBase_ID& idhelper)
{
  std::cout << "test_basic\n";
  idhelper.tower_id (3, 0, 0, 0, 0);
  // jTower positive side (detSide/subDet, sampling regionNum, etaNum, phiNum
  basic_print_id (idhelper, idhelper.tower_id (3, 0, 0, 0, 0));
  basic_print_id (idhelper, idhelper.tower_id (3, 0, 0, 1, 15));
  basic_print_id (idhelper, idhelper.tower_id (3, 0, 1, 2, 10));

  // jTower negative side
  basic_print_id (idhelper, idhelper.tower_id (-3, 0, 0, 0, 0));
  basic_print_id (idhelper, idhelper.tower_id (-3, 0, 0, 1, 15));
  basic_print_id (idhelper, idhelper.tower_id (-3, 0, 1, 2, 10));

  // jTower positive side, had sampling (detSide/subDet, sampling regionNum, etaNum, phiNum
  basic_print_id (idhelper, idhelper.tower_id (3, 1, 0, 0, 0));
  basic_print_id (idhelper, idhelper.tower_id (3, 1, 0, 1, 15));
  basic_print_id (idhelper, idhelper.tower_id (3, 1, 1, 2, 10));
}


int main()
{
  JTower_ID* idhelper = make_helper();
  JTower_ID* idhelper_n = make_helper(true);
  try {
    test_basic (*idhelper);
    test_towers (*idhelper);
    test_exceptions (*idhelper, 3);
    test4 (*idhelper_n);
    testRegionInfo(*idhelper);
  }
  catch(CaloID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
