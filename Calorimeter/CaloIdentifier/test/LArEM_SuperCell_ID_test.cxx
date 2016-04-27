/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArEM_SuperCell_ID_test.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Unit test for LArEM_SuperCell_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArEM_SuperCell_ID.h"
#include "IdDictParser/IdDictParser.h"
#include <iostream>


#include "larem_id_test_common.cxx"


LArEM_SuperCell_ID* make_helper (bool do_neighbours = false)
{
  LArEM_SuperCell_ID* idhelper = new LArEM_SuperCell_ID;
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("LArCalorimeter",
                                    "IdDictLArCalorimeter_DC3-05-Comm-01.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


void test_basic (const LArEM_Base_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (1, 1, 1, 0, 1)); 
  basic_print_id (idhelper, idhelper.channel_id (2, 2, 1, 0, 10));
  basic_print_id (idhelper, idhelper.channel_id (3, 2, 0, 0, 20));

  basic_print_id (idhelper, idhelper.channel_id (-1, 1, 1, 0, 1)); 
  basic_print_id (idhelper, idhelper.channel_id (-2, 2, 1, 0, 10));
  basic_print_id (idhelper, idhelper.channel_id (-3, 2, 0, 0, 20));
}


// neighbors
void test4 (const LArEM_SuperCell_ID& /*em_id*/)
{
  std::cout << "test4\n";

#if 0
  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < em_id.channel_hash_max(); ++iCell){
    em_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    for (;last!=first; first++){
      
      IdentifierHash neighbourHash=(*first);
      if(neighbourHash < hash_min ) {
        hash_min = neighbourHash;
      }
      if(neighbourHash > hash_max) {
        hash_max = neighbourHash;
      }
      
    }
  }

  assert (hash_min == 0);
  assert (hash_max == em_id.channel_hash_max()-1);
#endif
}


int main()
{
  LArEM_SuperCell_ID* idhelper = make_helper();
  LArEM_SuperCell_ID* idhelper_n = make_helper(true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, true);
    test_exceptions (*idhelper, true);
    test4 (*idhelper_n);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
