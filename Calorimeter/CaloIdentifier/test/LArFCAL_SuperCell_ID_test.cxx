/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArFCAL_SuperCell_ID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for LArFCAL_SuperCell_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "boost/foreach.hpp"
#include <iostream>
#include <string>
#include <vector>


#include "larfcal_id_test_common.cxx"


LArFCAL_SuperCell_ID* make_helper ()
{
  LArFCAL_SuperCell_ID* idhelper = new LArFCAL_SuperCell_ID;
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("LArCalorimeter",
                                    "IdDictLArCalorimeter_DC3-05-Comm-01.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  idhelper->set_do_neighbours (false);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


void test_basic (const LArFCAL_SuperCell_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (2, 1, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 2, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 3, 1, 1));

  basic_print_id (idhelper, idhelper.channel_id (-2, 1, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 2, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 3, 1, 1));
}


int main()
{
  LArFCAL_SuperCell_ID* idhelper = make_helper();
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, true);
    test_exceptions (*idhelper);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
